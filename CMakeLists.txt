cmake_minimum_required(VERSION 3.25)
project(lowlat_bench LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(LLB_NATIVE "Tune for this machine's CPU (-march=native)" OFF)
option(LLB_WERROR "Treat warnings as errors (CI turns this on)" OFF)

# Google Benchmark, built from source in Release mode so results are not
# skewed by a debug library (Ubuntu's libbenchmark-dev is built that way).
include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_Declare(benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.5
  GIT_SHALLOW TRUE
  SYSTEM)
FetchContent_MakeAvailable(benchmark)

# One executable per topic: bench/<name>.cpp -> build/bench/<name>
function(llb_add_bench name)
  add_executable(${name} bench/${name}.cpp)
  target_link_libraries(${name} PRIVATE benchmark::benchmark)
  target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
  if(LLB_WERROR)
    target_compile_options(${name} PRIVATE -Werror)
  endif()
  if(LLB_NATIVE)
    target_compile_options(${name} PRIVATE -march=native)
  endif()
  set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bench)
endfunction()

llb_add_bench(layout)
llb_add_bench(cache_traversal)
