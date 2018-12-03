/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProton_v1.cxx
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Implementation file of the AFPProton_v1 class.
 * 
 */

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPProton_v1.h"

namespace xAOD {

   AFPProton_v1::AFPProton_v1() : IParticle(), m_p4(), m_p4Cached( false ) {}

   double AFPProton_v1::pt() const {

      return p4().Pt();
   }

   double AFPProton_v1::eta() const {

      return p4().Eta();
   }

   double AFPProton_v1::phi() const {

      return p4().Phi();
   }

   double AFPProton_v1::m() const {

      return p4().M();
   }

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( AFPProton_v1, float, double, e )

   double AFPProton_v1::rapidity() const {

      return p4().Rapidity();
   }

   const AFPProton_v1::FourMom_t& AFPProton_v1::p4() const {

      // Check if we need to reset the cached object:
      if( ! m_p4Cached ) {
         m_p4.SetPxPyPzE( px(), py(), pz(), e() );
         m_p4Cached = true;
      }

      // Return the cached object:
      return m_p4;
   }

   Type::ObjectType AFPProton_v1::type() const {

      return Type::Particle;
   }

   AUXSTORE_PRIMITIVE_GETTER( AFPProton_v1, float, px )
   AUXSTORE_PRIMITIVE_GETTER( AFPProton_v1, float, py )
   AUXSTORE_PRIMITIVE_GETTER( AFPProton_v1, float, pz )

   void AFPProton_v1::setPxPyPzE( float px, float py, float pz, float e ) {

      setPx( px );
      setPy( py );
      setPz( pz );
      setE( e );

      return;
   }

   void AFPProton_v1::setPx( float value ) {

      static const Accessor< float > acc( "px" );
      acc( *this ) = value;
      m_p4Cached = false;
      return;
   }

   void AFPProton_v1::setPy( float value ) {

      static const Accessor< float > acc( "py" );
      acc( *this ) = value;
      m_p4Cached = false;
      return;
   }

   void AFPProton_v1::setPz( float value ) {

      static const Accessor< float > acc( "pz" );
      acc( *this ) = value;
      m_p4Cached = false;
      return;
   }

   void AFPProton_v1::setE( float value ) {

      static const Accessor< float > acc( "e" );
      acc( *this ) = value;
      m_p4Cached = false;
      return;
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( AFPProton_v1, float, chi2, setChi2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( AFPProton_v1, int, side, setSide )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( AFPProton_v1, int, methodID,
                                         setMethodID )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( AFPProton_v1,
                                      std::vector< AFPProton_v1::AFPTrackLink_t >,
                                      afpTrackLinks, setAFPTrackLinks )

   /// Helper variable
   static const
   SG::AuxElement::Accessor< std::vector< AFPProton_v1::AFPTrackLink_t > >
   tracksAcc( "afpTrackLinks" );

   void AFPProton_v1::addAFPTrackLink( const AFPTrackLink_t& newTrack ) {

      tracksAcc( *this ).push_back( newTrack );
      return;
   }

   size_t AFPProton_v1::nTracks() const {

      // Check if the links are available:
      if( ! tracksAcc.isAvailable( *this ) ) {
         return 0;
      }

      return tracksAcc( *this ).size();
   }

   const AFPTrack* AFPProton_v1::track( size_t index ) const {

      // Check if the links are available:
      if( ! tracksAcc.isAvailable( *this ) ) {
         return nullptr;
      }

      // Use the at(...) function so the code would throw an exception if an
      // impossible index is requested:
      auto link = tracksAcc( *this ).at( index );

      // Check if the link can be de-referenced:
      if( ! link.isValid() ) {
         return nullptr;
      }

      // Return the pointer:
      return *link;
   }

} // namespace xAOD
