cmake_minimum_required(VERSION 3.28)
project(PhyLinkSim C)

set(CMAKE_C_STANDARD 11)

add_executable(PhyLinkSim main.c
        link_sim.h
        link_sim.c
        waveform.h
        waveform.c
        channel.h
        channel.c)

target_link_libraries(PhyLinkSim m)
