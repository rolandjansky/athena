/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODAFPSiHitContainerCnv.h"


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
    
 
xAODAFPSiHitContainerCnv::xAODAFPSiHitContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPSiHitContainerCnvBase( svcLoc ) {
 
}
 
/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODAFPSiHitContainerCnv::createObj( IOpaqueAddress* pAddr,
						   DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPSiHitContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPSiHitContainer* xAODAFPSiHitContainerCnv::createPersistent( xAOD::AFPSiHitContainer* trans )
{
 
  // Create a view copy of the container:
  xAOD::AFPSiHitContainer* result =
    new xAOD::AFPSiHitContainer( trans->begin(), trans->end(),
				    SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPSiHitContainer::iterator itr = result->begin();
  xAOD::AFPSiHitContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPSiHitContainer* xAODAFPSiHitContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "6BE5A00E-A13D-4904-9358-CB28D4C57879" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::AFPSiHitContainer* c =
      poolReadObject< xAOD::AFPSiHitContainer >();
    setStoreLink( c, m_key );
    return c;
  }
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitContainer found" );
  return 0;
}
 
void xAODAFPSiHitContainerCnv::toPersistent( xAOD::AFPSiHit* hit ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  hit->toPersistent();
 
  return;
}

