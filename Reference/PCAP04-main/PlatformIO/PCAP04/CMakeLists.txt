cmake_minimum_required(VERSION 3.21)
project(PCAP04)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(lib/pcap04IIC/src)

add_executable(PCAP04
        include/PCAP04Functions.h
        include/prog_defines.h
        include/prog_globals.h
        include/prog_types.h
        include/SDFunctions.h
        include/webserverFunctions.h
        lib/pcap04IIC/src/PCAP04IIC.cpp
        lib/pcap04IIC/src/pcap04IIC.h
        lib/pcap04IIC/src/pcap_calibrations.h
        lib/pcap04IIC/src/pcap_configurations.h
        lib/pcap04IIC/src/pcap_defines.h
        lib/pcap04IIC/src/pcap_firmware.h
        lib/pcap04IIC/src/pcap_registers.h
        lib/pcap04IIC/src/pcap_types.h
        src/main.cpp
        src/PCAP04Functions.cpp
        src/SDFunctions.cpp
        src/webserverFunctions.cpp)
