cmake_minimum_required(VERSION 3.14)
project(SmartPointers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/tests)

# ============================================================
# ТЕСТЫ (собираем только test_smart_ptrs.cpp)
# ============================================================
add_executable(test_runner tests/test_smart_ptrs.cpp)

# ============================================================
# БЕНЧМАРКИ (собираем только benchmarks.cpp)
# ============================================================
add_executable(benchmark_runner tests/benchmarks.cpp)
target_compile_options(benchmark_runner PRIVATE -O2)

# ============================================================
# Custom команды для удобства
# ============================================================
add_custom_target(run-tests
    COMMAND test_runner
    DEPENDS test_runner
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests..."
)

add_custom_target(run-benchmarks
    COMMAND benchmark_runner
    DEPENDS benchmark_runner
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running benchmarks..."
)