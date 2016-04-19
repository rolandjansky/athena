/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonAuxContainerCnv.cxx 635115 2014-12-10 17:10:47Z ssnyder $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

// Local include(s):
#include "xAODPhotonAuxContainerCnv.h"
#include "xAODPhotonAuxContainerCnv_v1.h"
#include "xAODPhotonAuxContainerCnv_v2.h"

xAODPhotonAuxContainerCnv::xAODPhotonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODPhotonAuxContainerCnvBase( svcLoc ) {

}

xAOD::PhotonAuxContainer*
xAODPhotonAuxContainerCnv::
createPersistent( xAOD::PhotonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::PhotonAuxContainer* xAODPhotonAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
   static const pool::Guid v1_guid( "67A1818E-4591-4100-B8BD-9A3C9E0D4EBB" );
   static const pool::Guid v2_guid( "CAE4C9A6-B3D0-429B-9A4F-1F174D892CA5" );
   static const pool::Guid v3_guid( "5C389DEC-4B00-11E4-B9F0-02163E00A5BB" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v3_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::PhotonAuxContainer >();

   }else if( compareClassGuid( v2_guid ) ) {

     // The v2 converter:
     static xAODPhotonAuxContainerCnv_v2 converter;
     
     // Read in the v2 version:
     std::unique_ptr< xAOD::PhotonAuxContainer_v2 >
       old( poolReadObject< xAOD::PhotonAuxContainer_v2 >() );
     
     // Return the converted object:
     return converter.createTransient( old.get(), msg() );

   } else if( compareClassGuid( v1_guid ) ) {

     // The v1 converter:
     static xAODPhotonAuxContainerCnv_v1 converter;
     
     // Read in the v1 version:
     std::unique_ptr< xAOD::PhotonAuxContainer_v1 >
       old( poolReadObject< xAOD::PhotonAuxContainer_v1 >() );
     
     // Return the converted object:
     return converter.createTransient( old.get(), msg() );

   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::PhotonAuxContainer found" );
   return 0;
}
