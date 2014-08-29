/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertex_v1.cxx 601415 2014-06-12 11:40:17Z abuckley $

#include <cmath>

#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTruth/versions/TruthVertex_v1.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {


  TruthVertex_v1::TruthVertex_v1()
    : SG::AuxElement(), m_v4(), m_v4Cached( false )
  {  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //            Implementation for the "MC specific" functions
  //

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, int, id, setId )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthVertex_v1, int, barcode, setBarcode )

  // AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthVertex_v1, std::vector<float>, weights, setWeights )

  //
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //           Implementation for the links to the truth particles
  //

  /// Accessor for the incoming particles
  static SG::AuxElement::Accessor< std::vector< ElementLink< TruthParticleContainer > > >
  incomingParticleLinksAcc( "incomingParticleLinks" );


  /// Get all the incoming particles
  const std::vector< ElementLink< TruthParticleContainer > >& TruthVertex_v1::incomingParticleLinks() const {
    return incomingParticleLinksAcc( *this );
  }


  /// Set all the incoming particles
  void TruthVertex_v1::setIncomingParticleLinks(const std::vector< ElementLink< TruthParticleContainer > >& val) {
    incomingParticleLinksAcc( *this ) = val;
  }


  size_t TruthVertex_v1::nIncomingParticles() const {
    if( ! incomingParticleLinksAcc.isAvailable( *this ) ) return 0;
    return incomingParticleLinksAcc( *this ).size();
  }


  const TruthParticle* TruthVertex_v1::incomingParticle( size_t index ) const {
    if( ( ! incomingParticleLinksAcc.isAvailable( *this ) ) ||
        ( incomingParticleLinksAcc( *this ).size() <= index ) ) {
      return 0;
    }
    const ElementLink< TruthParticleContainer >& ipl =
       incomingParticleLinksAcc( *this )[ index ];
    if (ipl.isValid()) {
      return *ipl;
    } else {
      return 0;
    }
  }


  void TruthVertex_v1::addIncomingParticleLink( const ElementLink< TruthParticleContainer >& link ) {
    incomingParticleLinksAcc( *this ).push_back( link );
    return;
  }


  void TruthVertex_v1::clearIncomingParticleLinks() {
    incomingParticleLinksAcc( *this ).clear();
    return;
  }


  /// Accessor for the outgoing particles
  static SG::AuxElement::Accessor< std::vector< ElementLink< TruthParticleContainer > > >
  outgoingParticleLinksAcc( "outgoingParticleLinks" );


  /// Get all the outgoing particles
  const std::vector< ElementLink< TruthParticleContainer > >& TruthVertex_v1::outgoingParticleLinks() const {
    return outgoingParticleLinksAcc( *this );
  }


  /// Set all the outgoing particles
  void TruthVertex_v1::setOutgoingParticleLinks(const std::vector< ElementLink< TruthParticleContainer > >& val) {
    outgoingParticleLinksAcc( *this ) = val;
  }


  size_t TruthVertex_v1::nOutgoingParticles() const {
    if (! outgoingParticleLinksAcc.isAvailable( *this ) ) return 0;
    return outgoingParticleLinksAcc( *this ).size();
  }


  const TruthParticle* TruthVertex_v1::outgoingParticle( size_t index ) const {
    if (( ! outgoingParticleLinksAcc.isAvailable( *this ) ) ||
        ( outgoingParticleLinksAcc( *this ).size() <= index ) ) {
      return 0;
    }
    const ElementLink< TruthParticleContainer >& opl = outgoingParticleLinksAcc( *this )[ index ];
    if (opl.isValid()) {
      return *opl;
    } else {
      return 0;
    }
  }


  void TruthVertex_v1::addOutgoingParticleLink( const ElementLink< TruthParticleContainer >& link ) {
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

  AUXSTORE_PRIMITIVE_GETTER( TruthVertex_v1, float, x)
  AUXSTORE_PRIMITIVE_GETTER( TruthVertex_v1, float, y)
  AUXSTORE_PRIMITIVE_GETTER( TruthVertex_v1, float, z)

  void TruthVertex_v1::setX(float x) {
    static SG::AuxElement::Accessor< float > acc("x");
    m_v4Cached = false;
    acc(*this) = x;
  }

  void TruthVertex_v1::setY(float y) {
    static SG::AuxElement::Accessor< float > acc("y");
    m_v4Cached = false;
    acc(*this) = y;
  }

  void TruthVertex_v1::setZ(float z) {
    static SG::AuxElement::Accessor< float > acc("z");
    m_v4Cached = false;
    acc(*this) = z;
  }

  float TruthVertex_v1::perp() const {
    // Do the calculation by hand. Could make it faster than this even in a future iteration...
    return std::sqrt( x()*x() + y()*y() );
  }

  float TruthVertex_v1::eta() const {
    // This is not necessarily what Andy was thinking about...
    return v4().Eta();
  }

  float TruthVertex_v1::phi() const {
    // This is not necessarily what Andy was thinking about...
    return v4().Phi();
  }

  AUXSTORE_PRIMITIVE_GETTER( TruthVertex_v1, float, t )

  void TruthVertex_v1::setT(float t) {
    static SG::AuxElement::Accessor< float > acc("t");
    m_v4Cached = false;
    acc(*this) = t;
  }

  const TruthVertex_v1::FourVec_t& TruthVertex_v1::v4() const {
    // Cache the 4-vector if it's not already:
    if( ! m_v4Cached ) {
      m_v4.SetXYZT( x(), y(), z(), t() );
      m_v4Cached = true;
    }
    return m_v4;
  }

  Type::ObjectType TruthVertex_v1::type() const {
    return Type::TruthVertex;
  }

  void TruthVertex_v1::toPersistent() {
    #ifndef XAOD_STANDALONE
    if (incomingParticleLinksAcc.isAvailableWritable(*this)) {
      for (std::vector< ElementLink<TruthParticleContainer > >::iterator it = incomingParticleLinksAcc(*this).begin();
           it != incomingParticleLinksAcc(*this).end(); ++it) {
        it->toPersistent();
      }
    }

    if (outgoingParticleLinksAcc.isAvailableWritable(*this)) {
      for (std::vector< ElementLink<TruthParticleContainer > >::iterator it = outgoingParticleLinksAcc(*this).begin();
           it != outgoingParticleLinksAcc(*this).end(); ++it) {
        it->toPersistent();
      }
    }
    #endif // not XAOD_STANDALONE
  }

  //
  /////////////////////////////////////////////////////////////////////////////


} // namespace xAOD
