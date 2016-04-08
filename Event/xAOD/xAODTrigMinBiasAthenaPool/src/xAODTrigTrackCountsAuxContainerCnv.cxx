/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigTrackCountsAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigTrackCountsAuxContainerCnv.h"

xAODTrigTrackCountsAuxContainerCnv::xAODTrigTrackCountsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigTrackCountsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigTrackCountsAuxContainer*
xAODTrigTrackCountsAuxContainerCnv::
createPersistent( xAOD::TrigTrackCountsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigTrackCountsAuxContainer* result =
      new xAOD::TrigTrackCountsAuxContainer( *trans );

   return result;
}

xAOD::TrigTrackCountsAuxContainer* xAODTrigTrackCountsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "0701BEB0-1207-11E4-9D08-02163E00A892" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigTrackCountsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigTrackCountsAuxContainer found" );
   return 0;
}
