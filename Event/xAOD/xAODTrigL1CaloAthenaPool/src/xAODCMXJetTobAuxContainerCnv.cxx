/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXJetTobAuxContainerCnv.h"

xAODCMXJetTobAuxContainerCnv::xAODCMXJetTobAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXJetTobAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXJetTobAuxContainer* 
xAODCMXJetTobAuxContainerCnv::createPersistent( xAOD::CMXJetTobAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXJetTobAuxContainer* result =
  new xAOD::CMXJetTobAuxContainer( *trans );

  return result;
}

xAOD::CMXJetTobAuxContainer*
xAODCMXJetTobAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "094F5655-229B-49BE-870D-1E4D13866CED" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXJetTobAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXJetTobAuxContainer found" );

  return 0;
}

