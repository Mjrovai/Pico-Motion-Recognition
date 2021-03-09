cmake_minimum_required(VERSION 3.13) 

include(pico_sdk_import.cmake)

project(accelerometer_mma7361l_project C CXX ASM) 
set(CMAKE_C_STANDARD 11) 
set(CMAKE_CXX_STANDARD 17) 
pico_sdk_init()

add_executable(accel_ml 
    accel_mma7361l.c
)

pico_enable_stdio_usb(accel_ml 1)

pico_add_extra_outputs(accel_ml) 
target_link_libraries(accel_ml pico_stdlib hardware_adc)
