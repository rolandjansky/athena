/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODAFPToFHitContainerCnv.h"


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
    
 
xAODAFPToFHitContainerCnv::xAODAFPToFHitContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPToFHitContainerCnvBase( svcLoc ) {
 
}
 
/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODAFPToFHitContainerCnv::createObj( IOpaqueAddress* pAddr,
						   DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPToFHitContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPToFHitContainer* xAODAFPToFHitContainerCnv::createPersistent( xAOD::AFPToFHitContainer* trans )
{
 
  // Create a view copy of the container:
  xAOD::AFPToFHitContainer* result =
    new xAOD::AFPToFHitContainer( trans->begin(), trans->end(),
				    SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
//  xAOD::AFPToFHitContainer::iterator itr = result->begin();
//  xAOD::AFPToFHitContainer::iterator end = result->end();
//  for( ; itr != end; ++itr ) {
//    toPersistent( *itr );
//  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPToFHitContainer* xAODAFPToFHitContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "AF3603F3-CDC9-4597-B111-155D9BC2E8D1" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::AFPToFHitContainer* c =
      poolReadObject< xAOD::AFPToFHitContainer >();
    setStoreLink( c, m_key );
    return c;
  }
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPToFHitContainer found" );
  return 0;
}
 
void xAODAFPToFHitContainerCnv::toPersistent( xAOD::AFPToFHit* hit ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  hit->toPersistent();
 
  return;
}

