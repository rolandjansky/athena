/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.cxx 581660 2014-02-05 15:52:29Z janus $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTauJetAuxContainerCnv.h"

xAODTauJetAuxContainerCnv::xAODTauJetAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTauJetAuxContainerCnvBase( svcLoc ) {

}

xAOD::TauJetAuxContainer*
xAODTauJetAuxContainerCnv::
createPersistent( xAOD::TauJetAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TauJetAuxContainer* result =
      new xAOD::TauJetAuxContainer( *trans );

   return result;
}

xAOD::TauJetAuxContainer* xAODTauJetAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "EA3CE9A0-18D8-49FD-B978-62857D8D8FD0" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TauJetAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauJetAuxContainer found" );
   return 0;
}
