/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetRoIAuxContainerCnv.h"
#include "xAODJetRoIAuxContainerCnv_v1.h"

xAODJetRoIAuxContainerCnv::
xAODJetRoIAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJetRoIAuxContainerCnvBase( svcLoc ) {

}

xAOD::JetRoIAuxContainer*
xAODJetRoIAuxContainerCnv::
createPersistent( xAOD::JetRoIAuxContainer* trans ) {
   // Create a copy of the container:
   xAOD::JetRoIAuxContainer* result =
      new xAOD::JetRoIAuxContainer( *trans );

   return result;
}

xAOD::JetRoIAuxContainer*
xAODJetRoIAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "64C2E2A4-5E9A-11E3-BEF2-02163E00A725" );
   static const pool::Guid v2_guid( "8448ECAF-10E0-4E8B-9E67-3A00FEA36317" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JetRoIAuxContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODJetRoIAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::JetRoIAuxContainer_v1 >
         old( poolReadObject< xAOD::JetRoIAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );   
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetRoIAuxContainer found" );
   return 0;
}
