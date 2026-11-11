
/*#include "integration.h"

double integrate_unroll(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    double sum0 = 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;

    std::uint64_t k = 0;

    // 每轮处理4个采样点
    for (; config.N - k >= 4; k += 4) {
        const double x0 = k * h;
        const double x1 = (k + 1) * h;
        const double x2 = (k + 2) * h;
        const double x3 = (k + 3) * h;

        const double xx0 = x0 * x0;
        const double xx1 = x1 * x1;
        const double xx2 = x2 * x2;
        const double xx3 = x3 * x3;

        const double p0 =
            ((-1.0 / 5040.0 * xx0 + 1.0 / 120.0)
              * xx0 - 1.0 / 6.0) * xx0 + 1.0;

        const double p1 =
            ((-1.0 / 5040.0 * xx1 + 1.0 / 120.0)
              * xx1 - 1.0 / 6.0) * xx1 + 1.0;

        const double p2 =
            ((-1.0 / 5040.0 * xx2 + 1.0 / 120.0)
              * xx2 - 1.0 / 6.0) * xx2 + 1.0;

        const double p3 =
            ((-1.0 / 5040.0 * xx3 + 1.0 / 120.0)
              * xx3 - 1.0 / 6.0) * xx3 + 1.0;

        sum0 += x0 * p0;
        sum1 += x1 * p1;
        sum2 += x2 * p2;
        sum3 += x3 * p3;
    }

    // 处理剩余不足4个的采样点
    double sum = sum0 + sum1 + sum2 + sum3;

    for (; k < config.N; ++k) {
        const double x = k * h;
        const double x2 = x * x;

        const double p =
            ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
              * x2 - 1.0 / 6.0)
              * x2 + 1.0;

        sum += x * p;
    }

    return h * sum;
}*/

/*#include "integration.h"

double integrate_unroll(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    double sum0 = 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;
    double sum4 = 0.0;
    double sum5 = 0.0;
    double sum6 = 0.0;
    double sum7 = 0.0;

    std::uint64_t k = 0;

    // 每轮处理8个采样点
    for (; config.N - k >= 8; k += 8) {
        const double x0 = (k + 0) * h;
        const double x1 = (k + 1) * h;
        const double x2 = (k + 2) * h;
        const double x3 = (k + 3) * h;
        const double x4 = (k + 4) * h;
        const double x5 = (k + 5) * h;
        const double x6 = (k + 6) * h;
        const double x7 = (k + 7) * h;

        const double xx0 = x0 * x0;
        const double xx1 = x1 * x1;
        const double xx2 = x2 * x2;
        const double xx3 = x3 * x3;
        const double xx4 = x4 * x4;
        const double xx5 = x5 * x5;
        const double xx6 = x6 * x6;
        const double xx7 = x7 * x7;

        const double p0 =
            ((-1.0 / 5040.0 * xx0 + 1.0 / 120.0)
             * xx0 - 1.0 / 6.0) * xx0 + 1.0;

        const double p1 =
            ((-1.0 / 5040.0 * xx1 + 1.0 / 120.0)
             * xx1 - 1.0 / 6.0) * xx1 + 1.0;

        const double p2 =
            ((-1.0 / 5040.0 * xx2 + 1.0 / 120.0)
             * xx2 - 1.0 / 6.0) * xx2 + 1.0;

        const double p3 =
            ((-1.0 / 5040.0 * xx3 + 1.0 / 120.0)
             * xx3 - 1.0 / 6.0) * xx3 + 1.0;

        const double p4 =
            ((-1.0 / 5040.0 * xx4 + 1.0 / 120.0)
             * xx4 - 1.0 / 6.0) * xx4 + 1.0;

        const double p5 =
            ((-1.0 / 5040.0 * xx5 + 1.0 / 120.0)
             * xx5 - 1.0 / 6.0) * xx5 + 1.0;

        const double p6 =
            ((-1.0 / 5040.0 * xx6 + 1.0 / 120.0)
             * xx6 - 1.0 / 6.0) * xx6 + 1.0;

        const double p7 =
            ((-1.0 / 5040.0 * xx7 + 1.0 / 120.0)
             * xx7 - 1.0 / 6.0) * xx7 + 1.0;

        sum0 += x0 * p0;
        sum1 += x1 * p1;
        sum2 += x2 * p2;
        sum3 += x3 * p3;
        sum4 += x4 * p4;
        sum5 += x5 * p5;
        sum6 += x6 * p6;
        sum7 += x7 * p7;
    }

    double sum =
        sum0 + sum1 + sum2 + sum3 +
        sum4 + sum5 + sum6 + sum7;

    // 处理剩余采样点
    for (; k < config.N; ++k) {
        const double x = k * h;
        const double x2 = x * x;

        const double p =
            ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
             * x2 - 1.0 / 6.0)
             * x2 + 1.0;

        sum += x * p;
    }

    return h * sum;
}*/
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

double integrate_unroll(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    // 16个独立累加器
    double sum0  = 0.0;
    double sum1  = 0.0;
    double sum2  = 0.0;
    double sum3  = 0.0;
    double sum4  = 0.0;
    double sum5  = 0.0;
    double sum6  = 0.0;
    double sum7  = 0.0;
    double sum8  = 0.0;
    double sum9  = 0.0;
    double sum10 = 0.0;
    double sum11 = 0.0;
    double sum12 = 0.0;
    double sum13 = 0.0;
    double sum14 = 0.0;
    double sum15 = 0.0;

    std::uint64_t k = 0;

    // 每轮处理16个采样点
    for (; config.N - k >= 16; k += 16) {
        sum0  += horner_value((k + 0)  * h);
        sum1  += horner_value((k + 1)  * h);
        sum2  += horner_value((k + 2)  * h);
        sum3  += horner_value((k + 3)  * h);
        sum4  += horner_value((k + 4)  * h);
        sum5  += horner_value((k + 5)  * h);
        sum6  += horner_value((k + 6)  * h);
        sum7  += horner_value((k + 7)  * h);
        sum8  += horner_value((k + 8)  * h);
        sum9  += horner_value((k + 9)  * h);
        sum10 += horner_value((k + 10) * h);
        sum11 += horner_value((k + 11) * h);
        sum12 += horner_value((k + 12) * h);
        sum13 += horner_value((k + 13) * h);
        sum14 += horner_value((k + 14) * h);
        sum15 += horner_value((k + 15) * h);
    }

    // 合并16个累加器
    double sum =
        sum0 + sum1 + sum2 + sum3 +
        sum4 + sum5 + sum6 + sum7 +
        sum8 + sum9 + sum10 + sum11 +
        sum12 + sum13 + sum14 + sum15;

    // 处理剩余采样点
    for (; k < config.N; ++k) {
        sum += horner_value(k * h);
    }

    return h * sum;
}
