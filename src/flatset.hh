#ifndef FLATSET_HH_
# define FLATSET_HH_

# include <algorithm>
# include <functional>
# include <vector>


#include <iostream>

namespace flat
{
  template <typename Key,
           typename Compare = std::less<Key>>
  class flat_set
  {
  public:
    using key_type = Key;
    using value_type = Key;
    using size_type = std::size_t;
    using key_compare = Compare;
    using value_compare = Compare;
  private:
    using vect_type = std::vector<key_type>;
  public:
    using iterator = typename vect_type::const_iterator;
    using const_iterator = typename vect_type::const_iterator;

    flat_set()
    {}

    flat_set(std::initializer_list<Key> l)
      : vect_{l}
    {
      sort(vect_.begin(), vect_.end());
    }

    template <typename InputIt>
    flat_set(const InputIt& first, const InputIt& last)
      : vect_()
    {
      vect_.reserve(last - first);
      for (InputIt i = first; i != last; ++i)
        vect_.push_back(*i);
      sort(vect_.begin(), vect_.end());
    }

  private:
    vect_type vect_;

  public:

    std::pair<iterator,bool>
    insert(const value_type& value)
    {
      auto it = find(value);
      if (it != vect_.end())
        return {vect_.push_back(value), true};
      return {it, false};
    }

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


  };
}

#endif /* !FLATSET_HH_ */
