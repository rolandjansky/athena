/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/getThinnedFlags.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Return vector of flags giving which container elements were thinned.
 */


#include "AthContainers/tools/getThinnedFlags.h"
#include "AthenaKernel/IThinningSvc.h"


namespace SG {


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
                       std::vector<unsigned char>& flags)
{
  nremaining = size;
  flags.clear();
  if (!svc) return false;
  if (size == 0) return false;
  if (!svc->thinningOccurred (container)) return false;

  nremaining = 0;
  flags.resize (size);
  for (size_t i = 0; i < size; i++) {
    flags[i] = IThinningSvc::isThinned (svc->index (container, i));
    if (!flags[i]) ++nremaining;
  }
  return true;
}


} // namespace SG
