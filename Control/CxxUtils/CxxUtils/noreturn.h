// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/noreturn.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Macro to mark a method as not returning.
 *        Expands to the c++11 noreturn attribute if we're using c++11,
 *        otherwise to the GNU noreturn attribute if we're using gcc,
 *        otherwise expands to just the declaration.
 *
 * Use like:
 *
 *   ATH_NORETURN (void func (int a, double b));
 *
 * Be careful if the return type contains commas (in a template argument
 * list) --- you may need a typedef in that case.
 */


#ifndef CXXUTILS_NORETURN_H
#define CXXUTILS_NORETURN_H


// [[noreturn] is c++11.  gcc implemented it in 4.8.
#if __cplusplus > 201100
# if defined(__GNUC__) && !(__GNUC__ > 4 || __GNUC_MINOR__ >= 8)
#  define ATH_HAS_NORETURN 0 // gcc < 4.8 --- not implemented
# else
#  define ATH_HAS_NORETURN 1
# endif
#else
# define ATH_HAS_NORETURN 0  // not c++11
#endif


#if ATH_HAS_NORETURN
# define ATH_NORETURN(X) [[noreturn]] X
#else
# if defined(__GNUC__)
#  define ATH_NORETURN(X) X __attribute__ ((noreturn))
# else
#  define ATH_NORETURN(X) X
# endif
#endif


#endif // not CXXUTILS_NORETURN_H
