/**
 * @file access_control.h
 * @brief Controlador principal del sistema de control de acceso con FreeRTOS
 * @author Sistema de Control de Acceso  
 * @date 2025
 * 
 * Este módulo implementa la máquina de estados principal del sistema de
 * control de acceso utilizando FreeRTOS para comunicación entre tareas.
 */

#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <stdbool.h>
#include <stdint.h>
#include "database.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Estados posibles del sistema de control de acceso
 */
typedef enum {
    STATE_IDLE,                     /**< Sistema en espera */
    STATE_ENTERING_ID,              /**< Usuario ingresando ID */
    STATE_ENTERING_PASSWORD,        /**< Usuario ingresando contraseña */
    STATE_PROCESSING,               /**< Sistema procesando autenticación */
    STATE_ACCESS_GRANTED,           /**< Acceso concedido */
    STATE_ACCESS_DENIED,            /**< Acceso denegado */
    STATE_TIMEOUT,                  /**< Tiempo agotado durante el proceso */
    STATE_CHANGE_PASSWORD,          /**< Modo de cambio de contraseña activo */
    STATE_CHANGE_ENTERING_ID,       /**< Cambio: ingresando ID de usuario */
    STATE_CHANGE_ENTERING_OLD_PASS, /**< Cambio: ingresando contraseña actual */
    STATE_CHANGE_ENTERING_NEW_PASS, /**< Cambio: ingresando nueva contraseña */
    STATE_CHANGE_PROCESSING         /**< Cambio: procesando cambio de contraseña */
} system_state_t;

/**
 * @brief Eventos del sistema de control de acceso
 */
typedef enum {
    ACCESS_EVENT_KEY_PRESSED,
    ACCESS_EVENT_TIMEOUT,
    ACCESS_EVENT_RESET
} access_event_type_t;

/**
 * @brief Estructura para eventos del sistema de control de acceso
 */
typedef struct {
    access_event_type_t type;
    char key;                    /**< Tecla presionada (solo para KEY_PRESSED) */
    uint32_t timestamp;
} access_event_t;

/** @brief Tiempo máximo para completar el proceso de autenticación */
#define TIMEOUT_MS 10000    

/** @brief Tiempo de antirebote entre pulsaciones de teclas */
#define DEBOUNCE_MS 300     

/**
 * @brief Inicializa el sistema de control de acceso
 * 
 * Configura el estado inicial del sistema, inicializa variables internas,
 * y crea las colas necesarias para comunicación entre tareas.
 * 
 * @return true Si la inicialización fue exitosa
 * @return false Si hubo error en la inicialización
 */
bool access_control_init(void);

/**
 * @brief Tarea de FreeRTOS para el control de acceso
 * 
 * Esta tarea maneja la máquina de estados del sistema de control de acceso,
 * procesa eventos desde otras tareas y coordina las respuestas del sistema.
 * 
 * @param pvParameters Parámetros de la tarea (no utilizados)
 */
void access_control_task(void *pvParameters);

/**
 * @brief Envía un evento al sistema de control de acceso
 * 
 * @param type Tipo de evento
 * @param key Tecla presionada (solo para KEY_PRESSED)
 * @return true Si el evento se envió exitosamente
 * @return false Si hubo error al enviar el evento
 */
bool access_control_send_event(access_event_type_t type, char key);

/**
 * @brief Obtiene el estado actual del sistema
 * 
 * @return system_state_t Estado actual del sistema de control de acceso
 */
system_state_t access_control_get_state(void);

#endif // ACCESS_CONTROL_H
