/**
 * @file main_rtos.c
 * @brief Aplicación principal del Sistema de Control de Acceso con FreeRTOS y Display SSD1306
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Sistema de control de acceso completo que utiliza FreeRTOS para manejo concurrente de:
 * - Teclado matricial 4x4
 * - Sistema de LEDs de señalización 
 * - Display SSD1306 I2C para interfaz de usuario
 * - Base de datos de usuarios
 * - Control de acceso con máquina de estados
 * 
 * Hardware requerido:
 * - Raspberry Pi Pico
 * - Teclado matricial 4x4
 * - 3 LEDs (Verde, Rojo, Amarillo)
 * - Display SSD1306 OLED I2C 128x32
 * - Resistencias limitadoras de corriente para LEDs
 * 
 * Conexiones:
 * - Teclado: Filas GP6-GP9, Columnas GP10-GP13
 * - LEDs: Verde GP15, Rojo GP16, Amarillo GP17
 * - Display SSD1306: SDA GP4, SCL GP5
 */

#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOSConfig.h"

#include "keypad.h"
#include "leds.h"
#include "database.h"
#include "access_control.h"
#include "ssd1306_display.h"

/**
 * @brief Función principal del sistema con FreeRTOS
 * 
 * Inicializa todos los módulos del sistema, crea las tareas de FreeRTOS
 * y inicia el scheduler del sistema operativo.
 * 
 * @return int Código de salida (nunca se alcanza en sistemas embebidos)
 */
int main() {
    /**
     * Inicialización de comunicación serie USB
     * Timeout extendido para establecer conexión
     */
    stdio_init_all();
    sleep_ms(3000); // Espera para conexión USB

    // Mostrar información del sistema
    printf("=== SISTEMA DE CONTROL DE ACCESO CON FREERTOS ===\n");
    printf("Raspberry Pi Pico - Teclado Matricial 4x4 + Display SSD1306\n");
    printf("Implementación: FreeRTOS con tareas concurrentes\n");
    printf("Desarrollado para Sistemas Embebidos\n\n");

    printf("Layout del teclado:\n");
    printf("[1] [2] [3] [A]\n");
    printf("[4] [5] [6] [B]\n");
    printf("[7] [8] [9] [C]\n");
    printf("[*] [0] [#] [D]\n\n");
    
    /**
     * Inicialización de módulos del sistema
     * Orden optimizado para el sistema con FreeRTOS
     */
    
    // Inicializar base de datos de usuarios
    database_init();
    printf("Base de datos inicializada\n");
    
    // Inicializar sistema de LEDs
    if (!leds_init()) {
        printf("ERROR: No se pudo inicializar el sistema de LEDs\n");
        return -1;
    }
    printf("Sistema de LEDs inicializado\n");
    
    // Inicializar display SSD1306
    if (!ssd1306_init()) {
        printf("ERROR: No se pudo inicializar el display SSD1306\n");
        return -1;
    }
    printf("Display SSD1306 inicializado\n");
    
    // Inicializar teclado matricial
    if (!keypad_init()) {
        printf("ERROR: No se pudo inicializar el teclado\n");
        return -1;
    }
    printf("Teclado matricial inicializado\n");
    
    // Inicializar sistema de control de acceso
    if (!access_control_init()) {
        printf("ERROR: No se pudo inicializar el control de acceso\n");
        return -1;
    }
    printf("Sistema de control de acceso inicializado\n");
    
    // Mostrar información de usuarios para pruebas
    printf("\n=== USUARIOS REGISTRADOS ===\n");
    printf("ID: 123456, Contraseña: 1234\n");
    printf("ID: 789012, Contraseña: 5678\n");
    printf("ID: 345678, Contraseña: 9012\n");
    printf("ID: 901234, Contraseña: 3456\n");
    printf("ID: 567890, Contraseña: 7890\n");
    printf("Para cambiar contraseña: presione '*' al inicio\n\n");
    
    /**
     * Crear tareas de FreeRTOS
     * 
     * Cada módulo funcional del sistema se ejecuta como una tarea independiente,
     * permitiendo procesamiento concurrente y comunicación através de colas.
     */
    
    // Tarea del teclado matricial (prioridad alta)
    if (xTaskCreate(keypad_task, "Keypad", 512, NULL, 
                    4, NULL) != pdPASS) {
        printf("ERROR: No se pudo crear la tarea del teclado\n");
        return -1;
    }
    printf("Tarea del teclado creada\n");
    
    // Tarea de LEDs (prioridad media)
    if (xTaskCreate(led_task, "LEDs", 256, NULL, 
                    3, NULL) != pdPASS) {
        printf("ERROR: No se pudo crear la tarea de LEDs\n");
        return -1;
    }
    printf("Tarea de LEDs creada\n");
    
    // Tarea del display (prioridad media)
    if (xTaskCreate(display_task, "Display", 1024, NULL, 
                    3, NULL) != pdPASS) {
        printf("ERROR: No se pudo crear la tarea del display\n");
        return -1;
    }
    printf("Tarea del display creada\n");
    
    // Tarea de control de acceso (prioridad más alta)
    if (xTaskCreate(access_control_task, "AccessControl", 512, NULL, 
                    5, NULL) != pdPASS) {
        printf("ERROR: No se pudo crear la tarea de control de acceso\n");
        return -1;
    }
    printf("Tarea de control de acceso creada\n");
    
    printf("\nTodas las tareas creadas exitosamente\n");
    printf("Iniciando FreeRTOS scheduler...\n\n");
    
    /**
     * Iniciar el scheduler de FreeRTOS
     * 
     * Una vez que se inicia el scheduler, el control pasa al kernel de FreeRTOS
     * y las tareas comienzan a ejecutarse según sus prioridades y algoritmos
     * de scheduling configurados.
     * 
     * Esta función nunca retorna en operación normal.
     */
    vTaskStartScheduler();
    
    /**
     * El código siguiente solo se ejecuta si hay un error crítico
     * en el sistema que causa que el scheduler falle
     */
    printf("ERROR CRÍTICO: El scheduler de FreeRTOS falló\n");
    printf("Esto puede indicar memoria insuficiente o configuración incorrecta\n");
    
    // En caso de falla del scheduler, hacer parpadear LED rojo indefinidamente
    gpio_init(LED_ROJO_PIN);
    gpio_set_dir(LED_ROJO_PIN, GPIO_OUT);
    
    while (true) {
        gpio_put(LED_ROJO_PIN, 1);
        sleep_ms(200);
        gpio_put(LED_ROJO_PIN, 0);
        sleep_ms(200);
    }
    
    return -1; // Nunca se alcanza en operación normal
}

