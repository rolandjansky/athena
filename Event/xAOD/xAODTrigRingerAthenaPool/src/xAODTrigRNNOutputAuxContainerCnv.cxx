/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRNNOutputAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local includ
#include "xAODTrigRNNOutputAuxContainerCnv.h"

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

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigRNNOutputAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRNNOutputAuxContainer found" );
   return 0;
}
