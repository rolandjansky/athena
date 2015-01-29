/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXJetHitsContainerCnv.h"

xAODCMXJetHitsContainerCnv::xAODCMXJetHitsContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXJetHitsContainerCnvBase( svcLoc ) { 
}

xAOD::CMXJetHitsContainer* 
xAODCMXJetHitsContainerCnv::createPersistent( xAOD::CMXJetHitsContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXJetHitsContainer* result = 
  new xAOD::CMXJetHitsContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXJetHitsContainer::iterator itr = result->begin();
  xAOD::CMXJetHitsContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXJetHitsContainer* xAODCMXJetHitsContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "492E21EF-A9B0-4262-80EE-4ADECDEA44E7" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    return poolReadObject< xAOD::CMXJetHitsContainer >();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXJetHitsContainer found" );

 return 0;
}

void xAODCMXJetHitsContainerCnv::toPersistent( xAOD::CMXJetHits* ) const
{
  return;
}

