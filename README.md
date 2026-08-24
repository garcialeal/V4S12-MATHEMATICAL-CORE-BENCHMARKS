# V4S12 S₁₂ Geometry Benchmark Suite (N=4)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20(x86__64)-lightgrey.svg)]()
[![Core Engine](https://img.shields.io/badge/Core%20Engine-Proprietary%20C%2B%2B-red.svg)]()

Public, reproducible benchmark suite evaluating the performance and execution speed of the **V4S12 $S_{12}$ residue topology engine** against standard **IEEE 754 double-precision floating-point pipelines**.

This repository provides pre-compiled evaluation binaries and sample domain datasets (CAD vector entities and 3D molecular structures) to independently audit and verify hyper-dimensional performance baselines without exposing proprietary source code.

---

## Performance Baseline (N=4)

| Evaluation Metric | Standard IEEE 754 Pipeline | V4S12 $S_{12}$ Engine | Optimization / Gain |
| :--- | :--- | :--- | :--- |
| **Throughput Acceleration ($N=4$)** | Baseline ($1.0\times$) | **$1.72\times$ Speedup** | **$+72\%$ Throughput** |
| **Precision Error / Drift** | Cumulative FP Rounding Error | **$0.00\%$ (Exact Integer Grid)** | **Zero Floating-Point Drift** |
| **Arithmetic Operations** | Standard FLOP Overhead | **Bitwise Butterfly Transforms** | **Zero-FLOP Grid Shifts** |
| **Spatial Scaling** | Degrades at $N \ge 4$ | **Linear $O(N)$ Determinism** | **Optimized Hyper-Volume Processing** |

---

## Repository Structure

```text
v4s12-geometry-benchmark/
├── data/
│   ├── cad_sample.dxf       # CAD vector sample (DXF polyline entity nodes)
│   └── protein_sample.pdb   # 3D spatial molecular data (PDB format)
├── v4s12_benchmark          # Pre-compiled standalone evaluation binary (Linux x86_64)
├── LICENSE                  # GPLv3 Harness License & Dual-Licensing Terms
└── README.md                # Project documentation