#include <bidirectional_vector.h>
#include <vector>
#include <queue>

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

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}