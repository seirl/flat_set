#include "flatset.hh"

#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "tuple.hh"

using namespace std;

#define BENCH_(Set_Type, Num, Body, Before)                             \
  {                                                                     \
    using namespace std::chrono;                                        \
    using Set = Set_Type;                                               \
    constexpr auto N = Num;                                             \
    {                                                                   \
      time_point<system_clock> start, end;                              \
      Set s;                                                            \
      Before;                                                           \
      start = system_clock::now();                                      \
      {                                                                 \
        for (size_t i = 0; i < N; i++) {                                \
          Body;                                                         \
        }                                                               \
      }                                                                 \
      end = system_clock::now();                                        \
      std::chrono::duration<double> elapsed_seconds = end - start;      \
      std::cout << elapsed_seconds.count() << "s\n";                    \
    }                                                                   \
  }


#define BENCH(N, Title, Body, Before)             \
  {                                               \
    cout << N << " " Title << endl;               \
    cout << "flat_set: ";                         \
    BENCH_(flat::flat_set<int>, N, Body, Before); \
    cout << "set:      ";                         \
    BENCH_(set<int>, N, Body, Before);            \
    cout << endl;                                 \
  }

int main(int , char *[])
{
  using namespace flat;

  {
    flat_set<int> s{1, 3, 5};
    assert(s.size() == 3);
    flat_set<int> s2(s);
    assert(s2.size() == 3);
    assert(s == s2);

    s.insert(2);
    assert(s.size() == 4);
    assert(std::is_sorted(s.begin(), s.end()));
    s.insert(4);
    assert(s.size() == 5);
    assert(std::is_sorted(s.begin(), s.end()));
    s.insert(0);
    assert(s.size() == 6);
    assert(std::is_sorted(s.begin(), s.end()));
    s.insert(6);
    assert(s.size() == 7);
    assert(std::is_sorted(s.begin(), s.end()));
    s.insert(6);
    assert(s.size() == 7);
    assert(std::is_sorted(s.begin(), s.end()));


    assert(s.find(2) != s.end());
    assert(s.find(6) != s.end());
    assert(s.find(0) != s.end());
    assert(s.find(7) == s.end());

  }

  {
    std::vector<int> t {3, 1, 3};
    flat_set<int> s3 (t.begin(), t.end());
    assert(s3.size() == 2);

    assert(*s3.lower_bound(1) == 1);
    assert(*s3.lower_bound(2) == 3);
    assert(*s3.lower_bound(3) == 3);
    assert(*s3.lower_bound(0) == 1);
    assert(s3.lower_bound(6) == s3.end());

    assert(*s3.upper_bound(1) == 3);
    assert(*s3.upper_bound(2) == 3);
    assert(s3.upper_bound(3) == s3.end());
    assert(*s3.upper_bound(0) == 1);
    assert(s3.upper_bound(6) == s3.end());
  }
  {
    flat_set<int> s4{flat_set<int>{3, 5, 6}};
    assert(s4.size() == 3);
  }

#ifndef DNDEBUG
  cout << "All tests passed!" << endl;
#endif

  cout << "Benchmark:" << endl << endl;

  BENCH(5000000, "insertions, ordered", s.insert(i),);
  BENCH(50000, "insertions, reverse", s.insert(-i),);
  BENCH(200000, "insertions, reverse", s.insert(-i),);
  srand(time(0));
  BENCH(300000, "insertions, random(MAX_INT)", s.insert(rand()),);
  BENCH(20000000, "insertions, random(1000)", s.insert(rand() % 1000),);
  BENCH(2000000, "random lookups, full array of 1000000",
        s.find(rand() % 1000000), for(int i=0;i<1000000;++i)s.insert(i));
  BENCH(5000000, "random lookups, sparse array (10 % of 1000000)",
        s.find(rand() % 1000000), for(int i=0;i<1000000;i+=10)s.insert(i));
  BENCH(10000000, "creations from initializer_list of size 1000000, ordered",
        create_initializer_list_<Set>(seq),
        auto seq = index_sequence<1000000>());
  BENCH(10000000, "creations from initializer_list of size 1000000, reverse",
        create_initializer_list_reverse_<Set>(seq),
        auto seq = index_sequence<1000000>());
  BENCH(10000000, "creations from initializer_list of size 1000000, random(10000)",
        create_initializer_list_random_<Set>(seq),
        auto seq = index_sequence<1000000>());

  return 0;
}
