/**
 * @file leds.h
 * @brief Módulo de control de señales luminosas para sistema de acceso con FreeRTOS
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Este módulo maneja las señales luminosas del sistema de control de acceso
 * utilizando FreeRTOS, incluyendo LEDs de estado y señalización de eventos.
 */

#ifndef LEDS_H
#define LEDS_H

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "queue.h"

/** @brief Pin GPIO para LED verde - indica acceso concedido */
#define LED_VERDE_PIN   15  

/** @brief Pin GPIO para LED rojo - indica acceso denegado */
#define LED_ROJO_PIN    16  

/** @brief Pin GPIO para LED amarillo - estado del sistema */
#define LED_AMARILLO_PIN 17 

/**
 * @brief Estados posibles de los LEDs
 */
typedef enum {
    LED_OFF,    /**< LED apagado */
    LED_ON,     /**< LED encendido permanente */
    LED_BLINK   /**< LED parpadeando */
} led_state_t;

/**
 * @brief Comandos para la tarea de LEDs
 */
typedef enum {
    LED_CMD_VERDE_ON,
    LED_CMD_VERDE_OFF,
    LED_CMD_ROJO_ON,
    LED_CMD_ROJO_OFF,
    LED_CMD_AMARILLO_ON,
    LED_CMD_AMARILLO_OFF,
    LED_CMD_AMARILLO_BLINK,
    LED_CMD_ALL_OFF,
    LED_CMD_ACCESO_CONCEDIDO,
    LED_CMD_ACCESO_DENEGADO,
    LED_CMD_SISTEMA_LISTO,
    LED_CMD_PROCESO_INICIADO,
    LED_CMD_ESPERANDO_CLAVE
} led_command_t;

/**
 * @brief Estructura para comandos de LED
 */
typedef struct {
    led_command_t command;
    uint32_t duration_ms;  /**< Duración del comando (0 = permanente) */
} led_cmd_t;

/**
 * @brief Inicializa todos los LEDs del sistema
 * 
 * Configura los pines GPIO como salidas y los inicializa en estado apagado.
 * Crea la cola para comandos de LEDs.
 * 
 * @return true Si la inicialización fue exitosa
 * @return false Si hubo error en la inicialización
 */
bool leds_init(void);

/**
 * @brief Tarea de FreeRTOS para manejar los LEDs
 * 
 * Esta tarea procesa comandos de LED desde una cola y maneja
 * los patrones de parpadeo automáticamente.
 * 
 * @param pvParameters Parámetros de la tarea (no utilizados)
 */
void led_task(void *pvParameters);

/**
 * @brief Envía un comando a la tarea de LEDs
 * 
 * @param command Comando a ejecutar
 * @param duration_ms Duración del comando (0 = permanente)
 * @return true Si el comando se envió exitosamente
 * @return false Si hubo error al enviar el comando
 */
bool led_send_command(led_command_t command, uint32_t duration_ms);

/* Funciones de conveniencia para señalización */

/**
 * @brief Señaliza acceso concedido con LED verde durante 5 segundos
 */
void signal_acceso_concedido(void);

/**
 * @brief Señaliza acceso denegado con LED rojo durante 2 segundos
 */
void signal_acceso_denegado(void);

/**
 * @brief Señaliza que el sistema está listo con LED amarillo permanente
 */
void signal_sistema_listo(void);

/**
 * @brief Señaliza que el proceso de autenticación ha iniciado
 */
void signal_proceso_iniciado(void);

/**
 * @brief Señaliza espera de contraseña con LED amarillo parpadeando
 */
void signal_esperando_clave(void);

#endif // LEDS_H
