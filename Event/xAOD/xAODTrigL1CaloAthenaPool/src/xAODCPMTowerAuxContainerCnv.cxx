/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMTowerAuxContainerCnv.h"

xAODCPMTowerAuxContainerCnv::
xAODCPMTowerAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMTowerAuxContainerCnvBase( svcLoc ) {

}

xAOD::CPMTowerAuxContainer*
xAODCPMTowerAuxContainerCnv::
createPersistent( xAOD::CPMTowerAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CPMTowerAuxContainer* result =
      new xAOD::CPMTowerAuxContainer( *trans );

   return result;
}

xAOD::CPMTowerAuxContainer*
xAODCPMTowerAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "529B6BD7-7B7D-438C-94D7-BAA49A022C47" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CPMTowerAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMTowerAuxContainer found" );
   return 0;
}
