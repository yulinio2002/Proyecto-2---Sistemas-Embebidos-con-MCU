/**
 * @file keypad.c
 * @brief Implementación híbrida del teclado matricial adaptada del Proyecto 1 a FreeRTOS
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * SISTEMA HÍBRIDO ADAPTADO DEL PROYECTO 1:
 * 
 * INTERRUPCIONES (Parte Reactiva):
 * - Detectan automáticamente cuando se presiona una tecla
 * - Identifican QUÉ FILA se activó (GP6, GP7, GP8, GP9)
 * - Activan el estado DEBOUNCE para iniciar el polling
 * - Despiertan la tarea FreeRTOS via semáforo
 * 
 * POLLING (Parte Activa - Dentro de FreeRTOS):
 * - Se ejecuta solo DESPUÉS de una interrupción
 * - Escanea las columnas secuencialmente usando TickType_t
 * - Combina fila (de IRQ) + columna (de polling) = tecla exacta
 * - Usa vTaskDelay() en lugar de absolute_time
 * 
 * FLUJO HÍBRIDO CON FREERTOS:
 * 1. KEYPAD_IDLE: Tarea esperando en semáforo
 * 2. IRQ detecta fila → KEYPAD_DEBOUNCE → xSemaphoreGiveFromISR()
 * 3. Tarea despierta → KEYPAD_COLUMN_SCAN: Polling → KEYPAD_PRESSED
 * 4. KEYPAD_PRESSED → KEYPAD_RELEASED → KEYPAD_IDLE
 */

#include "keypad.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define ROWS 4
#define COLS 4
#define DEBOUNCE_TIME_MS 30        // Del proyecto 1
#define RELEASE_TIME_MS 20         // Del proyecto 1
#define COLUMN_STABILIZATION_MS 1  // Adaptado para FreeRTOS

/**
 * @brief Pines GPIO asignados a las filas del teclado (con interrupciones)
 */
static const uint row_pins[ROWS] = {6, 7, 8, 9};

/**
 * @brief Pines GPIO asignados a las columnas del teclado (para polling)
 */
static const uint col_pins[COLS] = {10, 11, 12, 13};

/**
 * @brief Mapa de caracteres del teclado matricial 4x4 (del proyecto 1)
 */
