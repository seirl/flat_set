#include <boost/python.hpp>

#include "flatset.hh"

using fs = flat::flat_set<int, std::less<int>, std::allocator<int>>;
std::pair<fs::iterator, bool> (fs::*ins_val)(const fs::value_type&) = &fs::insert;

fs::const_iterator (fs::*cfind)(const fs::key_type&) = &fs::find;
fs::iterator (fs::*find)(const fs::key_type&) = &fs::find;

fs::const_iterator (fs::*clower)(const fs::key_type&) = &fs::lower_bound;
fs::iterator (fs::*lower)(const fs::key_type&) = &fs::lower_bound;
fs::const_iterator (fs::*cupper)(const fs::key_type&) = &fs::upper_bound;
fs::iterator (fs::*upper)(const fs::key_type&) = &fs::upper_bound;

std::pair<fs::const_iterator, fs::const_iterator> (fs::*ceqrange)(const fs::key_type&) = &fs::equal_range;
std::pair<fs::iterator, fs::iterator> (fs::*eqrange)(const fs::key_type&) = &fs::equal_range;

BOOST_PYTHON_MODULE(flatset)
{
  namespace bp = boost::python;

  bp::class_<fs>
    ("flat_int_set")
    //.def("__len__", &fs::size)
    .def("__contains__", &fs::count)
    .def("insert", ins_val)
    //.def("erase", &fs::erase, erase())
    //.def("emplace", &fs::emplace)
    .def("find", cfind)
    .def("find", find)
    .def("count", &fs::count)
    .def("lower_bound", clower)
    .def("lower_bound", lower)
    .def("upper_bound", cupper)
    .def("upper_bound", upper)
    .def("equal_range", ceqrange)
    .def("equal_range", eqrange)
    .def("swap", &fs::swap)
    .def("key_comp", &fs::key_comp)
    .def("value_comp", &fs::value_comp)
    ;
}

