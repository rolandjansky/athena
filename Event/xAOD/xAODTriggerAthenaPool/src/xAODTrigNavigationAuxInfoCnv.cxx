/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigNavigationAuxInfoCnv.cxx 710840 2015-11-26 08:45:42Z will $

// System include(s):
#include <stdexcept>

#ifndef XAOD_ANALYSIS
#include "AthenaKernel/ITrigNavigationThinningSvc.h" //thinning only possible in full athena
#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningCache.h"
#include "GaudiKernel/ThreadLocalContext.h"
#endif

// EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"



// Local include(s):
#include "xAODTrigNavigationAuxInfoCnv.h"
#include "xAODTrigger/TrigNavigation.h"

xAODTrigNavigationAuxInfoCnv::
xAODTrigNavigationAuxInfoCnv( ISvcLocator* svcLoc )
   : xAODTrigNavigationAuxInfoCnvBase( svcLoc ) {

}

xAOD::TrigNavigationAuxInfo*
xAODTrigNavigationAuxInfoCnv::
createPersistentWithKey( xAOD::TrigNavigationAuxInfo* trans,
                         const std::string& /*key*/) {

  xAOD::TrigNavigationAuxInfo* result = new xAOD::TrigNavigationAuxInfo();
#ifndef XAOD_ANALYSIS
  const ITrigNavigationThinningSvc* thinningSvc = nullptr;
  // Try to find the thinning service.
  if (const SG::ThinningCache* cache = SG::getThinningCache()) {
    thinningSvc = cache->trigNavigationThinningSvc();
  }
  if ( thinningSvc ) {
    ATH_MSG_DEBUG("Doing TrigNavigation slimming");
    xAOD::TrigNavigation wrapper;
    wrapper.setStore(result);
    std::vector< unsigned int > temp;
    const EventContext& ctx = Gaudi::Hive::currentContext();
     if ( thinningSvc->doSlimming(ctx, temp).isFailure() ) {
       ATH_MSG_WARNING("Failed to slim and store Trigger Navigation data in the output xAODTrigNavigation object");
     }
     wrapper.setSerialized(temp);
     wrapper.setStore((const SG::IConstAuxStore*)nullptr);
  } else {
    ATH_MSG_DEBUG("No TrigNavigation slimming");
    *result = *trans;
  }
#else
  //no navigation thinning in analysis releases
  *result = *trans;
#endif

  return result;
}

