/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonAuxContainerCnv.cxx 615003 2014-09-04 08:42:14Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#define protected public
#define private public
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"
#undef private
#undef protected

// Local include(s):
#include "xAODPhotonAuxContainerCnv.h"
#include "copyAux.h"

xAODPhotonAuxContainerCnv::xAODPhotonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODPhotonAuxContainerCnvBase( svcLoc ) {

}

xAOD::PhotonAuxContainer*
xAODPhotonAuxContainerCnv::
createPersistent( xAOD::PhotonAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::PhotonAuxContainer* result =
      new xAOD::PhotonAuxContainer( *trans );

   return result;
}

xAOD::PhotonAuxContainer* xAODPhotonAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
   static const pool::Guid v1_guid( "67A1818E-4591-4100-B8BD-9A3C9E0D4EBB" );
   static const pool::Guid v2_guid( "CAE4C9A6-B3D0-429B-9A4F-1F174D892CA5" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // Read in the v1 version:
      std::unique_ptr< xAOD::PhotonAuxContainer_v1 >
         old( poolReadObject< xAOD::PhotonAuxContainer_v1 >() );

      // Create a v2 object:
      xAOD::PhotonAuxContainer* result = new xAOD::PhotonAuxContainer();

      // Copy the payload of the v1 object into the v2 one by misusing
      // the thinning code a bit...
      copyAux( old.get(), result, old->vertexLinks.size() );

      // Return the new object:
      return result;

   } else if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::PhotonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::PhotonAuxContainer found" );
   return 0;
}
