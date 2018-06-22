
#pragma once

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>

using namespace std;

template <typename T>
vector<T> random_vector(size_t N, const auto& SEED)
{
    mt19937 mersenne_engine { SEED };  // Generates random integers

    uniform_int_distribution<T> dist{ std::numeric_limits<T>::lowest(),
                                        std::numeric_limits<T>::max() };

    auto gen = [&dist, &mersenne_engine](){
                   return dist(mersenne_engine);
               };


    vector<T> vec(N);
    generate(begin(vec), end(vec), gen);

    return vec;
}
