

#include "radix.h"
#include "random_generator.h"

#include <random>
#include <cassert>
#include <cstdlib>

#include <boost/sort/spreadsort/integer_sort.hpp>

#include <benchmark/benchmark.h>


static long N = 1000'000;

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

static void RadixMsbBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    radix_sort(&vec[0], &vec[state.range(0)], true);
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

int_t compare (const void * a, const void * b)
{
  return ( *(int_t*)a - *(int_t*)b );
}

static void BoostIntegerBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<int_t> vec = random_vector<int_t>(state.range(0), SEED);
    boost::sort::spreadsort::integer_sort(vec.begin(), vec.end());
    verify_output(vec);
  }
}


BENCHMARK(RadixBench)->Unit(benchmark::kMillisecond)->RangeMultiplier(16)->Range(1<<15, 1<<24);
BENCHMARK(RadixMsbBench)->Unit(benchmark::kMillisecond)->RangeMultiplier(16)->Range(1<<15, 1<<24);
BENCHMARK(StdSortBench)->Unit(benchmark::kMillisecond)->RangeMultiplier(16)->Range(1<<15, 1<<24);
BENCHMARK(BoostIntegerBench)->Unit(benchmark::kMillisecond)->RangeMultiplier(16)->Range(1<<15, 1<<24);

BENCHMARK_MAIN();
