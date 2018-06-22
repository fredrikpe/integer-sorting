
#pragma once

#include <cstdlib>
#include <algorithm>

template <typename T>
void radix_sort(T *begin, T *end)
{
  auto *begin1 = new T[end - begin];
  auto *end1 = begin1 + (end - begin);

  for (T shift = 0; shift < 32; shift += 8)
  {
    size_t count[0x100] = {};

    for (T *p = begin; p != end; p++)
      count[(*p >> shift) & 0xFF]++;

    T* bucket[0x100];
    auto *q = begin1;
    for (int i = 0; i < 0x100; q += count[i++])
      bucket[i] = q;

    for (T *p = begin; p != end; p++)
      *bucket[(*p >> shift) & 0xFF]++ = *p;

    std::swap(begin, begin1);
    std::swap(end, end1);
  }
  delete[] begin1;
}
