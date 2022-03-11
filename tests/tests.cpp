#include <bidirectional_vector.h>

#include <gtest/gtest.h>

TEST(bidirectional_vector, construct)
{
	ASSERT_NO_THROW(bidirectional_vector<int>{});

	bidirectional_vector<int> empty_vector;
	ASSERT_TRUE(empty_vector.empty());
	ASSERT_EQ(empty_vector.size(), 0);
	ASSERT_EQ(empty_vector.capacity(), 0);
	ASSERT_NE(empty_vector.max_size(), 0);
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}