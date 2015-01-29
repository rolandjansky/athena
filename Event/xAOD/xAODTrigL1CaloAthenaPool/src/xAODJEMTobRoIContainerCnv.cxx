/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMTobRoIContainerCnv.h"

xAODJEMTobRoIContainerCnv::xAODJEMTobRoIContainerCnv( ISvcLocator* svcLoc ) :
  xAODJEMTobRoIContainerCnvBase( svcLoc ) { 
}

xAOD::JEMTobRoIContainer* 
xAODJEMTobRoIContainerCnv::createPersistent( xAOD::JEMTobRoIContainer* trans )
{
  // Create a view copy of the container:
  xAOD::JEMTobRoIContainer* result = 
  new xAOD::JEMTobRoIContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::JEMTobRoIContainer::iterator itr = result->begin();
  xAOD::JEMTobRoIContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::JEMTobRoIContainer* xAODJEMTobRoIContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "2C7D0000-4D7B-45D4-8EE5-B18F172A70C3" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    return poolReadObject< xAOD::JEMTobRoIContainer >();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::JEMTobRoIContainer found" );

 return 0;
}

void xAODJEMTobRoIContainerCnv::toPersistent( xAOD::JEMTobRoI* ) const
{
  return;
}

