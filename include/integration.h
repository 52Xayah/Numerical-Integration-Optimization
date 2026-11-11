#pragma once

#include <cstdint>

struct IntegrationConfig {
    std::uint64_t N = 1'000'000'000ULL;
    double pi = 3.14159265358979323846;
};

double integrate_baseline(const IntegrationConfig& config);
double integrate_horner(const IntegrationConfig& config);
double integrate_unroll4(const IntegrationConfig& config);
double integrate_unroll8(const IntegrationConfig& config);
double integrate_unroll(const IntegrationConfig& config);
double integrate_simd(const IntegrationConfig& config);
double integrate_simd_fma(const IntegrationConfig& config);
double integrate_openmp_simd(const IntegrationConfig& config);
