# Performance Analysis

本文档分析 Numerical Integration Optimization 项目中不同优化版本的性能变化，并结合实验结果讨论优化收益与性能瓶颈。

## 1. Benchmark Methodology

### Workload

- Integration method: Left Riemann Sum
- Function: Polynomial approximation
- Default iteration count: \(N=10^9\)
- Data type: double
- Compiler optimization: `-O2`
- SIMD instruction set: AVX2
- Fused operation: FMA
- Parallel framework: OpenMP

### Measurement Protocol

实验采用统一的测试规范：

1. 每个版本第一次运行作为预热，不纳入正式统计。
2. 后续进行正式测量。
3. V0 保留历史三次正式测量结果。
4. 其他版本采用五次正式测量。
5. 使用算术平均值作为主要比较指标。
6. 保留原始测量结果，不删除异常数据，不改变统计口径。

## 2. Overall Performance

| Version | Optimization | Average Time (ms) | Speedup |
|---|---|---:|---:|
| V0 | Baseline | 3896.65 | 1.00x |
| V1 | Horner | 1596.62 | 2.44x |
| V2 | Unroll-4 | 1440.779 | 2.71x |
| V3 | Unroll-8 | 1509.47 | 2.58x |
| V4 | Unroll-16 | 1474.672 | 2.64x |
| V5 | AVX2 SIMD | 492.538 | 7.91x |
| V6 | AVX2 + FMA | 332.528 | 11.72x |
| V7 | AVX2 + FMA + OpenMP | 58.11 | 67.06x |

Speedup 计算方式：

\[
Speedup=\frac{T_{baseline}}{T_{optimized}}
\]

其中 \(T_{baseline}\) 为 V0 的平均运行时间。

最终 V7 相比 V0 获得约 67.06 倍的性能提升。

## 3. Stage-by-Stage Analysis

### 3.1 Algorithm Optimization

V0 到 V1：

- V0：3896.65 ms
- V1：1596.62 ms

霍纳法则减少了多项式计算中的重复乘法操作。

这一阶段说明，优化计算表达式本身能够带来显著收益，不需要立即引入复杂的硬件指令。

### 3.2 Loop Unrolling

V1 到 V2、V3、V4：

- Unroll-4：1440.779 ms
- Unroll-8：1509.47 ms
- Unroll-16：1474.672 ms

4 路展开取得了这一组实验中最好的结果。

8 路和 16 路展开没有进一步提升，说明循环控制开销并不是唯一的性能限制因素。

过度展开可能增加代码体积、寄存器压力和指令调度复杂度。

因此，循环展开应以实测结果为依据，而不是单纯追求更大的展开倍数。

### 3.3 SIMD Optimization

V5 使用 AVX2 SIMD。

其平均运行时间为 492.538 ms，相比 V0 提升约 7.91 倍。

AVX2 能够利用 256-bit 向量寄存器同时处理多个 double 数据，减少逐元素标量计算。

这是本项目中非常重要的一次性能跃升。

### 3.4 FMA Optimization

V6 将 AVX2 与 FMA 结合。

- V5：492.538 ms
- V6：332.528 ms

V6 相比 V5 进一步减少约 32.5% 的运行时间。

汇编检查发现 V6 包含 `vfmadd132pd` 指令，说明编译后的代码使用了融合乘加指令。

这一结果表明，表达式结构与硬件指令之间的匹配对于高性能计算非常重要。

### 3.5 OpenMP Parallelization

V7 在 AVX2 + FMA 的基础上引入 OpenMP。

线程扫描结果：

| Threads | Average Time (ms) |
|---:|---:|
| 1 | 355.02 |
| 2 | 189.96 |
| 4 | 111.11 |
| 8 | 58.11 |
| 12 | 67.15 |
| 16 | 62.53 |
| 24 | 62.68 |

8 线程取得了本次扫描中的最佳成绩。

相较于 V7 单线程的 355.02 ms，8 线程版本约提升 6.11 倍。

值得注意的是，12、16、24 线程并没有继续带来性能提升。

这说明并行性能受到 CPU 物理核心数量、超线程、调度开销、内存访问和同步成本等因素共同影响。

当前实验能够确认 8 线程是已测试配置中的最佳选择，但不足以证明其他机器或其他负载也具有相同的最优线程数。

## 4. Assembly-Level Analysis

本项目不仅比较运行时间，也通过汇编代码检查底层指令。

### V5: AVX2

汇编分析确认 V5 使用 AVX2 向量指令，说明其计算过程已经从标量处理转向向量化处理。

### V6: AVX2 + FMA

汇编中发现：

```asm
vfmadd132pd
```

该指令属于 packed double-precision fused multiply-add 指令。

它可以对多个 double 数据执行融合乘加操作。

### Analysis Notes

- 编译器自动向量化报告并不能完整代表所有 SIMD 实现。
- 使用 intrinsics 编写的向量代码应结合汇编进行确认。
- 没有发现明显的向量栈读写，并不等价于绝对不存在寄存器溢出。
- 汇编分析应与实际运行时间结合，避免仅凭某条指令判断整体性能。

## 5. Performance Bottlenecks and Limitations

### 5.1 Loop Unrolling Is Not Always Beneficial

循环展开减少了部分循环控制开销，但也可能带来更高的寄存器压力和代码体积。

因此，循环展开的最优倍数需要结合具体计算任务进行测试。

### 5.2 SIMD Utilization

SIMD 能够提高每条指令处理的数据量，但实际收益还取决于：

- 向量计算单元的利用率。
- 数据布局。
- 数据依赖关系。
- 指令调度。
- 向量归约成本。

### 5.3 Thread Scaling

增加线程数会带来额外的调度与同步开销。

当可用计算资源已经充分利用时，继续增加线程数可能不会带来收益。

### 5.4 Benchmark Variance

运行时间可能受到系统负载、CPU 频率、缓存状态和线程调度影响。

因此，应当：

- 使用一致的测试环境。
- 保留完整原始数据。
- 进行多次测量。
- 结合平均值和波动情况分析结果。
- 避免根据单次测试得出过度结论。

## 6. Further Optimization Directions

后续可以从以下方向继续研究：

1. **CPU Profiling：** 使用 perf 等工具分析热点函数、指令执行和缓存行为。
2. **Memory and Cache Analysis：** 分析数据访问模式及缓存层次对性能的影响。
3. **Thread Affinity：** 研究线程绑定与调度策略对多线程性能的影响。
4. **Compiler Comparison：** 比较不同编译器及优化参数生成的代码。
5. **Instruction Scheduling：** 结合汇编研究指令依赖链与执行资源利用情况。

这些方向属于后续研究计划，不代表本项目已经完成相关实验。

## 7. Conclusion

本项目展示了从算法层优化到 SIMD 指令级优化，再到 OpenMP 多线程并行的完整性能优化过程。

实验结果表明：

- 算法表达式优化能够显著降低计算成本。
- 循环展开的收益存在上限。
- AVX2 SIMD 带来了明显的性能提升。
- FMA 能够进一步提高向量计算效率。
- 多线程并行具有明显的资源依赖性，线程数需要通过实验确定。

最终版本 V7 相比基础版本获得约 67.06 倍的性能提升。

更重要的是，本项目通过统一实验任务、保留测试数据和检查汇编代码，将性能优化从单纯的代码修改转化为可测量、可分析和可验证的工程实践。