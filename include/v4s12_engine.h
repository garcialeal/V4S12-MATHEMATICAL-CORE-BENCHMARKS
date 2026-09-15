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

#pragma once

#include <string>
#include <vector>
#include <cstddef>

extern "C" {
#include "v4s12/v4s12.h"
#include "v4s12/v4_transform.h"
#include "v4s12/s12_quant.h"
#include "v4s12/v4_zigzag.h"
}

namespace v4s12 {

struct Point4D {
    double x;
    double y;
    double z;
    double w;
};

struct BenchmarkMetrics {
    std::string dataset_name;
    size_t point_count;
    double time_std_ms;
    double time_v4s12_ms;
    double speedup;
    double throughput_mvs;
    double mse;
};

class V4S12Engine {
public:
    static std::vector<Point4D> loadDXF(const std::string& filepath);
    static std::vector<Point4D> loadPDB(const std::string& filepath);
    static std::vector<Point4D> generateSyntheticLiDAR(size_t count);
    static BenchmarkMetrics runBenchmark(const std::string& name, const std::vector<Point4D>& input);
};

} // namespace v4s12