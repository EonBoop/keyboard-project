cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
#include (tusb_config.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(keyboard
main.c
)
project(keyboard C CXX ASM)
target_sources(keyboard PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

pico_add_extra_outputs(keyboard)
target_link_libraries(keyboard PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)
