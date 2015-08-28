/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXRoIAuxContainerCnv.h"

xAODCMXRoIAuxContainerCnv::xAODCMXRoIAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXRoIAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXRoIAuxContainer* 
xAODCMXRoIAuxContainerCnv::createPersistent( xAOD::CMXRoIAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXRoIAuxContainer* result =
  new xAOD::CMXRoIAuxContainer( *trans );

  return result;
}

xAOD::CMXRoIAuxContainer*
xAODCMXRoIAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "501FFFE3-9788-4A3C-9151-8466F32EF2B4" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXRoIAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXRoIAuxContainer found" );

  return 0;
}

