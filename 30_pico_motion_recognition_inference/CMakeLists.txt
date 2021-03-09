cmake_minimum_required(VERSION 3.13)

set(MODEL_FOLDER .)
set(EI_SDK_FOLDER edge-impulse-sdk)

include(pico_sdk_import.cmake)

project(pico_motion_detection_project C CXX ASM) 
set(CMAKE_C_STANDARD 11) 
set(CMAKE_CXX_STANDARD 17) 
pico_sdk_init()

add_executable(app 
    source/main.cpp
    source/ei_classifier_porting.cpp
)

include(${MODEL_FOLDER}/edge-impulse-sdk/cmake/utils.cmake)

pico_enable_stdio_usb(app 1)
 
target_link_libraries(app pico_stdlib hardware_adc)

add_subdirectory(${MODEL_FOLDER}/edge-impulse-sdk/cmake/zephyr)

target_include_directories(app PRIVATE
    ${MODEL_FOLDER}
    ${MODEL_FOLDER}/classifer
    ${MODEL_FOLDER}/tflite-model
    ${MODEL_FOLDER}/model-parameters
)

target_include_directories(app PRIVATE
    ${EI_SDK_FOLDER}
    ${EI_SDK_FOLDER}/third_party/ruy
    ${EI_SDK_FOLDER}/third_party/gemmlowp
    ${EI_SDK_FOLDER}/third_party/flatbuffers/include
    ${EI_SDK_FOLDER}/third_party
    ${EI_SDK_FOLDER}/tensorflow
    ${EI_SDK_FOLDER}/dsp
    ${EI_SDK_FOLDER}/classifier
    ${EI_SDK_FOLDER}/anomaly
    ${EI_SDK_FOLDER}/CMSIS/NN/Include
    ${EI_SDK_FOLDER}/CMSIS/DSP/PrivateInclude
    ${EI_SDK_FOLDER}/CMSIS/DSP/Include
    ${EI_SDK_FOLDER}/CMSIS/Core/Include
)

include_directories(${INCLUDES})

# find model source files
RECURSIVE_FIND_FILE(MODEL_FILES "${MODEL_FOLDER}/tflite-model" "*.cpp")
RECURSIVE_FIND_FILE(SOURCE_FILES "${EI_SDK_FOLDER}" "*.cpp")
RECURSIVE_FIND_FILE(CC_FILES "${EI_SDK_FOLDER}" "*.cc")
RECURSIVE_FIND_FILE(S_FILES "${EI_SDK_FOLDER}" "*.s")
RECURSIVE_FIND_FILE(C_FILES "${EI_SDK_FOLDER}" "*.c")
list(APPEND SOURCE_FILES ${S_FILES})
list(APPEND SOURCE_FILES ${C_FILES})
list(APPEND SOURCE_FILES ${CC_FILES})
list(APPEND SOURCE_FILES ${MODEL_FILES})

# add all sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})

pico_add_extra_outputs(app)