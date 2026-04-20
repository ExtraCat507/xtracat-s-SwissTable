#include <benchmark/benchmark.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "../my_swiss_table.h"

template <typename T>
std::vector<T> GenerateData(size_t n);

template <>
std::vector<int> GenerateData<int>(size_t n) {
  std::vector<int> v(n);
  std::iota(v.begin(), v.end(), 0);
  std::mt19937 rng{std::random_device{}()};
  std::shuffle(v.begin(), v.end(), rng);
  return v;
}

template <>
std::vector<double> GenerateData<double>(size_t n) {
  std::vector<double> v(n);
  std::mt19937 rng{std::random_device{}()};
  std::uniform_real_distribution<double> dist(0.0, 1e6);
  for (auto& x : v) {
    x = dist(rng);
  }
  return v;
}

template <>
std::vector<std::string> GenerateData<std::string>(size_t n) {
  std::vector<std::string> v(n);
  for (size_t i = 0; i < n; ++i) {
    v[i] = "str_" + std::to_string(i);
  }
  std::mt19937 rng{std::random_device{}()};
  std::shuffle(v.begin(), v.end(), rng);
  return v;
}

// ------------------ Benchmarking Sizes ------------------
// Set identical sizes to your original script
static void CustomSizes(benchmark::internal::Benchmark* b) {
  b->Arg(1000)->Arg(5000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(1000000)->Arg(10000000);
}

// ------------------ 1. Insert Metric ------------------
template <typename Container, typename T>
static void BM_Insert(benchmark::State& state) {
  size_t n = state.range(0);
  auto data = GenerateData<T>(n);

  for (auto _ : state) {
    // Pause timing while we recreate an empty container
    // so we ONLY measure the raw insertions!
    state.PauseTiming();
    Container table;
    state.ResumeTiming();

    for (const auto& x : data) {
      table.insert(x);
    }
    benchmark::ClobberMemory();  // Force compiler to execute
  }
}

// ------------------ 2. Find Metric ------------------
template <typename Container, typename T>
static void BM_Find(benchmark::State& state) {
  size_t n = state.range(0);
  auto data = GenerateData<T>(n);

  // Setup the table BEFORE timing starts
  Container table;
  for (const auto& x : data) {
    table.insert(x);
  }

  for (auto _ : state) {
    for (const auto& x : data) {
      benchmark::DoNotOptimize(table.find(x));
    }
    benchmark::ClobberMemory();
  }
}

// ------------------ 3. Erase Metric ------------------
template <typename Container, typename T>
static void BM_Erase(benchmark::State& state) {
  size_t n = state.range(0);
  auto data = GenerateData<T>(n);

  for (auto _ : state) {
    // Pause timing: We have to completely repopulate the table
    // before we can measure erasing it again
    state.PauseTiming();
    Container table;
    for (const auto& x : data) {
      table.insert(x);
    }
    state.ResumeTiming();

    for (const auto& x : data) {
      table.erase(x);
    }
    benchmark::ClobberMemory();
  }
}

// --------------------------------------------------
// STRUCTURE 1: SwissTable
// --------------------------------------------------
BENCHMARK_TEMPLATE(BM_Insert, SwissTable<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, SwissTable<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, SwissTable<int>, int)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, SwissTable<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, SwissTable<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, SwissTable<double>, double)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, SwissTable<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, SwissTable<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, SwissTable<std::string>, std::string)->Apply(CustomSizes);

// --------------------------------------------------
// STRUCTURE 2: std::unordered_set
// --------------------------------------------------
BENCHMARK_TEMPLATE(BM_Insert, std::unordered_set<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::unordered_set<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::unordered_set<int>, int)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, std::unordered_set<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::unordered_set<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::unordered_set<double>, double)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, std::unordered_set<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::unordered_set<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::unordered_set<std::string>, std::string)->Apply(CustomSizes);

// --------------------------------------------------
// STRUCTURE 3: std::set
// --------------------------------------------------
BENCHMARK_TEMPLATE(BM_Insert, std::set<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::set<int>, int)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::set<int>, int)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, std::set<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::set<double>, double)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::set<double>, double)->Apply(CustomSizes);

BENCHMARK_TEMPLATE(BM_Insert, std::set<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Find, std::set<std::string>, std::string)->Apply(CustomSizes);
BENCHMARK_TEMPLATE(BM_Erase, std::set<std::string>, std::string)->Apply(CustomSizes);

BENCHMARK_MAIN();
