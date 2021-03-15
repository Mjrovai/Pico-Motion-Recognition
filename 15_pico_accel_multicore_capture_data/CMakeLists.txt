cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico-accelerometer-multicore)

pico_sdk_init()

add_executable(app
        main.cpp
)

target_link_libraries(app
        pico_multicore
        pico_stdlib
        hardware_adc
)

pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 0)

pico_add_extra_outputs(app)