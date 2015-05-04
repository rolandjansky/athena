/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "xAODSlowMuonContainerCnv.h"

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

xAODSlowMuonContainerCnv::xAODSlowMuonContainerCnv( ISvcLocator* svcLoc )
: xAODSlowMuonContainerCnvBase( svcLoc ) {
}

xAOD::SlowMuonContainer*
  xAODSlowMuonContainerCnv::
createPersistent( xAOD::SlowMuonContainer* trans ) {

   // Create a view copy of the container:
  xAOD::SlowMuonContainer* result =
    new xAOD::SlowMuonContainer( trans->begin(), trans->end(),
    SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
  xAOD::SlowMuonContainer::iterator itr = result->begin();
  xAOD::SlowMuonContainer::iterator end = result->end();
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
StatusCode xAODSlowMuonContainerCnv::createObj( IOpaqueAddress* pAddr,
DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::SlowMuonContainer: " << m_key );

   // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::SlowMuonContainer* xAODSlowMuonContainerCnv::createTransient() {

   // The known ID(s) for this container:
  static pool::Guid v1_guid( "78E011F9-AD91-40A8-95E9-E288A5A583FE");

   // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::SlowMuonContainer* c = poolReadObject< xAOD::SlowMuonContainer >();
    setStoreLink( c, m_key );
    return c;
  }

   // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::SlowMuonContainer found" );
  return 0;
}

void xAODSlowMuonContainerCnv::
toPersistent( xAOD::SlowMuon* /*slowMuon*/ ) const {
  using namespace xAOD;
  // const_cast<ElementLink< MuonContainer >&>(slowMuon->muonLink()).toPersistent();
  return;
}
