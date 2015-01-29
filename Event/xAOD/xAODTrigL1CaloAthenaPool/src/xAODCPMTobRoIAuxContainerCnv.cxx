/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMTobRoIAuxContainerCnv.h"

xAODCPMTobRoIAuxContainerCnv::xAODCPMTobRoIAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODCPMTobRoIAuxContainerCnvBase( svcLoc ) {
}

xAOD::CPMTobRoIAuxContainer* 
xAODCPMTobRoIAuxContainerCnv::createPersistent( xAOD::CPMTobRoIAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::CPMTobRoIAuxContainer* result =
  new xAOD::CPMTobRoIAuxContainer( *trans );

  return result;
}

xAOD::CPMTobRoIAuxContainer*
xAODCPMTobRoIAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "38CB660E-139B-4280-9517-B571CA680A37" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CPMTobRoIAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::CPMTobRoIAuxContainer found" );

  return 0;
}

