/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTruth/versions/TruthEvent_v1.h"
#include "TruthAccessors_v1.h"

namespace xAOD {


  TruthEvent_v1::TruthEvent_v1()
    : TruthEventBase_v1() {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Simple, always-present event properties

  //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, int, signalProcessId, setSignalProcessId )
  //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, int, eventNumber, setEventNumber )

  /// @todo Need upgrade to allow string-valued map-like access... or access a corresponding vector of names
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TruthEvent_v1, std::vector<float>, weights, setWeights )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, float, crossSection, setCrossSection )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TruthEvent_v1, float, crossSectionError, setCrossSectionError )

  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Optional PDF info accessors

  // Retrieve as int
  bool TruthEvent_v1::pdfInfoParameter(int& value, TruthEvent_v1::PdfParam information) const {
    xAOD::TruthEvent_v1::Accessor<int>* acc = pdfInfoAccessorV1<int>( information );
    if ( ! acc ) return false;
    value = ( *acc )( *this );
    return true;
  }
  // Set as int
  void TruthEvent_v1::setPdfParameter(int value, TruthEvent_v1::PdfParam information) {
    xAOD::TruthEvent_v1::Accessor<int>* acc = pdfInfoAccessorV1<int>( information );
    ( *acc )( *this ) = value;
  }

  // Retrieve as float
  bool TruthEvent_v1::pdfInfoParameter(float& value, TruthEvent_v1::PdfParam information) const {
    xAOD::TruthEvent_v1::Accessor<float>* acc = pdfInfoAccessorV1<float>( information );
    if ( ! acc ) return false;
    value = ( *acc )( *this );
    return true;
  }
  // Set as float
  void TruthEvent_v1::setPdfParameter(float value, TruthEvent_v1::PdfParam information) {
    xAOD::TruthEvent_v1::Accessor<float>* acc = pdfInfoAccessorV1<float>( information );
    ( *acc )( *this ) = value;
  }

  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  // Optional heavy ion accessors

  // Retrieve as int
  bool TruthEvent_v1::heavyIonParameter(int& value, TruthEvent_v1::HIParam information) const {
    xAOD::TruthEvent_v1::Accessor<int>* acc = heavyIonAccessorV1<int>( information );
    if ( ! acc ) return false;
    value = ( *acc )( *this );
    return true;
  }
  // Set as int
  void TruthEvent_v1::setHeavyIonParameter(int value, TruthEvent_v1::HIParam information) {
    xAOD::TruthEvent_v1::Accessor<int>* acc = heavyIonAccessorV1<int>( information );
    ( *acc )( *this ) = value;
  }

  // Retrieve as float
  bool TruthEvent_v1::heavyIonParameter(float& value, TruthEvent_v1::HIParam information) const {
    xAOD::TruthEvent_v1::Accessor<float>* acc = heavyIonAccessorV1<float>( information );
    if ( ! acc ) return false;
    value = ( *acc )( *this );
    return true;
  }
  // Set as float
  void TruthEvent_v1::setHeavyIonParameter(float value, TruthEvent_v1::HIParam information) {
    xAOD::TruthEvent_v1::Accessor<float>* acc = heavyIonAccessorV1<float>( information );
    ( *acc )( *this ) = value;
  }

  /////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////////////
  //
  //           Implementation for the links to the truth particles
  //

  // Accessor for the signal vertex
  static SG::AuxElement::Accessor< ElementLink<TruthVertexContainer> >
  signalProcessVertexLinkAcc( "signalProcessVertexLink" );

  const TruthVertex* TruthEvent_v1::signalProcessVertex() const {
    if (!signalProcessVertexLinkAcc.isAvailable(*this)) return NULL;
    const ElementLink<TruthVertexContainer>& vertLink = signalProcessVertexLinkAcc(*this);
    return (vertLink.isValid()) ? *vertLink : NULL;
  }

  void TruthEvent_v1::setSignalProcessVertexLink(const ElementLink<TruthVertexContainer>& link) {
    signalProcessVertexLinkAcc(*this) = link;
  }


  // Accessor for the beam particles
  static SG::AuxElement::Accessor< ElementLink<TruthParticleContainer> >
  beamParticle1LinkAcc( "beamParticle1Link" );
  static SG::AuxElement::Accessor< ElementLink<TruthParticleContainer> >
  beamParticle2LinkAcc( "beamParticle2Link" );

  typedef std::pair<const TruthParticle*, const TruthParticle*> TruthParticlePair;
  TruthParticlePair TruthEvent_v1::beamParticles() const {
    const ElementLink<TruthParticleContainer>& el1 = beamParticle1LinkAcc(*this);
    const ElementLink<TruthParticleContainer>& el2 = beamParticle2LinkAcc(*this);
    return (el1.isValid() && el2.isValid()) ? TruthParticlePair(*el1, *el2) : TruthParticlePair(NULL, NULL);
  }

  void TruthEvent_v1::setBeamParticleLinks(const ElementLink<TruthParticleContainer>& pcl1,
                                           const ElementLink<TruthParticleContainer>& pcl2) {
    beamParticle1LinkAcc(*this) = pcl1;
    beamParticle2LinkAcc(*this) = pcl2;
  }
  void TruthEvent_v1::setBeamParticle1Link(const ElementLink<TruthParticleContainer>& pcl1) {
    beamParticle1LinkAcc(*this) = pcl1;
  }
  void TruthEvent_v1::setBeamParticle2Link(const ElementLink<TruthParticleContainer>& pcl2) {
    beamParticle2LinkAcc(*this) = pcl2;
  }


  /////////////////////////////////////////////////////////////////////////////


  Type::ObjectType TruthEvent_v1::type() const {
    return Type::TruthEvent;
  }


  void TruthEvent_v1::toPersistent() {
    #ifndef XAOD_STANDALONE
    if (beamParticle1LinkAcc.isAvailableWritable(*this)) beamParticle1LinkAcc(*this).toPersistent();
    if (beamParticle2LinkAcc.isAvailableWritable(*this)) beamParticle2LinkAcc(*this).toPersistent();
    if (signalProcessVertexLinkAcc.isAvailableWritable(*this)) signalProcessVertexLinkAcc(*this).toPersistent();
    TruthEventBase_v1::toPersistent();
    #endif // not XAOD_STANDALONE
  }


} // namespace xAOD
