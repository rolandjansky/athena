/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigSpacePointCountsAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigSpacePointCountsAuxContainerCnv.h"

xAODTrigSpacePointCountsAuxContainerCnv::xAODTrigSpacePointCountsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigSpacePointCountsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigSpacePointCountsAuxContainer*
xAODTrigSpacePointCountsAuxContainerCnv::
createPersistent( xAOD::TrigSpacePointCountsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigSpacePointCountsAuxContainer* result =
      new xAOD::TrigSpacePointCountsAuxContainer( *trans );

   return result;
}

xAOD::TrigSpacePointCountsAuxContainer* xAODTrigSpacePointCountsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "C682FD32-73CC-4773-ACBF-30B2657C5991" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigSpacePointCountsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigSpacePointCountsAuxContainer found" );
   return 0;
}
