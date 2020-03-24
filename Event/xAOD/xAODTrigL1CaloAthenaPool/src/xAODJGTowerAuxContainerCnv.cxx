/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <exception>

// Local include(s):
#include "xAODJGTowerAuxContainerCnv.h"

xAODJGTowerAuxContainerCnv::
xAODJGTowerAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJGTowerAuxContainerCnvBase( svcLoc ) {

}

xAOD::JGTowerAuxContainer*
xAODJGTowerAuxContainerCnv::
createPersistent( xAOD::JGTowerAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::JGTowerAuxContainer* result =
      new xAOD::JGTowerAuxContainer( *trans );

   return result;
}



xAOD::JGTowerAuxContainer*
xAODJGTowerAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid guid( "b12228ce-de5c-49dd-82ba-d2d5436f42c1" );
//   static const pool::Guid v2_guid( "07FF691A-A920-43B2-A4E7-637D335E6929" );

   // Check which version of the container we're reading:
   if( compareClassGuid( guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JGTowerAuxContainer >();
   }
/*   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::JGTowerAuxContainer found - bail out for now" );
    return 0;    
   }*/

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JGTowerAuxContainer found" );
   return 0;
}
