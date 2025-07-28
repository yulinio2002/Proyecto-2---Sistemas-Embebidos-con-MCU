/**
 * @file ssd1306_display.c
 * @brief Implementación del driver para display SSD1306 I2C
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Implementación del driver del display SSD1306 OLED I2C adaptado para
 * el sistema de control de acceso con FreeRTOS.
 */

#include "ssd1306_display.h"
#include "ssd1306_font.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Configuración del display */
#define SSD1306_HEIGHT              32
#define SSD1306_WIDTH               128
#define SSD1306_I2C_ADDR            0x3C
#define SSD1306_I2C_CLK             400

/* Comandos del SSD1306 */
#define SSD1306_SET_MEM_MODE        0x20
#define SSD1306_SET_COL_ADDR        0x21
#define SSD1306_SET_PAGE_ADDR       0x22
#define SSD1306_SET_DISP_START_LINE 0x40
#define SSD1306_SET_CONTRAST        0x81
#define SSD1306_SET_CHARGE_PUMP     0x8D
#define SSD1306_SET_SEG_REMAP       0xA0
#define SSD1306_SET_ENTIRE_ON       0xA4
#define SSD1306_SET_NORM_DISP       0xA6
#define SSD1306_SET_INV_DISP        0xA7
#define SSD1306_SET_MUX_RATIO       0xA8
#define SSD1306_SET_DISP            0xAE
#define SSD1306_SET_COM_OUT_DIR     0xC0
#define SSD1306_SET_DISP_OFFSET     0xD3
#define SSD1306_SET_DISP_CLK_DIV    0xD5
#define SSD1306_SET_PRECHARGE       0xD9
#define SSD1306_SET_COM_PIN_CFG     0xDA
#define SSD1306_SET_VCOM_DESEL      0xDB
#define SSD1306_SET_SCROLL          0x2E

#define SSD1306_PAGE_HEIGHT         8
#define SSD1306_NUM_PAGES           (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN             (SSD1306_NUM_PAGES * SSD1306_WIDTH)

/* Variables globales */
static uint8_t display_buffer[SSD1306_BUF_LEN];
static QueueHandle_t display_queue;

/**
 * @brief Obtiene la fecha y hora actual formateada
 * 
 * @param date_str Buffer para almacenar la fecha (DD/MM/YY)
 * @param time_str Buffer para almacenar la hora (HH:MM:SS)
 */
static void get_current_datetime(char* date_str, char* time_str) {
    // Usar tiempo simulado basado en ticks de FreeRTOS
    uint32_t ticks = xTaskGetTickCount();
    uint32_t seconds = (ticks / 1000) % 86400; // Segundos en el día
    
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    // Fecha simulada (27 de Julio de 2025)
    strcpy(date_str, "27/07/25");
    
    // Hora basada en ticks del sistema
    snprintf(time_str, 12, "%02lu:%02lu:%02lu", hours, minutes, secs);
}

/**
 * @brief Envía un comando al display SSD1306
 */
static void ssd1306_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
}

/**
 * @brief Envía una lista de comandos al display
 */
static void ssd1306_send_cmd_list(uint8_t *buf, int num) {
    for (int i = 0; i < num; i++) {
        ssd1306_send_cmd(buf[i]);
    }
}

/**
 * @brief Envía el buffer de datos al display
 */
static void ssd1306_send_buf(uint8_t buf[], int buflen) {
    uint8_t *temp_buf = malloc(buflen + 1);
    temp_buf[0] = 0x40;
    memcpy(temp_buf + 1, buf, buflen);
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp_buf, buflen + 1, false);
    free(temp_buf);
}

/**
 * @brief Renderiza el buffer completo en el display
 */
static void ssd1306_render(void) {
    uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR, 0, SSD1306_WIDTH - 1,
        SSD1306_SET_PAGE_ADDR, 0, SSD1306_NUM_PAGES - 1
    };
    ssd1306_send_cmd_list(cmds, sizeof(cmds));
    ssd1306_send_buf(display_buffer, SSD1306_BUF_LEN);
}

/**
 * @brief Obtiene el índice de fuente para un carácter
 */
static inline int get_font_index(uint8_t ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 1;
    } else if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 1;
    } else if (ch >= '0' && ch <= '9') {
        return ch - '0' + 27;
    } else {
        return 0; // Espacio
    }
}

/**
 * @brief Escribe un carácter en el buffer
 */
static void write_char(int16_t x, int16_t y, uint8_t ch) {
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;

    y = y / 8;
    ch = toupper(ch);
    int idx = get_font_index(ch);
    int fb_idx = y * 128 + x;

    for (int i = 0; i < 8; i++) {
        display_buffer[fb_idx++] = font[idx * 8 + i];
    }
}

/**
 * @brief Escribe una cadena en el buffer
 */
static void write_string(int16_t x, int16_t y, const char *str) {
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;

    while (*str && x <= SSD1306_WIDTH - 8) {
        write_char(x, y, *str++);
        x += 8;
    }
}

/**
 * @brief Inicializa el display SSD1306 I2C
 */
