// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PyROOTIteratorFuncs.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2010
 * @brief Provide functions for iterator comparisons.
 *
 * If an iterator type does not define comparison operators as members,
 * but instead as free functions, then it can be difficult to generate
 * dictionaries for these in a way that pyroot will see them.
 *
 * This header defines a class RootUtils::PyROOTIteratorFuncs<T>,
 * which contains comparison functions for an iterator type T.
 * Further, when you load the PyROOTFixes module, we set things
 * up so that when an iterator class is made in pyroot, any comparison
 * functions defined in a PyROOTIteratorFuncs class will be
 * automatically added to the python class.
 *
 * So to get the proper comparison operators for iterators in python,
 * you should generate a dictionary for PyROOTIteratorFuncs<T>
 * (and make sure PyROOTFixes is loaded).
 */


#ifndef ROOTUTILS_PYROOTITERATORFUNCS_H
#define ROOTUTILS_PYROOTITERATORFUNCS_H


#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"
#include <iterator>


namespace RootUtils {


/**
 * @brief Provide functions for iterator comparisons.
 */
template <class T>
class PyROOTIteratorFuncs
{
public:
  /// Equality comparisons.  All iterators have this.
  static bool eq (T a, T b) { return a == b; }
  static bool ne (T a, T b) { return a != b; }


  /// Ordering comparison.  Only supported by random access iterators.
  static typename boost::enable_if<
    boost::is_base_of<std::random_access_iterator_tag,
                      typename std::iterator_traits<T>::iterator_category>,
     bool>::type
  lt (T a, T b) { return a < b; }
};


} // namespace RootUtils


#endif // not ROOTUTILS_PYROOTITERATORFUNCS_H
