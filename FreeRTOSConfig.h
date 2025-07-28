/**
 * @file FreeRTOSConfig.h
 * @brief Configuración de FreeRTOS para Sistema de Control de Acceso
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Configuración optimizada para Raspberry Pi Pico con sistema de control de acceso
 * que incluye teclado matricial, LEDs, base de datos y display SSD1306 I2C.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include <stdint.h>

/* Hardware specifics. */
extern uint32_t SystemCoreClock;

/* Set configUSE_PREEMPTION to 1 to use pre-emptive scheduling.
 * Set configUSE_PREEMPTION to 0 to use co-operative scheduling.
 */
#define configUSE_PREEMPTION                    1

/* Set configUSE_PORT_OPTIMISED_TASK_SELECTION to 1 to enable port optimised 
 * task selection. Leave as 0 to use the generic C task selection algorithm. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Set configUSE_TICKLESS_IDLE to 1 for low power tickless mode or 0 to keep the
 * periodic tick interrupt running. */
#define configUSE_TICKLESS_IDLE                 0

/* configCPU_CLOCK_HZ must be set to the frequency of the clock that drives
 * the peripheral used to generate the kernels periodic tick interrupt. */
#define configCPU_CLOCK_HZ                      125000000

/* configTICK_RATE_HZ sets the frequency of the periodic tick interrupt. */
#define configTICK_RATE_HZ                      1000

/* configMAX_PRIORITIES sets the number of available task priorities. Tasks can
 * be assigned priorities of 0 to (configMAX_PRIORITIES - 1). Zero is the lowest
 * priority. */
#define configMAX_PRIORITIES                    5

/* configMINIMAL_STACK_SIZE defines the size of the stack used by the Idle task.
 * Generally this should not be reduced from the value set in the FreeRTOSConfig.h
 * files provided with the demo applications. */
#define configMINIMAL_STACK_SIZE                256

/* configMAX_TASK_NAME_LEN sets the maximum length for task names including
 * the NULL terminator. */
#define configMAX_TASK_NAME_LEN                 16

/* Set configUSE_16_BIT_TICKS to 1 to use 16-bit tick counts or 0 to use 32-bit
 * tick counts. */
#define configUSE_16_BIT_TICKS                  0

/* Set configIDLE_SHOULD_YIELD to 1 if the Idle task should yield between idle
 * priority tasks, or 0 if it should not. */
#define configIDLE_SHOULD_YIELD                 1

/* Set configUSE_TASK_NOTIFICATIONS to 1 to include task notification functionality
 * in the build, or 0 to exclude task notification functionality from the build. */
#define configUSE_TASK_NOTIFICATIONS            1

/* Set configUSE_MUTEXES to 1 to include mutex functionality in the build, or 0
 * to exclude mutex functionality from the build. */
#define configUSE_MUTEXES                       1

/* Set configUSE_RECURSIVE_MUTEXES to 1 to include recursive mutex functionality
 * in the build, or 0 to exclude recursive mutex functionality from the build. */
#define configUSE_RECURSIVE_MUTEXES             1

/* Set configUSE_COUNTING_SEMAPHORES to 1 to include counting semaphore
 * functionality in the build, or 0 to exclude counting semaphore functionality
 * from the build. */
#define configUSE_COUNTING_SEMAPHORES           1

/* Set configUSE_QUEUE_SETS to 1 to include queue set functionality in the build,
 * or 0 to exclude queue set functionality from the build. */
#define configUSE_QUEUE_SETS                    0

/* Set configUSE_TIME_SLICING to 1 to have the scheduler switch between Ready
 * state tasks of equal priority on every tick interrupt, or 0 to prevent the
 * scheduler switching between Ready state tasks just because a tick interrupt
 * has occurred. */
#define configUSE_TIME_SLICING                  1

/* Set configUSE_NEWLIB_REENTRANT to 1 if newlib reentrant functions should be
 * used, or 0 if not. */
#define configUSE_NEWLIB_REENTRANT              0

/* Sets the length of the queues used by the queue registry. */
#define configQUEUE_REGISTRY_SIZE               10

/* Enable run time stats gathering. */
#define configGENERATE_RUN_TIME_STATS           0

/* Enable run time stats gathering. */
#define configUSE_TRACE_FACILITY                1

/* Enable/disable the built in task stats formatting functions. */
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              1

/* Normal assert() semantics without relying on the provision of an assert.h
 * header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* Hardware description related definitions. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (1UL << (8 - configPRIO_BITS))

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names - or at least those used in the unmodified vector table. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* Heap configuration for access control system with display */
#define configTOTAL_HEAP_SIZE                   (128 * 1024)

/* Queue sizes for inter-task communication */
#define configKEYPAD_QUEUE_SIZE                 10
#define configACCESS_CONTROL_QUEUE_SIZE         5
#define configDISPLAY_QUEUE_SIZE                5

#endif /* FREERTOS_CONFIG_H */
