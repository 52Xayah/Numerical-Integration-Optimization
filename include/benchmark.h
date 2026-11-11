#pragma once

#include <chrono>
#include <utility>

template <typename Func>
double benchmark(Func&& func) {
    const auto start = std::chrono::steady_clock::now();

    std::forward<Func>(func)();

    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::milli>(
        end - start
    ).count();
}