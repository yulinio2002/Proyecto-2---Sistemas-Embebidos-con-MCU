/**
 * @file access_control.c
 * @brief Implementación del controlador principal del sistema de control de acceso con FreeRTOS
 * @author Sistema de Control de Acceso
 * @date 2025
 */

#include "access_control.h"
#include "leds.h"
#include "database.h"
#include "ssd1306_display.h"
#include "keypad.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/** @brief Buffer para almacenar el ID del usuario */
static char user_id[ID_LENGTH + 1];

/** @brief Buffer para almacenar la contraseña */
static char password[PASSWORD_LENGTH + 1];

/** @brief Buffer para almacenar la nueva contraseña durante cambio */
static char new_password[PASSWORD_LENGTH + 1];

/** @brief Contador de caracteres ingresados para ID */
static int id_count;

/** @brief Contador de caracteres ingresados para contraseña */
static int password_count;

/** @brief Contador de caracteres para nueva contraseña */
static int new_password_count;

/** @brief Estado actual del sistema */
static system_state_t current_state;

/** @brief Cola para eventos del control de acceso */
static QueueHandle_t access_control_queue;

/** @brief Handle de la tarea de timeout */
static TaskHandle_t timeout_task_handle = NULL;

/**
 * @brief Tarea para manejar timeouts del sistema
 */
static void timeout_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(TIMEOUT_MS));
    
    // Enviar evento de timeout si la tarea no fue cancelada
    access_control_send_event(ACCESS_EVENT_TIMEOUT, 0);
    
    // La tarea se elimina a sí misma
    timeout_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Inicia el timeout del sistema
 */
static void start_timeout(void) {
    if (timeout_task_handle != NULL) {
        vTaskDelete(timeout_task_handle);
        timeout_task_handle = NULL;
    }
    
    xTaskCreate(timeout_task, "Timeout", 128, NULL, tskIDLE_PRIORITY + 1, &timeout_task_handle);
}

/**
 * @brief Cancela el timeout del sistema
 */
static void cancel_timeout(void) {
    if (timeout_task_handle != NULL) {
        vTaskDelete(timeout_task_handle);
        timeout_task_handle = NULL;
    }
}

/**
 * @brief Tarea para timeout de acceso concedido
 */
static void granted_timeout_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(5000)); // 5 segundos para acceso concedido
    
    // Enviar evento de reset para volver al estado inicial
    access_control_send_event(ACCESS_EVENT_RESET, 0);
    
    // La tarea se elimina a sí misma
    timeout_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Tarea para timeout de acceso denegado
 */
static void denied_timeout_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(3000)); // 3 segundos para acceso denegado
    
    // Enviar evento de reset para volver al estado inicial
    access_control_send_event(ACCESS_EVENT_RESET, 0);
    
    // La tarea se elimina a sí misma
    timeout_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Inicia timeout para acceso concedido
 */
static void start_granted_timeout(void) {
    if (timeout_task_handle != NULL) {
        vTaskDelete(timeout_task_handle);
        timeout_task_handle = NULL;
    }
    
    xTaskCreate(granted_timeout_task, "GrantedTimeout", 128, NULL, tskIDLE_PRIORITY + 1, &timeout_task_handle);
}

/**
 * @brief Inicia timeout para acceso denegado
 */
static void start_denied_timeout(void) {
    if (timeout_task_handle != NULL) {
        vTaskDelete(timeout_task_handle);
        timeout_task_handle = NULL;
    }
    
    xTaskCreate(denied_timeout_task, "DeniedTimeout", 128, NULL, tskIDLE_PRIORITY + 1, &timeout_task_handle);
}

/**
 * @brief Resetea el sistema al estado inicial
 */
static void reset_system(void) {
    cancel_timeout();
    
    // Limpiar buffers
    memset(user_id, 0, sizeof(user_id));
    memset(password, 0, sizeof(password));
    memset(new_password, 0, sizeof(new_password));
    id_count = 0;
    password_count = 0;
    new_password_count = 0;
    
    // Volver al estado inicial
    current_state = STATE_IDLE;
    
    // Señalizar sistema listo
    signal_sistema_listo();
    ssd1306_send_command(DISPLAY_MSG_STANDBY, NULL, 0);
    
    printf("Sistema reseteado - Estado: IDLE\n");
}

/**
 * @brief Procesa una tecla presionada según el estado actual
 */
