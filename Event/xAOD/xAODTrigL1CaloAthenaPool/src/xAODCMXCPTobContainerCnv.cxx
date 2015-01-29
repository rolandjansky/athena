/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXCPTobContainerCnv.h"

xAODCMXCPTobContainerCnv::xAODCMXCPTobContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXCPTobContainerCnvBase( svcLoc ) { 
}

xAOD::CMXCPTobContainer* 
xAODCMXCPTobContainerCnv::createPersistent( xAOD::CMXCPTobContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXCPTobContainer* result = 
  new xAOD::CMXCPTobContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXCPTobContainer::iterator itr = result->begin();
  xAOD::CMXCPTobContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXCPTobContainer* xAODCMXCPTobContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "5E70EF98-47ED-441F-8CC3-F1906AA2E8CD" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    return poolReadObject< xAOD::CMXCPTobContainer >();
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXCPTobContainer found" );

 return 0;
}

void xAODCMXCPTobContainerCnv::toPersistent( xAOD::CMXCPTob* ) const
{
  return;
}

