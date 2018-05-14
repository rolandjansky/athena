// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/features.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Some additional feature test macros.
 */


#ifndef CXXUTILS_FEATURES_H
#define CXXUTILS_FEATURES_H


/// Do we have function multiversioning?
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC)
# define HAVE_FUNCTION_MULTIVERSIONING 1
#else
# define HAVE_FUNCTION_MULTIVERSIONING 0
#endif


/// Do we have the bit-counting intrinsics?
// __builtin_ctz
// __builtin_ctzl
// __builtin_ctzll
// __builtin_clz
// __builtin_clzl
// __builtin_clzll
// __builtin_popcount
// __builtin_popcountl
// __builtin_popcountll
#if defined(__GNUC__) || defined(__clang__)
# define HAVE_BITCOUNT_INTRINSICS 1
#else
# define HAVE_BITCOUNT_INTRINSICS 0
#endif


#endif // not CXXUTILS_FEATURES_H
