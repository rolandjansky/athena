/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.cxx 565402 2013-10-15 09:44:40Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCaloClusterAuxContainerCnv.h"

xAODCaloClusterAuxContainerCnv::xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloClusterAuxContainerCnvBase( svcLoc ) {

}

xAOD::CaloClusterAuxContainer*
xAODCaloClusterAuxContainerCnv::
createPersistent( xAOD::CaloClusterAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::CaloClusterAuxContainer* result =
      new xAOD::CaloClusterAuxContainer( *trans );

   return result;
}

xAOD::CaloClusterAuxContainer* xAODCaloClusterAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "CE498B3B-A32D-43A3-B9B3-C13D136BACFC" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CaloClusterAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CaloClusterAuxContainer found" );
   return 0;
}
