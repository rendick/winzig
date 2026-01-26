cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

enable_language(C CXX)
project(winzig_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

add_executable(winzig
    src/winzig.cpp
    src/api.cpp
    hw_config.c
)

file(GLOB LUA_SOURCE "lib/lua/*.c")
add_library(lua STATIC ${LUA_SOURCE})
target_include_directories(lua PUBLIC lib/lua)
target_compile_definitions(lua PUBLIC LUA_32BITS)

add_subdirectory(lib/pico-ssd1306)

target_include_directories(winzig PRIVATE 
	${CMAKE_SOURCE_DIR}/lib/pico-ssd1306
)

pico_enable_stdio_usb(winzig 1)
pico_enable_stdio_uart(winzig 1)
pico_add_extra_outputs(winzig)

target_link_libraries(winzig pico_stdlib pico_ssd1306 hardware_i2c lua FatFs_SPI)
