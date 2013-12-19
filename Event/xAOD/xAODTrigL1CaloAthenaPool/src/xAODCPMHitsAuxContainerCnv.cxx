/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMHitsAuxContainerCnv.h"

xAODCPMHitsAuxContainerCnv::
xAODCPMHitsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMHitsAuxContainerCnvBase( svcLoc ) {

}

xAOD::CPMHitsAuxContainer*
xAODCPMHitsAuxContainerCnv::
createPersistent( xAOD::CPMHitsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CPMHitsAuxContainer* result =
      new xAOD::CPMHitsAuxContainer( *trans );

   return result;
}

xAOD::CPMHitsAuxContainer*
xAODCPMHitsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "28FCEB20-A9DC-4FED-9FDD-00F46948E92E" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CPMHitsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMHitsAuxContainer found" );
   return 0;
}
