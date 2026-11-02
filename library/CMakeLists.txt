add_library(syslog_analyzer STATIC src/syslog_analyzer.cpp)
target_include_directories(syslog_analyzer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(BUILD_TESTING)
    add_executable(syslog_tests tests/syslog_tests.cpp)
    target_link_libraries(syslog_tests PRIVATE syslog_analyzer)
    set_target_properties(syslog_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    add_test(NAME syslog_tests COMMAND syslog_tests)
endif()

if(SYSLOG_BUILD_BENCHMARKS)
    include("${PROJECT_SOURCE_DIR}/cmake/benchmark.cmake")

    add_executable(syslog_bench benchmarks/syslog_benchmark.cpp)
    target_link_libraries(syslog_bench PRIVATE syslog_analyzer benchmark::benchmark)
    set_target_properties(syslog_bench PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
    target_compile_definitions(syslog_bench PRIVATE
        SYSLOG_BENCHMARK_INPUT="${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/large.log"
    )
endif()
