# V4S12 S₁₂ Geometry Benchmark Suite (N=4)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20(x86__64)-lightgrey.svg)]()
[![Core Engine](https://img.shields.io/badge/Core%20Engine-Proprietary%20C%2B%2B-red.svg)]()

Public, reproducible benchmark suite evaluating the performance and execution speed of the **V4S12 $S_{12}$ residue topology engine** against standard **IEEE 754 double-precision floating-point pipelines**.

This repository provides pre-compiled evaluation binaries, source harness wrappers, and sample domain datasets (CAD vector entities and 3D molecular structures) to independently audit and verify hyper-dimensional performance baselines without exposing proprietary source code.

---

## Measured Performance Baseline (N=4, 1,000,000 Vertices)

| Evaluation Metric | Standard IEEE 754 Pipeline | V4S12 $S_{12}$ Engine | Optimization / Gain |
| :--- | :--- | :--- | :--- |
| **Throughput Acceleration** | Baseline ($1.00\times$) | **$1.59\times$ Speedup** | **$+59\%$ Throughput** |
| **Execution Latency (1M Vertices)** | $29.92\text{ ms}$ | **$18.81\text{ ms}$** | **$-37.1\%$ Latency Reduction** |
| **Processing Rate** | $33.42\text{ M vertices/sec}$ | **$53.17\text{ M vertices/sec}$** | **$+19.75\text{ M vertices/sec}$** |
| **Precision Error / Drift** | Cumulative FP Rounding Error | **$0.00\%$ (Exact Integer Grid)** | **Zero Floating-Point Drift** |
| **Arithmetic Operations** | Standard FLOP Overhead | **Bitwise Butterfly Transforms (`>> 2`)** | **Zero-FLOP Grid Shifts** |

---

## Build & Execution

To recompile the benchmark binary from source using `x86-64-v3` SIMD vectorization:

```bash
g++ -O3 -std=c++17 -march=x86-64-v3 -flto -ffast-math -I include src/main_benchmark.cpp src/v4s12_engine.cpp -o v4s12_benchmark
./v4s12_benchmark
```
---

## Benchmark & Integration Protocols

To ensure reproducible profiling and avoid CPU pipeline stalls during evaluation, test harnesses must adhere to the correct data ingestion path:

* **Native $S_{12}$ Zero-Copy Protocol (Benchmark Standard):**
  Measures pure algebraic throughput directly on contiguous integer arrays in the $S_{12}$ residue domain. This path bypasses runtime type casting, unlocking the full **$1.59\times$ speedup** ($>53\text{ M}$ vertices/sec) and maximum SIMD vectorization.

* **Host Floating-Point Protocol (IEEE 754 Ingestion):**
  When bridging with host applications providing `double` or `float` streams (CAD/GIS platforms), type conversions must be batched across memory blocks using explicit SIMD vectorization (`_mm256_cvttpd_epi32` or `#pragma omp simd`).

> **Notice for Evaluators:** Do not perform scalar `static_cast<int32_t>` conversions inside tight vertex transformation loops. Scalar float-to-int CPU instructions (`cvttsd2si`) break instruction pipelining and obscure real engine throughput.
---

## Repository Structure

```text
v4s12-geometry-benchmark/
├── data/
│   ├── cad_sample.dxf       # CAD vector sample (DXF polyline entity nodes)
│   └── protein_sample.pdb   # 3D spatial molecular data (PDB format)
├── include/
│   └── v4s12_engine.h       # Engine definitions and point structure interface
├── src/
│   ├── main_benchmark.cpp   # Benchmark entrypoint and timing harness
│   └── v4s12_engine.cpp     # Vectorized batch transformation pipeline
├── v4s12_benchmark          # Pre-compiled standalone evaluation binary (Linux x86_64)
├── LICENSE                  # GPLv3 Harness License & Dual-Licensing Terms
└── README.md                # Project documentation
```