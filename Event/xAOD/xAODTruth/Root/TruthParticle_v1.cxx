// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticle_v1.cxx 690336 2015-08-20 10:54:57Z abuckley $

// System include(s):
#include <cmath>
#include <iostream>
#include <stdexcept>

// Utility include(s):
#include "TruthUtils/PIDHelpers.h"

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTruth/versions/TruthParticle_v1.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "TruthAccessors_v1.h"

namespace xAOD {

   TruthParticle_v1::TruthParticle_v1()
   : IParticle() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //  Implementation for functions identifying the particle in the MC record
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, pdgId,
                                         setPdgId )

   int TruthParticle_v1::absPdgId() const {

      return std::abs( pdgId() );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, barcode,
                                         setBarcode )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, status,
                                         setStatus )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //                Implementation of the links to the vertices
   //

   /// Accessor for the production vertex
   static const SG::AuxElement::Accessor< ElementLink< TruthVertexContainer > >
      prodVtxLinkAcc( "prodVtxLink" );
   /// Accessor for the decay vertex
   static const SG::AuxElement::Accessor< ElementLink< TruthVertexContainer > >
      decayVtxLinkAcc( "decayVtxLink" );

   bool TruthParticle_v1::hasProdVtx() const {

      return ( prodVtxLinkAcc.isAvailable( *this ) &&
               prodVtxLinkAcc( *this ).isValid() );
   }

   const TruthVertex* TruthParticle_v1::prodVtx() const {

      return hasProdVtx() ? *prodVtxLink() : 0;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthParticle_v1,
                                      ElementLink< TruthVertexContainer >,
                                      prodVtxLink, setProdVtxLink )

   bool TruthParticle_v1::hasDecayVtx() const {

      return ( decayVtxLinkAcc.isAvailable( *this ) &&
               decayVtxLinkAcc( *this ).isValid() );
   }

   const TruthVertex* TruthParticle_v1::decayVtx() const {

      return hasDecayVtx() ? *decayVtxLink() : 0;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthParticle_v1,
                                      ElementLink< TruthVertexContainer >,
                                      decayVtxLink, setDecayVtxLink )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //                 Direct access to parents and children
   //

   size_t TruthParticle_v1::nParents() const {

      return hasProdVtx() ? prodVtx()->nIncomingParticles() : 0;
   }

   const TruthParticle_v1* TruthParticle_v1::parent( size_t i ) const {

      return hasProdVtx() ? prodVtx()->incomingParticle( i ) : 0;
   }

   size_t TruthParticle_v1::nChildren() const {

      return hasDecayVtx() ? decayVtx()->nOutgoingParticles() : 0;
   }

   const TruthParticle_v1* TruthParticle_v1::child( size_t i ) const {

      return hasDecayVtx() ? decayVtx()->outgoingParticle( i ) : 0;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation of the IParticle interface
   //

   double TruthParticle_v1::pt() const {

      // Do the calculation by hand:
      const double localPx = static_cast< double >( px() );
      const double localPy = static_cast< double >( py() );
      return std::sqrt( localPx * localPx + localPy * localPy );
   }

   double TruthParticle_v1::eta() const {

      // Calculate the pseudo-rapidity using TLorentzVector.
      // Could do something more lightweight later on.
      return genvecP4().Eta();
   }

   double TruthParticle_v1::phi() const {

      // Calculate the azimuth angle using TLorentzVector.
      // Could do something more lightweight later on.
      return genvecP4().Phi();
   }

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TruthParticle_v1, float, double, m )
   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TruthParticle_v1, float, double, e )

   double TruthParticle_v1::rapidity() const {

      return genvecP4().Rapidity();
   }

   TruthParticle_v1::FourMom_t TruthParticle_v1::p4() const {
     return FourMom_t(px(), py(), pz(), e() );
   }

   // not very different in this case; just adding for uniformity
   TruthParticle_v1::GenVecFourMom_t TruthParticle_v1::genvecP4() const {
     return GenVecFourMom_t(px(), py(), pz(), e() );
   }

   Type::ObjectType TruthParticle_v1::type() const {

      return Type::TruthParticle;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //    Implementation of the truth particle specific 4-momentum functions
   //

   double TruthParticle_v1::abseta() const {

      return std::abs( eta() );
   }

   double TruthParticle_v1::absrapidity() const {

      return std::abs( rapidity() );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, float, px, setPx )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, float, py, setPy )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, float, pz, setPz )


   void TruthParticle_v1::setE( float value ) {
      static const Accessor< float > acc( "e" );
      acc( *this ) = value;
      return;
   }
   
   void TruthParticle_v1::setM( float value ) {
      static const Accessor< float > acc( "m" );
      acc( *this ) = value;
      return;
   }
   
   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the particle species decoder functions
   //

