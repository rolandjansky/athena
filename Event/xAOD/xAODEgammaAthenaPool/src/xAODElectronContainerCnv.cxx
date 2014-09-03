/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODElectronContainerCnv.cxx 596436 2014-05-11 17:34:00Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODElectronContainerCnv.h"


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

xAODElectronContainerCnv::xAODElectronContainerCnv( ISvcLocator* svcLoc )
   : xAODElectronContainerCnvBase( svcLoc ) {
}


/** 
 * This function needs to be re-implemented in order to figure out the StoreGate 
 * key of the container that's being created. After that's done, it lets the 
 * base class do its normal task. 
 */

StatusCode xAODElectronContainerCnv::createObj( IOpaqueAddress* pAddr, 
					   DataObject*& pObj ) { 
  
  // Get the key of the container that we'll be creating: 
  m_key = *( pAddr->par() + 1 ); 
  ATH_MSG_VERBOSE( "Key of xAOD::ElectronContainer: " << m_key ); 
  
  // Let the base class do its thing now: 
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
} 


xAOD::ElectronContainer*
xAODElectronContainerCnv::createPersistent( xAOD::ElectronContainer* trans ) {

   // Create a view copy of the container:
   xAOD::ElectronContainer* result =
      new xAOD::ElectronContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );
   // Prepare the objects to be written out:
   xAOD::ElectronContainer::iterator itr = result->begin();
   xAOD::ElectronContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }
   // Return the new container:
   return result;
}

xAOD::ElectronContainer* xAODElectronContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "9CA52CF4-E219-45B8-9971-6DAA89125952" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::ElectronContainer* c = poolReadObject< xAOD::ElectronContainer >(); 
     setStoreLink( c, m_key ); 
     return c; 
   }
   
   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::ElectronContainer found" );
   return 0;
}

void xAODElectronContainerCnv::toPersistent( xAOD::Electron* electron ) const {

   // Tell the object to prepare all its smart pointers for persistification:
  size_t nClusters= electron->nCaloClusters();
  for (size_t i=0; i<nClusters;++i){
    ElementLink<xAOD::CaloClusterContainer>& link = const_cast<ElementLink<xAOD::CaloClusterContainer>& > (electron->caloClusterLink(i));
    link.toPersistent();
  }

  size_t nTrackParticles= electron->nTrackParticles();
  for (size_t i=0; i<nTrackParticles;++i){
    ElementLink<xAOD::TrackParticleContainer>& link = const_cast<ElementLink<xAOD::TrackParticleContainer>& > (electron->trackParticleLink(i));
    link.toPersistent();
  }
  
   return;
}
