# Numerical Integration Optimization

基于 C++ 的数值积分性能优化实验项目，通过逐步引入算法优化、循环展开、AVX2 SIMD、FMA 指令和 OpenMP 多线程并行，研究不同底层优化技术对计算性能的影响。

本项目以大规模数值积分为实验场景，从基础标量实现出发，逐步探索编译器优化、指令级并行和线程级并行，记录不同实现的运行时间，并结合汇编分析理解优化效果。

## 1. Project Overview

实验计算函数：

\[
f(x)=x-\frac{x^3}{3!}+\frac{x^5}{5!}-\frac{x^7}{7!}
\]

采用左矩形法计算定积分：

\[
I=\int_0^\pi f(x)\,dx
\]

将积分区间划分为 \(N\) 个小区间，步长为：

\[
h=\frac{\pi}{N}
\]

数值积分近似为：

\[
I\approx h\sum_{i=0}^{N-1}f(ih)
\]

默认规模：

```text
N = 1,000,000,000
```

实验重点不是单纯实现积分算法，而是研究同一计算任务在不同实现方式下的性能变化。

## 2. Optimization Techniques

项目包含八个版本，按照优化路线逐步演进。

| Version | Optimization | Main Technique |
|---|---|---|
| V0 | Baseline | 基础标量实现 |
| V1 | Horner | 霍纳法则优化多项式计算 |
| V2 | Unroll-4 | 4 路循环展开 |
| V3 | Unroll-8 | 8 路循环展开 |
| V4 | Unroll-16 | 16 路循环展开 |
| V5 | AVX2 SIMD | AVX2 向量化计算 |
| V6 | AVX2 + FMA | 向量融合乘加指令 |
| V7 | AVX2 + FMA + OpenMP | SIMD 与多线程并行结合 |

### Optimization Pipeline

```text
Baseline
   |
   v
Horner's Method
   |
   v
Loop Unrolling
   |
   v
AVX2 SIMD
   |
   v
FMA
   |
   v
OpenMP + SIMD
```

## 3. Benchmark Results

实验规模：\(N=10^9\)。

以下数据为各版本正式测量的平均运行时间。V0 为历史三次正式测量的平均值，其余版本为五次正式测量的平均值。

| Version | Average Time (ms) | Speedup vs V0 |
|---|---:|---:|
| V0 Baseline | 3896.65 | 1.00x |
| V1 Horner | 1596.62 | 2.44x |
| V2 Unroll-4 | 1440.779 | 2.71x |
| V3 Unroll-8 | 1509.47 | 2.58x |
| V4 Unroll-16 | 1474.672 | 2.64x |
| V5 AVX2 SIMD | 492.538 | 7.91x |
| V6 AVX2 + FMA | 332.528 | 11.72x |
| V7 OpenMP + SIMD | 58.11 | 67.06x |

> V7 使用 8 个线程，是本次线程数扫描中表现最好的配置。

## 4. Key Findings

- **算法层优化：** 霍纳法则减少多项式计算中的乘法操作，显著降低计算开销。
- **循环展开：** 可以减少循环控制开销并增加指令级并行机会，但展开倍数并非越大越好。
- **SIMD：** AVX2 将多个数据元素放入向量寄存器并行计算，是本实验中重要的性能提升来源。
- **FMA：** 融合乘加指令减少独立乘法和加法指令的执行开销。
- **OpenMP：** 通过多线程将计算任务分配到多个 CPU 核心，实现线程级并行。

本实验也表明，优化效果取决于计算任务、硬件资源、编译器和并行策略。不同优化技术的收益并不总是简单叠加。

## 5. Build and Run

### Requirements

- C++17 compatible compiler
- CMake
- OpenMP
- CPU supporting AVX2
- FMA support for V6 and V7

### Build

```bash
cmake -S . -B build
cmake --build build -j
```

### Run

```bash
# Run individual versions
./build/integration v0
./build/integration v1
./build/integration v2
./build/integration v3
./build/integration v4
./build/integration v5
./build/integration v6
./build/integration v7
```

可以传入较小的 N 进行功能验证：

```bash
./build/integration v7 1000000
```

默认计算规模为 \(10^9\)。

## 6. Project Structure

```text
.
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── optimization.md
│   └── performance_analysis.md
├── experiments/
│   ├── configs.md
│   └── results.csv
├── include/
│   ├── benchmark.h
│   └── integration.h
├── scripts/
│   └── plot_results.py
└── src/
    ├── baseline.cpp
    ├── horner.cpp
    ├── unroll.cpp
    ├── unroll4.cpp
    ├── unroll8.cpp
    ├── simd.cpp
    ├── fma_simd.cpp
    ├── openmp_simd.cpp
    └── main.cpp
```

## 7. Documentation

- [Optimization Details](docs/optimization.md)：各阶段优化方法与实现原理。
- [Performance Analysis](docs/performance_analysis.md)：实验数据、性能变化与瓶颈分析。
- [Experiment Configuration](experiments/configs.md)：实验环境与测试规范。

## 8. Skills Demonstrated

- C++ performance optimization
- Numerical computing
- CPU architecture and SIMD programming
- AVX2 and FMA intrinsics
- OpenMP parallel programming
- Compiler optimization and assembly inspection
- Benchmark design and performance analysis
- CMake-based project organization

## 9. Conclusion

本项目围绕一个统一的数值计算任务，构建了从算法优化到指令级并行、再到多线程并行的完整性能优化实验流程。

通过控制实验规模、保留原始测试结果、比较不同优化策略，并结合汇编指令分析，理解了现代 CPU 上高性能计算的基本优化方法与实际限制。