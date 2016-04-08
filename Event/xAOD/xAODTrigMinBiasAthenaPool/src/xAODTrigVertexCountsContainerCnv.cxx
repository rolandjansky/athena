/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigVertexCountsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigVertexCountsContainerCnv.h"

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


xAODTrigVertexCountsContainerCnv::xAODTrigVertexCountsContainerCnv( ISvcLocator* svcLoc )
  : xAODTrigVertexCountsContainerCnvBase( svcLoc ) {

}

xAOD::TrigVertexCountsContainer*
xAODTrigVertexCountsContainerCnv::
createPersistent( xAOD::TrigVertexCountsContainer* trans ) {

  // Create a view copy of the container:
  xAOD::TrigVertexCountsContainer* result =
      new xAOD::TrigVertexCountsContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::TrigVertexCountsContainer::iterator itr = result->begin();
  xAOD::TrigVertexCountsContainer::iterator end = result->end();
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
StatusCode xAODTrigVertexCountsContainerCnv::createObj(IOpaqueAddress* pAddr,
						DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigVertexCountsContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigVertexCountsContainer* xAODTrigVertexCountsContainerCnv::createTransient() {
  ATH_MSG_VERBOSE("try to run createTransient()");
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "07EE6B72-1207-11E4-8B69-02163E00A892" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::TrigVertexCountsContainer* c = poolReadObject< xAOD::TrigVertexCountsContainer >();
    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::TrigVertexCountsContainer found" );
  return 0;
}

void xAODTrigVertexCountsContainerCnv::
toPersistent( xAOD::TrigVertexCounts* obj ) const {
  ATH_MSG_VERBOSE("xAOD::TrigVertexCounts: vtxNtrks size:: "<< obj->vtxNtrks().size() );
  ATH_MSG_VERBOSE("xAOD::TrigVertexCounts: vtxTrkPtSqSum size:: "<< obj->vtxTrkPtSqSum().size() );

  return;
}