static void process_key_input(char key) {
    printf("Estado: %d, Tecla: %c\n", current_state, key);
    
    switch (current_state) {
        case STATE_IDLE:
            if (key == '*') {
                // Modo de cambio de contraseña
                current_state = STATE_CHANGE_PASSWORD;
                ssd1306_send_command(DISPLAY_MSG_CHANGE_USER, NULL, 0);
                printf("Modo cambio de contraseña activado\n");
            } else if (isdigit(key)) {
                // Iniciar ingreso de ID
                current_state = STATE_ENTERING_ID;
                id_count = 0;
                user_id[id_count++] = key;
                user_id[id_count] = '\0';
                
                // Apagar LED amarillo cuando se presiona el primer dígito
                signal_proceso_iniciado(); // Apaga LED amarillo
                ssd1306_send_command(DISPLAY_MSG_ENTER_ID, NULL, 0);
                start_timeout();
                
                printf("Iniciando ingreso de ID: %s\n", user_id);
            }
            break;
            
        case STATE_ENTERING_ID:
            if (isdigit(key) && id_count < ID_LENGTH) {
                user_id[id_count++] = key;
                user_id[id_count] = '\0';
                printf("ID actual: %s\n", user_id);
            } else if (key == '#' && id_count > 0) {
                // Confirmar ID y pasar a contraseña
                current_state = STATE_ENTERING_PASSWORD;
                password_count = 0;
                memset(password, 0, sizeof(password));
                
                // Cuando confirma ID (6 dígitos), LED amarillo empieza a titilar para contraseña
                signal_esperando_clave(); // LED amarillo titilando a 0.5Hz
                ssd1306_send_command(DISPLAY_MSG_ENTER_PASSWORD, NULL, 0);
                
                printf("ID confirmado: %s - Esperando contraseña\n", user_id);
            } else if (key == '*') {
                // Cancelar y volver al inicio
                reset_system();
            }
            break;
            
        case STATE_ENTERING_PASSWORD:
            if (isdigit(key) && password_count < PASSWORD_LENGTH) {
                password[password_count++] = key;
                password[password_count] = '\0';
                printf("Contraseña: ");
                for (int i = 0; i < password_count; i++) {
                    printf("*");
                }
                printf("\n");
            } else if (key == '#' && password_count > 0) {
                // Procesar autenticación
                current_state = STATE_PROCESSING;
                cancel_timeout();
                
                // Apagar LED amarillo durante procesamiento
                led_send_command(LED_CMD_AMARILLO_OFF, 0);
                
                printf("Procesando autenticación...\n");
                
                // Verificar credenciales
                if (authenticate_user(user_id, password) == AUTH_SUCCESS) {
                    current_state = STATE_ACCESS_GRANTED;
                    signal_acceso_concedido();
                    ssd1306_send_command(DISPLAY_MSG_WELCOME, NULL, 0);
                    printf("Acceso CONCEDIDO para usuario: %s\n", user_id);
                    
                    // Programar reset con timeout task
                    start_granted_timeout();
                } else {
                    current_state = STATE_ACCESS_DENIED;
                    signal_acceso_denegado();
                    ssd1306_send_command(DISPLAY_MSG_INVALID, NULL, 0);
                    printf("Acceso DENEGADO para usuario: %s\n", user_id);
                    
                    // Programar reset con timeout task
                    start_denied_timeout();
                }
            } else if (key == '*') {
                // Cancelar y volver al inicio
                reset_system();
            }
            break;
            
        case STATE_CHANGE_PASSWORD:
            if (key == '*') {
                reset_system();
            } else if (isdigit(key)) {
                // Iniciar ingreso de ID para cambio de contraseña
                current_state = STATE_CHANGE_ENTERING_ID;
                id_count = 0;
                user_id[id_count++] = key;
                user_id[id_count] = '\0';
                
                signal_proceso_iniciado(); // Apagar LED amarillo
                ssd1306_send_command(DISPLAY_MSG_CUSTOM, "ID Usuario:", 0);
                start_timeout();
                
                printf("Cambio contraseña - Ingresando ID: %s\n", user_id);
            }
            break;
            
        case STATE_CHANGE_ENTERING_ID:
            if (isdigit(key) && id_count < ID_LENGTH) {
                user_id[id_count++] = key;
                user_id[id_count] = '\0';
                printf("ID para cambio: %s\n", user_id);
            } else if (key == '#' && id_count == ID_LENGTH) {
                // ID completo, pedir contraseña actual
                current_state = STATE_CHANGE_ENTERING_OLD_PASS;
                password_count = 0;
                memset(password, 0, sizeof(password));
                
                signal_esperando_clave(); // LED amarillo titilando
                ssd1306_send_command(DISPLAY_MSG_CUSTOM, "Clave Actual:", 0);
                
                printf("ID confirmado para cambio: %s - Esperando contraseña actual\n", user_id);
            } else if (key == '*') {
                reset_system();
            }
            break;
            
        case STATE_CHANGE_ENTERING_OLD_PASS:
            if (isdigit(key) && password_count < PASSWORD_LENGTH) {
                password[password_count++] = key;
                password[password_count] = '\0';
                printf("Contraseña actual: ");
                for (int i = 0; i < password_count; i++) {
                    printf("*");
                }
                printf("\n");
            } else if (key == '#' && password_count == PASSWORD_LENGTH) {
                // Verificar contraseña actual antes de permitir cambio
                if (authenticate_user(user_id, password) == AUTH_SUCCESS) {
                    current_state = STATE_CHANGE_ENTERING_NEW_PASS;
                    new_password_count = 0;
                    memset(new_password, 0, sizeof(new_password));
                    
                    ssd1306_send_command(DISPLAY_MSG_CUSTOM, "Nueva Clave:", 0);
                    printf("Contraseña actual correcta - Ingrese nueva contraseña\n");
                } else {
                    // Contraseña actual incorrecta
                    current_state = STATE_ACCESS_DENIED;
                    signal_acceso_denegado();
                    ssd1306_send_command(DISPLAY_MSG_CUSTOM, "Clave Incorrecta", 0);
                    printf("Contraseña actual incorrecta - Cambio cancelado\n");
                    start_denied_timeout();
                }
            } else if (key == '*') {
                reset_system();
            }
            break;
            
        case STATE_CHANGE_ENTERING_NEW_PASS:
            if (isdigit(key) && new_password_count < PASSWORD_LENGTH) {
                new_password[new_password_count++] = key;
                new_password[new_password_count] = '\0';
                printf("Nueva contraseña: ");
                for (int i = 0; i < new_password_count; i++) {
                    printf("*");
                }
                printf("\n");
            } else if (key == '#' && new_password_count == PASSWORD_LENGTH) {
                // Nueva contraseña completa, procesar cambio directamente
                current_state = STATE_CHANGE_PROCESSING;
                cancel_timeout();
                
                led_send_command(LED_CMD_AMARILLO_OFF, 0);
                
                printf("Procesando cambio de contraseña...\n");
                
                // Realizar cambio de contraseña directamente
                if (change_user_password(user_id, password, new_password)) {
                    current_state = STATE_ACCESS_GRANTED;
                    signal_acceso_concedido();
                    ssd1306_send_command(DISPLAY_MSG_CUSTOM, "Clave Cambiada", 0);
                    printf("Contraseña cambiada exitosamente para usuario: %s\n", user_id);
                    start_granted_timeout();
                } else {
                    current_state = STATE_ACCESS_DENIED;
                    signal_acceso_denegado();
                    ssd1306_send_command(DISPLAY_MSG_CUSTOM, "Error Cambio", 0);
                    printf("Error al cambiar contraseña para usuario: %s\n", user_id);
                    start_denied_timeout();
                }
            } else if (key == '*') {
                reset_system();
            }
            break;
            
        default:
            // En estados de procesamiento, ignorar teclas
            break;
    }
}

