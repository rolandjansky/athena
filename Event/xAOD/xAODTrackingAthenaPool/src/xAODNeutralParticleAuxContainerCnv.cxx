/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODNeutralParticleAuxContainerCnv.cxx 581533 2014-02-05 09:28:36Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODNeutralParticleAuxContainerCnv.h"

xAODNeutralParticleAuxContainerCnv::
xAODNeutralParticleAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODNeutralParticleAuxContainerCnvBase( svcLoc ) {

}

xAOD::NeutralParticleAuxContainer*
xAODNeutralParticleAuxContainerCnv::
createPersistent( xAOD::NeutralParticleAuxContainer* trans ) {

   // Create a copy of the container:
   xAOD::NeutralParticleAuxContainer* result =
      new xAOD::NeutralParticleAuxContainer( *trans );

   return result;
}

xAOD::NeutralParticleAuxContainer*
xAODNeutralParticleAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "006862BC-5CEB-11E3-9FB4-02163E00A92F" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::NeutralParticleAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::NeutralParticleAuxContainer found" );
   return 0;
}