/// Helper macro to implement the functions that rely in functions from MC::PID
#define MC_PID_HELPER( TYPE, FNAME )      \
   TYPE TruthParticle_v1::FNAME() const { \
      return MC::PID::FNAME( pdgId() );   \
   }

   MC_PID_HELPER( double, charge )
   MC_PID_HELPER( int, threeCharge )

   MC_PID_HELPER( bool, isCharged )
   MC_PID_HELPER( bool, isNeutral )

   MC_PID_HELPER( bool, isPhoton )
   MC_PID_HELPER( bool, isLepton )
   MC_PID_HELPER( bool, isChLepton )
   MC_PID_HELPER( bool, isElectron )
   MC_PID_HELPER( bool, isMuon )
   MC_PID_HELPER( bool, isTau )
   MC_PID_HELPER( bool, isNeutrino )

   MC_PID_HELPER( bool, isHadron )
   MC_PID_HELPER( bool, isMeson )
   MC_PID_HELPER( bool, isBaryon )

   MC_PID_HELPER( bool, hasStrange )
   MC_PID_HELPER( bool, hasCharm )
   MC_PID_HELPER( bool, hasBottom )

   MC_PID_HELPER( bool, isLightMeson )
   MC_PID_HELPER( bool, isLightBaryon )
   MC_PID_HELPER( bool, isLightHadron )

   MC_PID_HELPER( bool, isHeavyMeson )
   MC_PID_HELPER( bool, isHeavyBaryon )
   MC_PID_HELPER( bool, isHeavyHadron )

   MC_PID_HELPER( bool, isBottomMeson )
   MC_PID_HELPER( bool, isBottomBaryon )
   MC_PID_HELPER( bool, isBottomHadron )

   MC_PID_HELPER( bool, isCharmMeson )
   MC_PID_HELPER( bool, isCharmBaryon )
   MC_PID_HELPER( bool, isCharmHadron )

   MC_PID_HELPER( bool, isStrangeMeson )
   MC_PID_HELPER( bool, isStrangeBaryon )
   MC_PID_HELPER( bool, isStrangeHadron )

   MC_PID_HELPER( bool, isQuark )
   MC_PID_HELPER( bool, isParton )
   MC_PID_HELPER( bool, isTop )
   MC_PID_HELPER( bool, isW )
   MC_PID_HELPER( bool, isZ )
   MC_PID_HELPER( bool, isHiggs )
   MC_PID_HELPER( bool, isResonance )
   MC_PID_HELPER( bool, isGenSpecific )

// Forget about this macro:
#undef MC_PID_HELPER

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the optional polarization accessors
   //

   bool TruthParticle_v1::polarizationParameter( float& value,
                                                 PolParam param ) const {

      // Get the accessor object:
      const Accessor< float >* acc = polarizationAccessorV1( param );
      if( ! acc ) {
         // The user asked for a non-existent parameter type. o.O
         std::cerr << "xAOD::TruthParticle_v1::polarizationParameter ERROR "
                   << "Request for an unknown (" << param << ") polarization "
                   << "parameter type" << std::endl;
         return false;
      }
      // Check if the variable is available:
      if( ! acc->isAvailable( *this ) ) {
         // No, it is not.
         return false;
      }

      // Read the value:
      value = ( *acc )( *this );
      return true;
   }

   bool TruthParticle_v1::setPolarizationParameter( float value,
                                                    PolParam param ) {

      // Get the accessor object:
      const Accessor< float >* acc = polarizationAccessorV1( param );
      if( ! acc ) {
         // The user asked for a non-existent parameter type. o.O
         std::cerr << "xAOD::TruthParticle_v1::setPolarizationParameter ERROR "
                   << "Request for an unknown (" << param << ") polarization "
                   << "parameter type" << std::endl;
         return false;
      }

      // Set the value:
      ( *acc )( *this ) = value;
      return true;
   }

   float TruthParticle_v1::polarizationPatameter( PolParam param ) const {

      // Get the accessor object:
      const Accessor< float >* acc = polarizationAccessorV1( param );
      if( ! acc ) {
         // Throw an exception:
         throw std::runtime_error( "Unrecognized polarization parameter "
                                   "requested" );
      }

      // Read the value:
      return ( *acc )( *this );
   }

   TruthParticle_v1::Polarization TruthParticle_v1::polarization() const {

      // Construct the object:
      Polarization rtn;
      polarizationParameter( rtn.phi, polarizationPhi );
      polarizationParameter( rtn.theta, polarizationTheta );

      return rtn;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   void TruthParticle_v1::toPersistent() {

      if( prodVtxLinkAcc.isAvailableWritable( *this ) ) {
         prodVtxLinkAcc( *this ).toPersistent();
      }
      if( decayVtxLinkAcc.isAvailableWritable( *this ) ) {
         decayVtxLinkAcc( *this ).toPersistent();
      }
      return;
   }

} // namespace xAOD
