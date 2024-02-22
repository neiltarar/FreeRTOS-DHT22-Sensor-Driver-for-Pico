cmake_minimum_required(VERSION 3.15)

set(APP_NAME "dht22-pico")

include(pico_sdk_import.cmake)
project(${APP_NAME}
        LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${APP_NAME}
        DHT22-master/DHT22.c
        DHT22-master/main.c
        )

# Enable/disable STDIO via USB and UART
pico_enable_stdio_usb(${APP_NAME} 1)
pico_enable_stdio_uart(${APP_NAME} 0)

target_link_libraries(dht22-pico pico_stdlib)

pico_add_extra_outputs(dht22-pico)