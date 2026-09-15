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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "v4s12_engine.h"

static std::string resolveDataPath(const std::initializer_list<std::string>& candidates) {
    for (const auto& name : candidates) {
        std::ifstream f1("data/" + name);
        if (f1.good()) return "data/" + name;
        std::ifstream f2("../data/" + name);
        if (f2.good()) return "../data/" + name;
    }
    return "";
}

void printMetrics(const v4s12::BenchmarkMetrics& m) {
    std::cout << std::left << std::setw(24) << m.dataset_name
              << std::right << std::setw(12) << m.point_count
              << std::setw(12) << std::fixed << std::setprecision(2) << m.time_std_ms
              << std::setw(12) << m.time_v4s12_ms
              << std::setw(10) << m.speedup << "x"
              << std::setw(14) << m.throughput_mvs
              << std::setw(12) << std::scientific << std::setprecision(3) << m.mse << "\n";
}

void exportCSV(const std::vector<v4s12::BenchmarkMetrics>& results, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return;

    out << "dataset,vertices,time_fp64_ms,time_v4s12_ms,speedup,throughput_mvs,mse\n";
    for (const auto& m : results) {
        out << "\"" << m.dataset_name << "\","
            << m.point_count << ","
            << std::fixed << std::setprecision(4) << m.time_std_ms << ","
            << m.time_v4s12_ms << ","
            << m.speedup << ","
            << m.throughput_mvs << ","
            << std::scientific << std::setprecision(6) << m.mse << "\n";
    }
    std::cout << "\n[+ Exported CSV]: " << filepath << "\n";
}

void exportJSON(const std::vector<v4s12::BenchmarkMetrics>& results, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return;

    out << "[\n";
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& m = results[i];
        out << "  {\n"
            << "    \"dataset\": \"" << m.dataset_name << "\",\n"
            << "    \"vertices\": " << m.point_count << ",\n"
            << "    \"time_fp64_ms\": " << std::fixed << std::setprecision(4) << m.time_std_ms << ",\n"
            << "    \"time_v4s12_ms\": " << m.time_v4s12_ms << ",\n"
            << "    \"speedup\": " << m.speedup << ",\n"
            << "    \"throughput_mvs\": " << m.throughput_mvs << ",\n"
            << "    \"mse\": " << std::scientific << std::setprecision(6) << m.mse << "\n"
            << "  }" << (i + 1 < results.size() ? "," : "") << "\n";
    }
    out << "]\n";
    std::cout << "[+ Exported JSON]: " << filepath << "\n";
}

int main() {
    std::vector<v4s12::BenchmarkMetrics> results;

    std::cout << "=========================================================================================\n";
    std::cout << "               V4S12-MATHEMATICAL-CORE BENCHMARK SUITE (v1.0.0 Core)                     \n";
    std::cout << "=========================================================================================\n";
    std::cout << std::left << std::setw(24) << "Dataset"
              << std::right << std::setw(12) << "Vertices"
              << std::setw(12) << "FP64 (ms)"
              << std::setw(12) << "V4S12 (ms)"
              << std::setw(11) << "Speedup"
              << std::setw(14) << "M vert/sec"
              << std::setw(12) << "MSE" << "\n";
    std::cout << "-----------------------------------------------------------------------------------------\n";

    // 1. CAD File Test
    std::string dxf_path = resolveDataPath({"cad_sample.dxf", "cad.dxf", "sample.dxf"});
    if (!dxf_path.empty()) {
        auto cad_data = v4s12::V4S12Engine::loadDXF(dxf_path);
        auto m_cad = v4s12::V4S12Engine::runBenchmark("CAD (cad_sample.dxf)", cad_data);
        printMetrics(m_cad);
        results.push_back(m_cad);
    } else {
        std::cout << std::left << std::setw(24) << "CAD (cad_sample.dxf)" << "[SKIP / File empty or missing]\n";
    }

    // 2. Protein PDB File Test
    std::string pdb_path = resolveDataPath({"protein.pdb", "protein_sample.pdb", "sample.pdb"});
    if (!pdb_path.empty()) {
        auto pdb_data = v4s12::V4S12Engine::loadPDB(pdb_path);
        auto m_pdb = v4s12::V4S12Engine::runBenchmark("PDB (protein.pdb)", pdb_data);
        printMetrics(m_pdb);
        results.push_back(m_pdb);
    } else {
        std::cout << std::left << std::setw(24) << "PDB (protein.pdb)" << "[SKIP / File empty or missing]\n";
    }

    // 3. Synthetic LiDAR Point Cloud (10M points stress test)
    auto lidar_data = v4s12::V4S12Engine::generateSyntheticLiDAR(10'000'000);
    auto m_lidar = v4s12::V4S12Engine::runBenchmark("LiDAR 3D (10M Stress)", lidar_data);
    printMetrics(m_lidar);
    results.push_back(m_lidar);

    std::cout << "=========================================================================================\n";

    // Exportación automática de métricas
    exportCSV(results, "benchmark_results.csv");
    exportJSON(results, "benchmark_results.json");

    return 0;
}