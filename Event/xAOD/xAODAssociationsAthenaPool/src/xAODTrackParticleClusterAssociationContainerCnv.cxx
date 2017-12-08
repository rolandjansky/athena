/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "xAODTrackParticleClusterAssociationContainerCnv.h"

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

xAODTrackParticleClusterAssociationContainerCnv::xAODTrackParticleClusterAssociationContainerCnv( ISvcLocator* svcLoc )
: xAODTrackParticleClusterAssociationContainerCnvBase( svcLoc ) {
}

xAOD::TrackParticleClusterAssociationContainer*
  xAODTrackParticleClusterAssociationContainerCnv::
createPersistent( xAOD::TrackParticleClusterAssociationContainer* trans ) {

   // Create a view copy of the container:
  xAOD::TrackParticleClusterAssociationContainer* result =
    new xAOD::TrackParticleClusterAssociationContainer( trans->begin(), trans->end(),
    SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
  xAOD::TrackParticleClusterAssociationContainer::iterator itr = result->begin();
  xAOD::TrackParticleClusterAssociationContainer::iterator end = result->end();
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
StatusCode xAODTrackParticleClusterAssociationContainerCnv::createObj( IOpaqueAddress* pAddr,
DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::TrackParticleClusterAssociationContainer: " << m_key );

   // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrackParticleClusterAssociationContainer* xAODTrackParticleClusterAssociationContainerCnv::createTransient() {

   // The known ID(s) for this container:
  static pool::Guid v1_guid( "F2BC2005-C793-49E4-B79F-7848E732B284" );

   // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::TrackParticleClusterAssociationContainer* c = poolReadObject< xAOD::TrackParticleClusterAssociationContainer >();
    setStoreLink( c, m_key );
    return c;
  }

   // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::TrackParticleClusterAssociationContainer found" );
  return 0;
}

void xAODTrackParticleClusterAssociationContainerCnv::
toPersistent( xAOD::TrackParticleClusterAssociation* trackParticleClusterAssociation ) const {
  using namespace xAOD;
  const_cast<ElementLink< TrackParticleContainer >&>(trackParticleClusterAssociation->trackParticleLink()).toPersistent();

  /// FIXME - guess this try/catch isn't needed any more, but it doesn't hurt... EJWM
  try {

     typedef const std::vector< ElementLink< xAOD::CaloClusterContainer > > caloClusterLinks_t;
     caloClusterLinks_t::const_iterator it = trackParticleClusterAssociation->caloClusterLinks().begin();
     caloClusterLinks_t::const_iterator itEnd = trackParticleClusterAssociation->caloClusterLinks().end();
     for (;it!=itEnd;++it){
        caloClusterLinks_t::reference cl = const_cast<caloClusterLinks_t::reference>(*it);
        cl.toPersistent();
     }

  } catch( const SG::ExcBadAuxVar& ) {
     // Sometimes, very rarely, the dynamically defined ElementLink doesn't
     // exist. The code should not die in that case, but just continue.
     // The I/O system should be fine with the element links missing...
  }

  return;
}
