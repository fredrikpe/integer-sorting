
#pragma once

#include <queue>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>

typedef std::deque<int>            deque_type;
typedef std::vector<deque_type>    vector_type;
typedef std::list<int>             list_type;

void radixSort(list_type & values) {

	bool flag = true;
	int divisor = 1;
	
	while (flag) {
		vector_type buckets(10);
		flag = false;

		std::for_each(values.begin(), values.end(), 
									copyIntoBuckets(divisor, buckets_flag));

		std::accumulate(buckets.begin(), buckets.end(), 
										values.begin(), copyList);

		divisor *= 10;
	}

}


list_type::iterator 
copyList(list_type::iterator c, deque_type &lst)
{
	return std::copy(lst.begin(), lst.end(), c);
}


class copyIntoBuckets {

public:
	copyIntoBuckets(int d, vector_type &b, bool &f) 
				: divisor(d), buckets(b), flag(f) {}

	void operator()(unsigned int v) {
		int index = (v / divisor) % 10;

		// flag is set to true if any bucket 
		// other than zeroth is used
		if (index) flag = true; 
		buckets[index].push_back(v);
	}
	 
	int         divisor;
	vector_type &buckets;
	bool        &flag;

};

