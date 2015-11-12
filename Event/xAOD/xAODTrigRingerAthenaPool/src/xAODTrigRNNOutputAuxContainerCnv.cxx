/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputAuxContainerCnv.cxx 707590 2015-11-12 19:09:03Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODTrigRNNOutputAuxContainerCnv.h"
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

xAODTrigRNNOutputAuxContainerCnv::
xAODTrigRNNOutputAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigRNNOutputAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigRNNOutputAuxContainer*
xAODTrigRNNOutputAuxContainerCnv::
createPersistent( xAOD::TrigRNNOutputAuxContainer* trans ) {

   // Create a (possibly thinned) copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::TrigRNNOutputAuxContainer* xAODTrigRNNOutputAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "5090A739-C400-4322-9F91-C49441C9141F" );
   static const pool::Guid v2_guid( "725ADE43-9D9D-4856-90C6-8DE105BE62A4" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigRNNOutputAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODTrigRNNOutputAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::TrigRNNOutputAuxContainer_v1 >
         old( poolReadObject< xAOD::TrigRNNOutputAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );   
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRNNOutputAuxContainer found" );
   return 0;
}
