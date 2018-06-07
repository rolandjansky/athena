/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMXEtSumsContainerCnv.h"

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


xAODCMXEtSumsContainerCnv::xAODCMXEtSumsContainerCnv( ISvcLocator* svcLoc ) :
  xAODCMXEtSumsContainerCnvBase( svcLoc ) { 
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCMXEtSumsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CMXEtSumsContainer* 
xAODCMXEtSumsContainerCnv::createPersistent( xAOD::CMXEtSumsContainer* trans )
{
  // Create a view copy of the container:
  xAOD::CMXEtSumsContainer* result = 
  new xAOD::CMXEtSumsContainer( trans->begin() , trans->end() , SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::CMXEtSumsContainer::iterator itr = result->begin();
  xAOD::CMXEtSumsContainer::iterator end = result->end();
  for( ; itr != end; ++itr ){
    toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

xAOD::CMXEtSumsContainer* xAODCMXEtSumsContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "751F0DD4-FF7F-4950-9928-3198DA4B1AB4" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::CMXEtSumsContainer* c = poolReadObject< xAOD::CMXEtSumsContainer >();
    setStoreLink( c , m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of xAOD::CMXEtSumsContainer found" );

 return 0;
}

void xAODCMXEtSumsContainerCnv::toPersistent( xAOD::CMXEtSums* ) const
{
  return;
}

