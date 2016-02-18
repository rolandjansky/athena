// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/make_unique.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Provide a version of the C++14 make_unique.
 *
 * `std::unique_ptr` is best used with `make_unique`.
 * However, make_unique is not in C++11, only C++14.
 * This header provides a version of `make_unique` in the `CxxUtils`
 * namespace usable with either C++11 or C++14.
 *
 * Here, we only bother with the version of `make_unique` for single objects.
 */


#ifndef CXXUTILS_MAKE_UNIQUE_H
#define CXXUTILS_MAKE_UNIQUE_H


#if __cplusplus > 201100


#include <memory>


namespace CxxUtils {


#if __cplusplus > 201103L
// C++14 --- just import the library implementation.
using std::make_unique;
#else
/// std::make_unique for single objects
// Copied from libstdc++
template<typename Tp, typename... Args>
//inline typename _MakeUniq<_Tp>::__single_object
inline
std::unique_ptr<Tp>
make_unique(Args&&... args)
{ return std::unique_ptr<Tp>(new Tp(std::forward<Args>(args)...)); }
#endif



} // namespace CxxUtils


#endif // __cplusplus > 201100

#endif // not CXXUTILS_MAKE_UNIQUE_H
