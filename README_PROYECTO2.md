# Sistema de Control de Acceso con FreeRTOS y Display SSD1306

## Descripción del Proyecto

Este proyecto implementa un sistema de control de acceso completo utilizando FreeRTOS como sistema operativo en tiempo real. El sistema migra la funcionalidad del Proyecto 1 a una arquitectura basada en tareas concurrentes y agrega un display SSD1306 I2C como interfaz de usuario.

## Características del Sistema

### Hardware Requerido
- **Microcontrolador**: Raspberry Pi Pico
- **Teclado**: Matricial 4x4
- **LEDs**: 3 LEDs de señalización (Verde, Rojo, Amarillo)
- **Display**: SSD1306 OLED I2C 128x32 píxeles
- **Resistencias**: Limitadoras de corriente para LEDs

### Conexiones de Hardware

#### Teclado Matricial 4x4
- **Filas**: GP6, GP7, GP8, GP9
- **Columnas**: GP10, GP11, GP12, GP13

#### LEDs de Señalización
- **LED Verde** (Acceso Concedido): GP15
- **LED Rojo** (Acceso Denegado): GP16
- **LED Amarillo** (Estado del Sistema): GP17

#### Display SSD1306 I2C
- **SDA**: GP4 (PICO_DEFAULT_I2C_SDA_PIN)
- **SCL**: GP5 (PICO_DEFAULT_I2C_SCL_PIN)
- **VCC**: 3.3V
- **GND**: GND

## Arquitectura del Sistema

### Tareas de FreeRTOS

El sistema está dividido en 4 tareas principales que se ejecutan concurrentemente:

1. **Tarea del Teclado** (`keypad_task`)
   - **Prioridad**: 4 (Alta)
   - **Stack**: 512 bytes
   - **Función**: Escanea continuamente el teclado matricial y envía eventos a través de colas

2. **Tarea de LEDs** (`led_task`)
   - **Prioridad**: 3 (Media)
   - **Stack**: 256 bytes
   - **Función**: Maneja todos los patrones de LEDs incluyendo parpadeo automático

3. **Tarea del Display** (`display_task`)
   - **Prioridad**: 3 (Media)
   - **Stack**: 1024 bytes
   - **Función**: Actualiza el display con mensajes del sistema y fecha/hora

4. **Tarea de Control de Acceso** (`access_control_task`)
   - **Prioridad**: 5 (Más Alta)
   - **Stack**: 512 bytes
   - **Función**: Implementa la máquina de estados principal del sistema

### Comunicación Entre Tareas

Las tareas se comunican utilizando **colas de FreeRTOS**:
- **Cola del Teclado**: Envía eventos de teclas presionadas
- **Cola de LEDs**: Recibe comandos para controlar los LEDs
- **Cola del Display**: Recibe comandos para actualizar la pantalla
- **Cola de Control de Acceso**: Maneja eventos del sistema como timeouts

## Funcionalidad del Display SSD1306

### Mensajes del Sistema

El display muestra diferentes mensajes según el estado del sistema:

1. **Estado de Espera**: 
   - Muestra "SISTEMA ACCESO" y fecha/hora actualizada cada segundo

2. **Ingreso de ID**: 
   - Muestra "INGRESE SU ID"

3. **Ingreso de Contraseña**: 
   - Muestra "INGRESE SU CONTRASENA"

4. **Acceso Concedido**: 
   - Muestra "BIENVENIDO" por 5 segundos

5. **Acceso Denegado**: 
   - Muestra "USUARIO O CONTRASENA INVALIDOS" por 3 segundos

6. **Cambio de Usuario**: 
   - Muestra mensajes específicos para el proceso de cambio de contraseña

## Señalización LED

### Patrones de LEDs

- **LED Amarillo Encendido**: Sistema en espera, listo para recibir entrada
- **LED Amarillo Apagado**: Usuario ingresando ID
- **LED Amarillo Parpadeando**: Usuario ingresando contraseña
- **LED Verde 5 segundos**: Acceso concedido
- **LED Rojo 2 segundos**: Acceso denegado

## Base de Datos de Usuarios

### Usuarios Predeterminados

El sistema incluye 5 usuarios de prueba:

