/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL1TopoRawDataAuxContainerCnv.h"

xAODL1TopoRawDataAuxContainerCnv::
xAODL1TopoRawDataAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODL1TopoRawDataAuxContainerCnvBase( svcLoc ) {

}

xAOD::L1TopoRawDataAuxContainer*
xAODL1TopoRawDataAuxContainerCnv::
createPersistent( xAOD::L1TopoRawDataAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::L1TopoRawDataAuxContainer* result =
      new xAOD::L1TopoRawDataAuxContainer( *trans );

   return result;
}

xAOD::L1TopoRawDataAuxContainer*
xAODL1TopoRawDataAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "E3D3AF1E-843A-4F21-A639-EE97040D3B6B" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::L1TopoRawDataAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L1TopoRawDataAuxContainer found" );
   return 0;
}
