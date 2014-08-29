/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTruth/versions/TruthEventBase_v1.h"

namespace xAOD {


  TruthEventBase_v1::TruthEventBase_v1()
    : SG::AuxElement() {
  }


  /////////////////////////////////////////////////////////////////////////////


  // Accessor for the truth particles
  static SG::AuxElement::Accessor< std::vector< ElementLink<TruthParticleContainer> > >
  truthParticleLinksAcc( "truthParticleLinks" );

  // Get all the truth particles
  const std::vector< ElementLink<TruthParticleContainer> >& TruthEventBase_v1::truthParticleLinks() const {
    return truthParticleLinksAcc( *this );
  }

  // Set all the truth particles
  void TruthEventBase_v1::setTruthParticleLinks(const std::vector< ElementLink<TruthParticleContainer> >& val) {
    truthParticleLinksAcc( *this ) = val;
  }

  size_t TruthEventBase_v1::nTruthParticles() const {
    return truthParticleLinksAcc( *this ).size();
  }

  const TruthParticle* TruthEventBase_v1::truthParticle( size_t index ) const {
    const ElementLink<TruthParticleContainer>& tpLink = truthParticleLink(index);
    return tpLink.isValid() ? *tpLink : NULL;
  }

  const ElementLink<TruthParticleContainer>& TruthEventBase_v1::truthParticleLink( size_t index ) const {
    return truthParticleLinksAcc( *this ).at(index);
  }

  void TruthEventBase_v1::addTruthParticleLink( const ElementLink<TruthParticleContainer>& link ) {
    truthParticleLinksAcc( *this ).push_back( link );
  }

  void TruthEventBase_v1::clearTruthParticleLinks() {
    truthParticleLinksAcc( *this ).clear();
  }


  // Accessor for the truth vertices
  static SG::AuxElement::Accessor< std::vector< ElementLink<TruthVertexContainer> > >
  truthVertexLinksAcc( "truthVertexLinks" );

  // Get all the truth vertices
  const std::vector< ElementLink<TruthVertexContainer> >& TruthEventBase_v1::truthVertexLinks() const {
    return truthVertexLinksAcc( *this );
  }

  // Set all the truth vertices
  void TruthEventBase_v1::setTruthVertexLinks(const std::vector< ElementLink<TruthVertexContainer> >& val) {
    truthVertexLinksAcc( *this ) = val;
  }

  size_t TruthEventBase_v1::nTruthVertices() const {
    return truthVertexLinksAcc( *this ).size();
  }

  const TruthVertex* TruthEventBase_v1::truthVertex( size_t index ) const {
    const ElementLink<TruthVertexContainer>& tvLink = truthVertexLink(index);
    return tvLink.isValid() ? *tvLink : NULL;
  }

  const ElementLink<TruthVertexContainer>& TruthEventBase_v1::truthVertexLink( size_t index ) const {
    return truthVertexLinksAcc( *this ).at(index);
  }

  void TruthEventBase_v1::addTruthVertexLink( const ElementLink<TruthVertexContainer>& link ) {
    truthVertexLinksAcc( *this ).push_back( link );
  }

  void TruthEventBase_v1::clearTruthVertexLinks() {
    truthVertexLinksAcc( *this ).clear();
  }


  // Type::ObjectType TruthEventBase_v1::type() const {
  //   return Type::TruthEventBase;
  // }


  void TruthEventBase_v1::toPersistent() {
    #ifndef XAOD_STANDALONE
    if (truthParticleLinksAcc.isAvailableWritable(*this)) {
      /// @todo Excellent use-case for C++11 for-each!
      for (std::vector< ElementLink<TruthParticleContainer > >::iterator it = truthParticleLinksAcc(*this).begin();
           it != truthParticleLinksAcc(*this).end(); ++it) {
        it->toPersistent();
      }
    }
    if (truthVertexLinksAcc.isAvailableWritable(*this)) {
      /// @todo Excellent use-case for C++11 for-each!
      for (std::vector< ElementLink<TruthVertexContainer > >::iterator it = truthVertexLinksAcc(*this).begin();
           it != truthVertexLinksAcc(*this).end(); ++it) {
        it->toPersistent();
      }
    }
    #endif // not XAOD_STANDALONE
  }


} // namespace xAOD
