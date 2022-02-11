// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/aligned_vector.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief @c std::vector with extra alignment.
 *
 * The payload of a @c std::vector<T> is aligned appropriately for @c T.
 * However, there are some times when one would like to force a larger
 * alignment, for example if the vector is being used to hold data
 * to be processed with vector instructions.  @c aligned_vector
 * defined here is a @c std::vector with higher alignment.
 * @c vec_aligned_vector is a vector that should be sufficiently
 * aligned for any vector instructions on this platform.
 */


#ifndef CXXUTILS_ALIGNED_VECTOR_H
#define CXXUTILS_ALIGNED_VECTOR_H


#include "boost/align/aligned_allocator.hpp"
#include <vector>


namespace CxxUtils {


/**
 * @brief A @c std::vector with extra alignment.
 *
 * The alignment for the payload will be at least either that required by @c T
 * or @c Alignment, whichever is greater.
 */
template <class T, size_t Alignment=1>
using aligned_vector = std::vector<T, boost::alignment::aligned_allocator<T, Alignment> >;


/**
 * @brief A @c std::vector with alignment sufficient for any vector
 *        instructions on this platform.
 *
 * For now, just hard code 64, which is sufficient for a 512-bit vector,
 * the largest supported by any x86 processors.  We may want to make this
 * platform-dependent in the future.
 */
template <class T>
using vec_aligned_vector = aligned_vector<T, 64>;


} // namespace CxxUtils


#endif // not CXXUTILS_ALIGNED_VECTOR_H
