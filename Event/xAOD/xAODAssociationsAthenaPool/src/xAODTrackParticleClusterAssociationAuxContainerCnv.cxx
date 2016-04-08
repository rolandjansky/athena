/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrackParticleClusterAssociationAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrackParticleClusterAssociationAuxContainerCnv::
xAODTrackParticleClusterAssociationAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackParticleClusterAssociationAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrackParticleClusterAssociationAuxContainer*
xAODTrackParticleClusterAssociationAuxContainerCnv::
createPersistent( xAOD::TrackParticleClusterAssociationAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrackParticleClusterAssociationAuxContainer*
xAODTrackParticleClusterAssociationAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "1E0569F0-C3A0-4EB2-9264-85B8F1C10D26" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrackParticleClusterAssociationAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackParticleClusterAssociationAuxContainer found" );
   return 0;
}
