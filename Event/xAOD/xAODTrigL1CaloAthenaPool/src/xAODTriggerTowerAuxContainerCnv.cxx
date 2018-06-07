/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTriggerTowerAuxContainerCnv.h"

xAODTriggerTowerAuxContainerCnv::
xAODTriggerTowerAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTriggerTowerAuxContainerCnvBase( svcLoc ) {

}

xAOD::TriggerTowerAuxContainer*
xAODTriggerTowerAuxContainerCnv::
createPersistent( xAOD::TriggerTowerAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TriggerTowerAuxContainer* result =
      new xAOD::TriggerTowerAuxContainer( *trans );

   return result;
}

xAOD::TriggerTowerAuxContainer*
xAODTriggerTowerAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "3C5D20CF-6F77-4E67-B182-674EDFD2FE38" );
   static const pool::Guid v2_guid( "07FF691A-A920-43B2-A4E7-637D335E6929" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TriggerTowerAuxContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::TriggerTowerAuxContainer found - bail out for now" );
    return 0;    
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TriggerTowerAuxContainer found" );
   return 0;
}
