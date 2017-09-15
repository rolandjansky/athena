/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleContainerCnv.cxx 702662 2015-10-23 11:31:50Z nstyles $

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// Local include(s):
#include "xAODTrackParticleContainerCnv.h"

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

xAODTrackParticleContainerCnv::xAODTrackParticleContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackParticleContainerCnvBase( svcLoc ) {
}

xAOD::TrackParticleContainer*
xAODTrackParticleContainerCnv::
createPersistent( xAOD::TrackParticleContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrackParticleContainer* result =
      new xAOD::TrackParticleContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODTrackParticleContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TrackParticleContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrackParticleContainer* xAODTrackParticleContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "F7564EE8-3BD2-11E3-A42F-6C3BE51AB9F1" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrackParticleContainer* c = poolReadObject< xAOD::TrackParticleContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackParticleContainer found" );
   return 0;
}
