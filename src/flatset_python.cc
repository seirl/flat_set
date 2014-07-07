#include <boost/python.hpp>
#include <iostream>

#include "flatset.hh"

namespace bp = boost::python;

class flatset_wrapper
{
  using object_t = bp::object;
  using fs = flat::flat_set<object_t>;

private:
  fs flatset;

public:
  void add(const object_t& b)
  {
    flatset.insert(b);
  }

  bool contains(const object_t& b)
  {
    return flatset.find(b) != flatset.end();
  }

  void clear()
  {
    flatset.clear();
  }

  bool len()
  {
    return flatset.size();
  }

  void remove(const object_t& b)
  {
    flatset.erase(b);
  }

  std::string repr()
  {
    std::stringstream os;
    os << "flatset([";
    int first = true;
    for (const auto& o: flatset)
    {
      os << (first ? "" : ", ");
      os << std::string(bp::extract<std::string>(o.attr("__repr__")()));
      first = false;
    }
    os << "])";
    return os.str();
  }

  auto begin() -> decltype(fs().begin()) { return flatset.begin(); }
  auto end() -> decltype(fs().end()) { return flatset.end(); }

  using iterator = fs::iterator;
};

BOOST_PYTHON_MODULE(flatset)
{
  using fw = flatset_wrapper;

  bp::class_<flatset_wrapper>
    ("flatset")
    .def("__contains__", &fw::contains)
    .def("__iter__", bp::iterator<fw>())
    .def("__repr__", &fw::repr)
    .def("__str__", &fw::repr)
    .def("__len__", &fw::len)
    .def("add", &fw::add)
    .def("clear", &fw::clear)
    .def("remove", &fw::remove)
    ;
}

