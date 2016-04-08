/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>

// Local include(s):
#include "xAODRingSetContainerCnv.h"


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

xAODRingSetContainerCnv::xAODRingSetContainerCnv( ISvcLocator* svcLoc )
   : xAODRingSetContainerCnvBase( svcLoc ) {
}


/** 
 * This function needs to be re-implemented in order to figure out the StoreGate 
 * key of the container that's being created. After that's done, it lets the 
 * base class do its normal task. 
 */

StatusCode xAODRingSetContainerCnv::createObj( IOpaqueAddress* pAddr, 
					   DataObject*& pObj ) { 
  
  // Get the key of the container that we'll be creating: 
  m_key = *( pAddr->par() + 1 ); 
  ATH_MSG_VERBOSE( "Key of xAOD::RingSetContainer: " << m_key ); 
  
  // Let the base class do its thing now: 
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
} 


xAOD::RingSetContainer*
xAODRingSetContainerCnv::createPersistent( xAOD::RingSetContainer* trans ) {

  // Create a view copy of the container:
  xAOD::RingSetContainer* result =
     new xAOD::RingSetContainer( trans->begin(), trans->end(),
                             SG::VIEW_ELEMENTS );

  // Return the new container:
  return result;
}

xAOD::RingSetContainer* xAODRingSetContainerCnv::createTransient() {


  // The known ID(s) for this container:
  static pool::Guid v1_guid( "C9CFE7DE-44E4-40AF-8CBF-6CCCB8FB0A8F" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::RingSetContainer* c = poolReadObject< xAOD::RingSetContainer >(); 
    setStoreLink( c, m_key ); 
    return c; 
  }
  
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::RingSetContainer found" );
  return 0;
}
