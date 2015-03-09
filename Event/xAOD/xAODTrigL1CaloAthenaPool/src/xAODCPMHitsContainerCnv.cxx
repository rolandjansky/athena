/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMHitsContainerCnv.h"

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


xAODCPMHitsContainerCnv::xAODCPMHitsContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMHitsContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCPMHitsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CPMHitsContainer*
xAODCPMHitsContainerCnv::
createPersistent( xAOD::CPMHitsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CPMHitsContainer* result =
      new xAOD::CPMHitsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CPMHitsContainer::iterator itr = result->begin();
   xAOD::CPMHitsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CPMHitsContainer* xAODCPMHitsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "3D15DF3F-E014-42E3-9614-37E4CD535F9C" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::CPMHitsContainer* c = poolReadObject< xAOD::CPMHitsContainer >();
     setStoreLink( c , m_key );
     return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMHitsContainer found" );
   return 0;
}

void xAODCPMHitsContainerCnv::
toPersistent( xAOD::CPMHits* /*cluster*/ ) const {

   return;
}
