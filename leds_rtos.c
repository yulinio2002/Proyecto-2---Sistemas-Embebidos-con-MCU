/**
 * @file leds.c
 * @brief Implementación del módulo de control de LEDs para FreeRTOS
 * @author Sistema de Control de Acceso
 * @date 2025
 */

#include "leds.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

/** @brief Cola para comandos de LEDs */
static QueueHandle_t led_queue;

/** @brief Estados actuales de los LEDs */
static struct {
    led_state_t verde;
    led_state_t rojo;
    led_state_t amarillo;
} led_states = {LED_OFF, LED_OFF, LED_OFF};

/**
 * @brief Inicializa todos los LEDs del sistema
 */
bool leds_init(void) {
    // Configurar pines GPIO como salidas
    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_put(LED_VERDE_PIN, 0);

    gpio_init(LED_ROJO_PIN);
    gpio_set_dir(LED_ROJO_PIN, GPIO_OUT);
    gpio_put(LED_ROJO_PIN, 0);

    gpio_init(LED_AMARILLO_PIN);
    gpio_set_dir(LED_AMARILLO_PIN, GPIO_OUT);
    gpio_put(LED_AMARILLO_PIN, 0);

    // Crear cola para comandos de LEDs
    led_queue = xQueueCreate(10, sizeof(led_cmd_t));
    if (led_queue == NULL) {
        return false;
    }

    printf("LEDs inicializados correctamente\n");
    return true;
}

/**
 * @brief Controla físicamente un LED
 */
static void set_led_physical(uint gpio_pin, bool state) {
    gpio_put(gpio_pin, state ? 1 : 0);
}

/**
 * @brief Tarea de FreeRTOS para manejar los LEDs
 */
