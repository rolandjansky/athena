/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXJetHitsAuxContainerCnv.h"

xAODCMXJetHitsAuxContainerCnv::xAODCMXJetHitsAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXJetHitsAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXJetHitsAuxContainer* 
xAODCMXJetHitsAuxContainerCnv::createPersistent( xAOD::CMXJetHitsAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXJetHitsAuxContainer* result =
  new xAOD::CMXJetHitsAuxContainer( *trans );

  return result;
}

xAOD::CMXJetHitsAuxContainer*
xAODCMXJetHitsAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "509DB371-6A95-4DE1-B950-751B99EBF5EC" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXJetHitsAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXJetHitsAuxContainer found" );

  return 0;
}

