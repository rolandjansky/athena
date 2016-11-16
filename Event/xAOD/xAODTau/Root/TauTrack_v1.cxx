/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTau/versions/TauTrack_v1.h"
#define TAUTRACK_V1_ACCESSORS
#include "TauJetAccessors_v3.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauJet.h"

#include <cmath>

namespace xAOD {

  TauTrack_v1::TauTrack_v1()
    : IParticle(), m_p4(), m_p4Cached( false ) {
  }

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauTrack_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauTrack_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauTrack_v1, float, double, phi)

  // Pion mass, as assumed for the track:
  static const double PION_MASS = 139.570;

  const TauTrack_v1::FourMom_t& TauTrack_v1::p4() const {

     // Update the 4-momentum object if necessary:
     if( ! m_p4Cached ) {
        m_p4.SetPtEtaPhiM( pt(), eta(), phi(), PION_MASS );//using pion mass
        m_p4Cached = true;
     }

     // Return the object:
     return m_p4;
  }


  void TauTrack_v1::setP4(double pt, double eta, double phi, double /*m*/)  {

     // Set the raw variables:
     static Accessor< float > acc1( "pt" );
     static Accessor< float > acc2( "eta" );
     static Accessor< float > acc3( "phi" );
     acc1( *this ) = pt;
     acc2( *this ) = eta;
     acc3( *this ) = phi;

     // Reset the cache:
     m_p4Cached = false;

     return;
  }


  Type::ObjectType TauTrack_v1::type() const {
      return Type::CompositeParticle;
  }


  double TauTrack_v1::rapidity() const {
    return p4().Rapidity();
  }

  double TauTrack_v1::m() const {
     return PION_MASS;
  }

  double TauTrack_v1::e() const {
    return p4().E();
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, TauTrack_v1::TrackFlagType, flagSet, setFlagSet)

  bool TauTrack_v1::flag(TauJetParameters::TauTrackFlag flag) const{
    static Accessor< TauTrack_v1::TrackFlagType > trackFlags("flagSet");
    TrackFlagType f(trackFlags(*this));
    std::bitset<8*sizeof(TrackFlagType)> thisTracksFlags(f);
    return thisTracksFlags[flag];
  }

  bool TauTrack_v1::flagWithMask(unsigned int flags) const{
    static Accessor< TauTrack_v1::TrackFlagType > trackFlags("flagSet");
    TrackFlagType f(trackFlags(*this));
    // std::bitset<8*sizeof(TrackFlagType)> thisTracksFlags(f);
    // std::bitset<8*sizeof(TrackFlagType)> tracksFlags(flags);
    // for(int i = 0; i != 8*sizeof(TrackFlagType); ++i){
    //   if(tracksFlags[i]==1 && thisTracksFlags[i]==0) return false;
    // }
    // return true;
    return ((f&flags)==flags);
  }

  void TauTrack_v1::setFlag(TauJetParameters::TauTrackFlag flag, bool value){
    static Accessor< TauTrack_v1::TrackFlagType > trackFlags("flagSet");
    TrackFlagType& f(trackFlags(*this));
    std::bitset<8*sizeof(TrackFlagType)> thisTracksFlags(f);
    thisTracksFlags[flag] = value;
    f = thisTracksFlags.to_ulong();
  }

  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, z0sinThetaTJVA, setZ0sinThetaTJVA)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, rConv, setRConv)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, rConvII, setRConvII)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, dRJetSeedAxis, setDRJetSeedAxis)

  float TauTrack_v1::z0sinThetaTJVA(const xAOD::IParticle& part ) const{
    const xAOD::TrackParticle* xTrackParticle = this->track();
    const xAOD::TauJet* tau = dynamic_cast<const xAOD::TauJet*> (&part);
    if( tau ) {
       float zv = 0.0;
       if( tau->vertexLink().isValid() ) {
          zv = tau->vertex()->z();
       }
       return ( ( xTrackParticle->z0() + xTrackParticle->vz() - zv ) *
                std::sin( xTrackParticle->theta() ) );
    } else {
      std::cerr << "ERROR xAOD::TauTrack::z0sinThetaTJVA cannot get TauJet" << std::endl;
      return 0;
    }
  }

  float TauTrack_v1::rConv(const xAOD::IParticle& ) const{
    const xAOD::TrackParticle* xTrackParticle = this->track();
    return std::sqrt(std::fabs(xTrackParticle->d0())*xTrackParticle->pt()/(.3 /*0.15*2.*/));
  }

  float TauTrack_v1::rConvII(const xAOD::IParticle& ) const{
    const xAOD::TrackParticle* xTrackParticle = this->track();
    return std::sqrt( std::fabs( xTrackParticle->d0() * xTrackParticle->pt() ) / (0.3)  )*(xTrackParticle->d0()/fabs(xTrackParticle->d0()))*xTrackParticle->charge();
  }

  float TauTrack_v1::dRJetSeedAxis(const xAOD::IParticle& part) const{
    const xAOD::TauJet* tau = dynamic_cast<const xAOD::TauJet*> (&part);
    if(tau)
      return tau->jet()->p4().DeltaR(this->p4());//this function should take jet seed as input
    else {
      std::cerr << "ERROR xAOD::TauTrack::z0sinThetaTJVA cannot get TauJet" << std::endl;
    }
    return 0.;
  }


  
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, etaStrip, setEtaStrip)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauTrack_v1, float, phiStrip, setPhiStrip)

  bool TauTrack_v1::detail( TauJetParameters::TrackDetail detail, float& value ) const{
    // Get the detail accessor:
    Accessor< float >* acc = xAODTau::trackDetailsAccessorV3<float>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;

  }

  void TauTrack_v1::setDetail( TauJetParameters::TrackDetail detail, float value ){
    ( *( xAODTau::trackDetailsAccessorV3<float>( detail ) ) )( *this ) = value;    
  }

  // setters and getters for bdt values
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauTrack_v1,
                                     std::vector<float>,
                                     bdtScores,
                                     setBdtScores )
  
  static SG::AuxElement::Accessor< std::vector<float> > bdtScoreAcc( "bdtScores" );
  
  float TauTrack_v1::bdtScore( size_t i) const {
    return bdtScoreAcc(*this).at(i);
  }
  
  //number of clusters
  size_t TauTrack_v1::nBdtScores() const{
    return bdtScoreAcc( *this ).size();
  }
  
  /// add a cluster to the tau
  void TauTrack_v1::addBdtScore( const float score){
    bdtScoreAcc( *this ).push_back(score);
  }
  
  /// Remove all clusters from the tau
  void TauTrack_v1::clearBdtScores(){
    bdtScoreAcc( *this ).clear();
  }

  // ----------------------------------------------------------------------------
  // setters and getters for the track link
  // ----------------------------------------------------------------------------
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauTrack_v1,
  				     TauTrack_v1::TrackParticleLinks_t,
  				     trackLinks,
  				     setTrackLinks )
  
  static SG::AuxElement::Accessor< TauTrack_v1::TrackParticleLinks_t > trackAcc( "trackLinks" );
  
  const TrackParticle* TauTrack_v1::track() const {
    return ( *trackAcc( *this )[0] );
  }

  void TauTrack_v1::addTrackLink( const ElementLink< TrackParticleContainer >& tr) {
    //clearTrackLinks();
    trackAcc( *this ).push_back( tr );
    return;
  }
  
  void TauTrack_v1::clearTrackLinks() {
   trackAcc( *this ).clear();
   return;
  }

  

} // namespace xAOD