void led_task(void *pvParameters) {
    led_cmd_t cmd;
    TickType_t last_blink_time = 0;
    bool blink_state = false;
    
    while (1) {
        // Verificar si hay comandos en la cola
        if (xQueueReceive(led_queue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
            switch (cmd.command) {
                case LED_CMD_VERDE_ON:
                    led_states.verde = LED_ON;
                    set_led_physical(LED_VERDE_PIN, true);
                    printf("LED Verde: ON\n");
                    
                    // Si tiene duración, programar apagado
                    if (cmd.duration_ms > 0) {
                        vTaskDelay(pdMS_TO_TICKS(cmd.duration_ms));
                        led_states.verde = LED_OFF;
                        set_led_physical(LED_VERDE_PIN, false);
                        printf("LED Verde: OFF (timeout)\n");
                    }
                    break;

                case LED_CMD_VERDE_OFF:
                    led_states.verde = LED_OFF;
                    set_led_physical(LED_VERDE_PIN, false);
                    printf("LED Verde: OFF\n");
                    break;

                case LED_CMD_ROJO_ON:
                    led_states.rojo = LED_ON;
                    set_led_physical(LED_ROJO_PIN, true);
                    printf("LED Rojo: ON\n");
                    
                    // Si tiene duración, programar apagado
                    if (cmd.duration_ms > 0) {
                        vTaskDelay(pdMS_TO_TICKS(cmd.duration_ms));
                        led_states.rojo = LED_OFF;
                        set_led_physical(LED_ROJO_PIN, false);
                        printf("LED Rojo: OFF (timeout)\n");
                    }
                    break;

                case LED_CMD_ROJO_OFF:
                    led_states.rojo = LED_OFF;
                    set_led_physical(LED_ROJO_PIN, false);
                    printf("LED Rojo: OFF\n");
                    break;

                case LED_CMD_AMARILLO_ON:
                    led_states.amarillo = LED_ON;
                    set_led_physical(LED_AMARILLO_PIN, true);
                    printf("LED Amarillo: ON\n");
                    break;

                case LED_CMD_AMARILLO_OFF:
                    led_states.amarillo = LED_OFF;
                    set_led_physical(LED_AMARILLO_PIN, false);
                    printf("LED Amarillo: OFF\n");
                    break;

                case LED_CMD_AMARILLO_BLINK:
                    led_states.amarillo = LED_BLINK;
                    last_blink_time = xTaskGetTickCount();
                    blink_state = true;
                    set_led_physical(LED_AMARILLO_PIN, true);
                    printf("LED Amarillo: BLINK\n");
                    break;

                case LED_CMD_ALL_OFF:
                    led_states.verde = LED_OFF;
                    led_states.rojo = LED_OFF;
                    led_states.amarillo = LED_OFF;
                    set_led_physical(LED_VERDE_PIN, false);
                    set_led_physical(LED_ROJO_PIN, false);
                    set_led_physical(LED_AMARILLO_PIN, false);
                    printf("Todos los LEDs: OFF\n");
                    break;

                case LED_CMD_ACCESO_CONCEDIDO:
                    // LED verde por 5 segundos
                    led_states.verde = LED_ON;
                    set_led_physical(LED_VERDE_PIN, true);
                    printf("Señal: Acceso Concedido\n");
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    led_states.verde = LED_OFF;
                    set_led_physical(LED_VERDE_PIN, false);
                    break;

                case LED_CMD_ACCESO_DENEGADO:
                    // LED rojo por 2 segundos
                    led_states.rojo = LED_ON;
                    set_led_physical(LED_ROJO_PIN, true);
                    printf("Señal: Acceso Denegado\n");
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    led_states.rojo = LED_OFF;
                    set_led_physical(LED_ROJO_PIN, false);
                    break;

                case LED_CMD_SISTEMA_LISTO:
                    led_states.amarillo = LED_ON;
                    set_led_physical(LED_AMARILLO_PIN, true);
                    printf("Señal: Sistema Listo\n");
                    break;

                case LED_CMD_PROCESO_INICIADO:
                    led_states.amarillo = LED_OFF;
                    set_led_physical(LED_AMARILLO_PIN, false);
                    printf("Señal: Proceso Iniciado\n");
                    break;

                case LED_CMD_ESPERANDO_CLAVE:
                    led_states.amarillo = LED_BLINK;
                    last_blink_time = xTaskGetTickCount();
                    blink_state = true;
                    set_led_physical(LED_AMARILLO_PIN, true);
                    printf("Señal: Esperando Clave\n");
                    break;
            }
        }

        // Manejar parpadeo del LED amarillo a 0.5Hz (período de 2 segundos, 1 segundo ON, 1 segundo OFF)
        if (led_states.amarillo == LED_BLINK) {
            TickType_t current_time = xTaskGetTickCount();
            if (current_time - last_blink_time >= pdMS_TO_TICKS(1000)) { // Cambio cada 1 segundo para 0.5Hz
                blink_state = !blink_state;
                set_led_physical(LED_AMARILLO_PIN, blink_state);
                last_blink_time = current_time;
            }
        }
    }
}

/**
 * @brief Envía un comando a la tarea de LEDs
 */
bool led_send_command(led_command_t command, uint32_t duration_ms) {
    if (led_queue == NULL) {
        return false;
    }

    led_cmd_t cmd = {
        .command = command,
        .duration_ms = duration_ms
    };

    return xQueueSend(led_queue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE;
}

/* Funciones de conveniencia */

void signal_acceso_concedido(void) {
    led_send_command(LED_CMD_ACCESO_CONCEDIDO, 0);
}

void signal_acceso_denegado(void) {
    led_send_command(LED_CMD_ACCESO_DENEGADO, 0);
}

void signal_sistema_listo(void) {
    led_send_command(LED_CMD_SISTEMA_LISTO, 0);
}

void signal_proceso_iniciado(void) {
    led_send_command(LED_CMD_PROCESO_INICIADO, 0);
}

void signal_esperando_clave(void) {
    led_send_command(LED_CMD_ESPERANDO_CLAVE, 0);
}
