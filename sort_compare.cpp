

#include "radix.h"
#include "radix_msb.h"
#include "random_generator.h"

#include <random>
#include <cassert>
#include <cstdlib>

#include <boost/sort/spreadsort/integer_sort.hpp>

#include <benchmark/benchmark.h>


static long N = 10'000'000;

using T = int;


static auto SEED = random_device{}();


void print_vec(const std::vector<T>& v)
{
  for (auto& e : v)
  {
    std::cout << e << ", ";
  }
  std::cout << std::endl;
}

bool verify_output(const std::vector<T>& v)
{
  return std::accumulate(v.begin(), v.end(), std::numeric_limits<T>::lowest(),
      [](const auto& a, const auto& b) {
        assert(a <= b);
        return b;
        });
}

void fix(std::vector<T>& v)
{
  for (auto it = v.begin(); it != v.end(); ++it)
  {
    if (*it < 0)
    {
      std::rotate(v.begin(), it, v.end());
      return;
    }
  }
}

static void RadixBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<T> vec = random_vector<T>(N, SEED);
    radix_sort<T>(&vec[0], &vec[N]);

    fix(vec);
    verify_output(vec);
  }
}

static void RadixMsbBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<T> vec = random_vector<T>(N, SEED);
    radix_sort_msb(&vec[0], &vec[N]);

    fix(vec);
    verify_output(vec);
  }
}

static void StdSortBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<T> vec = random_vector<T>(N, SEED);
    std::sort(vec.begin(), vec.end());
    verify_output(vec);
  }
}

T compare (const void * a, const void * b)
{
  return ( *(T*)a - *(T*)b );
}

static void BoostIntegerBench(benchmark::State& state) {
  for (auto _ : state)
  {
    std::vector<T> vec = random_vector<T>(N, SEED);
    boost::sort::spreadsort::integer_sort(vec.begin(), vec.end());
    verify_output(vec);
  }
}


BENCHMARK(RadixBench)->Unit(benchmark::kMillisecond);
BENCHMARK(RadixMsbBench)->Unit(benchmark::kMillisecond);
BENCHMARK(StdSortBench)->Unit(benchmark::kMillisecond);
BENCHMARK(BoostIntegerBench)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