bool ssd1306_init(void) {
    // Configurar I2C
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Secuencia de inicialización del display
    uint8_t cmds[] = {
        SSD1306_SET_DISP,               // Display off
        SSD1306_SET_MEM_MODE, 0x00,     // Horizontal addressing mode
        SSD1306_SET_DISP_START_LINE,    // Start line 0
        SSD1306_SET_SEG_REMAP | 0x01,   // Segment re-map
        SSD1306_SET_MUX_RATIO, SSD1306_HEIGHT - 1,
        SSD1306_SET_COM_OUT_DIR | 0x08, // COM scan direction
        SSD1306_SET_DISP_OFFSET, 0x00,  // No offset
        SSD1306_SET_COM_PIN_CFG, 0x02,  // COM pins configuration
        SSD1306_SET_DISP_CLK_DIV, 0x80, // Clock divide ratio
        SSD1306_SET_PRECHARGE, 0xF1,    // Pre-charge period
        SSD1306_SET_VCOM_DESEL, 0x30,   // VCOMH deselect level
        SSD1306_SET_CONTRAST, 0xFF,     // Contrast
        SSD1306_SET_ENTIRE_ON,          // Follow RAM content
        SSD1306_SET_NORM_DISP,          // Normal display
        SSD1306_SET_CHARGE_PUMP, 0x14,  // Charge pump
        SSD1306_SET_SCROLL | 0x00,      // Deactivate scroll
        SSD1306_SET_DISP | 0x01         // Display on
    };

    ssd1306_send_cmd_list(cmds, sizeof(cmds));

    // Crear cola para comandos del display
    display_queue = xQueueCreate(5, sizeof(display_command_t));
    if (display_queue == NULL) {
        return false;
    }

    // Limpiar display inicial
    ssd1306_clear();
    
    return true;
}

/**
 * @brief Limpia completamente el display
 */
void ssd1306_clear(void) {
    memset(display_buffer, 0, SSD1306_BUF_LEN);
    ssd1306_render();
}

/**
 * @brief Muestra un mensaje en el display
 */
void ssd1306_show_message(display_message_type_t type, const char* custom_message) {
    ssd1306_clear();
    
    switch (type) {
        case DISPLAY_MSG_STANDBY: {
            // Mostrar fecha y hora en tiempo real
            char date_str[12];
            char time_str[12];
            get_current_datetime(date_str, time_str);
            
            write_string(20, 0, "SISTEMA LISTO");
            write_string(32, 12, date_str);  // Fecha: DD/MM/YY
            write_string(32, 24, time_str);  // Hora: HH:MM:SS
            break;
        }
            
        case DISPLAY_MSG_ENTER_ID:
            write_string(20, 8, "INGRESE SU ID");
            break;
            
        case DISPLAY_MSG_ENTER_PASSWORD:
            write_string(8, 4, "INGRESE SU");
            write_string(20, 16, "CONTRASENA");
            break;
            
        case DISPLAY_MSG_WELCOME:
            write_string(30, 8, "BIENVENIDO");
            break;
            
        case DISPLAY_MSG_INVALID:
            write_string(8, 0, "USUARIO O");
            write_string(8, 8, "CONTRASENA");
            write_string(20, 16, "INVALIDOS");
            break;
            
        case DISPLAY_MSG_CHANGE_USER:
            write_string(16, 0, "CAMBIAR USUARIO");
            write_string(8, 16, "NUEVA CONTRASENA");
            break;
            
        case DISPLAY_MSG_CUSTOM:
            if (custom_message) {
                write_string(8, 8, custom_message);
            }
            break;
    }
    
    ssd1306_render();
}

/**
 * @brief Actualiza la fecha y hora en el display
 */
void ssd1306_update_datetime(void) {
    // Esta función ahora se llama automáticamente desde la tarea
    // Solo actualizar si estamos en modo standby
    ssd1306_show_message(DISPLAY_MSG_STANDBY, NULL);
}

/**
 * @brief Tarea de FreeRTOS para manejar el display
 */
void display_task(void *pvParameters) {
    display_command_t cmd;
    TickType_t last_datetime_update = xTaskGetTickCount();
    bool in_standby_mode = true;
    
    printf("Tarea del display iniciada\n");
    
    // Mostrar mensaje inicial
    ssd1306_show_message(DISPLAY_MSG_STANDBY, NULL);
    
    while (1) {
        // Verificar si hay comandos en la cola (con timeout de 1 segundo)
        if (xQueueReceive(display_queue, &cmd, pdMS_TO_TICKS(1000)) == pdTRUE) {
            // Procesar comando del display
            ssd1306_show_message(cmd.type, cmd.custom_message);
            
            // Determinar si estamos en modo standby
            in_standby_mode = (cmd.type == DISPLAY_MSG_STANDBY);
            
            // Si el mensaje tiene tiempo limitado, esperar y volver a standby
            if (cmd.display_time_ms > 0) {
                vTaskDelay(pdMS_TO_TICKS(cmd.display_time_ms));
                ssd1306_show_message(DISPLAY_MSG_STANDBY, NULL);
                in_standby_mode = true;
            }
        } else {
            // Timeout - actualizar fecha/hora cada segundo solo en standby
            if (in_standby_mode && 
                (xTaskGetTickCount() - last_datetime_update >= pdMS_TO_TICKS(1000))) {
                ssd1306_show_message(DISPLAY_MSG_STANDBY, NULL);
                last_datetime_update = xTaskGetTickCount();
            }
        }
    }
}

/**
 * @brief Envía un comando al display desde otras tareas
 */
bool ssd1306_send_command(display_message_type_t type, const char* custom_message, uint32_t display_time_ms) {
    if (display_queue == NULL) {
        return false;
    }
    
    display_command_t cmd = {
        .type = type,
        .display_time_ms = display_time_ms
    };
    
    if (custom_message) {
        strncpy(cmd.custom_message, custom_message, sizeof(cmd.custom_message) - 1);
        cmd.custom_message[sizeof(cmd.custom_message) - 1] = '\0';
    }
    
    return xQueueSend(display_queue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE;
}
