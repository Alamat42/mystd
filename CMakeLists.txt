cmake_minimum_required(VERSION 3.13)

project(mystd
    DESCRIPTION "My educational implementation of C++ STL templates"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(mystd INTERFACE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(mystd INTERFACE forward_list)
