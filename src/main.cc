#include "flatset.hh"

#include <cassert>
#include <iostream>

using namespace std;

int main(int , char *[])
{
  flat::flat_set<int> s{1, 3, 5};
  assert(s.size() == 3);
  flat::flat_set<int> s2(s);
  assert(s2.size() == 3);
  std::vector<int> t {3, 1, 3};
  flat::flat_set<int> s3 (t.begin(), t.end());
  assert(s3.size() == 2);

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




  cout << "Success!" << endl;
  return 0;
}
