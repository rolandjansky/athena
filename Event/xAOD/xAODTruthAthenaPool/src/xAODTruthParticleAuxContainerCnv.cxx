/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthParticleAuxContainerCnv.cxx 621073 2014-10-10 10:42:51Z krasznaa $

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODTruthParticleAuxContainerCnv.h"

xAODTruthParticleAuxContainerCnv::
xAODTruthParticleAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthParticleAuxContainerCnvBase( svcLoc ) {

}

xAOD::TruthParticleAuxContainer*
xAODTruthParticleAuxContainerCnv::
createPersistent( xAOD::TruthParticleAuxContainer* trans ) {

   // Access the thinning svc, if thinning is defined for this object:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Create the persistent object using the helper function from AthContainers:
   return SG::copyThinned( *trans, thinSvc );
}

xAOD::TruthParticleAuxContainer*
xAODTruthParticleAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "BA8FA08F-8DD6-420D-97D5-8B54EABECD65" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TruthParticleAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthParticleAuxContainer found" );
   return 0;
}
