#include "integration.h"

double integrate_baseline(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    double sum = 0.0;

    for (std::uint64_t k = 0; k < config.N; ++k) {
        const double x = k * h;
        const double x2 = x * x;

        const double fx =
            x - x * x2 / 6.0
              + x * x2 * x2 / 120.0
              - x * x2 * x2 * x2 / 5040.0;

        sum += fx;
    }

    return h * sum;
}