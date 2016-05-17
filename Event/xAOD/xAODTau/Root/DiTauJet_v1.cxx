/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DiTauJet_v1.cxx 631921 2015-09-23 23:30:59Z dkirchme $


// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTau/versions/DiTauJet_v1.h"
// #include "xAODDiTau/versions/TauJetCalibMapper_v1.h"
#include "DiTauJetAccessors_v1.h"

namespace xAOD {
  
  DiTauJet_v1::DiTauJet_v1()
    : IParticle(), m_p4(), m_p4Cached( false ) {
  }
  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( DiTauJet_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( DiTauJet_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( DiTauJet_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( DiTauJet_v1, float, double, m)


  const DiTauJet_v1::FourMom_t& DiTauJet_v1::p4() const {
      m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m());
      m_p4Cached = true;

    return m_p4;
  }


  void DiTauJet_v1::setP4(double pt, double eta, double phi, double m)  {
    static Accessor< float > acc1( "pt" );
    static Accessor< float > acc2( "eta" );
    static Accessor< float > acc3( "phi" );
    static Accessor< float > acc4( "m" );
    acc1( *this ) = pt;
    acc2( *this ) = eta;
    acc3( *this ) = phi;
    acc4( *this ) = m;
  }


  Type::ObjectType DiTauJet_v1::type() const {
      return Type::Jet;
  }


  double DiTauJet_v1::rapidity() const {
    m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m());

    return p4().Rapidity();
  }


  double DiTauJet_v1::e() const {
    m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m());
    return p4().E();
  }


