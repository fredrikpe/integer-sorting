
#pragma once

#include <cstdlib>
#include <utility>


template <typename T>
void _radix_sort_lsb(T *begin, T *end, T *begin1, T maxshift)
{
	T *end1 = begin1 + (end - begin);

	for (T shift = 0; shift <= maxshift; shift += 8)
	{
		size_t count[0x100] = {};
    
		for (T *p = begin; p != end; p++)
			count[(*p >> shift) & 0xFF]++;

		T *bucket[0x100], *q = begin1;
		for (int i = 0; i < 0x100; q += count[i++])
			bucket[i] = q;

		for (T *p = begin; p != end; p++)
			*bucket[(*p >> shift) & 0xFF]++ = *p;

		std::swap(begin, begin1);
		std::swap(end, end1);
	}
}

template <typename T>
void _radix_sort_msb(T *begin, T *end, T *begin1, T shift)
{
	T *end1 = begin1 + (end - begin);

	size_t count[0x100] = {};
	for (T *p = begin; p != end; p++)
		count[(*p >> shift) & 0xFF]++;
	T *bucket[0x100], *obucket[0x100], *q = begin1;
	for (int i = 0; i < 0x100; q += count[i++])
		obucket[i] = bucket[i] = q;
	for (T *p = begin; p != end; p++)
		*bucket[(*p >> shift) & 0xFF]++ = *p;
	for (int i = 0; i < 0x100; ++i)
		_radix_sort_lsb(obucket[i], bucket[i], begin + (obucket[i] - begin1), shift - 8);
}

template <typename T>
void radix_sort_msb(T *begin, T *end)
{
	T *begin1 = new T[end - begin];
	_radix_sort_msb(begin, end, begin1, 24);
	delete[] begin1;
}
