/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMTowerContainerCnv.h"

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


xAODCPMTowerContainerCnv::xAODCPMTowerContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMTowerContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCPMTowerContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CPMTowerContainer*
xAODCPMTowerContainerCnv::
createPersistent( xAOD::CPMTowerContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CPMTowerContainer* result =
      new xAOD::CPMTowerContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CPMTowerContainer::iterator itr = result->begin();
   xAOD::CPMTowerContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CPMTowerContainer* xAODCPMTowerContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "D220C61E-ECD1-427C-86D0-2C88E00ABBD2" );
   static pool::Guid v2_guid( "87CC5511-E7E5-4068-8F40-F44B0A1E9A3F" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {
     xAOD::CPMTowerContainer* c = poolReadObject< xAOD::CPMTowerContainer >();
     setStoreLink( c , m_key );
     return c;
   }
   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::CPMTowerContainer found - bail out for now" );
    return 0;    
   }   

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMTowerContainer found" );
   return 0;
}

void xAODCPMTowerContainerCnv::
toPersistent( xAOD::CPMTower* /*cluster*/ ) const {

   return;
}
