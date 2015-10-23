/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigNavigationAuxInfoCnv.cxx 675923 2015-06-17 11:21:49Z tbold $

// System include(s):
#include <stdexcept>

// Core include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "TrigNavTools/ITrigNavigationThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"



// Local include(s):
#include "xAODTrigNavigationAuxInfoCnv.h"
#include "xAODTrigger/TrigNavigation.h"

xAODTrigNavigationAuxInfoCnv::
xAODTrigNavigationAuxInfoCnv( ISvcLocator* svcLoc )
   : xAODTrigNavigationAuxInfoCnvBase( svcLoc ) {

}


ITrigNavigationThinningSvc* iface_cast(IThinningSvc* base)  {
  ITrigNavigationThinningSvc* isvc(0);
  if ( base->queryInterface(ITrigNavigationThinningSvc::interfaceID(), (void**)&isvc).isFailure() ) {
    return 0;
  }
  return isvc;
}


xAOD::TrigNavigationAuxInfo*
xAODTrigNavigationAuxInfoCnv::
createPersistent( xAOD::TrigNavigationAuxInfo* trans ) {

  // see if the ThinningSvc implements an interface capable of slimming the navation 
  xAOD::TrigNavigationAuxInfo* result = new xAOD::TrigNavigationAuxInfo();
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
  return result;
}

xAOD::TrigNavigationAuxInfo*
xAODTrigNavigationAuxInfoCnv::createTransient() {

   // The known ID(s) for this object:
   static const pool::Guid v1_guid( "924049A6-25B4-4406-A70A-CCAC2E4233E1" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // Read in the v1 object:
      xAOD::TrigNavigationAuxInfo_v1* obj =
         poolReadObject< xAOD::TrigNavigationAuxInfo_v1 >();

      // Return the object:
      return obj;
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigNavigationAuxInfo found" );
   return 0;
}
