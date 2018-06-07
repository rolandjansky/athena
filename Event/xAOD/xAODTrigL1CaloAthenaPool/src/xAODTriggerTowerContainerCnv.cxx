/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTriggerTowerContainerCnv.h"

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

xAODTriggerTowerContainerCnv::xAODTriggerTowerContainerCnv( ISvcLocator* svcLoc )
   : xAODTriggerTowerContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODTriggerTowerContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TriggerTowerContainer*
xAODTriggerTowerContainerCnv::createPersistent( xAOD::TriggerTowerContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TriggerTowerContainer* result =
      new xAOD::TriggerTowerContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TriggerTowerContainer::iterator itr = result->begin();
   xAOD::TriggerTowerContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TriggerTowerContainer* xAODTriggerTowerContainerCnv::createTransient() {
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "DE2A7891-B5FE-4811-8178-EF106743312C" );
  static pool::Guid v2_guid( "730DE7B8-C24A-4567-A66D-0386DC50E9AC" );   

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v2_guid ) ) {
    xAOD::TriggerTowerContainer* c = poolReadObject< xAOD::TriggerTowerContainer >();
    setStoreLink( c , m_key );
    return c;
  }
  if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::TriggerTowerContainer found - bail out for now" );
    return 0;    
  }   

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::TriggerTowerContainer found" );
  return 0;
}

void xAODTriggerTowerContainerCnv::
toPersistent( xAOD::TriggerTower* /*cluster*/ ) const {

   return;
}
