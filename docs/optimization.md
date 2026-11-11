# Optimization Techniques

本文档介绍 Numerical Integration Optimization 项目中采用的优化技术，按照从算法层到硬件并行层的顺序展开。

## 1. Baseline: Scalar Implementation

Baseline 使用最直接的方式计算积分：

```cpp
for (std::uint64_t i = 0; i < N; ++i) {
    double x = i * h;
    sum += f(x);
}
```

主要计算流程：

1. 根据索引计算当前采样点。
2. 计算函数值。
3. 将结果累加到总和。
4. 完成全部迭代后乘以步长。

该版本作为性能对照基准。

其主要特点是实现简单，但循环中存在重复的多项式计算和累加依赖。

## 2. Horner's Method

原始多项式：

\[
f(x)=x-\frac{x^3}{3!}+\frac{x^5}{5!}-\frac{x^7}{7!}
\]

提取公因子后：

\[
f(x)=x\left(1+x^2\left(-\frac{1}{6}
+x^2\left(\frac{1}{120}-\frac{x^2}{5040}\right)\right)\right)
\]

霍纳形式将多项式转化为嵌套乘加结构。

### Optimization Principle

- 减少重复幂运算。
- 减少乘法操作数量。
- 提高计算表达式的紧凑程度。
- 为后续 FMA 优化提供更合适的计算结构。

霍纳法则是一种算法层面的优化，不依赖特定 CPU 指令集。

## 3. Loop Unrolling

循环展开通过一次循环执行多次迭代，减少循环控制指令的执行频率。

例如 4 路展开：

```cpp
for (std::uint64_t i = 0; i < N; i += 4) {
    // Process four elements
}
```

### Expected Benefits

- 减少循环条件判断和递增操作。
- 降低分支控制开销。
- 增加指令级并行机会。
- 在适当情况下帮助编译器进行指令调度。

### Experimental Comparison

本项目比较了 4、8、16 路循环展开。

实验发现，4 路展开表现最好，但 8 路和 16 路没有进一步提升。

这说明循环展开的收益受到寄存器资源、指令调度、代码体积和编译器优化策略影响。

**重要结论：循环展开倍数越大，不代表性能越好。**

## 4. AVX2 SIMD

SIMD（Single Instruction, Multiple Data）允许一条指令同时处理多个数据元素。

本项目使用 AVX2 指令集进行双精度浮点向量计算。

一个 256-bit AVX2 寄存器可以容纳：

\[
256/64=4
\]

个 double 类型数据。

因此，AVX2 可以在一次向量运算中处理四个采样点。

### Optimization Principle

传统标量计算：

```text
x0 -> f(x0)
x1 -> f(x1)
x2 -> f(x2)
x3 -> f(x3)
```

SIMD 向量计算：

```text
[x0, x1, x2, x3]
          |
          v
   Vector Operations
          |
          v
[f(x0), f(x1), f(x2), f(x3)]
```

### Main Benefits

- 提高单条指令的数据处理能力。
- 减少逐元素标量计算。
- 提高 CPU 向量计算单元利用率。

本项目通过汇编检查确认 V5 使用了 AVX2 向量指令。

## 5. FMA Optimization

FMA（Fused Multiply-Add）是一种融合乘加指令：

\[
a\times b+c
\]

它将乘法和加法融合为一次指令操作。

在支持 FMA 的 CPU 上，可以减少独立乘法与加法指令的执行开销。

### Why Horner + FMA?

霍纳形式包含嵌套乘加表达式：

\[
a_0+x(a_1+x(a_2+\cdots))
\]

这种结构适合使用 FMA 指令实现。

本项目通过汇编分析确认 V6 中出现了 `vfmadd132pd` 指令。

需要注意，FMA 可能改变浮点运算的舍入行为，因此性能与数值结果都应结合具体实现理解。

## 6. OpenMP Parallelization

OpenMP 是一种支持共享内存并行编程的 API。

本项目使用 OpenMP 将大规模积分任务划分给多个线程。

### Parallelization Principle

```text
              Total Iterations
                     |
          +----------+----------+
          |          |          |
        Thread 0   Thread 1   Thread N
          |          |          |
       Partial    Partial    Partial
         Sum        Sum        Sum
          |          |          |
          +----------+----------+
                     |
                 Reduction
                     |
                 Final Sum
```

各线程处理不同的迭代区间，最后通过归约操作合并局部结果。

### Thread Scaling

本项目对不同线程数进行了测试：

| Threads | Average Time (ms) |
|---:|---:|
| 1 | 355.02 |
| 2 | 189.96 |
| 4 | 111.11 |
| 8 | 58.11 |
| 12 | 67.15 |
| 16 | 62.53 |
| 24 | 62.68 |

8 线程在本次测试中表现最好。

线程数增加后，性能没有继续提升，可能与线程调度、同步、资源竞争及 CPU 实际硬件资源有关。仅凭当前数据不能确定单一原因。

## 7. Combined Optimization

最终版本 V7 结合：

- AVX2：数据级并行。
- FMA：融合乘加。
- OpenMP：线程级并行。

三种技术作用于不同层次：

| Optimization | Parallelism / Improvement Level |
|---|---|
| Horner | 算法与计算表达式 |
| Loop Unrolling | 指令级并行机会 |
| AVX2 | 数据级并行 |
| FMA | 指令执行效率 |
| OpenMP | 线程级并行 |

最终优化不是简单地增加代码复杂度，而是针对计算过程中的不同开销逐层优化。

## 8. Lessons Learned

1. 先优化算法，再考虑底层指令。
2. 循环展开需要通过实验确定合适的展开倍数。
3. SIMD 的收益通常比单纯增加循环展开倍数更显著。
4. FMA 应结合适合的表达式结构使用。
5. 多线程并行存在资源限制，线程数不是越多越好。
6. 不能只根据运行时间判断优化是否生效，还应结合汇编和编译器报告进行分析。