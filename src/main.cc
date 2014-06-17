#include "flatset.hh"

#include <cassert>
#include <iostream>

using namespace std;

int main(int , char *[])
{
  flat::flat_set<int> s{1, 2, 3};
  assert(s.size() == 3);
  flat::flat_set<int> s2(s);
  assert(s2.size() == 3);
  std::vector<int> t {3, 1, 2};
  flat::flat_set<int> s3 (t.begin(), t.end());
  assert(s3.size() == 3);

  cout << "Success!" << endl;
  return 0;
}
