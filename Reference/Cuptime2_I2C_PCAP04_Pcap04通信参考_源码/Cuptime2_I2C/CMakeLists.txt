cmake_minimum_required(VERSION 3.25)
project(Cuptime2_I2C C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Cuptime2_I2C
        I2C.c
        I2C.h
        main.c
        pcap04.c
        pcap04.h
        usart1.c
        usart1.h)
