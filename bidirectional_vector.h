#pragma once

#include <iterator>
#include <limits>
#include <memory>

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

	// Iterator to const_iterator conversion
	template <typename TIter>
	normal_iterator(const normal_iterator<TIter,
					typename std::enable_if<
						(std::is_same<TIter, typename TContainer::pointer>::value),
						TContainer>::type>& i) noexcept
		: _current(i.base()) {}

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
	~bidirectional_vector()
	{
		destroy(_begin, _end);
		deallocate(_begin_of_storage);
	}

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

	// Modifiers
	void clear()
	{
		destroy(_begin, _end);
		_end = _begin;
	}

	template <typename... Args>
	iterator emplace(const_iterator position, Args &&...args)
	{
		const auto n = position - begin();
		if (_begin != _begin_of_storage && (static_cast<std::size_t>(n) < (size() / 2)))
		{
			if (position == begin())
			{
				construct(_begin - 1, std::forward<Args>(args)...);
				--_begin;
			}
			else
			{
				insert_with_left_shift(begin() + n, std::forward<Args>(args)...);
			}
		}
		else if (_end != _end_of_storage)
		{
			if (position == end())
			{
				construct(_end, std::forward<Args>(args)...);
				++_end;
			}
			else
			{
				insert_with_right_shift(begin() + n, std::forward<Args>(args)...);
			}
		}
		else
		{
			realloc_insert(begin() + n, std::forward<Args>(args)...);
		}

		return iterator(_begin + n);
	}

private:
	[[nodiscard]] pointer allocate(std::size_t n)
	{
		// TODO: check max_size()
		const auto align = static_cast<std::align_val_t>(alignof(value_type));
		return static_cast<pointer>(::operator new(n * sizeof(value_type), align));
	}

	void deallocate(pointer p) noexcept
	{
		::operator delete(p, static_cast<std::align_val_t>(alignof(value_type)));
	}

	template <typename... Args>
	void construct(pointer value_ptr, Args &&...args) noexcept(
		noexcept(::new (static_cast<void *>(value_ptr)) value_type(std::forward<Args>(args)...)))
	{
		::new (static_cast<void *>(value_ptr)) value_type(std::forward<Args>(args)...);
	}

	void destroy(pointer value_ptr)
	{
		std::destroy_n(value_ptr, 1);
	}

	void destroy(pointer begin, pointer end)
	{
		std::destroy(begin, end);
	}

	std::size_t check_len(std::size_t len_to_add) const
	{
		if (max_size() - size() < len_to_add)
		{
			throw std::length_error("bidirectional_vector: check_len");
		}

		const std::size_t new_len = size() + std::max(size(), len_to_add);
		return (new_len < size() || new_len > max_size()) ? max_size() : new_len;
	}

	template <typename... Args>
	void realloc_insert(iterator position, Args &&...args)
	{
		// TODO: maybe it's better to create tmp_object and swap?
		pointer old_begin = _begin;
		pointer old_end = _end;

		const std::size_t new_len = check_len(1);
		const std::size_t elems_before = position - begin();

		const pointer new_begin_of_storage = allocate(new_len);
		const pointer new_begin = new_begin_of_storage + (new_len / 4); // [empty = n/4 | data = n/2 | empty = n/4]
		pointer new_end = nullptr;

		const pointer new_position = new_begin + elems_before;

		try
		{
			construct(new_begin + elems_before, std::forward<Args>(args)...);

			new_end = std::uninitialized_move(old_begin, position.base(), new_begin);
			++new_end;
			new_end = std::uninitialized_move(position.base(), old_end, new_end);
		}
		catch (...)
		{
			if (!new_end)
			{
				destroy(new_position);
			}
			else
			{
				destroy(new_begin, new_end);
			}

			deallocate(new_begin);
			throw;
		}

		destroy(old_begin, old_end);
		deallocate(_begin_of_storage);

		_begin_of_storage = new_begin_of_storage;
		_end_of_storage = new_begin_of_storage + new_len;
		_begin = new_begin;
		_end = new_end;
	}

	template <typename Arg>
	void insert_with_left_shift(iterator position, Arg &&arg)
	{
		construct(_begin - 1, std::move(*(_begin)));
		--_begin;

		std::move(_begin + 2, position.base(), _begin + 1);

		*(--position) = std::forward<Arg>(arg);
	}

	template <typename Arg>
	void insert_with_right_shift(iterator position, Arg &&arg)
	{
		construct(_end, std::move(*(_end - 1)));
		++_end;

		std::move_backward(position.base(), _end - 2, _end - 1);

		*position = std::forward<Arg>(arg);
	}
};
