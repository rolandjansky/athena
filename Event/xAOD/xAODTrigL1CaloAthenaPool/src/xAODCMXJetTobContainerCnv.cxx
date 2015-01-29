/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXJetTobContainerCnv.h"

xAODCMXJetTobContainerCnv::xAODCMXJetTobContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXJetTobContainerCnvBase( svcLoc ) { 
}

xAOD::CMXJetTobContainer* 
xAODCMXJetTobContainerCnv::createPersistent( xAOD::CMXJetTobContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXJetTobContainer* result = 
  new xAOD::CMXJetTobContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXJetTobContainer::iterator itr = result->begin();
  xAOD::CMXJetTobContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXJetTobContainer* xAODCMXJetTobContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "4116184D-A8AF-43B1-9990-9DF5C1010AB6" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    return poolReadObject< xAOD::CMXJetTobContainer >();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXJetTobContainer found" );

 return 0;
}

void xAODCMXJetTobContainerCnv::toPersistent( xAOD::CMXJetTob* ) const
{
  return;
}

