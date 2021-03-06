

#include "radix.h"
#include "AOS_radix.h"
#include "cpu007_radix.h"
#include "random_generator.h"

#include <random>
#include <cassert>
#include <cstdlib>

#include <boost/sort/spreadsort/spreadsort.hpp>

#include <benchmark/benchmark.h>


using int_t = int;


static auto SEED = std::random_device{}();


void print_vec(const std::vector<int_t>& v)
{
  for (auto& e : v)
  {
    std::cout << e << ", ";
  }
  std::cout << std::endl;
}

bool verify_output(const std::vector<int_t>& v)
{
  return std::accumulate(v.begin(), v.end(), std::numeric_limits<int_t>::lowest(),
      [](const auto& a, const auto& b) {
        assert(a <= b);
        return b;
        });
}

static void RadixBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    radix_sort<int_t>(&vec[0], &vec[state.range(0)]);
    verify_output(vec);
  }

}

static void RadixAOSBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    int_radix_sort(&vec[0], state.range(0));
    verify_output(vec);
  }
}

static void RadixMsbBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    radix_sort<int_t>(&vec[0], &vec[state.range(0)], true);
    verify_output(vec);
  }
}

static void StdSortBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    std::sort(vec.begin(), vec.end());
    verify_output(vec);
  }
}

static void BoostSpreadsortBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    boost::sort::spreadsort::spreadsort(vec.begin(), vec.end());
    verify_output(vec);
  }
}


BENCHMARK(RadixBench)->Unit(benchmark::kMillisecond)
  ->RangeMultiplier(16)->Range(1<<16, 1<<24);
BENCHMARK(RadixAOSBench)->Unit(benchmark::kMillisecond)
  ->RangeMultiplier(16)->Range(1<<16, 1<<24);
BENCHMARK(RadixMsbBench)->Unit(benchmark::kMillisecond)
  ->RangeMultiplier(16)->Range(1<<16, 1<<24);
BENCHMARK(StdSortBench)->Unit(benchmark::kMillisecond)
  ->RangeMultiplier(16)->Range(1<<16, 1<<24);
BENCHMARK(BoostSpreadsortBench)->Unit(benchmark::kMillisecond)
  ->RangeMultiplier(16)->Range(1<<16, 1<<24);

BENCHMARK_MAIN();
