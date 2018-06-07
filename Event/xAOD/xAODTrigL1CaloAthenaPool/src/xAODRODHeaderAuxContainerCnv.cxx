/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRODHeaderAuxContainerCnv.cxx 694352 2015-09-11 12:05:29Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODRODHeaderAuxContainerCnv.h"
#include "xAODRODHeaderAuxContainerCnv_v1.h"

xAODRODHeaderAuxContainerCnv::
xAODRODHeaderAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODRODHeaderAuxContainerCnvBase( svcLoc ) {

}

xAOD::RODHeaderAuxContainer*
xAODRODHeaderAuxContainerCnv::
createPersistent( xAOD::RODHeaderAuxContainer* trans ) {

   // Create a (possibly thinned) copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::RODHeaderAuxContainer*
xAODRODHeaderAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v2_guid( "A94BA0CD-E57C-4C69-ADA1-CCB91BC0C36F" );
   static const pool::Guid v1_guid( "6524E46D-CC9C-43D2-BAE0-55310A6B316C" );


   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::RODHeaderAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODRODHeaderAuxContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::RODHeaderAuxContainer_v1 >
         old( poolReadObject< xAOD::RODHeaderAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   } 

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::RODHeaderAuxContainer found" );
   return 0;
}
