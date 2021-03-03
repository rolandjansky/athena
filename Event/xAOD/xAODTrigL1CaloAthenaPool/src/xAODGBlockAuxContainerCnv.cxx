/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODGBlockAuxContainerCnv.h"

xAODGBlockAuxContainerCnv::
    xAODGBlockAuxContainerCnv(ISvcLocator *svcLoc)
    : xAODGBlockAuxContainerCnvBase(svcLoc)
{
}

xAOD::GBlockAuxContainer *
xAODGBlockAuxContainerCnv::
    createPersistent(xAOD::GBlockAuxContainer *trans)
{

  // Create a copy of the container:
  xAOD::GBlockAuxContainer *result =
      new xAOD::GBlockAuxContainer(*trans);

  return result;
}

xAOD::GBlockAuxContainer *
xAODGBlockAuxContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static const pool::Guid guid("52F340EE-B2CB-4F65-A865-670E20D43383");
  //   static const pool::Guid v2_guid( "07FF691A-A920-43B2-A4E7-637D335E6929" );

  // Check which version of the container we're reading:
  if (compareClassGuid(guid))
  {
    // It's the latest version, read it directly:
    return poolReadObject<xAOD::GBlockAuxContainer>();
  }
  /*   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::GBlockAuxContainer found - bail out for now" );
    return 0;    
   }*/

  // If we didn't recognise the ID:
  throw std::runtime_error("Unsupported version of "
                           "xAOD::GBlockAuxContainer found");
  return 0;
}
