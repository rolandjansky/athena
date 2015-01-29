/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXCPHitsContainerCnv.h"

xAODCMXCPHitsContainerCnv::xAODCMXCPHitsContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXCPHitsContainerCnvBase( svcLoc ) { 
}

xAOD::CMXCPHitsContainer* 
xAODCMXCPHitsContainerCnv::createPersistent( xAOD::CMXCPHitsContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXCPHitsContainer* result = 
  new xAOD::CMXCPHitsContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXCPHitsContainer::iterator itr = result->begin();
  xAOD::CMXCPHitsContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXCPHitsContainer* xAODCMXCPHitsContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "96ED5A94-473C-4E96-A348-78077310635F" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    return poolReadObject< xAOD::CMXCPHitsContainer >();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXCPHitsContainer found" );

 return 0;
}

void xAODCMXCPHitsContainerCnv::toPersistent( xAOD::CMXCPHits* ) const
{
  return;
}

