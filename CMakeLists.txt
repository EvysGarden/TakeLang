cmake_minimum_required(VERSION 3.20)
project(Take VERSION 0.0.1)
set(DCMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_MAKEFILE on)

find_package (Python3 COMPONENTS Interpreter Development)

add_executable(take
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/taker.cpp
)

add_custom_command(
    TARGET take
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/main.tl
        ${CMAKE_BINARY_DIR}/main.tl
)

target_link_libraries(take tcc)
target_link_libraries(take ${Python3_LIBRARIES})