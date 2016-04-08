/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetConfContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>

// Local include(s):
#include "xAODRingSetConfContainerCnv.h"


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

xAODRingSetConfContainerCnv::xAODRingSetConfContainerCnv( ISvcLocator* svcLoc )
   : xAODRingSetConfContainerCnvBase( svcLoc ) {
}


/** 
 * This function needs to be re-implemented in order to figure out the StoreGate 
 * key of the container that's being created. After that's done, it lets the 
 * base class do its normal task. 
 */

StatusCode xAODRingSetConfContainerCnv::createObj( IOpaqueAddress* pAddr, 
					   DataObject*& pObj ) { 
  
  // Get the key of the container that we'll be creating: 
  m_key = *( pAddr->par() + 1 ); 
  ATH_MSG_VERBOSE( "Key of xAOD::RingSetConfContainer: " << m_key ); 
  
  // Let the base class do its thing now: 
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
} 


xAOD::RingSetConfContainer*
xAODRingSetConfContainerCnv::createPersistent( xAOD::RingSetConfContainer* trans ) {

  // Create a view copy of the container:
  xAOD::RingSetConfContainer* result =
     new xAOD::RingSetConfContainer( trans->begin(), trans->end(),
                             SG::VIEW_ELEMENTS );

  // Return the new container:
  return result;
}

xAOD::RingSetConfContainer* xAODRingSetConfContainerCnv::createTransient() {


  // The known ID(s) for this container:
  static pool::Guid v1_guid( "A3E47D26-235E-49BC-8D9A-52A9CA097D7D" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::RingSetConfContainer* c = poolReadObject< xAOD::RingSetConfContainer >(); 
    setStoreLink( c, m_key ); 
    return c; 
  }
  
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::RingSetConfContainer found" );
  return 0;
}
