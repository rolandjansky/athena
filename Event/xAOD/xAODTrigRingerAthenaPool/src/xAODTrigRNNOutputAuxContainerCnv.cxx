/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRNNOutputAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local includ
#include "xAODTrigRNNOutputAuxContainerCnv.h"
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

xAODTrigRNNOutputAuxContainerCnv::xAODTrigRNNOutputAuxContainerCnv( ISvcLocator* svcLoc ): xAODTrigRNNOutputAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigRNNOutputAuxContainer* xAODTrigRNNOutputAuxContainerCnv::createPersistent( xAOD::TrigRNNOutputAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigRNNOutputAuxContainer* result = new xAOD::TrigRNNOutputAuxContainer( *trans );
   return result;
}

xAOD::TrigRNNOutputAuxContainer* xAODTrigRNNOutputAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "5090A739-C400-4322-9F91-C49441C9141F" );
   static const pool::Guid v2_guid( "725ADE43-9D9D-4856-90C6-8DE105BE62A4" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigRNNOutputAuxContainer >();
   }

   if( compareClassGuid( v1_guid ) ) {
        // The v1 converter:
        static xAODTrigRNNOutputAuxContainerCnv_v1 converter;
  
        // Read in the v1 object:
        std::unique_ptr< xAOD::TrigRNNOutputAuxContainer_v1 > old( poolReadObject< xAOD::TrigRNNOutputAuxContainer_v1 >() );
  
        // Return the converted object:
        return converter.createTransient( old.get(), msg() );   
     }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRNNOutputAuxContainer found" );
   return 0;
}
