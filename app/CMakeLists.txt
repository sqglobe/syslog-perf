add_executable(syslog_perf src/main.cpp)
target_link_libraries(syslog_perf PRIVATE syslog_analyzer)
set_target_properties(syslog_perf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(syslog_perf PRIVATE -fno-omit-frame-pointer -ggdb)
endif()
