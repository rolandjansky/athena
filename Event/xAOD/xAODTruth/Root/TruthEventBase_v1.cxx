/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventBase_v1.cxx 622193 2014-10-16 16:08:34Z krasznaa $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTruth/versions/TruthEventBase_v1.h"

namespace xAOD {

   TruthEventBase_v1::TruthEventBase_v1()
      : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   // Implementation of the truth particle accessor functions

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEventBase_v1,
                                      TruthEventBase_v1::TruthParticleLinks_t,
                                      truthParticleLinks,
                                      setTruthParticleLinks )

   /// Accessor for the truth particles
   static const SG::AuxElement::Accessor< TruthEventBase_v1::TruthParticleLinks_t >
      truthParticleLinksAcc( "truthParticleLinks" );

   size_t TruthEventBase_v1::nTruthParticles() const {

      // If the variable is not available, we don't have any truth particles
      // associated...
      if( ! truthParticleLinksAcc.isAvailable( *this ) ) {
         return 0;
      }

      // Return the size of the vector:
      return truthParticleLinksAcc( *this ).size();
   }

   const TruthEventBase_v1::TruthParticleLink_t&
   TruthEventBase_v1::truthParticleLink( size_t index ) const {

      return truthParticleLinksAcc( *this ).at( index );
   }

   const TruthParticle* TruthEventBase_v1::truthParticle( size_t index ) const {

      // Check if the variable is available:
      if( ! truthParticleLinksAcc.isAvailable( *this ) ) {
         return 0;
      }

      // Check if the link is valid:
      const TruthParticleLinks_t& links = truthParticleLinksAcc( *this );
      if( ! links[ index ].isValid() ) {
         return 0;
      }

      // Return the de-referenced link:
      return *( links[ index ] );
   }

   void
   TruthEventBase_v1::addTruthParticleLink( const TruthParticleLink_t& link ) {

      truthParticleLinksAcc( *this ).push_back( link );
      return;
   }

   void TruthEventBase_v1::clearTruthParticleLinks() {

      truthParticleLinksAcc( *this ).clear();
      return;
   }

   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   // Implementation of the truth vertex accessor functions

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEventBase_v1,
                                      TruthEventBase_v1::TruthVertexLinks_t,
                                      truthVertexLinks,
                                      setTruthVertexLinks )

   /// Accessor for the truth vertices
   static const SG::AuxElement::Accessor< TruthEventBase_v1::TruthVertexLinks_t >
      truthVertexLinksAcc( "truthVertexLinks" );

   size_t TruthEventBase_v1::nTruthVertices() const {

      // If the variable is not available, we don't have any truth particles
      // associated...
      if( ! truthVertexLinksAcc.isAvailable( *this ) ) {
         return 0;
      }

      // Return the size of the vector:
      return truthVertexLinksAcc( *this ).size();
   }

   const TruthEventBase_v1::TruthVertexLink_t&
   TruthEventBase_v1::truthVertexLink( size_t index ) const {

      return truthVertexLinksAcc( *this ).at(index);
   }

   const TruthVertex* TruthEventBase_v1::truthVertex( size_t index ) const {

      // Check if the variable is available:
      if( ! truthVertexLinksAcc.isAvailable( *this ) ) {
         return 0;
      }

      // Check if the link is valid:
      const TruthVertexLinks_t& links = truthVertexLinksAcc( *this );
      if( ! links[ index ].isValid() ) {
         return 0;
      }

      // Return the de-referenced link:
      return *( links[ index ] );
   }

   void TruthEventBase_v1::addTruthVertexLink( const TruthVertexLink_t& link ) {

      truthVertexLinksAcc( *this ).push_back( link );
      return;
   }

   void TruthEventBase_v1::clearTruthVertexLinks() {

      truthVertexLinksAcc( *this ).clear();
      return;
   }

   /////////////////////////////////////////////////////////////////////////////

   void TruthEventBase_v1::toPersistent() {

      // Prepare the truth particle links for writing:
      if( truthParticleLinksAcc.isAvailableWritable( *this ) ) {
         TruthParticleLinks_t::iterator itr =
            truthParticleLinksAcc( *this ).begin();
         TruthParticleLinks_t::iterator end =
            truthParticleLinksAcc( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      // Prepare the truth vertex links for writing:
      if( truthVertexLinksAcc.isAvailableWritable( *this ) ) {
         TruthVertexLinks_t::iterator itr =
            truthVertexLinksAcc( *this ).begin();
         TruthVertexLinks_t::iterator end =
            truthVertexLinksAcc( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      return;
   }

} // namespace xAOD
