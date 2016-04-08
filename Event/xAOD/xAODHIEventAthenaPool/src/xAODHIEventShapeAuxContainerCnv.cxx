/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeAuxContainerCnv.cxx 693658 2015-09-08 10:00:53Z krasznaa $

// System include(s):
#include <stdexcept>
#include <memory>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"

// EDM include(s):
#include "AthContainers/tools/copyThinned.h"
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v1.h"

// Local include(s):
#include "xAODHIEventShapeAuxContainerCnv.h"
#include "xAODHIEventShapeAuxContainerCnv_v1.h"

xAODHIEventShapeAuxContainerCnv::
xAODHIEventShapeAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODHIEventShapeAuxContainerCnvBase( svcLoc ) {

}

xAOD::HIEventShapeAuxContainer*
xAODHIEventShapeAuxContainerCnv::
createPersistent( xAOD::HIEventShapeAuxContainer* trans ) {

   // Create a (possibly) thinned copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::HIEventShapeAuxContainer*
xAODHIEventShapeAuxContainerCnv::createTransient() {

   // The known ID(s) for the object:
   static const pool::Guid v1_guid( "06FA81A6-71A7-11E4-9A9D-02163E010E29" );
   static const pool::Guid v2_guid( "8A34CCBF-509A-4A15-8234-07FBC0BFF1B0" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it without further fuss:
      return poolReadObject< xAOD::HIEventShapeAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODHIEventShapeAuxContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::HIEventShapeAuxContainer_v1 >
         old( poolReadObject< xAOD::HIEventShapeAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::HIEventShapeAuxContainer found" );
   return 0;
}
