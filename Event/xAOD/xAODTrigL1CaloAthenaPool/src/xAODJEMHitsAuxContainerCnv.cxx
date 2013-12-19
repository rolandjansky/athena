/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMHitsAuxContainerCnv.h"

xAODJEMHitsAuxContainerCnv::
xAODJEMHitsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMHitsAuxContainerCnvBase( svcLoc ) {

}

xAOD::JEMHitsAuxContainer*
xAODJEMHitsAuxContainerCnv::
createPersistent( xAOD::JEMHitsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::JEMHitsAuxContainer* result =
      new xAOD::JEMHitsAuxContainer( *trans );

   return result;
}

xAOD::JEMHitsAuxContainer*
xAODJEMHitsAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "CE2C5182-53DD-46E0-9DFA-61C214A8FCE5" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JEMHitsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMHitsAuxContainer found" );
   return 0;
}
