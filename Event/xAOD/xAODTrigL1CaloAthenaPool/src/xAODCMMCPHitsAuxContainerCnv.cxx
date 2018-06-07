/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMCPHitsAuxContainerCnv.h"

xAODCMMCPHitsAuxContainerCnv::
xAODCMMCPHitsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMCPHitsAuxContainerCnvBase( svcLoc ) {

}

xAOD::CMMCPHitsAuxContainer*
xAODCMMCPHitsAuxContainerCnv::
createPersistent( xAOD::CMMCPHitsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CMMCPHitsAuxContainer* result =
      new xAOD::CMMCPHitsAuxContainer( *trans );

   return result;
}

xAOD::CMMCPHitsAuxContainer*
xAODCMMCPHitsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "1951D4B4-D471-4256-8F0D-D666043E2889" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CMMCPHitsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMCPHitsAuxContainer found" );
   return 0;
}
