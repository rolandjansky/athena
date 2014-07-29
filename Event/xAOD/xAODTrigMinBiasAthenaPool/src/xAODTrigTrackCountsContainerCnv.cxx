/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigTrackCountsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigTrackCountsContainerCnv.h"

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


xAODTrigTrackCountsContainerCnv::xAODTrigTrackCountsContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigTrackCountsContainerCnvBase( svcLoc ) {

}

xAOD::TrigTrackCountsContainer*
xAODTrigTrackCountsContainerCnv::
createPersistent( xAOD::TrigTrackCountsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigTrackCountsContainer* result =
      new xAOD::TrigTrackCountsContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TrigTrackCountsContainer::iterator itr = result->begin();
   xAOD::TrigTrackCountsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/
StatusCode xAODTrigTrackCountsContainerCnv::createObj(IOpaqueAddress* pAddr,
                                                DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigTrackCountsContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigTrackCountsContainer* xAODTrigTrackCountsContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "0650DED0-1207-11E4-9CD1-02163E00A892" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::TrigTrackCountsContainer* c = poolReadObject< xAOD::TrigTrackCountsContainer >();
     setStoreLink( c, m_key );
     return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigTrackCountsContainer found" );
   return 0;
}

void xAODTrigTrackCountsContainerCnv::
toPersistent( xAOD::TrigTrackCounts* obj ) const {
    ATH_MSG_VERBOSE( "xAOD::TrigTrackCountsContainer: zOBins: " << obj->z0Bins() );
   return;
}
