/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXEtSumsAuxContainerCnv.h"

xAODCMXEtSumsAuxContainerCnv::xAODCMXEtSumsAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXEtSumsAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXEtSumsAuxContainer* 
xAODCMXEtSumsAuxContainerCnv::createPersistent( xAOD::CMXEtSumsAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXEtSumsAuxContainer* result =
  new xAOD::CMXEtSumsAuxContainer( *trans );

  return result;
}

xAOD::CMXEtSumsAuxContainer*
xAODCMXEtSumsAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "B96297C3-769B-4084-9524-9D4D233B4896" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXEtSumsAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXEtSumsAuxContainer found" );

  return 0;
}

