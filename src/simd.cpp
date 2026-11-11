
#include "integration.h"
#include <immintrin.h>

double integrate_simd(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    // 四个double组成一个256-bit向量
    const __m256d vh = _mm256_set1_pd(h);

    // 采样点偏移：0、1、2、3
    const __m256d offsets =
        _mm256_set_pd(3.0, 2.0, 1.0, 0.0);

    // Horner多项式系数
    const __m256d c1 = _mm256_set1_pd(-1.0 / 5040.0);
    const __m256d c2 = _mm256_set1_pd( 1.0 / 120.0);
    const __m256d c3 = _mm256_set1_pd(-1.0 / 6.0);
    const __m256d c4 = _mm256_set1_pd(1.0);

    __m256d vsum = _mm256_setzero_pd();

    std::uint64_t k = 0;

    // 每次处理4个采样点
    for (; config.N - k >= 4; k += 4) {
        const __m256d vk = _mm256_set1_pd(
            static_cast<double>(k)
        );

        const __m256d indices =
            _mm256_add_pd(vk, offsets);

        const __m256d x =
            _mm256_mul_pd(indices, vh);

        const __m256d x2 =
            _mm256_mul_pd(x, x);

        // Horner求值：使用独立乘法和加法，不使用FMA
        __m256d p = _mm256_add_pd(
            _mm256_mul_pd(c1, x2), c2
        );

        p = _mm256_add_pd(
            _mm256_mul_pd(p, x2), c3
        );

        p = _mm256_add_pd(
            _mm256_mul_pd(p, x2), c4
        );

        const __m256d fx =
            _mm256_mul_pd(x, p);

        vsum = _mm256_add_pd(vsum, fx);
    }

    // 将四个向量通道取出并合并
    alignas(32) double lanes[4];

    _mm256_store_pd(lanes, vsum);

    double sum =
        lanes[0] + lanes[1] +
        lanes[2] + lanes[3];

    // 处理剩余不足4个的采样点
    for (; k < config.N; ++k) {
        const double x = static_cast<double>(k) * h;
        const double x2 = x * x;

        const double p =
            ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
              * x2 - 1.0 / 6.0)
              * x2 + 1.0;

        sum += x * p;
    }

    return h * sum;
}
