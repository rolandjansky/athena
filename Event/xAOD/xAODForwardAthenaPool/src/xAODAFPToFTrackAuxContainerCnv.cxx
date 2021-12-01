/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODAFPToFTrackAuxContainerCnv.h"

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// EDM include(s):
#include "xAODForward/versions/AFPToFTrackAuxContainer_v1.h"

// System include(s):
#include <exception>

xAODAFPToFTrackAuxContainerCnv::xAODAFPToFTrackAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPToFTrackAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPToFTrackAuxContainer* xAODAFPToFTrackAuxContainerCnv::createPersistent( xAOD::AFPToFTrackAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPToFTrackAuxContainer* xAODAFPToFTrackAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "85C26A25-81A9-4554-85D7-4432B064CC00" );

  // Check which version of the container we're reading:
  if ( compareClassGuid(v1_guid) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPToFTrackAuxContainer >();

  }


  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPToFTrackAuxContainer found" );
  return nullptr;
}