/**
 * @brief Inicializa el sistema de control de acceso
 */
bool access_control_init(void) {
    // Inicializar variables
    current_state = STATE_IDLE;
    id_count = 0;
    password_count = 0;
    new_password_count = 0;
    memset(user_id, 0, sizeof(user_id));
    memset(password, 0, sizeof(password));
    memset(new_password, 0, sizeof(new_password));
    timeout_task_handle = NULL;
    
    // Crear cola para eventos
    access_control_queue = xQueueCreate(5, sizeof(access_event_t));
    if (access_control_queue == NULL) {
        return false;
    }
    
    // Señalizar sistema listo
    signal_sistema_listo();
    
    printf("Sistema de control de acceso inicializado\n");
    return true;
}

/**
 * @brief Tarea de FreeRTOS para el control de acceso
 */
void access_control_task(void *pvParameters) {
    access_event_t event;
    keypad_event_t keypad_event;
    
    while (1) {
        // Verificar eventos del teclado
        if (keypad_get_event(&keypad_event, 100)) {
            process_key_input(keypad_event.key);
        }
        
        // Verificar eventos del sistema de control de acceso
        if (xQueueReceive(access_control_queue, &event, 0) == pdTRUE) {
            switch (event.type) {
                case ACCESS_EVENT_TIMEOUT:
                    printf("Timeout del sistema\n");
                    // LED rojo por 2 segundos para timeout
                    led_send_command(LED_CMD_ROJO_ON, 2000);
                    ssd1306_send_command(DISPLAY_MSG_CUSTOM, "TIMEOUT", 2000);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    reset_system();
                    break;
                    
                case ACCESS_EVENT_RESET:
                    reset_system();
                    break;
                    
                default:
                    break;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @brief Envía un evento al sistema de control de acceso
 */
bool access_control_send_event(access_event_type_t type, char key) {
    if (access_control_queue == NULL) {
        return false;
    }
    
    access_event_t event = {
        .type = type,
        .key = key,
        .timestamp = xTaskGetTickCount()
    };
    
    return xQueueSend(access_control_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE;
}

/**
 * @brief Obtiene el estado actual del sistema
 */
system_state_t access_control_get_state(void) {
    return current_state;
}
