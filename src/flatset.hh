#ifndef FLATSET_HH_
# define FLATSET_HH_

# include <algorithm>
# include <cassert>
# include <functional>
# include <vector>

/*-------------------------------------------------------.
| Flat_set.                                              |
|                                                        |
| This class implements the same interface as std::set,  |
| but uses a vector for the storage of the keys.         |
`-------------------------------------------------------*/

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
      sort(begin(), end(), key_comp());
      vect_.erase(std::unique(begin(), end()), end());
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
      sort(begin(), end(), key_comp());
      vect_.erase(std::unique(begin(), end()), end());
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
      if (it != vect_.end() && key_equal_(*it, value))
        return {it, false};
      return {vect_.insert(it, value), true};
    }

    std::pair<iterator,bool>
    insert(value_type&& value)
    {
      assert(std::is_sorted(begin(), end(), key_comp()));
      auto it = std::lower_bound(vect_.begin(), vect_.end(), value,
                                 key_comp());
      if (it != vect_.end() && key_equal_(*it, value))
        return {it, false};
      return {vect_.insert(it, std::move(value)), true};
    }

    std::pair<iterator,bool>
    insert(const_iterator position, const value_type& value)
    {
      if (position != end() && key_comp()(*position, value))
      {
        if (++position != end() && key_comp()(value, *position))
          return {vect_.emplace(--position, value), true};
      }
      return insert(value);
    }

    std::pair<iterator,bool>
    insert(const_iterator position, value_type&& value)
    {
      if (position != end() && key_comp()(*position, value))
      {
        if (++position == end() || key_comp()(value, *position))
          return {vect_.emplace(--position, value), true};
      }
      return insert(std::move(value));
    }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
      vect_.insert(end(), first, last);
      sort(begin(), end(), key_comp());
      vect_.erase(std::unique(begin(), end()), end());
    }

    void
    insert(std::initializer_list<value_type> il)
    {
      vect_.insert(end(), il);
      sort(begin(), end(), key_comp());
      vect_.erase(std::unique(begin(), end()), end());
    }

    iterator
    erase(const_iterator position)
    {
      return vect_.erase(position);
    }

    size_type
    erase(const value_type& val)
    {
      auto f = find(val);
      if (f == end())
        return 0;
      erase(f);
      return 1;
    }

    iterator
    erase(const_iterator first, const_iterator last)
    {
      return vect_.erase(first, last);
    }

    template <typename... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args)
    {
      value_type v(std::forward<Args>(args)...);
      auto lb = lower_bound(v);
      if (lb != end() && key_equal_(*lb, v))
        return {end(), false};
      return {vect_.emplace(lb, std::move(v)), true};
    }

    template <typename... Args>
    std::pair<iterator, bool>
    emplace_hint(const_iterator position, Args&&... args)
    {
      value_type v(std::forward<Args>(args)...);
      if (position != end())
      {
        if (key_comp()(*position, v))
        {
          if (++position != end() && key_comp()(v, *position))
            return {vect_.emplace(position, std::move(v)), true};
        }
        else if (key_comp()(v, *position))
        {
          if (--position != end() && key_comp()(v, *position))
            return {vect_.emplace(++position, std::move(v)), true};
        }
        else
          return {end(), false};
      }
      auto lb = lower_bound(v);
      if (lb != end() && key_equal_(*lb, v))
        return {end(), false};
      return {vect_.emplace(lb, std::move(v)), true};
    }


    iterator
    find(const key_type& key)
    {
      auto lb = lower_bound(key);
      return lb != end() && key_equal_(*lb, key) ? lb : end();
    }

    const_iterator
    find(const key_type& key) const
    {
      auto lb = lower_bound(key);
      return lb != cend() && key_equal_(*lb, key) ? lb : cend();
    }

    size_type
    count(const value_type& val) const
    {
      return find(val) != cend() ? 1 : 0;
    }

    std::pair<iterator, iterator>
    equal_range(const key_type& key)
    {
      return {lower_bound(key), upper_bound(key)};
    }

    std::pair<const_iterator, const_iterator>
    equal_range(const key_type& key) const
    {
      return {lower_bound(key), upper_bound(key)};
    }

    iterator
    lower_bound(const Key& key)
    {
      return std::lower_bound(begin(), end(), key, key_comp());
    }

    const_iterator
    lower_bound(const Key& key) const
    {
      return std::lower_bound(cbegin(), cend(), key, key_comp());
    }

    iterator
    upper_bound(const Key& key)
    {
      return std::upper_bound(begin(), end(), key, key_comp());
    }

    const_iterator
    upper_bound(const Key& key) const
    {
      return std::upper_bound(cbegin(), cend(), key, key_comp());
    }

    void
    swap(flat_set& s)
    {
      vect_.swap(s.vect_);
      std::swap(key_comp_, s.key_comp_);
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

    bool
    key_equal_(const value_type& lhs, const value_type& rhs) const
    {
      return !key_comp()(lhs, rhs) && !key_comp()(rhs, lhs);
    }

    key_compare key_comp_;

  };
}

#endif /* !FLATSET_HH_ */
