/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>
#include <memory>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// EDM include(s):
#include "xAODForward/versions/AFPSiHitAuxContainer_v1.h"

// Local include(s):
#include "xAODAFPSiHitAuxContainerCnv.h"
#include "xAODAFPSiHitAuxContainerCnv_v1.h"

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
  static const pool::Guid v2_guid( "E14F7E45-B4A5-4DC0-82AB-A1135FFD2C78" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v2_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::AFPSiHitAuxContainer >();
  } else if ( compareClassGuid(v1_guid) ) {
    static xAODAFPSiHitAuxContainerCnv_v1 converter;
    // Read in the v1 object:
    std::unique_ptr< xAOD::AFPSiHitAuxContainer_v1 > old( poolReadObject< xAOD::AFPSiHitAuxContainer_v1 >() );
    // Return the converted object:
    return converter.createTransient( old.get(), msg() );
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitAuxContainer found" );
  return nullptr;
}


