cmake_minimum_required(VERSION 3.20)
project(syslog_perf_lab LANGUAGES CXX)

include(CTest)
option(SYSLOG_BUILD_BENCHMARKS "Build the Google Benchmark executable" ON)

set (CMAKE_CXX_STANDARD 23)
set (CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(library)
add_subdirectory(app)
