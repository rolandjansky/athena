// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// Accessor include:
#include "AthContainers/AuxElement.h"

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
      acc_prodVtxLink( "prodVtxLink" );
   /// Accessor for the decay vertex
   static const SG::AuxElement::Accessor< ElementLink< TruthVertexContainer > >
      acc_decayVtxLink( "decayVtxLink" );

   bool TruthParticle_v1::hasProdVtx() const {

      return ( acc_prodVtxLink.isAvailable( *this ) &&
               acc_prodVtxLink( *this ).isValid() );
   }

   const TruthVertex* TruthParticle_v1::prodVtx() const {

      return hasProdVtx() ? *prodVtxLink() : nullptr;
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthParticle_v1,
                                      ElementLink< TruthVertexContainer >,
                                      prodVtxLink, setProdVtxLink )

   bool TruthParticle_v1::hasDecayVtx() const {

      return ( acc_decayVtxLink.isAvailable( *this ) &&
               acc_decayVtxLink( *this ).isValid() );
   }

   const TruthVertex* TruthParticle_v1::decayVtx() const {

      return hasDecayVtx() ? *decayVtxLink() : nullptr;
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

   // Accessor for links to parents
   static const SG::AuxElement::ConstAccessor< std::vector<ElementLink<xAOD::TruthParticleContainer> > >
      acc_parentLinks( "parentLinks" );
   // Accessor for links to children
   static const SG::AuxElement::ConstAccessor< std::vector<ElementLink<xAOD::TruthParticleContainer> > >
      acc_childLinks( "childLinks" );
   // Note that in some conditions the vertex might be saved in a different collection from
   // the daughters, causing the vertex to not know how many children or parents the particle has.
   // An extra test lets us ensure that we avoid this case.

   size_t TruthParticle_v1::nParents() const {
      if (hasProdVtx() && prodVtx()->nIncomingParticles()>0){
        return prodVtx()->nIncomingParticles();
      } else if ( acc_parentLinks.isAvailable( *this ) ) {
        return acc_parentLinks( *this ).size();
      }
      return 0;
   }

   const TruthParticle_v1* TruthParticle_v1::parent( size_t i ) const {
      if (hasProdVtx() && prodVtx()->nIncomingParticles()>0){
        return prodVtx()->incomingParticle( i );
      } else if ( acc_parentLinks.isAvailable( *this ) && i<acc_parentLinks( *this ).size() ) {
        return acc_parentLinks( *this )[i].isValid() ? *(acc_parentLinks( *this )[i]) : nullptr;
      }
      return nullptr;
   }

   size_t TruthParticle_v1::nChildren() const {
      if (hasDecayVtx() && decayVtx()->nOutgoingParticles()>0){
        return decayVtx()->nOutgoingParticles();
      } else if ( acc_childLinks.isAvailable( *this ) ) {
        return acc_childLinks( *this ).size();
      }
      return 0;
   }

   const TruthParticle_v1* TruthParticle_v1::child( size_t i ) const {
      if (hasDecayVtx() && decayVtx()->nOutgoingParticles()>0){
        return decayVtx()->outgoingParticle( i );
      } else if ( acc_childLinks.isAvailable( *this ) && i<acc_childLinks( *this ).size() ) {
        return acc_childLinks( *this )[i].isValid() ? *(acc_childLinks( *this )[i]) : nullptr;
      }
      return nullptr;
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
   MC_PID_HELPER( bool, isBSM )

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

   float TruthParticle_v1::polarizationParameter( PolParam param ) const {

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

      if( acc_prodVtxLink.isAvailableWritable( *this ) ) {
         acc_prodVtxLink( *this ).toPersistent();
      }
      if( acc_decayVtxLink.isAvailableWritable( *this ) ) {
         acc_decayVtxLink( *this ).toPersistent();
      }
      return;
   }

} // namespace xAOD
