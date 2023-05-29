cmake_minimum_required(VERSION 3.25)
project(KonfReader)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(KonfReader
        main.cpp
#        test.cpp
        gtest_lite.h
        memtrace.cpp
        memtrace.h
        konf_hash.hpp
#        exceptions.h
        menu.h
        menu.cpp)

if (MEMTRACE)
    add_definitions(-DMEMTRACE=1)
endif(MEMTRACE)