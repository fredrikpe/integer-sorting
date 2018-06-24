
#pragma once

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>


template <typename T>
std::vector<T> random_vector(size_t N, const std::random_device::result_type& SEED)
{
    std::mt19937 mersenne_engine { SEED };

    std::uniform_int_distribution<T> dist{ std::numeric_limits<T>::lowest(),
                                        std::numeric_limits<T>::max() };

    auto gen = [&dist, &mersenne_engine](){
                   return dist(mersenne_engine);
               };

    std::vector<T> vec(N);

    std::generate(vec.begin(), vec.end(), gen);

    return vec;
}
