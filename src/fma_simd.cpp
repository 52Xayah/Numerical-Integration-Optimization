
#include "integration.h"
#include <immintrin.h>
#include <cstdint>

double integrate_simd_fma(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    // 4 个 double 组成一个 256-bit 向量
    const __m256d vh = _mm256_set1_pd(h);
    const __m256d offsets =
        _mm256_set_pd(3.0, 2.0, 1.0, 0.0);

    // Horner 多项式系数
    const __m256d c1 = _mm256_set1_pd(-1.0 / 5040.0);
    const __m256d c2 = _mm256_set1_pd( 1.0 / 120.0);
    const __m256d c3 = _mm256_set1_pd(-1.0 / 6.0);
    const __m256d c4 = _mm256_set1_pd(1.0);

    __m256d vsum = _mm256_setzero_pd();

    std::uint64_t k = 0;

    for (; config.N - k >= 4; k += 4) {
        const __m256d vk =
            _mm256_set1_pd(static_cast<double>(k));

        const __m256d indices =
            _mm256_add_pd(vk, offsets);

        const __m256d x =
            _mm256_mul_pd(indices, vh);

        const __m256d x2 =
            _mm256_mul_pd(x, x);

        // FMA: a * b + c，融合乘加
        __m256d p = _mm256_fmadd_pd(c1, x2, c2);
        p = _mm256_fmadd_pd(p, x2, c3);
        p = _mm256_fmadd_pd(p, x2, c4);

        const __m256d fx =
            _mm256_mul_pd(x, p);

        vsum = _mm256_add_pd(vsum, fx);
    }

    // 将四个 SIMD 通道归约为一个标量
    alignas(32) double lanes[4];
    _mm256_store_pd(lanes, vsum);

    double sum =
        lanes[0] + lanes[1] + lanes[2] + lanes[3];

    // 处理不能被 4 整除的剩余元素
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
