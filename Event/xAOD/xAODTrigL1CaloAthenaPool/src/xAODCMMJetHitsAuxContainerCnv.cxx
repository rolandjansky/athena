/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMJetHitsAuxContainerCnv.h"

xAODCMMJetHitsAuxContainerCnv::
xAODCMMJetHitsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMJetHitsAuxContainerCnvBase( svcLoc ) {

}

xAOD::CMMJetHitsAuxContainer*
xAODCMMJetHitsAuxContainerCnv::
createPersistent( xAOD::CMMJetHitsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CMMJetHitsAuxContainer* result =
      new xAOD::CMMJetHitsAuxContainer( *trans );

   return result;
}

xAOD::CMMJetHitsAuxContainer*
xAODCMMJetHitsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "B3C8D000-0E7F-4A0D-8F05-6DA36FFC9DD7" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CMMJetHitsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMJetHitsAuxContainer found" );
   return 0;
}
