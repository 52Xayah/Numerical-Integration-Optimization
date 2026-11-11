
#include "integration.h"

double integrate_horner(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;
    double sum = 0.0;

    for (std::uint64_t k = 0; k < config.N; ++k) {
        const double x = k * h;
        const double x2 = x * x;

        // Horner 法计算多项式
        const double p =
            ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
              * x2 - 1.0 / 6.0)
              * x2 + 1.0;

        const double fx = x * p;
        sum += fx;
    }

    return h * sum;
}
