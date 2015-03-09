/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigT2ZdcSignalsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigT2ZdcSignalsContainerCnv.h"

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


xAODTrigT2ZdcSignalsContainerCnv::xAODTrigT2ZdcSignalsContainerCnv( ISvcLocator* svcLoc )
  : xAODTrigT2ZdcSignalsContainerCnvBase( svcLoc ) {

}

xAOD::TrigT2ZdcSignalsContainer*
xAODTrigT2ZdcSignalsContainerCnv::
createPersistent( xAOD::TrigT2ZdcSignalsContainer* trans ) {

  // Create a view copy of the container:
  xAOD::TrigT2ZdcSignalsContainer* result =
      new xAOD::TrigT2ZdcSignalsContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::TrigT2ZdcSignalsContainer::iterator itr = result->begin();
  xAOD::TrigT2ZdcSignalsContainer::iterator end = result->end();
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
StatusCode xAODTrigT2ZdcSignalsContainerCnv::createObj(IOpaqueAddress* pAddr,
						DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigT2ZdcSignalsContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigT2ZdcSignalsContainer* xAODTrigT2ZdcSignalsContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "8313279A-5E27-11E4-A635-02163E00A82C" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::TrigT2ZdcSignalsContainer* c = poolReadObject< xAOD::TrigT2ZdcSignalsContainer >();
    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::TrigT2ZdcSignalsContainer found" );
  return 0;
}

void xAODTrigT2ZdcSignalsContainerCnv::
toPersistent( xAOD::TrigT2ZdcSignals* obj ) const {
  ATH_MSG_VERBOSE("xAOD::TrigT2ZdcSignals: triggerEnergies size:: "<<obj->triggerEnergies().size());
  ATH_MSG_VERBOSE("xAOD::TrigT2ZdcSignals: triggerTimes size:: "<<obj->triggerTimes().size());
  return;
}