static const char keymap[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/**
 * @brief Estructura de control híbrido (adaptada del proyecto 1)
 */
typedef struct {
    keypad_fsm_state_t state;       /**< Estado actual de la FSM */
    TickType_t last_change;         /**< Timestamp del último cambio */
    uint8_t detected_row;           /**< Fila detectada por IRQ */
    uint8_t detected_col;           /**< Columna encontrada por polling */
    uint8_t current_column;         /**< Columna actual siendo escaneada */
    TickType_t column_change_time;  /**< Tiempo del cambio de columna */
    bool irq_pending;               /**< Bandera de IRQ pendiente */
} keypad_hybrid_controller_t;

/** @brief Controlador híbrido principal */
static keypad_hybrid_controller_t hybrid_ctrl = {
    .state = KEYPAD_IDLE,
    .last_change = 0,
    .detected_row = 0,
    .detected_col = 0,
    .current_column = 0,
    .column_change_time = 0,
    .irq_pending = false
};

/** @brief Cola para eventos del teclado */
static QueueHandle_t keypad_queue;

/** @brief Semáforo para despertar tarea desde ISR */
static SemaphoreHandle_t keypad_wakeup_semaphore;

/**
 * @brief ISR para interrupciones del teclado (adaptada del proyecto 1)
 * 
 * PARTE 1: DETECCIÓN POR INTERRUPCIONES
 * Las interrupciones detectan CUÁL fila se activó, pero no la columna específica
 */
static void keypad_gpio_isr(uint gpio, uint32_t events) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Solo procesar si estamos en IDLE
    if (hybrid_ctrl.state != KEYPAD_IDLE) {
        return;
    }

    // Identificar qué fila generó la interrupción
    for (int r = 0; r < ROWS; r++) {
        if (gpio == row_pins[r]) {
            hybrid_ctrl.detected_row = r;              // Guardar fila detectada por IRQ
            hybrid_ctrl.last_change = xTaskGetTickCountFromISR();
            hybrid_ctrl.state = KEYPAD_DEBOUNCE;       // Cambiar a estado de polling
            hybrid_ctrl.irq_pending = true;
            
            // Despertar tarea de procesamiento
            xSemaphoreGiveFromISR(keypad_wakeup_semaphore, &xHigherPriorityTaskWoken);
            break;
        }
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Inicializa el sistema híbrido del teclado (adaptado del proyecto 1)
 */
bool keypad_init(void) {
    printf("Inicializando teclado híbrido basado en Proyecto 1...\n");
    
    // *** CONFIGURACIÓN DEL SISTEMA HÍBRIDO (del proyecto 1) ***
    
    // Configurar columnas para permitir detección por interrupciones
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_OUT);
        gpio_put(col_pins[i], 0); // LOW para permitir interrupciones
        printf("Columna %d (GP%d) configurada como salida\n", i, col_pins[i]);
    }

    // Configurar filas con interrupciones (PARTE REACTIVA)
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_IN);
        gpio_pull_up(row_pins[i]);
        
        // INTERRUPCIÓN: Detecta flanco descendente cuando se presiona tecla
        gpio_set_irq_enabled_with_callback(row_pins[i], GPIO_IRQ_EDGE_FALL, true, &keypad_gpio_isr);
        printf("Fila %d (GP%d) configurada con IRQ en flanco descendente\n", i, row_pins[i]);
    }

    // Crear cola para eventos del teclado
    keypad_queue = xQueueCreate(10, sizeof(keypad_event_t));
    if (keypad_queue == NULL) {
        printf("ERROR: No se pudo crear la cola del teclado\n");
        return false;
    }
    
    // Crear semáforo para despertar tarea desde ISR
    keypad_wakeup_semaphore = xSemaphoreCreateBinary();
    if (keypad_wakeup_semaphore == NULL) {
        printf("ERROR: No se pudo crear el semáforo de despertar\n");
        return false;
    }

    // Estado inicial
    hybrid_ctrl.state = KEYPAD_IDLE;
    hybrid_ctrl.irq_pending = false;
    
    printf("Teclado híbrido inicializado (IRQ + Polling con FreeRTOS)\n");
    return true;
}

/**
 * @brief Ejecuta la máquina de estados híbrida (adaptada del proyecto 1)
 * 
 * PARTE 2: DETECCIÓN POR POLLING
 * La interrupción ya identificó la FILA, ahora polling identifica la COLUMNA
 */
