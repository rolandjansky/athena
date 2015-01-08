/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/copyAuxStoreThinned.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Helper to copy an aux store while applying thinning.
 */


#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthContainers/tools/getThinnedFlags.h"
#include "AthContainers/tools/foreach.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthenaKernel/IThinningSvc.h"
#include <vector>


namespace SG {


/**
 * @brief Helper to copy an aux store while applying thinning.
 * @param orig Source aux store from which to coy.
 * @param copy Destination aux store to which to copy.
 * @param svc The thinning service.
 *
 * @c orig and @c copy are both auxiliary store objects.
 * The data from @c orig will be copied to @c copy, with individual
 * elements removed according to thinning recorded for @c orig in @c svc.
 */
void copyAuxStoreThinned (const SG::IConstAuxStore& orig,
                          SG::IAuxStore& copy,
                          IThinningSvc* svc)
{
  copy.resize(0);
  size_t size = orig.size();
  if (size == 0) return;
  size_t nremaining = 0;
  std::vector<unsigned char> flags;
  bool thinned = getThinnedFlags (svc, orig, nremaining, flags);

  // Access the auxiliary type registry:
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  // The auxiliary IDs that the original container has:
  SG::auxid_set_t auxids = orig.getAuxIDs();

  SG::auxid_set_t dyn_auxids;
  SG::auxid_set_t sel_auxids;
  if (const IAuxStoreIO* iio = dynamic_cast<const IAuxStoreIO*> (&orig)) {
    dyn_auxids = iio->getDynamicAuxIDs();
    sel_auxids = iio->getSelectedAuxIDs();
  }

  copy.resize (nremaining);
  
  // Loop over all the variables of the original container:
  ATHCONTAINERS_FOREACH (SG::auxid_t auxid, auxids) {
    // Skip non-selected dynamic variables.
    if (dyn_auxids.count(auxid) > 0 && sel_auxids.count(auxid) == 0)
      continue;

    // Create the target variable:
    void* dst = copy.getData (auxid, nremaining, nremaining);

    // Access the source variable:
    const void* src = orig.getData (auxid);

    if (!src) continue;

    // Copy over all elements, with thinning.
    for (std::size_t isrc = 0, idst = 0; isrc < size; ++isrc) {
      if (!thinned || !flags[isrc]) {
        r.copyForOutput (auxid, dst, idst, src, isrc);
        ++idst;
      }
    }
  }
}


} // namespace SG
