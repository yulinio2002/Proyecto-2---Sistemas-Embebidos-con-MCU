# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

set(FREERTOS_KERNEL_PATH ${USERHOME}/.pico-sdk/FreeRTOS-Kernel)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

include(FreeRTOS_Kernel_import.cmake)

project(blink_simple C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(blink_simple
    main_rtos.c
    keypad.c
    leds_rtos.c
    database.c
    access_control_rtos.c
    ssd1306_display.c
)

# pull in common dependencies
target_link_libraries(blink_simple pico_stdlib hardware_gpio hardware_timer hardware_irq hardware_sync hardware_i2c hardware_rtc FreeRTOS-Kernel FreeRTOS-Kernel-Heap4 pico_multicore)

target_compile_definitions(blink_simple PRIVATE
    PICO_USE_IRQ=1
)

target_compile_options(blink_simple PRIVATE
    -Os
    -Wall
)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(blink_simple)

pico_enable_stdio_usb(blink_simple 1)
pico_enable_stdio_uart(blink_simple 0)
# call pico_set_program_url to set path to example on github, so users can find the source for an example via picotool
