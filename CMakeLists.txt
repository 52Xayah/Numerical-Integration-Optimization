cmake_minimum_required(VERSION 3.20)

project(NumericalIntegration
    VERSION 1.0
    LANGUAGES CXX
)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(integration
    src/main.cpp
    src/baseline.cpp
    src/horner.cpp
    src/unroll.cpp
    src/unroll4.cpp
    src/unroll8.cpp
    src/simd.cpp
    src/fma_simd.cpp
    src/openmp_simd.cpp
)

# AVX2
set_source_files_properties(
    src/simd.cpp
    PROPERTIES COMPILE_OPTIONS "-mavx2;-ffp-contract=off"
)

# AVX2 + FMA
set_source_files_properties(
    src/fma_simd.cpp
    src/openmp_simd.cpp
    PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma;-ffp-contract=off"
)

target_include_directories(integration
    PRIVATE include
)

target_compile_options(integration
    PRIVATE -O2 -Wall -Wextra
)

target_link_libraries(integration
    PRIVATE OpenMP::OpenMP_CXX
)
