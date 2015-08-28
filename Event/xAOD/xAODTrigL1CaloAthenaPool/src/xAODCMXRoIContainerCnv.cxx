/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXRoIContainerCnv.h"

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


xAODCMXRoIContainerCnv::xAODCMXRoIContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXRoIContainerCnvBase( svcLoc ) { 
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCMXRoIContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CMXRoIContainer* 
xAODCMXRoIContainerCnv::createPersistent( xAOD::CMXRoIContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXRoIContainer* result = 
  new xAOD::CMXRoIContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXRoIContainer::iterator itr = result->begin();
  xAOD::CMXRoIContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXRoIContainer* xAODCMXRoIContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "37420C80-6007-43B0-ADD3-63FAF6088EC8" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::CMXRoIContainer* c = poolReadObject< xAOD::CMXRoIContainer >();
    setStoreLink( c , m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXRoIContainer found" );

 return 0;
}

void xAODCMXRoIContainerCnv::toPersistent( xAOD::CMXRoI* ) const
{
  return;
}

