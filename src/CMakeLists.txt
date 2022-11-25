project(archiver)

cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
add_executable(
        archiver
        archiver.cpp
        compress.cpp
        basic.cpp
        extract.cpp
        exception.cpp
)
