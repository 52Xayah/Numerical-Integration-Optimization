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

double integrate_unroll4(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    double sum0 = 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;

    std::uint64_t k = 0;

    for (; config.N - k >= 4; k += 4) {
        sum0 += horner_value((k + 0) * h);
        sum1 += horner_value((k + 1) * h);
        sum2 += horner_value((k + 2) * h);
        sum3 += horner_value((k + 3) * h);
    }

    double sum = sum0 + sum1 + sum2 + sum3;

    for (; k < config.N; ++k) {
        sum += horner_value(k * h);
    }

    return h * sum;
}