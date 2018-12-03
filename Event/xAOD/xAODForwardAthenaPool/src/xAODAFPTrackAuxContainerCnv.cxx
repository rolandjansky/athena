/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// EDM include(s):
#include "xAODForward/versions/AFPTrackAuxContainer_v1.h"


// Local include(s):
#include "xAODAFPTrackAuxContainerCnv.h"
#include "xAODAFPTrackAuxContainerCnv_v1.h"

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
  static const pool::Guid v2_guid( "21E536F5-7B58-4BDE-8CEE-3FF40E355C3F" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v2_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPTrackAuxContainer >();
  } else if ( compareClassGuid(v1_guid) ) {
    static xAODAFPTrackAuxContainerCnv_v1 converter;
    // Read in the v1 object:
    std::unique_ptr< xAOD::AFPTrackAuxContainer_v1 > old( poolReadObject< xAOD::AFPTrackAuxContainer_v1 >() );
    // Return the converted object:
    return converter.createTransient( old.get(), msg() );
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPTrackAuxContainer found" );
  return nullptr;
}
