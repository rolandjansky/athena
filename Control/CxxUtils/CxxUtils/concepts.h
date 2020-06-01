// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/concepts.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Compatibility helpers for using some pieces of C++20
 *        concepts with older compilers.
 *
 * Use ATH_REQUIRES for a requires clause:
 *@code
 *  template <class T>
 *    ATH_REQUIRES( std::assignable_from<T, float> )
 *  void foo (T x);
 @endcode
 *
 * The body of the ATH_REQUIRES will be hidden for compilers
 * that don't support concepts.
 */


#ifndef CXXUTILS_CONCEPTS_H
#define CXXUTILS_CONCEPTS_H


#ifdef __cpp_concepts

#include <concepts>
#define ATH_REQUIRES(...) requires __VA_ARGS__

#else

#define ATH_REQUIRES(...)

#endif


#endif // not CXXUTILS_CONCEPTS_H
