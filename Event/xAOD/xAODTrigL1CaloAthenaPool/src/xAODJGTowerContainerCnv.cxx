/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <exception>

// Local include(s):
#include "xAODJGTowerContainerCnv.h"

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

xAODJGTowerContainerCnv::xAODJGTowerContainerCnv( ISvcLocator* svcLoc )
   : xAODJGTowerContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODJGTowerContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::JGTowerContainer*
xAODJGTowerContainerCnv::createPersistent( xAOD::JGTowerContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JGTowerContainer* result =
      new xAOD::JGTowerContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JGTowerContainer::iterator itr = result->begin();
   xAOD::JGTowerContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JGTowerContainer* xAODJGTowerContainerCnv::createTransient() {
  // The known ID(s) for this container:
  static pool::Guid guid( "d8759707-7ad9-4567-af03-753b7eed4521" );   

  // Check if we're reading the most up to date type:
  if( compareClassGuid( guid ) ) {
    xAOD::JGTowerContainer* c = poolReadObject< xAOD::JGTowerContainer >();
    setStoreLink( c , m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::JGTowerContainer found" );
  return 0;
}

void xAODJGTowerContainerCnv::
toPersistent( xAOD::JGTower* /*cluster*/ ) const {

   return;
}
