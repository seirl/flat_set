#ifndef TUPLE_HH_
# define TUPLE_HH_


template<int...> struct index_sequence
{ using type = index_sequence; };

template<class S1, class S2> struct concat;

template<int... I1, int... I2>
struct concat<index_sequence<I1...>, index_sequence<I2...>>
  : index_sequence<I1..., (sizeof...(I1)+I2)...>{};

template<class S1, class S2>
using Concat = typename concat<S1, S2>::type;

template<int N> struct make_index_sequence;
template<int N> using GenSeq =
  typename make_index_sequence<N>::type;

template<int N>
struct make_index_sequence : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

template<> struct make_index_sequence<0> : index_sequence<>{};
template<> struct make_index_sequence<1> : index_sequence<0>{};

template <typename Set, int... I>
Set
create_initializer_list_(index_sequence<I...>)
{
  return Set{I...};
}

template <typename Set, int... I>
Set
create_initializer_list_reverse_(index_sequence<I...>)
{
  return Set{-I...};
}

template <typename Set, int... I>
Set
create_initializer_list_random_(index_sequence<I...>)
{
  return Set{(0 * I + rand() % 10000)...};
}


#endif /* !TUPLE_HH_ */
