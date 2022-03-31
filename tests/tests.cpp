#include <bidirectional_vector.h>
#include <vector>
#include <queue>
#include <set>

#include <gtest/gtest.h>

TEST(bidirectional_vector, construct_empty)
{
	ASSERT_NO_THROW(bidirectional_vector<int>{});

	bidirectional_vector<int> empty_vector;
	ASSERT_TRUE(empty_vector.empty());
	ASSERT_EQ(empty_vector.size(), 0);
	ASSERT_EQ(empty_vector.capacity(), 0);
	ASSERT_NE(empty_vector.max_size(), 0);
}

TEST(bidirectional_vector, construct)
{
	bidirectional_vector<int> default_vector(10);
	ASSERT_FALSE(default_vector.empty());
	ASSERT_EQ(default_vector.size(), 10);
	ASSERT_EQ(default_vector.capacity(), 10);
	ASSERT_EQ(default_vector[0], 0);
	ASSERT_EQ(default_vector.front(), 0);
	ASSERT_EQ(default_vector.back(), 0);

	bidirectional_vector<int> vector_of_copies(42, 7);
	ASSERT_FALSE(vector_of_copies.empty());
	ASSERT_EQ(vector_of_copies.size(), 42);
	ASSERT_EQ(vector_of_copies.capacity(), 42);
	ASSERT_EQ(vector_of_copies[0], 7);
	ASSERT_EQ(vector_of_copies.front(), 7);
	ASSERT_EQ(vector_of_copies.back(), 7);
}

TEST(bidirectional_vector, emplace)
{
	bidirectional_vector<int> empty_vector;
	auto iter = empty_vector.emplace(empty_vector.begin(), 0);
	ASSERT_EQ(iter, empty_vector.begin());
	ASSERT_EQ(empty_vector.size(), 1);
	iter = empty_vector.emplace(empty_vector.end(), 2);
	ASSERT_EQ(iter + 1, empty_vector.end());
	iter = empty_vector.emplace(iter, 1);
	ASSERT_EQ(*iter, 1);
	ASSERT_EQ(*(iter + 1), 2);
	ASSERT_EQ(*(iter - 1), 0);
	ASSERT_EQ(empty_vector.size(), 3);
	empty_vector.clear();
	ASSERT_EQ(empty_vector.size(), 0);
}

TEST(bidirectional_vector, priority_queue)
{
	const auto data = {1,8,5,6,3,4,0,9,7,2};
 
	std::priority_queue<int> q;

    for(int n : data)
        q.push(n);
 
    std::priority_queue<int, bidirectional_vector<int>> bid_q;

	for(int n : data)
        bid_q.push(n);

	ASSERT_FALSE(q.empty());
	ASSERT_FALSE(bid_q.empty());
	ASSERT_EQ(q.size(), bid_q.size());
	ASSERT_EQ(q.top(), bid_q.top());

	q.pop();
	bid_q.pop();

	ASSERT_FALSE(q.empty());
	ASSERT_FALSE(bid_q.empty());
	ASSERT_EQ(q.size(), bid_q.size());
	ASSERT_EQ(q.top(), bid_q.top());
}

// https://stackoverflow.com/a/16220234
// (I'm too lazy to write my own test)

template <typename T>
double run_test(T& pq, int size, int iterations)
{
    struct timespec start, end;

    for(int i = 0; i < size; ++i)
        pq.push(rand());

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    for(int i = 0; i < iterations; ++i)
    {
        if(rand()%2)
            pq.pop();
        else
            pq.push(rand());

    }
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);

    end.tv_sec -= start.tv_sec;
    end.tv_nsec -= start.tv_nsec;
    if (end.tv_nsec < 0)
    {
        --end.tv_sec;
        end.tv_nsec += 1000000000ULL;
    }

    return (end.tv_sec*1e3 + end.tv_nsec/1e6);
}

template <class T>
class multiset_pq : public std::multiset<T>
{
public:
	multiset_pq() : std::multiset<T>() {};
	void push(T elm) { this->insert(elm); }
	void pop()
	{
		if (!this->empty())
			this->erase(this->begin());
	}
	const T& top() { return *this->begin(); }
};

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	auto gtest_result = RUN_ALL_TESTS();

	{
		const int size = 5000;
		const int iterations = 100000;

		std::priority_queue<int, bidirectional_vector<int>> pq_bidv;
		std::priority_queue<int> pqv;
		std::priority_queue<int, std::deque<int>> pqd;
		multiset_pq<int> pqms;

		srand(time(0));

		std::cout << "pq-w-bidirectional_vector: " << run_test(pq_bidv, size, iterations) << "ms" << std::endl;
		std::cout << "pq-w-vector: " << run_test(pqv, size, iterations) << "ms" << std::endl;
		std::cout << "pq-w-deque: " << run_test(pqd, size, iterations) << "ms" << std::endl;
		std::cout << "pq-w-multiset: " << run_test(pqms, size, iterations) << "ms" << std::endl;
	}

	return gtest_result;
}