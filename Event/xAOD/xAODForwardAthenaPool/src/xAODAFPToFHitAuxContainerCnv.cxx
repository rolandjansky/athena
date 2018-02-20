/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAFPToFHitAuxContainerCnv.h"

xAODAFPToFHitAuxContainerCnv::xAODAFPToFHitAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPToFHitAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPToFHitAuxContainer* xAODAFPToFHitAuxContainerCnv::createPersistent( xAOD::AFPToFHitAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPToFHitAuxContainer* xAODAFPToFHitAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "65012075-753E-4F3E-8A37-5BBB66C523D6" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPToFHitAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPToFHitAuxContainer found" );
  return 0;
}


