/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODAFPTrackAuxContainerCnv.h"

xAODAFPTrackAuxContainerCnv::xAODAFPTrackAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPTrackAuxContainerCnvBase( svcLoc ) {

}

xAOD::AFPTrackAuxContainer* xAODAFPTrackAuxContainerCnv::createPersistent( xAOD::AFPTrackAuxContainer* trans ) {

  // Access the thinning svc, if thinning is defined for this object:
  IThinningSvc* thinSvc = IThinningSvc::instance();

  // Create the persistent object using the helper function from AthContainers:
  return SG::copyThinned( *trans, thinSvc );
}

xAOD::AFPTrackAuxContainer* xAODAFPTrackAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "07BAB867-F028-4CCA-829A-3A6B93AD3411" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPTrackAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPTrackAuxContainer found" );
  return 0;
}
