/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMTobRoIContainerCnv.h"

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


xAODCPMTobRoIContainerCnv::xAODCPMTobRoIContainerCnv( ISvcLocator* svcLoc ) :
  xAODCPMTobRoIContainerCnvBase( svcLoc ) { 
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCPMTobRoIContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CPMTobRoIContainer* 
xAODCPMTobRoIContainerCnv::createPersistent( xAOD::CPMTobRoIContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CPMTobRoIContainer* result = 
  new xAOD::CPMTobRoIContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CPMTobRoIContainer::iterator itr = result->begin();
  xAOD::CPMTobRoIContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CPMTobRoIContainer* xAODCPMTobRoIContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "B84B5967-8B6C-4743-AC18-FF68E13D9EA6" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::CPMTobRoIContainer* c = poolReadObject< xAOD::CPMTobRoIContainer >();
    setStoreLink( c , m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CPMTobRoIContainer found" );

 return 0;
}

void xAODCPMTobRoIContainerCnv::toPersistent( xAOD::CPMTobRoI* ) const
{
  return;
}

