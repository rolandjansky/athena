// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/getThinnedFlags.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Return vector of flags giving which container elements were thinned.
 */


#ifndef ATHCONTAINERS_GETTHINNEDFLAGS_H
#define ATHCONTAINERS_GETTHINNEDFLAGS_H


#include <vector>
#include <cstddef>


class IThinningSvc;


namespace SG {


/**
 * @brief Return vector of flags giving which container elements were thinned.
 * @param svc The thinning service (or 0).
 * @param container The container for which to find thinning information.
 * @param nremaining[out] The number of elements in the container not thinned.
 * @param flags[out] Array telling which elements were thinned.
 *
 * If no thinning is done on @c CONTAINER, then return false with
 * @c nremaining equal to `container.size()`.  @c flags may be empty
 * in this case.
 *
 * Otherwise, the container was thinned, and the function returns true.
 * The size of @c flags will equal the container size; each element of
 * @c flags is non-zero if the corresponding container element has been
 * thinned.  @c nremaining will be set to the count of elements
 * that have _not_ been thinned.
 */
template <class CONTAINER>
bool getThinnedFlags (IThinningSvc* svc,
                      const CONTAINER& container,
                      size_t& nremaining,
                      std::vector<unsigned char>& flags);


/**
 * @brief Return vector of flags giving which container elements were thinned.
 * @param svc The thinning service (or 0).
 * @param container The container for which to find thinning information.
 * @param size Size of the container.
 * @param nremaining[out] The number of elements in the container not thinned.
 * @param flags[out] Array telling which elements were thinned.
 *
 * This is and out-of-line helper for @c getThinnedFlags.
 */
bool getThinnedFlags1 (IThinningSvc* svc,
                       const void* container,
                       size_t size,
                       size_t& nremaining,
                       std::vector<unsigned char>& flags);


} // namespace SG


#include "AthContainers/tools/getThinnedFlags.icc"


#endif // not ATHCONTAINERS_GETTHINNEDFLAGS_H
