#ifndef FLATSET_HH_
# define FLATSET_HH_

# include <algorithm>
# include <cassert>
# include <functional>
# include <vector>


namespace flat
{
  template <typename Key,
            typename Compare = std::less<Key>,
            typename Allocator = std::allocator<Key>>
  class flat_set
  {
  public:
    using key_type = Key;
    using value_type = Key;

  private:
    using vect_type = std::vector<key_type>;

  public:
    using size_type = typename vect_type::size_type;
    using difference_type = typename vect_type::difference_type;
    using key_compare = Compare;
    using value_compare = Compare;
    using allocator_type = Allocator;
    using reference = typename Allocator::reference;
    using const_reference = typename Allocator::const_reference;
    using pointer = typename Allocator::pointer;
    using iterator = typename vect_type::const_iterator;
    using const_iterator = typename vect_type::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    explicit flat_set(const Compare& comp = Compare(),
                      const Allocator& alloc = Allocator())
      : vect_(alloc)
      , key_comp_{comp}
    {}

    explicit flat_set(const Allocator& alloc)
      : vect_(alloc)
      , key_comp_{Compare()}
    {}

    flat_set(std::initializer_list<Key> l,
             const Compare& comp = Compare(),
             const Allocator& alloc = Allocator())
      : vect_{l, alloc}
      , key_comp_{comp}
    {
      sort(vect_.begin(), vect_.end(), key_comp());
      vect_.erase(std::unique(vect_.begin(), vect_.end()), vect_.end());
    }

    flat_set(std::initializer_list<Key> l,
             const Allocator& alloc)
      : flat_set(l, Compare(), alloc)
    {}

    template <typename InputIt>
    flat_set(const InputIt& first, const InputIt& last,
             const Compare& comp = Compare(),
             const Allocator& alloc = Allocator())
      : vect_(first, last)
      , key_comp_{comp}
    {
      sort(vect_.begin(), vect_.end(), key_comp());
      vect_.erase(std::unique(vect_.begin(), vect_.end()), vect_.end());
    }

    flat_set(const flat_set& other, const Allocator& alloc)
      : vect_(other.vect_, alloc)
      , key_comp_{other.key_comp()}
    {}

    flat_set(flat_set&& other, const Allocator& alloc)
      : vect_(alloc)
      , key_comp_{other.key_comp()}
    {
      vect_.swap(other.vect_);
    }

    std::pair<iterator,bool>
    insert(const value_type& value)
    {
      assert(std::is_sorted(begin(), end(), key_comp()));
      auto it = std::lower_bound(vect_.begin(), vect_.end(), value,
                                 key_comp());
      if (it != vect_.end() && *it == value)
        return {it, false};
      return {vect_.insert(it, value), true};
    }

    iterator
    find(const key_type& key)
    {
      auto lb = lower_bound(key);
      return lb != vect_.end() && *lb == key ? lb : vect_.end();
    }

    std::pair<iterator, iterator>
    equal_range(const key_type& key)
    {
      return {lower_bound(key), upper_bound(key)};
    }

    iterator
    lower_bound(const Key& key)
    {
      return std::lower_bound(vect_.begin(), vect_.end(), key, key_comp());
    }

    const_iterator
    lower_bound(const Key& key) const
    {
      return std::lower_bound(vect_.begin(), vect_.end(), key, key_comp());
    }

    iterator
    upper_bound(const Key& key)
    {
      return std::upper_bound(vect_.begin(), vect_.end(), key, key_comp());
    }

    const_iterator
    upper_bound(const Key& key) const
    {
      return std::upper_bound(vect_.begin(), vect_.end(), key, key_comp());
    }

    key_compare
    key_comp() const
    {
      return key_comp_;
    }

    value_compare
    value_comp() const
    {
      return key_comp();
    }


  private:
    // Declare early to please decltype
    vect_type vect_;

  public:

    /*-------------------.
    |  Const forwarding  |
    `-------------------*/

# define DEFINE(Name)                                       \
    template <typename... Args>                             \
    auto                                                    \
    Name(Args&&... args) const                              \
      -> decltype(vect_.Name(std::forward<Args>(args)...))  \
    {                                                       \
      return vect_.Name(std::forward<Args>(args)...);       \
    }

    DEFINE(cbegin);
    DEFINE(cend);
    DEFINE(crbegin);
    DEFINE(crend);
    DEFINE(empty);
    DEFINE(size);
    DEFINE(max_size);
    DEFINE(get_allocator);

# undef DEFINE

    /*-----------------------.
    |  Non-const forwarding  |
    `-----------------------*/

# define DEFINE(Name)                                       \
    template <typename... Args>                             \
    auto                                                    \
    Name(Args&&... args)                                    \
      -> decltype(vect_.Name(std::forward<Args>(args)...))  \
    {                                                       \
      return vect_.Name(std::forward<Args>(args)...);       \
    }

    DEFINE(begin);
    DEFINE(end);
    DEFINE(rbegin);
    DEFINE(rend);
    DEFINE(clear);
    DEFINE(erase);
    DEFINE(swap);

# undef DEFINE

# define DEFINE(Name, Op)                                        \
    friend bool Name (const flat_set& lhs, const flat_set& rhs)  \
    {                                                            \
      return lhs.vect_ Op rhs.vect_;                             \
    }

    DEFINE(operator==, ==);
    DEFINE(operator!=, !=);
    DEFINE(operator<, <);
    DEFINE(operator<=, <=);
    DEFINE(operator>, >);
    DEFINE(operator>=, >=);

# undef DEFINE


  private:
    key_compare key_comp_;

  };
}

#endif /* !FLATSET_HH_ */
