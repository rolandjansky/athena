/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonContainerCnv.cxx 741490 2016-04-20 01:34:00Z christos $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODPhotonContainerCnv.h"


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

xAODPhotonContainerCnv::xAODPhotonContainerCnv( ISvcLocator* svcLoc )
   : xAODPhotonContainerCnvBase( svcLoc ) {

}

/** 
 * This function needs to be re-implemented in order to figure out the StoreGate 
 * key of the container that's being created. After that's done, it lets the 
 * base class do its normal task. 
 */

StatusCode xAODPhotonContainerCnv::createObj( IOpaqueAddress* pAddr, 
					   DataObject*& pObj ) { 
  
  // Get the key of the container that we'll be creating: 
  m_key = *( pAddr->par() + 1 ); 
  ATH_MSG_VERBOSE( "Key of xAOD::PhotonContainer: " << m_key ); 
  
  // Let the base class do its thing now: 
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
} 


xAOD::PhotonContainer*
xAODPhotonContainerCnv::createPersistent( xAOD::PhotonContainer* trans ) {

   // Create a view copy of the container:
   xAOD::PhotonContainer* result =
     new xAOD::PhotonContainer( trans->begin(), trans->end(),
				SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::PhotonContainer* xAODPhotonContainerCnv::createTransient() {

   // The known ID(s) for this container:
  static const pool::Guid v1_guid( "5F045AAE-DBD8-47E4-90AC-9162530A9565" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::PhotonContainer* c = poolReadObject< xAOD::PhotonContainer >(); 
     setStoreLink( c, m_key ); 
     return c; 
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::PhotonContainer found" );
   return 0;
}


