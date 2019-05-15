// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/vectorize.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Helper to enable auto-vectorization.
 *
 * Athena is usually built with -O2, which doesn't fully enable
 * autovectorization.  Add ATH_ENABLE_VECTORIZATION; at the start
 * of a compilation unit to enable it for this file..
 */


#ifndef CXXUTILS_VECTORIZE_H
#define CXXUTILS_VECTORIZE_H


#if defined(__GNUC__) && !defined(__clang__)
# define ATH_ENABLE_VECTORIZATION _Pragma ("GCC optimize (\"tree-loop-vectorize\")") \
    class ATH_ENABLE_VECTORIZATION_SWALLOW_SEMICOLON
#else
# define ATH_ENABLE_VECTORIZATION class ATH_ENABLE_VECTORIZATION_SWALLOW_SEMICOLON
#endif

#endif // not CXXUTILS_VECTORIZE_H
