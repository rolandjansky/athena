/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigEMClusterAuxContainerCnv.h"

xAODTrigEMClusterAuxContainerCnv::xAODTrigEMClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigEMClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigEMClusterAuxContainer*
xAODTrigEMClusterAuxContainerCnv::
createPersistent( xAOD::TrigEMClusterAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigEMClusterAuxContainer* result =
      new xAOD::TrigEMClusterAuxContainer( *trans );

   return result;
}

xAOD::TrigEMClusterAuxContainer* xAODTrigEMClusterAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "CE08F151-A2BA-44ED-B854-7D8B9C8679DE" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigEMClusterAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigEMClusterAuxContainer found" );
   return 0;
}
