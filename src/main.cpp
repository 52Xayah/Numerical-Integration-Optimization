#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>

#include "integration.h"
#include "benchmark.h"

using Integrator = double (*)(const IntegrationConfig&);

struct Version {
    std::string_view id;
    std::string_view name;
    Integrator function;
};

int main(int argc, char* argv[]) {
    const Version versions[] = {
        {"v0", "V0 Baseline", integrate_baseline},
        {"v1", "V1 Horner", integrate_horner},
        {"v2", "V2 Unroll-4", integrate_unroll4},
        {"v3", "V3 Unroll-8", integrate_unroll8},
        {"v4", "V4 Unroll-16", integrate_unroll},
        {"v5", "V5 AVX2 SIMD", integrate_simd},
        {"v6", "V6 AVX2 + FMA", integrate_simd_fma},
        {"v7", "V7 AVX2 + FMA + OpenMP", integrate_openmp_simd}
    };

    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <v0-v7> [N]\n";
        return 1;
    }

    const std::string_view selected = argv[1];

    const Version* version = nullptr;

    for (const auto& item : versions) {
        if (item.id == selected) {
            version = &item;
            break;
        }
    }

    if (version == nullptr) {
        std::cerr << "Invalid version: " << selected << '\n';
        std::cerr << "Available versions: v0 v1 v2 v3 v4 v5 v6 v7\n";
        return 1;
    }

    IntegrationConfig config;

    // 可选参数 N，用于小规模功能验证
    if (argc == 3) {
        try {
            std::size_t pos = 0;
            const std::string input = argv[2];
            const auto n = std::stoull(input, &pos);

            if (pos != input.size() || n == 0) {
                throw std::invalid_argument("Invalid N");
            }

            config.N = n;
        } catch (const std::exception&) {
            std::cerr << "Invalid N. N must be a positive integer.\n";
            return 1;
        }
    }

    double result = 0.0;

    const double elapsed = benchmark([&]() {
        result = version->function(config);
    });

    std::cout << std::fixed << std::setprecision(12);

    std::cout << "Version: " << version->name << '\n';
    std::cout << "N: " << config.N << '\n';
    std::cout << "Result: " << result << '\n';
    std::cout << "Time: " << elapsed << " ms\n";

    return 0;
}
