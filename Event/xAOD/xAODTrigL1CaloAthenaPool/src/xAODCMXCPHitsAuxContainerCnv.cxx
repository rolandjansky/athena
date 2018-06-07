/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXCPHitsAuxContainerCnv.h"

xAODCMXCPHitsAuxContainerCnv::xAODCMXCPHitsAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXCPHitsAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXCPHitsAuxContainer* 
xAODCMXCPHitsAuxContainerCnv::createPersistent( xAOD::CMXCPHitsAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXCPHitsAuxContainer* result =
  new xAOD::CMXCPHitsAuxContainer( *trans );

  return result;
}

xAOD::CMXCPHitsAuxContainer*
xAODCMXCPHitsAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "F3F8A07D-536B-42EB-9792-E08DB1F76C1F" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXCPHitsAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXCPHitsAuxContainer found" );

  return 0;
}