/**
 * @brief Hook llamado cuando se agota la memoria del heap
 * 
 * Esta función es llamada por FreeRTOS cuando no hay suficiente memoria
 * disponible para crear tareas, colas, o asignar memoria dinámica.
 */
void vApplicationMallocFailedHook(void) {
    printf("ERROR: Memoria agotada en el heap de FreeRTOS\n");
    
    // Señalizar error con LED rojo parpadeando rápidamente
    gpio_init(LED_ROJO_PIN);
    gpio_set_dir(LED_ROJO_PIN, GPIO_OUT);
    
    while (true) {
        gpio_put(LED_ROJO_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_ROJO_PIN, 0);
        sleep_ms(100);
    }
}

/**
 * @brief Hook llamado cuando se detecta stack overflow en una tarea
 * 
 * @param pxTask Handle de la tarea que causó el overflow
 * @param pcTaskName Nombre de la tarea que causó el overflow
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    printf("ERROR: Stack overflow en la tarea: %s\n", pcTaskName);
    
    // Señalizar error con LED amarillo parpadeando rápidamente
    gpio_init(LED_AMARILLO_PIN);
    gpio_set_dir(LED_AMARILLO_PIN, GPIO_OUT);
    
    while (true) {
        gpio_put(LED_AMARILLO_PIN, 1);
        sleep_ms(150);
        gpio_put(LED_AMARILLO_PIN, 0);
        sleep_ms(150);
    }
}

/**
 * @brief Hook de la tarea idle de FreeRTOS
 * 
 * Esta función es llamada cada vez que no hay tareas listas para ejecutar.
 * Implementa eficiencia energética inteligente basada en el estado del teclado.
 */
void vApplicationIdleHook(void) {
    // Usar __wfi() solo cuando el teclado está en estado IDLE
    if (keypad_is_idle()) {
        __wfi(); // Wait For Interrupt - optimización energética
    }
    // Si el teclado está procesando, mantener CPU activo
}
    