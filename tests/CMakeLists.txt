# Test build config
add_executable(test_cpu
    test_cpu.cpp
    ../src/cpu.cpp
    ../src/memory.cpp
    ../src/instructions.cpp
)

target_include_directories(test_cpu PRIVATE ../include)

# Link (no external libs needed)