cmake_minimum_required(VERSION 3.13)

project(mystd
    DESCRIPTION "My educational implementation of C++ STL templates"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(mystd INTERFACE)

target_include_directories(mystd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(mystd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
