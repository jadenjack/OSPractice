cmake_minimum_required(VERSION 3.9)
project(HW2_OS C)

set(CMAKE_C_STANDARD 99)

add_executable(HW2_OS main.c disk.c disk.h fs.c fs.h mount.c)