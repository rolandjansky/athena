// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "xAODCore/AuxStoreAccessorMacros.h"
#include "TruthAccessors_v1.h"

#include "xAODTruth/versions/TruthParticle_v1.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {


  TruthParticle_v1::TruthParticle_v1()
    : IParticle(), m_p4(), m_p4Cached( false )
  {  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //            Implementation for the "MC specific" functions
  //

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, pdgId, setPdgId )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, status, setStatus )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthParticle_v1, int, barcode, setBarcode )

  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Optional polarization accessors

  bool TruthParticle_v1::polarizationParameter(float& value, TruthParticle_v1::PolParam information) const {
    xAOD::TruthParticle_v1::Accessor<float>* acc = polarizationAccessorV1( information );
    if ( ! acc ) return false;
    value = ( *acc )( *this );
    return true;
  }
  void TruthParticle_v1::setPolarizationParameter(float value, TruthParticle_v1::PolParam information) {
    xAOD::TruthParticle_v1::Accessor<float>* acc = polarizationAccessorV1( information );
    ( *acc )( *this ) = value;
  }

  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //                Implementation of the links to the vertices
  //

  /// Accessor for the production vertex
  static SG::AuxElement::Accessor< ElementLink< TruthVertexContainer > >
  prodVtxLinkAcc( "prodVtxLink" );
  /// Accessor for the decay vertex
  static SG::AuxElement::Accessor< ElementLink< TruthVertexContainer > >
  decayVtxLinkAcc( "decayVtxLink" );

  bool TruthParticle_v1::hasProdVtx() const {
    return ( prodVtxLinkAcc.isAvailable( *this ) && prodVtxLinkAcc( *this ).isValid() );
  }

  bool TruthParticle_v1::hasDecayVtx() const {
    return ( decayVtxLinkAcc.isAvailable( *this ) && decayVtxLinkAcc( *this ).isValid() );
  }

  const TruthVertex* TruthParticle_v1::prodVtx() const {
    return hasProdVtx() ? *prodVtxLink() : NULL;
  }

  const TruthVertex* TruthParticle_v1::decayVtx() const {
    return hasDecayVtx() ? *decayVtxLink() : NULL;
  }

  const ElementLink< TruthVertexContainer >& TruthParticle_v1::prodVtxLink() const {
    return prodVtxLinkAcc( *this );
  }

  const ElementLink< TruthVertexContainer >& TruthParticle_v1::decayVtxLink() const {
    return decayVtxLinkAcc( *this );
  }

  void TruthParticle_v1::setProdVtxLink( const ElementLink< TruthVertexContainer >& link ) {
    prodVtxLinkAcc( *this ) = link;
  }

  void TruthParticle_v1::setDecayVtxLink( const ElementLink< TruthVertexContainer >& link ) {
    decayVtxLinkAcc( *this ) = link;
  }

  //
  /////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////////////
  //
  // Direct access to parents and children
  //

  size_t TruthParticle_v1::nParents() const {
    return hasProdVtx() ? prodVtx()->nIncomingParticles() : 0;
  }

  const TruthParticle_v1* TruthParticle_v1::parent(const std::size_t i) const {
    return hasProdVtx() ? prodVtx()->incomingParticle(i) : NULL;
  }


  size_t TruthParticle_v1::nChildren() const {
    return hasDecayVtx() ? decayVtx()->nOutgoingParticles() : 0;
  }

  const TruthParticle_v1* TruthParticle_v1::child(const std::size_t i) const {
    return hasDecayVtx() ? decayVtx()->outgoingParticle(i) : NULL;
  }

  //
  /////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////////////
  //
  //               Implementation of the IParticle interface
  //

  double TruthParticle_v1::pt() const {
    // Do the calculation by hand. Could make it faster
    return std::sqrt( px() * px() + py() * py() );
  }

  double TruthParticle_v1::eta() const {
    // Calculate the pseudo-rapidity using TLorentzVector. Could do something more lightweight
    return p4().Eta();
  }

  double TruthParticle_v1::phi() const {
    // Calculate the azimuth angle using TLorentzVector. Could do something more lightweight
    return p4().Phi();
  }


  double TruthParticle_v1::m() const {
    static Accessor< float > acc( "m" );
    return acc( *this );
  }

  double TruthParticle_v1::e() const {
    static Accessor< float > acc( "e" );
    return acc( *this );
  }

  double TruthParticle_v1::rapidity() const {
    return p4().Rapidity();
  }

  const TruthParticle_v1::FourMom_t& TruthParticle_v1::p4() const {
    // Cache the 4-momentum if it's not already:
    if( ! m_p4Cached ) {
      m_p4.SetPxPyPzE( px(), py(), pz(), e() );
      m_p4Cached = true;
    }
    return m_p4;
  }

  Type::ObjectType TruthParticle_v1::type() const {
    return Type::TruthParticle;
  }

  void TruthParticle_v1::toPersistent() {
    #ifndef XAOD_STANDALONE
    if (prodVtxLinkAcc.isAvailableWritable(*this)) prodVtxLinkAcc(*this).toPersistent();
    if (decayVtxLinkAcc.isAvailableWritable(*this)) decayVtxLinkAcc(*this).toPersistent();
    #endif // not XAOD_STANDALONE
  }

  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //    Implementation of the truth particle specific 4-momentum functions
  //

  AUXSTORE_PRIMITIVE_GETTER( TruthParticle_v1, float, px )
  AUXSTORE_PRIMITIVE_GETTER( TruthParticle_v1, float, py )
  AUXSTORE_PRIMITIVE_GETTER( TruthParticle_v1, float, pz )

  void TruthParticle_v1::setPx( float value ) {
    static Accessor< float > acc( "px" );
    m_p4Cached = false;
    acc( *this ) = value;
    return;
  }

  void TruthParticle_v1::setPy( float value ) {
    static Accessor< float > acc( "py" );
    m_p4Cached = false;
    acc( *this ) = value;
    return;
  }

  void TruthParticle_v1::setPz( float value ) {
    static Accessor< float > acc( "pz" );
    m_p4Cached = false;
    acc( *this ) = value;
    return;
  }

  void TruthParticle_v1::setE( float value ) {
    static Accessor< float > acc( "e" );
    m_p4Cached = false;
    acc( *this ) = value;
    return;
  }

  void TruthParticle_v1::setM( float value ) {
    static Accessor< float > acc( "m" );
    // note: this does not invalidate the cache
    acc( *this ) = value;
    return;
  }

  //
  /////////////////////////////////////////////////////////////////////////////


} // namespace xAOD
