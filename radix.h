
#pragma once

#include <cstdlib>
#include <algorithm>

namespace {

template <typename T>
void _radix_lsb(T* begin, T* end, T* buf_begin, size_t max_shift = 8 * (sizeof(T) - 1))
{
  auto* buf_end = buf_begin + (end - begin);

  T* bucket[0x100];

  for (T shift = 0; shift <= max_shift; shift += 8)
  {
    size_t count[0x100] = {};

    for (T *p = begin; p != end; p++)
      count[(*p >> shift) & 0xFF]++;

    auto *q = buf_begin;
    for (int i = 0; i < 0x100; q += count[i++])
      bucket[i] = q;

    for (T *p = begin; p != end; p++)
      *bucket[(*p >> shift) & 0xFF]++ = *p;

    std::swap(begin, buf_begin);
    std::swap(end, buf_end);
  }
}

template <typename T>
void _radix_one_msb_pass_first(T *begin, T *end, T *buf_begin, size_t shift = 8 * (sizeof(T) - 1))
{
	auto *buf_end = buf_begin + (end - begin);

	size_t count[0x100] = {};
	for (T *p = begin; p != end; p++)
		count[(*p >> shift) & 0xFF]++;
  
  T* bucket[0x100];
  T* obucket[0x100];
  auto* q = buf_begin;

	for (int i = 0; i < 0x100; q += count[i++])
		obucket[i] = bucket[i] = q;

	for (T *p = begin; p != end; p++)
		*bucket[(*p >> shift) & 0xFF]++ = *p;

	for (int i = 0; i < 0x100; ++i)
		_radix_lsb(obucket[i], bucket[i], begin + (obucket[i] - buf_begin), shift - 8);
}

} // namespace 


template <typename UInt,
          typename std::enable_if<std::is_unsigned<UInt>::value, UInt>::type = 0>
void radix_sort(UInt *begin, UInt *end, bool one_msb_pass = false)
{
  UInt* buf_begin = new UInt[end - begin];

  if (one_msb_pass)
    _radix_one_msb_pass_first(begin, end, buf_begin);
  else
    _radix_lsb(begin, end, buf_begin);

  delete[] buf_begin;
}

template <typename Int,
          typename std::enable_if<std::is_signed<Int>::value, Int>::type = 0>
void radix_sort(Int *begin, Int *end, bool one_msb_pass = false)
{
  radix_sort(reinterpret_cast<typename std::make_unsigned<Int>::type*>(begin),
             reinterpret_cast<typename std::make_unsigned<Int>::type*>(end),
             one_msb_pass);

  const auto first_negative = std::upper_bound(begin, end, std::numeric_limits<Int>::max(),
      [](Int a, Int b) {
        return static_cast<typename std::make_unsigned<Int>::type>(a) <
               static_cast<typename std::make_unsigned<Int>::type>(b);
      });

  std::rotate(begin, first_negative, end);
}


