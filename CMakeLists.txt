cmake_minimum_required(VERSION 3.12)
project(340ass2v2 C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        client.c
        server.c)
include_directories(.)

add_executable(client client.c)

add_executable(server server.c)

