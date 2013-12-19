/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMRoIAuxContainerCnv.h"

xAODJEMRoIAuxContainerCnv::
xAODJEMRoIAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMRoIAuxContainerCnvBase( svcLoc ) {

}

xAOD::JEMRoIAuxContainer*
xAODJEMRoIAuxContainerCnv::
createPersistent( xAOD::JEMRoIAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::JEMRoIAuxContainer* result =
      new xAOD::JEMRoIAuxContainer( *trans );

   return result;
}

xAOD::JEMRoIAuxContainer*
xAODJEMRoIAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "959C24E5-9746-4C70-83FA-A0140C710039" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JEMRoIAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMRoIAuxContainer found" );
   return 0;
}