static void keypad_process_hybrid_fsm(void) {
    TickType_t current_time = xTaskGetTickCount();
    
    switch (hybrid_ctrl.state) {
        case KEYPAD_DEBOUNCE: {
            TickType_t time_diff = current_time - hybrid_ctrl.last_change;
            
            if (time_diff >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
                // Verificar que la fila sigue activa (debounce)
                bool row_still_active = !gpio_get(row_pins[hybrid_ctrl.detected_row]);
                
                if (row_still_active) {
                    // Inicializar escaneo de columnas
                    hybrid_ctrl.current_column = 0;
                    hybrid_ctrl.state = KEYPAD_COLUMN_SCAN;
                    
                    // Activar primera columna y marcar tiempo
                    gpio_put(col_pins[0], 1);
                    hybrid_ctrl.column_change_time = current_time;
                    
                    printf("Fila %d activa, iniciando escaneo de columnas\n", hybrid_ctrl.detected_row);
                } else {
                    // Rebote filtrado - volver a IDLE
                    hybrid_ctrl.state = KEYPAD_IDLE;
                    printf("Rebote filtrado en fila %d\n", hybrid_ctrl.detected_row);
                }
            }
            break;
        }

        case KEYPAD_COLUMN_SCAN: {
            TickType_t stabilization_time = current_time - hybrid_ctrl.column_change_time;
            
            // Usar timestamps para estabilización (adaptado de absolute_time a TickType_t)
            if (stabilization_time >= pdMS_TO_TICKS(COLUMN_STABILIZATION_MS)) {
                
                // POLLING: Leer estado de la fila conocida (de la IRQ)
                if (gpio_get(row_pins[hybrid_ctrl.detected_row])) {
                    // ¡Tecla encontrada por combinación IRQ + POLLING!
                    hybrid_ctrl.detected_col = hybrid_ctrl.current_column;
                    hybrid_ctrl.state = KEYPAD_PRESSED;
                    
                    char detected_key = keymap[hybrid_ctrl.detected_row][hybrid_ctrl.detected_col];
                    
                    // Crear evento y enviarlo a la cola
                    keypad_event_t event;
                    event.key = detected_key;
                    event.timestamp = current_time;
                    
                    if (xQueueSend(keypad_queue, &event, 0) == pdTRUE) {
                        printf("Tecla detectada (híbrido): '%c' en fila %d, columna %d\n", 
                               detected_key, hybrid_ctrl.detected_row, hybrid_ctrl.detected_col);
                    }
                    
                    // Restaurar columna para próxima detección
                    gpio_put(col_pins[hybrid_ctrl.current_column], 0);
                    
                } else {
                    // Esta columna no es la correcta, continuar escaneo
                    gpio_put(col_pins[hybrid_ctrl.current_column], 0);
                    hybrid_ctrl.current_column++;
                    
                    if (hybrid_ctrl.current_column < COLS) {
                        // Activar siguiente columna
                        gpio_put(col_pins[hybrid_ctrl.current_column], 1);
                        hybrid_ctrl.column_change_time = current_time;
                    } else {
                        // No se encontró tecla válida
                        hybrid_ctrl.state = KEYPAD_IDLE;
                        printf("No se encontró tecla válida en fila %d\n", hybrid_ctrl.detected_row);
                    }
                }
            }
            break;
        }

        case KEYPAD_PRESSED: {
            // POLLING: Verificar continuamente si la tecla sigue presionada
            bool still_pressed = !gpio_get(row_pins[hybrid_ctrl.detected_row]);
            
            if (!still_pressed) {
                hybrid_ctrl.state = KEYPAD_RELEASED;
                hybrid_ctrl.last_change = current_time;
                printf("Tecla liberada\n");
            }
            break;
        }
            
        case KEYPAD_RELEASED: {
            TickType_t release_time = current_time - hybrid_ctrl.last_change;
            
            // Debounce de liberación antes de permitir nuevas interrupciones
            if (release_time >= pdMS_TO_TICKS(RELEASE_TIME_MS)) {
                hybrid_ctrl.state = KEYPAD_IDLE;  // Listo para nueva IRQ
                hybrid_ctrl.irq_pending = false;
                printf("Sistema híbrido listo para nueva detección\n");
            }
            break;
        }

        default:
            hybrid_ctrl.state = KEYPAD_IDLE;
            hybrid_ctrl.irq_pending = false;
            break;
    }
}

/**
 * @brief Tarea de FreeRTOS para manejar el teclado híbrido
 */
void keypad_task(void *pvParameters) {
    printf("Tarea híbrida del teclado iniciada (Proyecto 1 + FreeRTOS)\n");
    
    while (1) {
        // Esperar señal de interrupción o procesar FSM si está activa
        if (hybrid_ctrl.state == KEYPAD_IDLE) {
            // Estado IDLE: Esperar semáforo de IRQ (bloqueo eficiente)
            xSemaphoreTake(keypad_wakeup_semaphore, portMAX_DELAY);
        } else {
            // Estados activos: Procesar FSM continuamente con pequeñas pausas
            keypad_process_hybrid_fsm();
            vTaskDelay(pdMS_TO_TICKS(5)); // Pequeña pausa para permitir otras tareas
        }
    }
}

/**
 * @brief Obtiene un evento del teclado
 */
bool keypad_get_event(keypad_event_t *event, uint32_t timeout_ms) {
    if (keypad_queue == NULL || event == NULL) {
        return false;
    }
    
    return xQueueReceive(keypad_queue, event, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

/**
 * @brief Verifica si el sistema está en estado IDLE (para WFI)
 */
bool keypad_is_idle(void) {
    return (hybrid_ctrl.state == KEYPAD_IDLE);
}
