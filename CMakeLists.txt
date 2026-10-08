cmake_minimum_required(VERSION 3.16)
project(kst LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(kst_core STATIC
    structures.cpp
    io.cpp
)
target_include_directories(kst_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_options(kst_core PUBLIC /W4 /permissive-)
else()
    target_compile_options(kst_core PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

option(KST_SANITIZE "Enable AddressSanitizer and UBSan" OFF)
if(KST_SANITIZE AND NOT MSVC)
    target_compile_options(kst_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(kst_core PUBLIC -fsanitize=address,undefined)
endif()

add_executable(main main.cpp)
target_link_libraries(main PRIVATE kst_core)