/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXCPTobAuxContainerCnv.h"

xAODCMXCPTobAuxContainerCnv::xAODCMXCPTobAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXCPTobAuxContainerCnvBase( svcLoc ) {
}

xAOD::CMXCPTobAuxContainer* 
xAODCMXCPTobAuxContainerCnv::createPersistent( xAOD::CMXCPTobAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CMXCPTobAuxContainer* result =
  new xAOD::CMXCPTobAuxContainer( *trans );

  return result;
}

xAOD::CMXCPTobAuxContainer*
xAODCMXCPTobAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "B4A55AC8-3D30-42AF-B954-82DA7D8A07CB" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CMXCPTobAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CMXCPTobAuxContainer found" );

  return 0;
}

