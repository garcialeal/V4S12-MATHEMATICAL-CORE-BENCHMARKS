/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * V4S12-MATHEMATICAL-CORE-BENCHMARKS
 *
 * Copyright (C) 2026 Antonio García Leal <support@v4s12.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "v4s12_engine.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <cmath>
#include <omp.h>

namespace v4s12 {

std::vector<Point4D> V4S12Engine::loadDXF(const std::string& filepath) {
    std::vector<Point4D> points;
    std::ifstream file(filepath);
    if (!file.is_open()) return points;

    std::string line;
    double x = 0, y = 0, z = 0;
    while (std::getline(file, line)) {
        if (line.find(" 10") == 0 || line == "10") {
            if (std::getline(file, line)) x = std::stod(line);
        } else if (line.find(" 20") == 0 || line == "20") {
            if (std::getline(file, line)) y = std::stod(line);
        } else if (line.find(" 30") == 0 || line == "30") {
            if (std::getline(file, line)) {
                z = std::stod(line);
                points.push_back({x, y, z, 1.0});
            }
        }
    }
    return points;
}

std::vector<Point4D> V4S12Engine::loadPDB(const std::string& filepath) {
    std::vector<Point4D> points;
    std::ifstream file(filepath);
    if (!file.is_open()) return points;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("ATOM", 0) == 0 || line.rfind("HETATM", 0) == 0) {
            if (line.length() >= 54) {
                try {
                    double x = std::stod(line.substr(30, 8));
                    double y = std::stod(line.substr(38, 8));
                    double z = std::stod(line.substr(46, 8));
                    points.push_back({x, y, z, 1.0});
                } catch (...) {}
            }
        }
    }
    return points;
}

std::vector<Point4D> V4S12Engine::generateSyntheticLiDAR(size_t count) {
    std::vector<Point4D> points(count);
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);

    for (size_t i = 0; i < count; ++i) {
        points[i] = {dist(rng), dist(rng), dist(rng), 1.0};
    }
    return points;
}

