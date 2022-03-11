#pragma once

#include <iterator>
#include <limits>

template <typename TIterator, typename TContainer>
class normal_iterator
{
private:
	TIterator _current;

	typedef std::iterator_traits<TIterator> traits_type;

public:
	typedef TIterator iterator_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::reference reference;
	typedef typename traits_type::pointer pointer;

	constexpr normal_iterator() noexcept
		: _current(TIterator()) {}

	explicit normal_iterator(const TIterator& i) noexcept
		: _current(i) {}

	// Forward iterator requirements
	reference operator*() const noexcept
	{
		return *_current;
	}

	pointer operator->() const noexcept
	{
		return _current;
	}

	normal_iterator& operator++() noexcept
	{
		++_current;
		return *this;
	}

	normal_iterator operator++(int) noexcept
	{
		return normal_iterator(_current++);
	}

	bool operator==(const normal_iterator& other) const noexcept
	{
		return base() == other.base();
	}

	// Bidirectional iterator requirements
	normal_iterator& operator--() noexcept
	{
		--_current;
		return *this;
	}

	normal_iterator operator--(int) noexcept
	{
		return normal_iterator(_current--);
	}

	// Random access iterator requirements
	reference operator[](difference_type n) const noexcept
	{
		return _current[n];
	}

	normal_iterator& operator+=(difference_type n) noexcept
	{
		_current += n;
		return *this;
	}

	normal_iterator operator+(difference_type n) const noexcept
	{
		return normal_iterator(_current + n);
	}

	normal_iterator& operator-=(difference_type n) noexcept
	{
		_current -= n;
		return *this;
	}

	normal_iterator operator-(difference_type n) const noexcept
	{
		return normal_iterator(_current - n);
	}

	difference_type operator-(const normal_iterator& other) const noexcept
	{
		return base() - other.base();
	}

	const TIterator& base() const noexcept
	{
		return _current;
	}
};

template <typename TValue>
class bidirectional_vector
{
public:
	using value_type = TValue;

	using pointer = TValue *;
	using const_pointer = const TValue *;

	using reference = TValue &;
	using const_reference = const TValue &;

	using iterator = normal_iterator<pointer, bidirectional_vector>;
	using const_iterator = normal_iterator<const_pointer, bidirectional_vector>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

private:
	pointer _begin = nullptr;
	pointer _end = nullptr;
	pointer _begin_of_storage = nullptr;
	pointer _end_of_storage = nullptr;

public:
	// Iterators
	iterator begin() { return iterator(_begin); }
	const_iterator begin() const { return const_iterator(_begin); }
	const_iterator cbegin() const { return const_iterator(_begin); }

	iterator end() { return iterator(_end); }
	const_iterator end() const { return const_iterator(_end); }
	const_iterator cend() const { return const_iterator(_end); }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }

	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

	// Capacity
	[[nodiscard]] bool empty() const { return begin() == end(); }

	std::size_t size() const { return std::distance(begin(), end()); }

	static std::size_t max_size() noexcept
	{
		const std::size_t diffmax = std::numeric_limits<typename iterator::difference_type>::max() / sizeof(value_type);
		return diffmax;
	}

	std::size_t capacity() const noexcept { return _end_of_storage - _begin_of_storage; }
};
