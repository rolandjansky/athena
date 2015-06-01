/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODMuonContainerCnv.h"

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

xAODMuonContainerCnv::xAODMuonContainerCnv( ISvcLocator* svcLoc )
: xAODMuonContainerCnvBase( svcLoc ) {
}

xAOD::MuonContainer*
  xAODMuonContainerCnv::
createPersistent( xAOD::MuonContainer* trans ) {

   // Create a view copy of the container:
  xAOD::MuonContainer* result =
    new xAOD::MuonContainer( trans->begin(), trans->end(),
    SG::VIEW_ELEMENTS );

   // Return the new container:
  return result;
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/
StatusCode xAODMuonContainerCnv::createObj( IOpaqueAddress* pAddr,
DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::MuonContainer: " << m_key );

   // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::MuonContainer* xAODMuonContainerCnv::createTransient() {

   // The known ID(s) for this container:
  static pool::Guid v1_guid( "F84AE51A-F309-4844-B286-8E94C655B724" );

   // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::MuonContainer* c = poolReadObject< xAOD::MuonContainer >();
    setStoreLink( c, m_key );
    return c;
  }

   // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::MuonContainer found" );
  return 0;
}
