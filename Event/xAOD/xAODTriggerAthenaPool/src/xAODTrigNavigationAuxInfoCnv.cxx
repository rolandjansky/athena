/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigNavigationAuxInfoCnv.cxx 710840 2015-11-26 08:45:42Z will $

// System include(s):
#include <stdexcept>

// Core include(s):
#include "AthenaKernel/IThinningSvc.h"

#ifndef XAOD_ANALYSIS
#include "TrigNavTools/ITrigNavigationThinningSvc.h" //thinning only possible in full athena
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

#ifndef XAOD_ANALYSIS
ITrigNavigationThinningSvc* iface_cast(IThinningSvc* base)  {
  ITrigNavigationThinningSvc* isvc(0);
  if ( base->queryInterface(ITrigNavigationThinningSvc::interfaceID(), (void**)&isvc).isFailure() ) {
    return 0;
  }
  return isvc;
}
#endif

xAOD::TrigNavigationAuxInfo*
xAODTrigNavigationAuxInfoCnv::
createPersistentWithKey( xAOD::TrigNavigationAuxInfo* trans,
                         const std::string& /*key*/) {

  // see if the ThinningSvc implements an interface capable of slimming the navation 
  xAOD::TrigNavigationAuxInfo* result = new xAOD::TrigNavigationAuxInfo();
#ifndef XAOD_ANALYSIS
  ITrigNavigationThinningSvc* thinningSvc = dynamic_cast<ITrigNavigationThinningSvc*>(IThinningSvc::instance());//iface_cast(IThinningSvc::instance());
  if ( thinningSvc ) {
    ATH_MSG_DEBUG("ThinningSvc is o type TrigNavigationThinningSvc, will request slimmed navigation from it");
    xAOD::TrigNavigation wrapper;
    wrapper.setStore(result);
    std::vector< unsigned int > temp;
     if ( thinningSvc->doSlimming(temp).isFailure() ) {
       ATH_MSG_WARNING("Failed to slim and store Trigger Navigation data in the output xAODTrigNavigation object");
     }
     wrapper.setSerialized(temp);
     wrapper.setStore((const SG::IConstAuxStore*)nullptr);
  } else {
    ATH_MSG_DEBUG("Default ThinningSvc, just copy");
    *result = *trans;
  }
#else
  //no navigation thinning in analysis releases
  *result = *trans;
#endif

  return result;
}

