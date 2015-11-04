/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterTrigAuxContainerCnv.cxx 705793 2015-11-04 13:13:26Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"

// Local include(s):
#include "xAODCaloClusterTrigAuxContainerCnv.h"
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v1.h"
#include "xAODCaloClusterTrigAuxContainerOfflineCnv_v2.h"

xAODCaloClusterTrigAuxContainerCnv::
xAODCaloClusterTrigAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloClusterTrigAuxContainerCnvBase( svcLoc ) {

}

xAOD::CaloClusterTrigAuxContainer*
xAODCaloClusterTrigAuxContainerCnv::
createPersistent( xAOD::CaloClusterTrigAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::CaloClusterTrigAuxContainer*
xAODCaloClusterTrigAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "F25EBF21-75CD-4CB7-B33E-76B46213A49B" );
   static const pool::Guid
      v2_offline_guid( "451393B0-69CD-11E4-A739-02163E00A64D" );
   static const pool::Guid
      v1_offline_guid( "CE498B3B-A32D-43A3-B9B3-C13D136BACFC" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::CaloClusterTrigAuxContainer >();

   } else if( compareClassGuid( v2_offline_guid ) ) {

      // The converter:
      static xAODCaloClusterTrigAuxContainerOfflineCnv_v2 converter;

      // Read in the offline v2 version:
      std::unique_ptr< xAOD::CaloClusterAuxContainer_v2 >
         old( poolReadObject< xAOD::CaloClusterAuxContainer_v2 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v1_offline_guid ) ) {

      // The converter:
      static xAODCaloClusterTrigAuxContainerOfflineCnv_v1 converter;

      // Read in the offline v1 version:
      std::unique_ptr< xAOD::CaloClusterAuxContainer_v1 >
         old( poolReadObject< xAOD::CaloClusterAuxContainer_v1 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CaloClusterTrigAuxContainer found" );
   return 0;
}
