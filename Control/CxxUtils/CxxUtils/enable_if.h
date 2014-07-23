// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/enable_if.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief C++98 definition of enable_if.
 *
 * This header defines @c CxxUtils::enable_if that can be used in both
 * c++98 and c++11; the standard library version is used if it's available.
 *
 * For the c++98 case, we can't just import @c enable_if from boost,
 * since @c std::enable_if actually corresponds to @c boost::enable_if_c.
 * The definition of @c enable_if is simple enough that we just copy it here.
 */


#ifndef CXXUTILS_ENABLE_IF_H
#define CXXUTILS_ENABLE_IF_H


#if __cplusplus > 201100
# include <type_traits>
namespace Cxxutils { using std::enable_if; }
#else

namespace CxxUtils {

  // Primary template.
  /// Define a member typedef @c type only if a boolean constant is true.
  template<bool, typename _Tp = void>
    struct enable_if 
    { };

  // Partial specialization for true.
  template<typename _Tp>
    struct enable_if<true, _Tp>
    { typedef _Tp type; };


} // namespace CxxUtils

#endif


#endif // not CXXUTILS_ENABLE_IF_H
