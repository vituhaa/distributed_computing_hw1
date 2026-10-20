cmake_minimum_required(VERSION 3.10)
project(HW1 C)
set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(program_1 program_1.c)
add_executable(program_2 program_2.c)

target_link_libraries(program_1 m)
target_link_libraries(program_2 m)

target_include_directories(program_1 PRIVATE ${HEADER_DIR})
target_include_directories(program_2 PRIVATE ${HEADER_DIR})
