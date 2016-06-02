/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODSCTRDOAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODSCTRDOAuxContainerCnv::xAODSCTRDOAuxContainerCnv( ISvcLocator* svcLoc )
  : xAODSCTRDOAuxContainerCnvBase( svcLoc )
{ }

xAOD::SCTRawHitValidationAuxContainer*
xAODSCTRDOAuxContainerCnv::createPersistent( xAOD::SCTRawHitValidationAuxContainer* trans )
{
  // Create a thinned copy of the container:
  return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::SCTRawHitValidationAuxContainer* xAODSCTRDOAuxContainerCnv::createTransient() {
   // The known ID(s) for this container:
  static const pool::Guid v1_guid( "BC5A8B61-9429-4704-B8C7-50B2FB676775" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::SCTRawHitValidationAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::SCTRawHitValidationAuxContainer found." );
}
