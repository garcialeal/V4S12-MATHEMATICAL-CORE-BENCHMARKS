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