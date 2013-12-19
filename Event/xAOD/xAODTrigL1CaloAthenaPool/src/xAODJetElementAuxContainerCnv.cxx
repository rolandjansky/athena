/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetElementAuxContainerCnv.h"

xAODJetElementAuxContainerCnv::
xAODJetElementAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODJetElementAuxContainerCnvBase( svcLoc ) {

}

xAOD::JetElementAuxContainer*
xAODJetElementAuxContainerCnv::
createPersistent( xAOD::JetElementAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::JetElementAuxContainer* result =
      new xAOD::JetElementAuxContainer( *trans );

   return result;
}

xAOD::JetElementAuxContainer*
xAODJetElementAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "7A0A2307-5729-4306-B520-4F75BBCC8E76" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::JetElementAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetElementAuxContainer found" );
   return 0;
}
