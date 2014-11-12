/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s)
#include "xAODTrigRingerRingsAuxContainerCnv.h"

xAODTrigRingerRingsAuxContainerCnv::xAODTrigRingerRingsAuxContainerCnv( ISvcLocator* svcLoc ): xAODTrigRingerRingsAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigRingerRingsAuxContainer* xAODTrigRingerRingsAuxContainerCnv::createPersistent( xAOD::TrigRingerRingsAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigRingerRingsAuxContainer  *result = new xAOD::TrigRingerRingsAuxContainer( *trans );
   return result;
}

xAOD::TrigRingerRingsAuxContainer* xAODTrigRingerRingsAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "4016ADBB-830C-4523-97E2-D5FED349D98D" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigRingerRingsAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRingerRingsAuxContainer found" );
   return 0;
}
