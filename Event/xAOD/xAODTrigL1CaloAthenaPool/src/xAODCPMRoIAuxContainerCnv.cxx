/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMRoIAuxContainerCnv.h"

xAODCPMRoIAuxContainerCnv::
xAODCPMRoIAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMRoIAuxContainerCnvBase( svcLoc ) {

}

xAOD::CPMRoIAuxContainer*
xAODCPMRoIAuxContainerCnv::
createPersistent( xAOD::CPMRoIAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CPMRoIAuxContainer* result =
      new xAOD::CPMRoIAuxContainer( *trans );

   return result;
}

xAOD::CPMRoIAuxContainer*
xAODCPMRoIAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "8FA92C8D-214F-4B8F-84CA-CB7267CAD0F2" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CPMRoIAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMRoIAuxContainer found" );
   return 0;
}
