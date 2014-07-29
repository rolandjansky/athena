/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2MbtsBitsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigT2MbtsBitsContainerCnv.h"

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


xAODTrigT2MbtsBitsContainerCnv::xAODTrigT2MbtsBitsContainerCnv( ISvcLocator* svcLoc )
  : xAODTrigT2MbtsBitsContainerCnvBase( svcLoc ) {

}

xAOD::TrigT2MbtsBitsContainer*
xAODTrigT2MbtsBitsContainerCnv::
createPersistent( xAOD::TrigT2MbtsBitsContainer* trans ) {

  // Create a view copy of the container:
  xAOD::TrigT2MbtsBitsContainer* result =
      new xAOD::TrigT2MbtsBitsContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::TrigT2MbtsBitsContainer::iterator itr = result->begin();
  xAOD::TrigT2MbtsBitsContainer::iterator end = result->end();
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
StatusCode xAODTrigT2MbtsBitsContainerCnv::createObj(IOpaqueAddress* pAddr,
						DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigT2MbtsBitsContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigT2MbtsBitsContainer* xAODTrigT2MbtsBitsContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "FF96BF06-1206-11E4-9605-02163E00A892" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::TrigT2MbtsBitsContainer* c = poolReadObject< xAOD::TrigT2MbtsBitsContainer >();
    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::TrigT2MbtsBitsContainer found" );
  return 0;
}

void xAODTrigT2MbtsBitsContainerCnv::
toPersistent( xAOD::TrigT2MbtsBits* obj ) const {
  ATH_MSG_VERBOSE("xAOD::TrigT2MbtsBits: triggerEnergies size:: "<<obj->triggerEnergies().size());
  ATH_MSG_VERBOSE("xAOD::TrigT2MbtsBits: triggerTimes size:: "<<obj->triggerTimes().size());
  return;
}
