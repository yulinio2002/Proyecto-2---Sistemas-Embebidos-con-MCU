/**
 * @mainpage Sistema de Control de Acceso con Raspberry Pi Pico
 * 
 * @section intro_sec Introducción
 * 
 * Sistema de control de acceso implementado en C para Raspberry Pi Pico que utiliza
 * un teclado matricial 4x4 para la autenticación de usuarios. El sistema incluye
 * una base de datos de usuarios, manejo de timeouts, señalización LED, y capacidades
 * de cambio de contraseñas.
 * 
 * @section features_sec Características Principales
 * 
 * - **Autenticación de usuarios**: ID de 6 dígitos + contraseña de 4 dígitos
 * - **Base de datos**: Hasta 10 usuarios con bloqueo por intentos fallidos
 * - **Timeout de seguridad**: 10 segundos para completar la autenticación
 * - **Señalización LED**: Estados visuales del sistema
 * - **Cambio de contraseñas**: Mecanismo seguro para actualizar credenciales
 * - **Antirebote por software**: 300ms entre pulsaciones de teclas
 * - **Arquitectura modular**: Separación clara de responsabilidades
 * 
 * @section hardware_sec Configuración de Hardware
 * 
 * @subsection keypad_hw Teclado Matricial 4x4
 * - **Filas (Salidas)**: GP6, GP7, GP8, GP9
 * - **Columnas (Entradas)**: GP10, GP11, GP12, GP13
 * - **Configuración**: Pull-down interno en columnas
 * 
 * @subsection leds_hw Sistema de LEDs
 * - **LED Verde (GP15)**: Acceso concedido (5 segundos)
 * - **LED Rojo (GP16)**: Acceso denegado (2 segundos)  
 * - **LED Amarillo (GP17)**: Estados del sistema
 *   - Encendido permanente: Sistema listo
 *   - Apagado: Ingresando ID
 *   - Parpadeando 0.5Hz: Ingresando contraseña
 * 
 * @section software_sec Arquitectura de Software
 * 
 * @subsection modules_sec Módulos del Sistema
 * 
 * - @ref keypad.h "Keypad Module": Controlador del teclado matricial
 * - @ref leds.h "LEDs Module": Sistema de señalización luminosa
 * - @ref database.h "Database Module": Gestión de usuarios y autenticación
 * - @ref access_control.h "Access Control Module": Lógica principal del sistema
 * 
 * @subsection states_sec Estados del Sistema
 * 
 * El sistema opera como una máquina de estados finitos:
 * 
 * 1. **STATE_IDLE**: Esperando entrada del usuario
 * 2. **STATE_ENTERING_ID**: Capturando ID de 6 dígitos
 * 3. **STATE_ENTERING_PASSWORD**: Capturando contraseña de 4 dígitos
 * 4. **STATE_PROCESSING**: Procesando autenticación
 * 5. **STATE_ACCESS_GRANTED**: Mostrando acceso concedido
 * 6. **STATE_ACCESS_DENIED**: Mostrando acceso denegado
 * 7. **STATE_TIMEOUT**: Timeout del proceso
 * 8. **STATE_CHANGE_PASSWORD**: Modo cambio de contraseña
 * 
 * @section usage_sec Modo de Uso
 * 
 * @subsection normal_auth Autenticación Normal
 * 1. Sistema muestra LED amarillo (listo)
 * 2. Usuario ingresa 6 dígitos de ID
 * 3. LED amarillo se apaga
 * 4. Usuario ingresa 4 dígitos de contraseña
 * 5. LED amarillo parpadea durante ingreso
 * 6. Sistema procesa y muestra resultado:
 *    - Verde 5s: Acceso concedido
 *    - Rojo 2s: Acceso denegado
 * 
 * @subsection password_change Cambio de Contraseña
 * 1. Presionar '*' cuando sistema está listo
 * 2. Ingresar ID de usuario (6 dígitos)
 * 3. Ingresar contraseña actual (4 dígitos)
 * 4. Ingresar nueva contraseña (4 dígitos)
 * 5. Sistema confirma el cambio
 * 
 * @section security_sec Características de Seguridad
 * 
 * - **Bloqueo automático**: 3 intentos fallidos bloquean permanentemente al usuario
 * - **Timeout de sesión**: 10 segundos máximo para completar autenticación
 * - **Ocultación de contraseña**: Se muestran asteriscos en lugar de dígitos
 * - **Sin posibilidad de borrar**: Según especificaciones, no se pueden corregir errores
 * - **Antirebote**: Previene pulsaciones accidentales múltiples
 * 
 * @section technical_sec Detalles Técnicos
 * 
 * @subsection libs_sec Librerías Utilizadas
 * - `hardware/gpio.h`: Control de pines GPIO
 * - `hardware/timer.h`: Manejo de timeouts y temporización
 * - `pico/stdlib.h`: Funciones estándar del SDK
 * 
 * @subsection timing_sec Temporización
 * - **Estabilización GPIO**: 50µs para estabilizar señales
 * - **Antirebote**: 300ms entre pulsaciones
 * - **Timeout autenticación**: 10 segundos
 * - **Parpadeo LED**: 0.5Hz (1s encendido, 1s apagado)
 * - **Bucle principal**: 10ms de pausa para optimización CPU
 * 
 * @section compilation_sec Compilación y Despliegue
 * 
 * El proyecto utiliza CMake con el SDK de Raspberry Pi Pico:
 * 
 * ```
 * mkdir build && cd build
 * cmake ..
 * make
 * ```
 * 
 * O usando las tareas de VS Code:
 * - "Compile Project" para construir
 * - "Run Project" para cargar al Pico
 * 
 * @section testing_sec Usuarios de Prueba
 * 
 * El sistema incluye 5 usuarios predefinidos para pruebas:
 * 
 * | ID     | Contraseña |
 * |--------|------------|
 * | 123456 | 1234       |
 * | 789012 | 5678       |
 * | 345678 | 9012       |
 * | 901234 | 3456       |
 * | 567890 | 7890       |
 * 
 * @section authors_sec Autores
 * 
 * Desarrollado para la práctica de Sistemas Embebidos
 * 
 * @section license_sec Licencia
 * 
 * Proyecto educativo - Sistemas Embebidos 2025
 */
