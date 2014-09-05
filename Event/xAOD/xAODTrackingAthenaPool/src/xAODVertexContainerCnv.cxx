/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODVertexContainerCnv.cxx 611354 2014-08-12 07:20:27Z arnaez $

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODVertexContainerCnv.h"

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

xAODVertexContainerCnv::xAODVertexContainerCnv( ISvcLocator* svcLoc )
   : xAODVertexContainerCnvBase( svcLoc ) {
}

xAOD::VertexContainer*
xAODVertexContainerCnv::
createPersistent( xAOD::VertexContainer* trans ) {

   // Create a view copy of the container:
   xAOD::VertexContainer* result =
      new xAOD::VertexContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::VertexContainer::iterator itr = result->begin();
   xAOD::VertexContainer::iterator end = result->end();
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
StatusCode xAODVertexContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::VertexContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::VertexContainer* xAODVertexContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "00B6DBA1-0E0D-4FDA-BFEF-F08EEFFE4BA0" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::VertexContainer* c = poolReadObject< xAOD::VertexContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::VertexContainer found" );
   return 0;
}

void xAODVertexContainerCnv::
toPersistent( xAOD::Vertex* vertex ) const {

   /// A convenience shorthand
   typedef xAOD::Vertex::TrackParticleLinks_t Type_t;

   // Call "toPersistent()" on all the ElementLinks:
   Type_t::const_iterator track_itr = vertex->trackParticleLinks().begin();
   Type_t::const_iterator track_end = vertex->trackParticleLinks().end();
   for( ; track_itr != track_end; ++track_itr ) {
      Type_t::reference el = const_cast< Type_t::reference >( *track_itr );
      el.toPersistent();
   }

   //same for neutrals
   typedef xAOD::Vertex::NeutralParticleLinks_t Type_n;
   Type_n::const_iterator neutral_itr = vertex->neutralParticleLinks().begin();
   Type_n::const_iterator neutral_end = vertex->neutralParticleLinks().end();
   for( ; neutral_itr != neutral_end; ++neutral_itr ) {
      Type_n::reference el = const_cast< Type_n::reference >( *neutral_itr );
      el.toPersistent();
   }

   return;
}
