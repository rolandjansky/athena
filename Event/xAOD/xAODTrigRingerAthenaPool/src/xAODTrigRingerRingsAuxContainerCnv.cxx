/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRingerRingsAuxContainerCnv.cxx 707600 2015-11-12 19:55:33Z krasznaa $

// System include(s):
#include <exception>

// Core include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s)
#include "xAODTrigRingerRingsAuxContainerCnv.h"
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

xAODTrigRingerRingsAuxContainerCnv::
xAODTrigRingerRingsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigRingerRingsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigRingerRingsAuxContainer*
xAODTrigRingerRingsAuxContainerCnv::
createPersistent( xAOD::TrigRingerRingsAuxContainer* trans ) {

   // Create a (possibly thinned) copy of the container:
   return SG::copyThinned( *trans, IThinningSvc::instance() );
}

xAOD::TrigRingerRingsAuxContainer*
xAODTrigRingerRingsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "4016ADBB-830C-4523-97E2-D5FED349D98D" );
   static const pool::Guid v2_guid( "5C14CCCA-B46E-4E2A-97F4-A15EAACB124C" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigRingerRingsAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODTrigRingerRingsAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::TrigRingerRingsAuxContainer_v1 >
         old( poolReadObject< xAOD::TrigRingerRingsAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );   
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRingerRingsAuxContainer found" );

   return 0;
}
