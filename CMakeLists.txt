cmake_minimum_required(VERSION 3.11...3.31)

project(
    a_tour_of_c++
    DESCRIPTION "This is a project description."
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    a_tour_of_c++
    src/a_tour_of_c++.cpp
    src/capitulo_1/capitulo_1.cpp
    src/capitulo_2/capitulo_2.cpp
    src/capitulo_3/capitulo_3.cpp
    src/capitulo_4/capitulo_4.cpp
    src/capitulo_5/capitulo_5.cpp
    src/capitulo_6/capitulo_6.cpp
    src/capitulo_7/capitulo_7.cpp
    src/capitulo_8/capitulo_8.cpp
    src/capitulo_10/capitulo_10.cpp
    src/capitulo_11/capitulo_11.cpp
    src/capitulo_12/capitulo_12.cpp
    src/capitulo_13/capitulo_13.cpp
    src/capitulo_14/capitulo_14.cpp
    src/capitulo_15/capitulo_15.cpp
    src/capitulo_16/capitulo_16.cpp
    src/capitulo_17/capitulo_17.cpp
    src/capitulo_18/capitulo_18.cpp
)

target_include_directories(
    a_tour_of_c++
    PRIVATE
    ~/.local/include
)

target_compile_options(
    a_tour_of_c++
    PRIVATE
    -fdiagnostics-color=always
    -Wall
    -Wextra
    -std=c++23
    -Og
    -march=native
    -fopenmp
)

target_link_directories(
    a_tour_of_c++
    PRIVATE
    ~/.local/lib
)

target_link_libraries(
    a_tour_of_c++
    PRIVATE
)

target_link_options(
    a_tour_of_c++
    PRIVATE
    -fopenmp
)