| ID     | Contraseña |
|--------|------------|
| 123456 | 1234       |
| 789012 | 5678       |
| 345678 | 9012       |
| 901234 | 3456       |
| 567890 | 7890       |

### Funcionalidades de Seguridad

- **Bloqueo por Intentos Fallidos**: 3 intentos máximos antes de bloqueo
- **Timeout del Sistema**: 10 segundos para completar el proceso
- **Cambio de Contraseña**: Presionar '*' al inicio para activar modo de cambio

## Compilación y Uso

### Requisitos de Software

- **SDK**: Raspberry Pi Pico SDK 2.1.1
- **FreeRTOS**: Kernel incluido en el SDK
- **Toolchain**: ARM GCC 14_2_Rel1
- **Build System**: CMake + Ninja

### Comandos de Compilación

```bash
# Compilar el proyecto
ninja -C build

# O usar la tarea de VS Code
Ctrl+Shift+P -> "Tasks: Run Task" -> "Compile Project"
```

### Archivos Principales

- **`main_rtos.c`**: Función principal y configuración de tareas
- **`keypad_rtos.c`**: Controlador del teclado para FreeRTOS
- **`leds_rtos.c`**: Controlador de LEDs para FreeRTOS
- **`access_control_rtos.c`**: Lógica de control de acceso para FreeRTOS
- **`ssd1306_display.c`**: Driver del display SSD1306
- **`FreeRTOSConfig.h`**: Configuración del sistema operativo
- **`database.c`**: Base de datos de usuarios (sin cambios)

## Uso del Sistema

### Proceso de Autenticación Normal

1. **Estado Inicial**: Display muestra fecha/hora, LED amarillo encendido
2. **Ingreso de ID**: Presionar dígitos (0-9), luego '#' para confirmar
3. **Ingreso de Contraseña**: Presionar dígitos (0-9), luego '#' para confirmar
4. **Resultado**: 
   - **Éxito**: LED verde 5 seg, display "BIENVENIDO"
   - **Fallo**: LED rojo 2 seg, display "USUARIO O CONTRASENA INVALIDOS"

### Cambio de Contraseña

1. Presionar '*' en estado inicial
2. El sistema entra en modo de cambio de contraseña
3. Seguir las instrucciones en el display

### Cancelación

- Presionar '*' en cualquier momento durante el proceso para cancelar y volver al estado inicial

## Características Técnicas de FreeRTOS

### Configuración del Kernel

- **Frecuencia del Tick**: 1000 Hz (1ms por tick)
- **Heap Size**: 128 KB
- **Algoritmo de Heap**: heap_4 (coalescencia automática)
- **Scheduler**: Preemptivo con time slicing
- **Prioridades**: 5 niveles (0-4)

### Optimizaciones

- **Memoria**: Stacks optimizados por tarea
- **CPU**: Prioridades balanceadas para respuesta rápida
- **Energía**: Uso del hook idle para optimización futura

## Manejo de Errores

### Hooks de FreeRTOS

El sistema incluye hooks para manejo de errores críticos:

- **`vApplicationMallocFailedHook()`**: Memoria agotada
- **`vApplicationStackOverflowHook()`**: Desbordamiento de stack
- **`vApplicationIdleHook()`**: Tarea idle (optimización energética)

### Señalización de Errores

- **LED Rojo Parpadeando Rápido**: Error de memoria
- **LED Amarillo Parpadeando Rápido**: Desbordamiento de stack
- **Mensajes por Puerto Serie**: Información detallada de errores

## Futuras Mejoras

1. **Comunicación UART/SPI**: Para sistemas distribuidos
2. **Almacenamiento Flash**: Persistencia de usuarios y configuración
3. **Red WiFi**: Acceso remoto y logging
4. **Sensores Adicionales**: Detección de presencia, temperatura
5. **Interfaz Web**: Configuración y monitoreo remoto

## Autor

**Sistema de Control de Acceso**  
Desarrollado para el curso de Sistemas Embebidos  
Fecha: 2025

---

*Este proyecto demuestra el uso efectivo de FreeRTOS para sistemas embebidos complejos, implementando comunicación entre tareas, manejo de recursos compartidos y interfaces de usuario avanzadas.*
