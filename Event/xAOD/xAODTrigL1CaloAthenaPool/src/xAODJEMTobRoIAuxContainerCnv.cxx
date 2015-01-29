/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMTobRoIAuxContainerCnv.h"

xAODJEMTobRoIAuxContainerCnv::xAODJEMTobRoIAuxContainerCnv( ISvcLocator* svcLoc ) :
  xAODJEMTobRoIAuxContainerCnvBase( svcLoc ) {
}

xAOD::JEMTobRoIAuxContainer* 
xAODJEMTobRoIAuxContainerCnv::createPersistent( xAOD::JEMTobRoIAuxContainer* trans )
{
  // Create a copy of the container:
  xAOD::JEMTobRoIAuxContainer* result =
  new xAOD::JEMTobRoIAuxContainer( *trans );

  return result;
}

xAOD::JEMTobRoIAuxContainer*
xAODJEMTobRoIAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "2EAA65B1-B1F3-4530-9413-E572D6282996" );

  // Check which version of the container we're reading:
  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::JEMTobRoIAuxContainer >();
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of xAOD::JEMTobRoIAuxContainer found" );

  return 0;
}

