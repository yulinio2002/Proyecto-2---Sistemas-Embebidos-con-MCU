/**
 * @file ssd1306_display.h
 * @brief Driver para display SSD1306 I2C para el sistema de control de acceso
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Driver simplificado del display SSD1306 OLED I2C para mostrar información
 * del sistema de control de acceso incluyendo fecha/hora, mensajes de entrada
 * y estado del sistema.
 */

#ifndef SSD1306_DISPLAY_H
#define SSD1306_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Tipo de mensaje para mostrar en el display
 */
typedef enum {
    DISPLAY_MSG_STANDBY,        /**< Mostrar fecha y hora en espera */
    DISPLAY_MSG_ENTER_ID,       /**< "Ingrese su ID" */
    DISPLAY_MSG_ENTER_PASSWORD, /**< "Ingrese su contraseña" */
    DISPLAY_MSG_WELCOME,        /**< "Bienvenido" */
    DISPLAY_MSG_INVALID,        /**< "Usuario o Contraseña inválidos" */
    DISPLAY_MSG_CHANGE_USER,    /**< Mensajes para cambio de usuario */
    DISPLAY_MSG_CUSTOM          /**< Mensaje personalizado */
} display_message_type_t;

/**
 * @brief Estructura para comandos del display
 */
typedef struct {
    display_message_type_t type;
    char custom_message[32];    /**< Para mensajes personalizados */
    uint32_t display_time_ms;   /**< Tiempo a mostrar el mensaje (0 = permanente) */
} display_command_t;

/**
 * @brief Inicializa el display SSD1306 I2C
 * 
 * Configura el hardware I2C, inicializa el display y prepara
 * la pantalla para mostrar información del sistema.
 * 
 * @return true Si la inicialización fue exitosa
 * @return false Si hubo error en la inicialización
 */
bool ssd1306_init(void);

/**
 * @brief Muestra un mensaje en el display
 * 
 * @param type Tipo de mensaje a mostrar
 * @param custom_message Mensaje personalizado (solo si type es DISPLAY_MSG_CUSTOM)
 */
void ssd1306_show_message(display_message_type_t type, const char* custom_message);

/**
 * @brief Actualiza la fecha y hora en el display
 * 
 * Esta función debe llamarse periódicamente para mantener
 * actualizada la información de fecha y hora.
 */
void ssd1306_update_datetime(void);

/**
 * @brief Limpia completamente el display
 */
void ssd1306_clear(void);

/**
 * @brief Tarea de FreeRTOS para manejar el display
 * 
 * Esta tarea maneja la cola de comandos del display y actualiza
 * la pantalla según los mensajes recibidos.
 * 
 * @param pvParameters Parámetros de la tarea (no utilizados)
 */
void display_task(void *pvParameters);

/**
 * @brief Envía un comando al display desde otras tareas
 * 
 * @param type Tipo de mensaje a mostrar
 * @param custom_message Mensaje personalizado (puede ser NULL)
 * @param display_time_ms Tiempo a mostrar el mensaje (0 = permanente)
 * @return true Si el comando se envió exitosamente
 * @return false Si hubo error al enviar el comando
 */
bool ssd1306_send_command(display_message_type_t type, const char* custom_message, uint32_t display_time_ms);

#endif /* SSD1306_DISPLAY_H */