BenchmarkMetrics V4S12Engine::runBenchmark(const std::string& name, const std::vector<Point4D>& input) {
    const size_t n = input.size();
    if (n == 0) return {name, 0, 0, 0, 0, 0, 0};

    const size_t block_count = (n + 3) / 4;
    std::vector<v4s12_int_t> core_blocks(block_count * 16, 0);

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
        size_t blk = i / 4;
        size_t idx = i % 4;
        core_blocks[blk * 16 + idx * 4 + 0] = static_cast<v4s12_int_t>(input[i].x);
        core_blocks[blk * 16 + idx * 4 + 1] = static_cast<v4s12_int_t>(input[i].y);
        core_blocks[blk * 16 + idx * 4 + 2] = static_cast<v4s12_int_t>(input[i].w);
        core_blocks[blk * 16 + idx * 4 + 3] = static_cast<v4s12_int_t>(input[i].z);
    }

    std::vector<double> fp64_buf(block_count * 16, 1.0);
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; ++i) {
        fp64_buf[i * 4 + 0] = input[i].x;
        fp64_buf[i * 4 + 1] = input[i].y;
        fp64_buf[i * 4 + 2] = input[i].z;
        fp64_buf[i * 4 + 3] = input[i].w;
    }

    // 1. FP64 Benchmark
    auto start_std = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for schedule(static)
    for (size_t b = 0; b < block_count; ++b) {
        double* blk = &fp64_buf[b * 16];
        #pragma omp simd
        for (int r = 0; r < 4; ++r) {
            double t0 = (blk[r*4 + 0] + blk[r*4 + 1]) * 0.5;
            double t1 = (blk[r*4 + 0] - blk[r*4 + 1]) * 0.5;
            double t2 = (blk[r*4 + 2] + blk[r*4 + 3]) * 0.5;
            double t3 = (blk[r*4 + 2] - blk[r*4 + 3]) * 0.5;
            blk[r*4 + 0] = t0 + t2; blk[r*4 + 1] = t1 + t3;
            blk[r*4 + 2] = t0 - t2; blk[r*4 + 3] = t1 - t3;
        }
        #pragma omp simd
        for (int i = 0; i < 16; ++i) {
            blk[i] = std::round(blk[i]);
        }
        #pragma omp simd
        for (int r = 0; r < 4; ++r) {
            double t0 = (blk[r*4 + 0] + blk[r*4 + 2]) * 0.5;
            double t1 = (blk[r*4 + 1] + blk[r*4 + 3]) * 0.5;
            double t2 = (blk[r*4 + 0] - blk[r*4 + 2]) * 0.5;
            double t3 = (blk[r*4 + 1] - blk[r*4 + 3]) * 0.5;
            blk[r*4 + 0] = t0 + t1; blk[r*4 + 1] = t0 - t1;
            blk[r*4 + 2] = t2 + t3; blk[r*4 + 3] = t2 - t3;
        }
    }
    auto end_std = std::chrono::high_resolution_clock::now();
    double time_std = std::chrono::duration<double, std::milli>(end_std - start_std).count();

    // 2. V4S12 Inlined SIMD Core Kernel (Zero Function Call Overhead)
    auto start_v4 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for schedule(static)
    for (size_t b = 0; b < block_count; ++b) {
        v4s12_int_t* blk = &core_blocks[b * 16];

        // Transformada 2D Entera Directa
        #pragma omp simd
        for (int r = 0; r < 4; ++r) {
            v4s12_int_t t0 = (blk[r*4 + 0] + blk[r*4 + 1]) >> 1;
            v4s12_int_t t1 = (blk[r*4 + 0] - blk[r*4 + 1]) >> 1;
            v4s12_int_t t2 = (blk[r*4 + 2] + blk[r*4 + 3]) >> 1;
            v4s12_int_t t3 = (blk[r*4 + 2] - blk[r*4 + 3]) >> 1;
            blk[r*4 + 0] = t0 + t2; blk[r*4 + 1] = t1 + t3;
            blk[r*4 + 2] = t0 - t2; blk[r*4 + 3] = t1 - t3;
        }

        // Cuantización Entera S12 de 12 bits
        #pragma omp simd
        for (int i = 0; i < 16; ++i) {
            blk[i] = (blk[i] >= 0) ? ((blk[i] + 2) >> 2) : -(((-blk[i]) + 2) >> 2);
        }

        // Transformada Inversa Entera
        #pragma omp simd
        for (int r = 0; r < 4; ++r) {
            v4s12_int_t t0 = (blk[r*4 + 0] + blk[r*4 + 2]) << 1;
            v4s12_int_t t1 = (blk[r*4 + 1] + blk[r*4 + 3]) << 1;
            v4s12_int_t t2 = (blk[r*4 + 0] - blk[r*4 + 2]) << 1;
            v4s12_int_t t3 = (blk[r*4 + 1] - blk[r*4 + 3]) << 1;
            blk[r*4 + 0] = t0 + t1; blk[r*4 + 1] = t0 - t1;
            blk[r*4 + 2] = t2 + t3; blk[r*4 + 3] = t2 - t3;
        }
    }
    auto end_v4 = std::chrono::high_resolution_clock::now();
    double time_v4 = std::chrono::duration<double, std::milli>(end_v4 - start_v4).count();

    double total_sq_err = 0.0;
    #pragma omp parallel for reduction(+:total_sq_err) schedule(static)
    for (size_t i = 0; i < n; ++i) {
        size_t blk = i / 4;
        size_t idx = i % 4;
        double rx = core_blocks[blk * 16 + idx * 4 + 0];
        double dx = rx - std::floor(input[i].x);
        total_sq_err += (dx * dx);
    }
    double mse = total_sq_err / static_cast<double>(n);

    double speedup = (time_v4 > 0.0) ? (time_std / time_v4) : 1.0;
    double throughput = (n / (time_v4 / 1000.0)) / 1'000'000.0;

    return {name, n, time_std, time_v4, speedup, throughput, mse};
}

} // namespace v4s12