/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file: src/ptree_wrap.cxx
 * @author: Ricardo Abreu
 *
 * @brief Python bindings for boost::property_tree::ptree
 */

#include "ptree_wrap.h"
#include "util.h"
#include "eformat/python/util.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <fstream>

using namespace boost::python;
using namespace boost::property_tree;
using std::string;

namespace
{
  //////////////////////////////////////////////////////////////////////////////
  enum ListTarget {KEYS, VALS, VALS_DATA, ITEMS};

  //////////////////////////////////////////////////////////////////////////////
  void readxml(ptree& p, const string& xml_filename)
  {
    std::ifstream input(xml_filename.c_str());
    int flags = xml_parser::no_comments | xml_parser::trim_whitespace;
    xml_parser::read_xml(input, p, flags);
  }

  // The methods involving a path couldn't be bound to python directly unless
  // we told python how to convert from string to ptree::path_type. We use
  // instead the implicit conversion of string to ptree::path_type in C++

  //////////////////////////////////////////////////////////////////////////////
  ptree& put(ptree& p, const string& path, const string& value)
  {
    return p.put(path, value);
  }

  //////////////////////////////////////////////////////////////////////////////
  ptree& add(ptree& p, const string& path, const string& value)
  {
    return p.add(path, value);
  }

  //////////////////////////////////////////////////////////////////////////////
  ptree& add_child(ptree& p, const string& path, ptree& child)
  {
    return p.add_child(path, child);
  }

  //////////////////////////////////////////////////////////////////////////////
  ptree& put_child(ptree& p, const string& path, ptree& child)
  {
    return p.put_child(path, child);
  }

  //////////////////////////////////////////////////////////////////////////////
  const ptree& get_child(const ptree& p, const string& path)
  {
    boost::optional<const ptree&> ret = p.get_child_optional(path);
    if(!ret)
    {
      PyErr_SetString(PyExc_KeyError, path.c_str());
      boost::python::throw_error_already_set();
    }
    return *ret;
  }

  //////////////////////////////////////////////////////////////////////////////
  const string& get(const ptree& p, const string& path)
  {
    return get_child(p, path).data();
  }

  //////////////////////////////////////////////////////////////////////////////
  void set_data(ptree& p, const string& data)
  {
    p.data() = data;
  }

  //////////////////////////////////////////////////////////////////////////////
  list to_list(const ptree& p, ListTarget target)
  {
    list ret;
    BOOST_FOREACH(const ptree::value_type& val, p)
      switch(target)
      {
      case KEYS:
        ret.append(val.first);
        break;
      case VALS:
        ret.append(val.second);
        break;
      case VALS_DATA:
        ret.append(val.second.data());
        break;
      default:
        ret.append(make_tuple(val.first, val.second));
      }

    return ret;
  }

  //////////////////////////////////////////////////////////////////////////////
  list keys(const ptree& p)
  {
    return to_list(p, KEYS);
  }

  //////////////////////////////////////////////////////////////////////////////
  list values(const ptree& p)
  {
    return to_list(p, VALS);
  }

  //////////////////////////////////////////////////////////////////////////////
  list values_data(const ptree& p)
  {
    return to_list(p, VALS_DATA);
  }

  //////////////////////////////////////////////////////////////////////////////
  list items(const ptree& p)
  {
    return to_list(p, ITEMS);
  }

  //////////////////////////////////////////////////////////////////////////////
  object iter(const ptree& p)
  {
    return object(handle<>(PyObject_GetIter(keys(p).ptr())));
  }

  //////////////////////////////////////////////////////////////////////////////
  bool in(const ptree& p, const string& path)
  {
    return static_cast<bool>(p.get_child_optional(path));
  }
}

////////////////////////////////////////////////////////////////////////////////
void HLTTestApps::wrap_ptree()
{
  const string& (ptree::* const_data)() const = &ptree::data;

  // some of the operations provided below are useful mainly for testing

  class_<ptree> ("ptree", docstr, init<>(docstr))

    // default policy
    .def(init<string>(docstr))
    .def("readxml", readxml, docstr)
    .def("__str__", to_string, docstr)
    .def("__len__", &ptree::size, docstr)
    .def("set_data", set_data, docstr)
    .def("keys", keys, docstr)
    .def("values", values, docstr)
    .def("values_data", values_data, docstr)
    .def("items", items, docstr)
    .def("__iter__", iter, docstr)
    .def("__contains__", in, docstr)

    // Strings are immutable => python does not accept refs => need to copy
    .def("data", const_data, return_value_policy<copy_const_reference>(),docstr)
    .def("__getitem__", get, return_value_policy<copy_const_reference>(),docstr)

    // Can return internal reference
    .def("__setitem__", put, return_internal_reference<>(), docstr)

    // if we need to add an item with a repeated path
    .def("add", add, return_internal_reference<>(), docstr)

    // if we need to add a sub-ptree (possibly with a repeated path)
    .def("add_child", add_child, return_internal_reference<>(), docstr)

    // if we need to add a sub-ptree (replacing existing path)
    .def("put_child", put_child, return_internal_reference<>(), docstr)

    // Can return internal reference. For this one, notice that const doesn't
    // mean anything at the python level, so, in python, the returned ptree
    // can be modified.
    .def("get_child", get_child, return_internal_reference<>(), docstr)
  ;
}

