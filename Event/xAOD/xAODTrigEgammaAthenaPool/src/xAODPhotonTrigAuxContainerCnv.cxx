/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerCnv.cxx 706221 2015-11-05 17:40:35Z lheinric $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

// Local include(s):
#include "xAODPhotonTrigAuxContainerCnv.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v1.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v2.h"
#include "xAODPhotonTrigAuxContainerOfflineCnv_v3.h"

xAODPhotonTrigAuxContainerCnv::
xAODPhotonTrigAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODPhotonTrigAuxContainerCnvBase( svcLoc ) {

}

xAOD::PhotonTrigAuxContainer*
xAODPhotonTrigAuxContainerCnv::
createPersistent( xAOD::PhotonTrigAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::PhotonTrigAuxContainer*
xAODPhotonTrigAuxContainerCnv::createTransient() {

// The known ID(s) for this container:
   static const pool::Guid v1_guid( "863989DB-F18B-4D93-A019-F3CBFF726176" ); //xAOD::PhotonTrigAuxContainer_v1
   static const pool::Guid
      v1_offline_guid( "67A1818E-4591-4100-B8BD-9A3C9E0D4EBB" ); //xAOD::PhotonAuxContainer_v1
   static const pool::Guid
      v2_offline_guid( "CAE4C9A6-B3D0-429B-9A4F-1F174D892CA5" );//xAOD::PhotonAuxContainer_v2
   static const pool::Guid
      v3_offline_guid( "5C389DEC-4B00-11E4-B9F0-02163E00A5BB" );//xAOD::PhotonAuxContainer_v3
  

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::PhotonTrigAuxContainer >();

   } else if( compareClassGuid( v3_offline_guid ) ) {

      // The converter:
      static xAODPhotonTrigAuxContainerOfflineCnv_v3 converter;

      // Read in the offline v3 version:
      std::unique_ptr< xAOD::PhotonAuxContainer_v3 >
         old( poolReadObject< xAOD::PhotonAuxContainer_v3 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v2_offline_guid ) ) {

      // The converter:
      static xAODPhotonTrigAuxContainerOfflineCnv_v2 converter;

      // Read in the offline v2 version:
      std::unique_ptr< xAOD::PhotonAuxContainer_v2 >
         old( poolReadObject< xAOD::PhotonAuxContainer_v2 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v1_offline_guid ) ) {

      // The converter:
      static xAODPhotonTrigAuxContainerOfflineCnv_v1 converter;

      // Read in the offline v1 version:
      std::unique_ptr< xAOD::PhotonAuxContainer_v1 >
         old( poolReadObject< xAOD::PhotonAuxContainer_v1 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::PhotonTrigAuxContainer found" );
   return 0;
}
