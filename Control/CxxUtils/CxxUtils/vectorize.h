// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/vectorize.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Helper to enable auto-vectorization.
 *
 * Athena is usually built with -O2, which doesn't fully enable
 * autovectorization in gcc (it does in clang).
 *
 * Add
 * ATH_ENABLE_VECTORIZATION;
 * at the start of a compilation unit
 * to enable it for this file.
 *
 * Add
 * ATH_ENABLE_FUNCTION_VECTORIZATION
 * before a function to enable it for just
 * this function
 */

#ifndef CXXUTILS_VECTORIZE_H
#define CXXUTILS_VECTORIZE_H

#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && !defined(__COVERITY__) &&  \
  !defined(__CUDACC__)
# define ATH_ENABLE_VECTORIZATION                                                              \
  _Pragma("GCC optimize (\"tree-vectorize\")") class ATH_ENABLE_VECTORIZATION_SWALLOW_SEMICOLON
#else
# define ATH_ENABLE_VECTORIZATION class ATH_ENABLE_VECTORIZATION_SWALLOW_SEMICOLON
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && !defined(__COVERITY__) &&  \
  !defined(__CUDACC__)
# define ATH_ENABLE_FUNCTION_VECTORIZATION __attribute__((optimize("tree-vectorize")))
#else
# define ATH_ENABLE_FUNCTION_VECTORIZATION
#endif

#endif // not CXXUTILS_VECTORIZE_H
