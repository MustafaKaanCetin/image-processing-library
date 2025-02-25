cmake_minimum_required(VERSION 3.30)
project(ImageProcessing)

set(CMAKE_CXX_STANDARD 20)

find_package(JPEG REQUIRED)

add_executable(ImageProcessing main.cpp
        PreProcessing.cpp
        PreProcessing.h)

target_link_libraries(ImageProcessing JPEG::JPEG)