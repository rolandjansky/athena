/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEmTauRoIAuxContainerCnv.h"
#include "xAODEmTauRoIAuxContainerCnv_v1.h"

xAODEmTauRoIAuxContainerCnv::
xAODEmTauRoIAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODEmTauRoIAuxContainerCnvBase( svcLoc ) {

}

xAOD::EmTauRoIAuxContainer*
xAODEmTauRoIAuxContainerCnv::
createPersistent( xAOD::EmTauRoIAuxContainer* trans ) {
   // Create a copy of the container:
   xAOD::EmTauRoIAuxContainer* result =
      new xAOD::EmTauRoIAuxContainer( *trans );

   return result;
}

xAOD::EmTauRoIAuxContainer*
xAODEmTauRoIAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "1825AE7E-5EA2-11E3-A41F-02163E00A743" );
   static const pool::Guid v2_guid( "74EA3AFB-B7BE-4E4B-AD5D-4297CA6EDBCD" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::EmTauRoIAuxContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODEmTauRoIAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::EmTauRoIAuxContainer_v1 >
         old( poolReadObject< xAOD::EmTauRoIAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::EmTauRoIAuxContainer found" );
   return 0;
}
