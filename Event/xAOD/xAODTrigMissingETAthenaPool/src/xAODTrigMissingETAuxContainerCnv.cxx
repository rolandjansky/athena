/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMissingETAuxContainerCnv.cxx 592512 2014-04-11 09:38:00Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigMissingETAuxContainerCnv.h"

xAODTrigMissingETAuxContainerCnv::xAODTrigMissingETAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigMissingETAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigMissingETAuxContainer*
xAODTrigMissingETAuxContainerCnv::
createPersistent( xAOD::TrigMissingETAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::TrigMissingETAuxContainer* result =
      new xAOD::TrigMissingETAuxContainer( *trans );

   return result;
}

xAOD::TrigMissingETAuxContainer* xAODTrigMissingETAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "E8C1613E-5E47-4B7F-8ED7-B30A8FE21DB4" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigMissingETAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigMissingETAuxContainer found" );
   return 0;
}
