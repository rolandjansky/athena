/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAFPSiHitAuxContainerCnv.h"

xAODAFPSiHitAuxContainerCnv::xAODAFPSiHitAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPSiHitAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPSiHitAuxContainer* xAODAFPSiHitAuxContainerCnv::createPersistent( xAOD::AFPSiHitAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPSiHitAuxContainer* xAODAFPSiHitAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "B7C50786-FA4B-4D48-86C3-180EF5E6DE43" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPSiHitAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitAuxContainer found" );
  return 0;
}


