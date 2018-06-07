/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXCPHitsContainerCnv.h"

namespace {
  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    
    // Give it to the container:
    cont->setStore( link );

    return;
  }   
} // private namespace


xAODCMXCPHitsContainerCnv::xAODCMXCPHitsContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXCPHitsContainerCnvBase( svcLoc ) { 
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCMXCPHitsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
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
    xAOD::CMXCPHitsContainer* c = poolReadObject< xAOD::CMXCPHitsContainer >();
    setStoreLink( c , m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXCPHitsContainer found" );

 return 0;
}

void xAODCMXCPHitsContainerCnv::toPersistent( xAOD::CMXCPHits* ) const
{
  return;
}

