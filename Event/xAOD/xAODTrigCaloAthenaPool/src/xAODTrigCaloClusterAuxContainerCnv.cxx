/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloClusterAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigCaloClusterAuxContainerCnv.h"

xAODTrigCaloClusterAuxContainerCnv::xAODTrigCaloClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigCaloClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigCaloClusterAuxContainer*
xAODTrigCaloClusterAuxContainerCnv::
createPersistent( xAOD::TrigCaloClusterAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigCaloClusterAuxContainer* result =
      new xAOD::TrigCaloClusterAuxContainer( *trans );

   return result;
}

xAOD::TrigCaloClusterAuxContainer* xAODTrigCaloClusterAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "4A2F55AF-D465-42A8-A4CF-2DB84D9628E5" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigCaloClusterAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigCaloClusterAuxContainer found" );
   return 0;
}
