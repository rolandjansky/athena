/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloRingsContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>

// Local include(s):
#include "xAODCaloRingsContainerCnv.h"


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

xAODCaloRingsContainerCnv::xAODCaloRingsContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloRingsContainerCnvBase( svcLoc ) {
}


/** 
 * This function needs to be re-implemented in order to figure out the StoreGate 
 * key of the container that's being created. After that's done, it lets the 
 * base class do its normal task. 
 */

StatusCode xAODCaloRingsContainerCnv::createObj( IOpaqueAddress* pAddr, 
					   DataObject*& pObj ) { 
  
  // Get the key of the container that we'll be creating: 
  m_key = *( pAddr->par() + 1 ); 
  ATH_MSG_VERBOSE( "Key of xAOD::CaloRingsContainer: " << m_key ); 
  
  // Let the base class do its thing now: 
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
} 


xAOD::CaloRingsContainer*
xAODCaloRingsContainerCnv::createPersistent( xAOD::CaloRingsContainer* trans ) {

  // Create a view copy of the container:
  xAOD::CaloRingsContainer* result =
     new xAOD::CaloRingsContainer( trans->begin(), trans->end(),
                             SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  //msg() << MSG::INFO << "before loop." << endreq;
  //for( xAOD::CaloRings *cr : *result ) {
  //  std::cout << "Rings on transient: ";
  //  for ( xAOD::RingSet *rs : *cr ) {
  //    for ( float rE : *rs ) {
  //      std::cout << rE << " ";
  //    }
  //  }
  //  std::cout << std::endl;
  //  cr->toPersistent();
  //}

  // Return the new container:
  return result;
}

xAOD::CaloRingsContainer* xAODCaloRingsContainerCnv::createTransient() {


  // The known ID(s) for this container:
  static pool::Guid v1_guid( "AC758B30-A488-4AC5-AB42-34B1C97CFE44" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::CaloRingsContainer* c = poolReadObject< xAOD::CaloRingsContainer >(); 
    setStoreLink( c, m_key ); 
    return c; 
  }
  
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::CaloRingsContainer found" );
  return 0;
}
