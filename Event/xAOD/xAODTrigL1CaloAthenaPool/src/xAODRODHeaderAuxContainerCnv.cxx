/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODRODHeaderAuxContainerCnv.h"

xAODRODHeaderAuxContainerCnv::
xAODRODHeaderAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODRODHeaderAuxContainerCnvBase( svcLoc ) {

}

xAOD::RODHeaderAuxContainer*
xAODRODHeaderAuxContainerCnv::
createPersistent( xAOD::RODHeaderAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::RODHeaderAuxContainer* result =
      new xAOD::RODHeaderAuxContainer( *trans );

   return result;
}

xAOD::RODHeaderAuxContainer*
xAODRODHeaderAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "6524E46D-CC9C-43D2-BAE0-55310A6B316C" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::RODHeaderAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::RODHeaderAuxContainer found" );
   return 0;
}
