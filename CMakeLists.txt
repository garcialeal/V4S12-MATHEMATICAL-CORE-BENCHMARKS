# SPDX-License-Identifier: GPL-3.0-or-later
#
# V4S12-MATHEMATICAL-CORE-BENCHMARKS
#
# Copyright (C) 2026 Antonio García Leal <support@v4s12.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15)
project(v4s12-mathematical-core-benchmarks VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O3 
        -march=native 
        -ffast-math 
        -flto 
        -funroll-loops 
        -ftree-vectorize
        -Wall 
        -Wextra
    )
    add_link_options(-flto -O3)
endif()

include(FetchContent)
FetchContent_Declare(
    v4s12_core
    GIT_REPOSITORY https://github.com/garcialeal/V4S12-MATHEMATICAL-CORE.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(v4s12_core)

add_executable(v4s12_benchmark
    src/main_benchmark.cpp
    src/v4s12_engine.cpp
)

target_include_directories(v4s12_benchmark PRIVATE
    include
    ${v4s12_core_SOURCE_DIR}/include
)

target_link_libraries(v4s12_benchmark PRIVATE v4s12 OpenMP::OpenMP_CXX)