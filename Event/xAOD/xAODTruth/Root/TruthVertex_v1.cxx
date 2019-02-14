/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertex_v1.cxx 624338 2014-10-27 15:08:55Z krasznaa $

// System include(s):
#include <cmath>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTruth/versions/TruthVertex_v1.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

   TruthVertex_v1::TruthVertex_v1()
   : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation for the "MC specific" functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, int, id, setId )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, int, barcode,
                                         setBarcode )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //           Implementation for the links to the truth particles
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthVertex_v1, TruthVertex_v1::TPLinks_t,
                                      incomingParticleLinks,
                                      setIncomingParticleLinks )

   /// Accessor for the incoming particles
   static const SG::AuxElement::Accessor< TruthVertex_v1::TPLinks_t >
      incomingParticleLinksAcc( "incomingParticleLinks" );

   size_t TruthVertex_v1::nIncomingParticles() const {
   
      // Check if the variable is available:
      if( ! incomingParticleLinksAcc.isAvailable( *this ) ) {
         // If not, just tell the user that there aren't any incoming particles:
         return 0;
      }

      // Return the size of the vector:
      return incomingParticleLinksAcc( *this ).size();
   }

   const TruthParticle* TruthVertex_v1::incomingParticle( size_t index ) const {

      // Check that the variable exists, and that it has enough elements in it:
      if( ( ! incomingParticleLinksAcc.isAvailable( *this ) ) ||
          ( incomingParticleLinksAcc( *this ).size() <= index ) ) {
         return 0;
      }

      // Retrieve the link object and check its validity:
      const TPLink_t& ipl = incomingParticleLinksAcc( *this )[ index ];
      if( ! ipl.isValid() ) {
         return 0;
      }

      // Finally, de-reference the link:
      return *ipl;
   }

   void TruthVertex_v1::addIncomingParticleLink( const TPLink_t& link ) {

      incomingParticleLinksAcc( *this ).push_back( link );
      return;
   }

   void TruthVertex_v1::clearIncomingParticleLinks() {

      incomingParticleLinksAcc( *this ).clear();
      return;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthVertex_v1, TruthVertex_v1::TPLinks_t,
                                      outgoingParticleLinks,
                                      setOutgoingParticleLinks )

   /// Accessor for the outgoing particles
   static const SG::AuxElement::Accessor< TruthVertex_v1::TPLinks_t >
      outgoingParticleLinksAcc( "outgoingParticleLinks" );

   size_t TruthVertex_v1::nOutgoingParticles() const {

      // Check if the variable is available:
      if( ! outgoingParticleLinksAcc.isAvailable( *this ) ) {
         // If not, just tell the user that there aren't any outgoing particles:
         return 0;
      }

      // Return the size of the vector:
      return outgoingParticleLinksAcc( *this ).size();
   }

   const TruthParticle* TruthVertex_v1::outgoingParticle( size_t index ) const {

      // Check that the variable exists, and that it has enough elements in it:
      if( ( ! outgoingParticleLinksAcc.isAvailable( *this ) ) ||
          ( outgoingParticleLinksAcc( *this ).size() <= index ) ) {
         return 0;
      }

      // Retrieve the link object and check its validity:
      const TPLink_t& opl = outgoingParticleLinksAcc( *this )[ index ];
      if( ! opl.isValid() ) {
         return 0;
      }
      
      // Finally, de-reference the link:
      return *opl;
   }

   void TruthVertex_v1::addOutgoingParticleLink( const TPLink_t& link ) {

      outgoingParticleLinksAcc( *this ).push_back( link );
      return;
   }

   void TruthVertex_v1::clearOutgoingParticleLinks() {

      outgoingParticleLinksAcc( *this ).clear();
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //     Implementation of the functions specifying the vertex's position
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, float, x, setX )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, float, y, setY )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, float, z, setZ )

   float TruthVertex_v1::perp() const {

      // Do the calculation by hand. Could make it faster than this even in a
      // future iteration...
      return std::sqrt( x() * x() + y() * y() );
   }

   float TruthVertex_v1::eta() const {

      // This is not necessarily what Andy was thinking about...
      return genvecV4().Eta();
   }

   float TruthVertex_v1::phi() const {

      // This is not necessarily what Andy was thinking about...
      return genvecV4().Phi();
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, float, t, setT )

   TruthVertex_v1::FourVec_t TruthVertex_v1::v4() const {
      return FourVec_t(x(), y(), z(), t());
   }

   TruthVertex_v1::GenVecFourVec_t TruthVertex_v1::genvecV4() const {
      return GenVecFourVec_t(x(), y(), z(), t());
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   Type::ObjectType TruthVertex_v1::type() const {

      return Type::TruthVertex;
   }

   void TruthVertex_v1::toPersistent() {

      // Prepare the incoming particle links for persistification:
      if( incomingParticleLinksAcc.isAvailableWritable( *this ) ) {
         TPLinks_t::iterator itr = incomingParticleLinksAcc( *this ).begin();
         TPLinks_t::iterator end = incomingParticleLinksAcc( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      // Prepare the outgoing particle links for persistification:
      if( outgoingParticleLinksAcc.isAvailableWritable( *this ) ) {
         TPLinks_t::iterator itr = outgoingParticleLinksAcc( *this ).begin();
         TPLinks_t::iterator end = outgoingParticleLinksAcc( *this ).end();
         for( ; itr != end; ++itr ) {
            itr->toPersistent();
         }
      }

      return;
   }

} // namespace xAOD
