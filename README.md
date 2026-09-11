# V4S12-MATHEMATICAL-CORE-BENCHMARKS

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20(x86__64)-lightgrey.svg)]()
[![Core Engine](https://img.shields.io/badge/Core%20Engine-Inlined%20SIMD%20C%2B%2B-red.svg)]()

Public, reproducible benchmark suite evaluating the performance and execution speed of the **V4S12-MATHEMATICAL-CORE $S_{12}$ residue topology engine** against standard **IEEE 754 double-precision floating-point (FP64) pipelines**.

This repository provides source harnesses, CMake build systems, and sample domain datasets (CAD vectors, 3D molecular structures, and 10M LiDAR point clouds) to audit hyper-dimensional integer transformation performance.

---

## Measured Performance Baseline (10,000,000 Vertices Stress Test)

| Evaluation Metric | Standard IEEE 754 (FP64) | V4S12 $S_{12}$ Engine | Optimization / Gain |
| :--- | :--- | :--- | :--- |
| **Throughput Acceleration** | Baseline ($1.00\times$) | **$1.74\times$ Speedup** | **$+74\%$ Throughput** |
| **Execution Latency (10M Vertices)** | $24.89\text{ ms}$ | **$14.33\text{ ms}$** | **$-42.4\%$ Latency Reduction** |
| **Processing Rate** | $401.76\text{ M vert/sec}$ | **$697.94\text{ M vert/sec}$** | **$+296.18\text{ M vert/sec}$** |
| **Data Precision / Format** | FP64 Double Precision | **12-bit $S_{12}$ Quantized Grid** | **Deterministic Integer Grid** |
| **Pipeline Vectorization** | FP64 Auto-vectorized Loop | **Inlined Zero-Call SIMD Operations** | **Zero Function Overhead** |

---

## Technical Highlights & Optimizations

* **Zero-Overhead Inlined Kernels:** Transformations (`v4_transform_2d_4x4`) and 12-bit S12 quantization (`s12_quantize_spline`) are directly inlined into the main engine loop, eliminating external function calls and enabling full AVX2/AVX-512 compiler auto-vectorization (`#pragma omp simd`).
* **Bitwise Arithmetic Operations:** Floating-point multiplications and divisions are replaced with hardware-efficient bitwise arithmetic (shifts `>> 1`, `<< 1` and additions).
* **Multi-Threaded Execution:** Parallelized via OpenMP (`#pragma omp parallel for schedule(static)`) with affinity pinning for hardware threads.
* **Automated Metric Export:** Automatically generates structured CSV and JSON execution reports (`benchmark_results.csv` and `benchmark_results.json`) upon benchmark completion.

---

## Build & Execution Instructions

### 1. Build via CMake

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 2. Execution with Thread Pinning

To achieve maximum SIMD and cache throughput, pin OpenMP threads to CPU cores prior to execution:

```bash
export OMP_PROC_BIND=spread
export OMP_PLACES=threads
export OMP_NUM_THREADS=$(nproc)

./v4s12_benchmark
```

---

## Output Data Artifacts

Upon running `./v4s12_benchmark`, the engine writes benchmark results directly to the build directory:

* **`benchmark_results.csv`**: Tabular metric exports for automated CI/CD logging and spreadsheet processing.
* **`benchmark_results.json`**: Structured JSON payload for automated dashboard rendering and plotting.

---

## Benchmark Protocols & Guidelines

* **Native S12 Protocol:** Computes 2D integer matrix transformations and spline quantization over dense memory blocks.
* **OpenMP Thread Allocation:** Ensure `OMP_NUM_THREADS` matches physical/logical core limits to eliminate context switching latency during high-density tests (e.g., 10M LiDAR points).
* **Compiler Optimization:** Requires `-O3` and C++17 support for vectorization hint evaluation.

---

## Repository Structure

```text
v4s12-mathematical-core-benchmarks/
├── CMakeLists.txt           # Build system configuration
├── data/
│   ├── cad_sample.dxf       # CAD vector sample (DXF polyline entity nodes)
│   └── protein_sample.pdb   # 3D spatial molecular data (PDB format)
├── include/
│   └── v4s12_engine.h       # Engine definitions and point structure interface
├── src/
│   ├── main_benchmark.cpp   # Benchmark entrypoint, console output, CSV/JSON exporters
│   └── v4s12_engine.cpp     # Inlined vectorized batch transformation pipeline
├── LICENSE                  # GPLv3 Harness License & Dual-Licensing Terms
└── README.md                # Project documentation
```