  // ----------------------------------------------------------------------------
  // setter and getter for the seed jet link
  // ----------------------------------------------------------------------------
  AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
                     DiTauJet_v1::JetLink_t,
                     jetLink,
                     setJetLink )

  static SG::AuxElement::Accessor< DiTauJet_v1::JetLink_t > jetAcc( "jetLink" );

  const Jet* DiTauJet_v1::jet() const {
   return ( *jetAcc( *this ) );
  }


  void DiTauJet_v1::setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet) {
    if( jetAcc( *this ).isValid() )
      jetAcc( *this ).reset();
    jetAcc( *this ).toContainedElement( *cont, jet );
    }

  // ----------------------------------------------------------------------------
  // getter and setter for subjet 4-mom
  // ----------------------------------------------------------------------------

  float DiTauJet_v1::subjetPt(unsigned int numSubjet) const
  {
    static Accessor< std::vector <float > > subjetPtAcc("subjet_pt");
    const std::vector<float>& vPt = subjetPtAcc(*this);
    if (vPt.size()<numSubjet+1)
      return -999.0; 
    else
      return vPt[numSubjet];
  }
  
  float DiTauJet_v1::subjetEta(unsigned int numSubjet) const
  {
    static Accessor< std::vector <float > > subjetEtaAcc("subjet_eta");
    const std::vector<float>& vEta = subjetEtaAcc(*this);
    if (vEta.size()<numSubjet+1)
      return -999.0; 
    else
      return vEta[numSubjet];
  }
  
  float DiTauJet_v1::subjetPhi(unsigned int numSubjet) const
  {
    static Accessor< std::vector <float > > subjetPhiAcc("subjet_phi");
    const std::vector<float>& vPhi = subjetPhiAcc(*this);
    if (vPhi.size()<numSubjet+1)
      return -999.0; 
    else
      return vPhi[numSubjet];
  }
  
  float DiTauJet_v1::subjetE(unsigned int numSubjet) const
  {
    static Accessor< std::vector <float > > subjetEAcc("subjet_e");
    const std::vector<float>& vE = subjetEAcc(*this);
    if (vE.size()<numSubjet+1)
      return -999.0; 
    else
      return vE[numSubjet];
  }

  float DiTauJet_v1::nSubjets() const
  {
    static Accessor < std::vector <float> > subjetPtAcc("subjet_pt");
    const std::vector<float>& vPt = subjetPtAcc(*this);
    return vPt.size();
  }
  
  void DiTauJet_v1::setSubjetPtEtaPhiE(unsigned int numSubjet, 
                                       float pt, float eta, float phi, float e)
  {
    static Accessor< std::vector <float > > subjetPtAcc("subjet_pt");
    static Accessor< std::vector <float > > subjetEtaAcc("subjet_eta");
    static Accessor< std::vector <float > > subjetPhiAcc("subjet_phi");
    static Accessor< std::vector <float > > subjetEAcc("subjet_e");
    std::vector<float>& vPt = subjetPtAcc(*this);
    std::vector<float>& vEta = subjetEtaAcc(*this);
    std::vector<float>& vPhi = subjetPhiAcc(*this);
    std::vector<float>& vE = subjetEAcc(*this);
    if (vPt.size()<numSubjet+1)
      vPt.resize(numSubjet+1); 
    if (vEta.size()<numSubjet+1)
      vEta.resize(numSubjet+1); 
    if (vPhi.size()<numSubjet+1)
      vPhi.resize(numSubjet+1); 
    if (vE.size()<numSubjet+1)
      vE.resize(numSubjet+1); 

    vPt[numSubjet] = pt;
    vEta[numSubjet] = eta;
    vPhi[numSubjet] = phi;
    vE[numSubjet] = e;
  }

  // ----------------------------------------------------------------------------
  // f_core getter and setter
  // ----------------------------------------------------------------------------
    float DiTauJet_v1::fCore(unsigned int numSubjet) const
  {
    static Accessor< std::vector <float > > fCoreAcc("subjet_f_core");
    const std::vector<float>& vec = fCoreAcc(*this);
    if (vec.size()<numSubjet+1)
      return -999.0; 
    else
      return vec[numSubjet];
  }
  
  void DiTauJet_v1::setfCore(unsigned int numSubjet, float fCore)
  {
    static Accessor< std::vector <float > > fCoreAcc("subjet_f_core");
    std::vector<float>& vec = fCoreAcc(*this);
    if (vec.size()<numSubjet+1)
      vec.resize(numSubjet+1); 

    vec[numSubjet] = fCore;
  }


  // ----------------------------------------------------------------------------
  // setter and getter for the primary vertex link
  // ----------------------------------------------------------------------------
  AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
                     DiTauJet_v1::VertexLink_t,
                     vertexLink,
                     setVertexLink )

  static SG::AuxElement::Accessor< DiTauJet_v1::VertexLink_t > vertexAcc( "vertexLink" );

  const Vertex* DiTauJet_v1::vertex() const {
    return ( *vertexAcc( *this ) );
  }


  void DiTauJet_v1::setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( vertexAcc( *this ).isValid() )
      vertexAcc( *this ).reset();
    vertexAcc( *this ).toContainedElement( *cont, vertex );
  }

  // // ----------------------------------------------------------------------------
  // // setters and getters for the secondary vertex links
  // // ----------------------------------------------------------------------------
  // AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
  //                    DiTauJet_v1::SecVertexLinks_t,
  //                    secVertexLinks,
  //                    setSecVertexLinks )
  
  // static SG::AuxElement::Accessor< DiTauJet_v1::SecVertexLinks_t > secVtxAcc( "secVertexLinks" );
  
  // const Vertex* DiTauJet_v1::secVertex( size_t i ) const {
  //   return ( *secVtxAcc( *this )[ i ] );
  // }
  
  // size_t DiTauJet_v1::nSecVertex() const {
  //   return secVtxAcc( *this ).size();
  // }
  
  // void DiTauJet_v1::addSecVertex( const xAOD::VertexContainer* pVertexCont, 
  //                                       const xAOD::Vertex* pVertex) {
  //   ElementLink<xAOD::VertexContainer> linkToVertex;
  //   linkToVertex.toContainedElement(*pVertexCont, pVertex);

  //   secVtxAcc( *this ).push_back( linkToVertex );

  //   return;
  // }
  
  // void DiTauJet_v1::clearSecVertexLinks() {
  //   secVtxAcc( *this ).clear();
  //   return;
  // }

  // ----------------------------------------------------------------------------
  // setters and getters for the track links
  // ----------------------------------------------------------------------------
  // tacks inside subjets
  AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
                     DiTauJet_v1::TrackParticleLinks_t,
                     trackLinks,
                     setTrackLinks )
  
  static SG::AuxElement::Accessor< DiTauJet_v1::TrackParticleLinks_t > trackAcc( "trackLinks" );
  
  const TrackParticle* DiTauJet_v1::track( size_t i ) const {
    return ( *trackAcc( *this )[ i ] );
  }
  
  size_t DiTauJet_v1::nTracks() const {
    return trackAcc( *this ).size();
  }
  
  void DiTauJet_v1::addTrack( const xAOD::TrackParticleContainer* pTrackParticleCont, 
                              const xAOD::TrackParticle* pTrackParticle) {
    ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
    linkToTrackParticle.toContainedElement(*pTrackParticleCont, pTrackParticle);

    trackAcc( *this ).push_back( linkToTrackParticle );

    return;
  }
  
  void DiTauJet_v1::clearTrackLinks() {
    trackAcc( *this ).clear();
    return;
  }


  // tacks inside isolation region
  AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
                     DiTauJet_v1::TrackParticleLinks_t,
                     isoTrackLinks,
                     setIsoTrackLinks )
  
  static SG::AuxElement::Accessor< DiTauJet_v1::TrackParticleLinks_t > isoTrackAcc( "isoTrackLinks" );
  
  const TrackParticle* DiTauJet_v1::isoTrack( size_t i ) const {
    return ( *isoTrackAcc( *this )[ i ] );
  }
  
  size_t DiTauJet_v1::nIsoTracks() const {
    return isoTrackAcc( *this ).size();
  }
  
  void DiTauJet_v1::addIsoTrack( const xAOD::TrackParticleContainer* pTrackParticleCont, 
                              const xAOD::TrackParticle* pTrackParticle) {
    ElementLink<xAOD::TrackParticleContainer> linkToIsoTrackParticle;
    linkToIsoTrackParticle.toContainedElement(*pTrackParticleCont, pTrackParticle);

    isoTrackAcc( *this ).push_back( linkToIsoTrackParticle );

    return;
  }
  
  void DiTauJet_v1::clearIsoTrackLinks() {
    isoTrackAcc( *this ).clear();
    return;
  }


  // tacks in seed jet that do not fulfill quality cuts
  AUXSTORE_OBJECT_SETTER_AND_GETTER( DiTauJet_v1,
                     DiTauJet_v1::TrackParticleLinks_t,
                     otherTrackLinks,
                     setOtherTrackLinks )
  
  static SG::AuxElement::Accessor< DiTauJet_v1::TrackParticleLinks_t > otherTrackAcc( "otherTrackLinks" );
  
  const TrackParticle* DiTauJet_v1::otherTrack( size_t i ) const {
    return ( *otherTrackAcc( *this )[ i ] );
  }
  
  size_t DiTauJet_v1::nOtherTracks() const {
    return otherTrackAcc( *this ).size();
  }
  
  void DiTauJet_v1::addOtherTrack( const xAOD::TrackParticleContainer* pTrackParticleCont, 
                                   const xAOD::TrackParticle* pTrackParticle) {
    ElementLink<xAOD::TrackParticleContainer> linkToOtherTrackParticle;
    linkToOtherTrackParticle.toContainedElement(*pTrackParticleCont, pTrackParticle);

    otherTrackAcc( *this ).push_back( linkToOtherTrackParticle );

    return;
  }
  
  void DiTauJet_v1::clearOtherTrackLinks() {
    otherTrackAcc( *this ).clear();
    return;
  }


  //-------------------------------------------------------------------------
  // Get int detail via enum
  //-------------------------------------------------------------------------
  bool DiTauJet_v1::detail( DiTauJetParameters::Detail detail, int& value ) const {
    // Get the detail accessor:
    Accessor< int >* acc = xAODDiTau::detailsAccessorV1<int>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float detail via enum
  //-------------------------------------------------------------------------
  bool DiTauJet_v1::detail( DiTauJetParameters::Detail detail, float& value ) const {
    // Get the detail accessor:
    Accessor< float >* acc = xAODDiTau::detailsAccessorV1<float>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int detail via enum
  //-------------------------------------------------------------------------
  void DiTauJet_v1::setDetail( DiTauJetParameters::Detail detail, int value ) {
    ( *( xAODDiTau::detailsAccessorV1<int>( detail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float detail via enum
  //-------------------------------------------------------------------------
  void DiTauJet_v1::setDetail( DiTauJetParameters::Detail detail, float value ) {
    ( *( xAODDiTau::detailsAccessorV1<float>( detail ) ) )( *this ) = value;
  }



} // namespace xAOD
