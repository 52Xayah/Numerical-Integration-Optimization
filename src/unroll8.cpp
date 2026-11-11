#include "integration.h"

namespace {
inline double horner_value(double x) {
    const double x2 = x * x;

    return x * (
        ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
          * x2 - 1.0 / 6.0)
          * x2 + 1.0
    );
}
}

double integrate_unroll8(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    double sums[8] = {};

    std::uint64_t k = 0;

    for (; config.N - k >= 8; k += 8) {
        for (int i = 0; i < 8; ++i) {
            sums[i] += horner_value((k + i) * h);
        }
    }

    double sum = 0.0;

    for (double partial : sums) {
        sum += partial;
    }

    for (; k < config.N; ++k) {
        sum += horner_value(k * h);
    }

    return h * sum;
}