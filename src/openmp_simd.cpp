
#include "integration.h"

#include <immintrin.h>
#include <cstdint>

double integrate_openmp_simd(const IntegrationConfig& config) {
    const double h = (config.pi / 2.0) / config.N;

    const __m256d vh = _mm256_set1_pd(h);
    const __m256d offsets =
        _mm256_set_pd(3.0, 2.0, 1.0, 0.0);

    const __m256d c1 = _mm256_set1_pd(-1.0 / 5040.0);
    const __m256d c2 = _mm256_set1_pd( 1.0 / 120.0);
    const __m256d c3 = _mm256_set1_pd(-1.0 / 6.0);
    const __m256d c4 = _mm256_set1_pd(1.0);

    const std::uint64_t blocks = config.N / 4;

    double total = 0.0;

    // 每个线程拥有独立的 SIMD 累加器
    #pragma omp parallel reduction(+:total)
    {
        __m256d vsum = _mm256_setzero_pd();

        // 静态划分任务，每个 block 处理 4 个 double
        #pragma omp for schedule(static)
        for (std::uint64_t block = 0;
             block < blocks;
             ++block) {

            const std::uint64_t k = block * 4;

            const __m256d vk =
                _mm256_set1_pd(static_cast<double>(k));

            const __m256d indices =
                _mm256_add_pd(vk, offsets);

            const __m256d x =
                _mm256_mul_pd(indices, vh);

            const __m256d x2 =
                _mm256_mul_pd(x, x);

            // AVX2 + FMA
            __m256d p = _mm256_fmadd_pd(c1, x2, c2);
            p = _mm256_fmadd_pd(p, x2, c3);
            p = _mm256_fmadd_pd(p, x2, c4);

            const __m256d fx =
                _mm256_mul_pd(x, p);

            vsum = _mm256_add_pd(vsum, fx);
        }

        // 每个线程将自己的四个 SIMD 通道归约为标量
        alignas(32) double lanes[4];

        _mm256_store_pd(lanes, vsum);

        const double local_sum =
            lanes[0] + lanes[1] + lanes[2] + lanes[3];

        // OpenMP 将各线程的局部结果合并
        total += local_sum;
    }

    // 处理不足 4 个元素的尾部
    for (std::uint64_t k = blocks * 4;
         k < config.N;
         ++k) {

        const double x = static_cast<double>(k) * h;
        const double x2 = x * x;

        const double p =
            ((-1.0 / 5040.0 * x2 + 1.0 / 120.0)
              * x2 - 1.0 / 6.0)
              * x2 + 1.0;

        total += x * p;
    }

    return h * total;
}
