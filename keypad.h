/**
 * @file keypad.h
 * @brief Controlador híbrido del teclado matricial 4x4 para FreeRTOS
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Sistema híbrido adaptado del Proyecto 1:
 * - Interrupciones detectan filas (GP6-GP9)
 * - Polling identifica columnas (GP10-GP13)
 * - Máquina de estados: IDLE → DEBOUNCE → COLUMN_SCAN → PRESSED → RELEASED
 * - FreeRTOS: Semáforos para sincronización ISR-tarea
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/**
 * @brief Estados de la máquina de estados híbrida (del Proyecto 1)
 */
typedef enum {
    KEYPAD_IDLE,           /**< Esperando interrupciones */
    KEYPAD_DEBOUNCE,       /**< Aplicando debounce después de IRQ */
    KEYPAD_COLUMN_SCAN,    /**< Polling secuencial de columnas */
    KEYPAD_PRESSED,        /**< Tecla confirmada como presionada */
    KEYPAD_RELEASED        /**< Esperando liberación completa */
} keypad_fsm_state_t;

/**
 * @brief Estructura para eventos del teclado
 */
typedef struct {
    char key;               /**< Tecla presionada */
    uint32_t timestamp;     /**< Timestamp de la pulsación */
} keypad_event_t;

/**
 * @brief Inicializa el sistema híbrido del teclado matricial
 * 
 * @return true Si la inicialización fue exitosa
 * @return false Si hubo error en la inicialización
 */
bool keypad_init(void);

/**
 * @brief Tarea de FreeRTOS para manejar el teclado híbrido
 * 
 * @param pvParameters Parámetros de la tarea (no utilizados)
 */
void keypad_task(void *pvParameters);

/**
 * @brief Obtiene un evento del teclado
 * 
 * @param event Puntero donde almacenar el evento
 * @param timeout_ms Tiempo máximo de espera en ms
 * @return true Si se recibió un evento
 * @return false Si hubo timeout
 */
bool keypad_get_event(keypad_event_t *event, uint32_t timeout_ms);

/**
 * @brief Verifica si el teclado está inactivo (para WFI)
 * 
 * @return true Si está inactivo
 * @return false Si está activo
 */
bool keypad_is_idle(void);

#endif // KEYPAD_H