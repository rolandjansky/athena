/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v2.cxx 747258 2016-05-15 02:57:19Z griffith $


// System include(s):
extern "C" {
#   include <cstdint>
}
#include <bitset>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v2.h"
#include "xAODTau/versions/TauJetCalibMapper_v1.h"
#include "TauJetAccessors_v2.h"

namespace xAOD {
  
  TauJet_v2::TauJet_v2()
    : IParticle() {
  }
  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, m)


  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mJetSeed)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mDetectorAxis)

  //primitive setters and getters for IntermediateAxis 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mIntermediateAxis)

  //primitive setters and getters for TauEnergyScale 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mTauEnergyScale)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mTauEtaCalib)

  // //primitive setters and getters for jetseed 4-vector
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mPanTauEFlowRecProto)

  // //primitive setters and getters for jetseed 4-vector
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mPanTauEFlowRec)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mPanTauCellBasedProto)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mPanTauCellBased)

  //primitive setters and getters for HLT 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, ptTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, etaTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, phiTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v2, float, double, mTrigCaloOnly)


  double TauJet_v2::e() const {
    return genvecP4().E();
  }


  double TauJet_v2::rapidity() const {
    return genvecP4().Rapidity();
  }

  TauJet_v2::FourMom_t TauJet_v2::p4() const {
    FourMom_t p4;
    p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    return p4;	
  }

  TauJet_v2::GenVecFourMom_t TauJet_v2::genvecP4() const {
    return GenVecFourMom_t(pt(), eta(), phi(), m());
  }

  TauJet_v2::FourMom_t TauJet_v2::p4(const TauJetParameters::TauCalibType calib) const {
    FourMom_t p4;
    switch(calib) {
    case TauJetParameters::JetSeed:
      p4.SetPtEtaPhiM( ptJetSeed(), etaJetSeed(), phiJetSeed(), mJetSeed());
      break;
    case TauJetParameters::DetectorAxis:
      p4.SetPtEtaPhiM( ptDetectorAxis(), etaDetectorAxis(), phiDetectorAxis(), mDetectorAxis());
      break;
    case TauJetParameters::IntermediateAxis:
      p4.SetPtEtaPhiM( ptIntermediateAxis(), etaIntermediateAxis(), phiIntermediateAxis(), mIntermediateAxis());
      break;
    case TauJetParameters::TauEnergyScale:
      p4.SetPtEtaPhiM( ptTauEnergyScale(), etaTauEnergyScale(), phiTauEnergyScale(), mTauEnergyScale());
      break;
    case TauJetParameters::TauEtaCalib:
      p4.SetPtEtaPhiM( ptTauEtaCalib(), etaTauEtaCalib(), phiTauEtaCalib(), mTauEtaCalib());
      break;
      // case TauJetParameters::PanTauEFlowRecProto:
      // 	p4.SetPtEtaPhiM( ptPanTauEFlowRecProto(), etaPanTauEFlowRecProto(), phiPanTauEFlowRecProto(), mPanTauEFlowRecProto());
      // 	break;
      // case TauJetParameters::PanTauEFlowRec:
      // 	p4.SetPtEtaPhiM( ptPanTauEFlowRec(), etaPanTauEFlowRec(), phiPanTauEFlowRec(), mPanTauEFlowRec());
      // 	break;
    case TauJetParameters::PanTauCellBasedProto:
      p4.SetPtEtaPhiM( ptPanTauCellBasedProto(), etaPanTauCellBasedProto(), phiPanTauCellBasedProto(), mPanTauCellBasedProto());
      break;
    case TauJetParameters::PanTauCellBased:
      p4.SetPtEtaPhiM( ptPanTauCellBased(), etaPanTauCellBased(), phiPanTauCellBased(), mPanTauCellBased());
      break;
    case TauJetParameters::TrigCaloOnly:
      p4.SetPtEtaPhiM( ptTrigCaloOnly(), etaTrigCaloOnly(), phiTrigCaloOnly(), mTrigCaloOnly());
      break;
    default:
      p4.SetPtEtaPhiM( pt(), eta(), phi(), m());
    }

    return p4;
  }

  TauJet_v2::GenVecFourMom_t TauJet_v2::genvecP4(const TauJetParameters::TauCalibType calib) const {
    switch(calib) {
    case TauJetParameters::JetSeed:
      return GenVecFourMom_t( ptJetSeed(), etaJetSeed(), phiJetSeed(), mJetSeed());
    case TauJetParameters::DetectorAxis:
      return GenVecFourMom_t( ptDetectorAxis(), etaDetectorAxis(), phiDetectorAxis(), mDetectorAxis());
    case TauJetParameters::IntermediateAxis:
      return GenVecFourMom_t( ptIntermediateAxis(), etaIntermediateAxis(), phiIntermediateAxis(), mIntermediateAxis());
    case TauJetParameters::TauEnergyScale:
      return GenVecFourMom_t( ptTauEnergyScale(), etaTauEnergyScale(), phiTauEnergyScale(), mTauEnergyScale());
    case TauJetParameters::TauEtaCalib:
      return GenVecFourMom_t( ptTauEtaCalib(), etaTauEtaCalib(), phiTauEtaCalib(), mTauEtaCalib());
      // case TauJetParameters::PanTauEFlowRecProto:
      // 	return GenVecFourMom_t( ptPanTauEFlowRecProto(), etaPanTauEFlowRecProto(), phiPanTauEFlowRecProto(), mPanTauEFlowRecProto());
      // case TauJetParameters::PanTauEFlowRec:
      // 	return GenVecFourMom_t( ptPanTauEFlowRec(), etaPanTauEFlowRec(), phiPanTauEFlowRec(), mPanTauEFlowRec());
    case TauJetParameters::PanTauCellBasedProto:
      return GenVecFourMom_t( ptPanTauCellBasedProto(), etaPanTauCellBasedProto(), phiPanTauCellBasedProto(), mPanTauCellBasedProto());
    case TauJetParameters::PanTauCellBased:
      return GenVecFourMom_t( ptPanTauCellBased(), etaPanTauCellBased(), phiPanTauCellBased(), mPanTauCellBased());
    case TauJetParameters::TrigCaloOnly:
      return GenVecFourMom_t( ptTrigCaloOnly(), etaTrigCaloOnly(), phiTrigCaloOnly(), mTrigCaloOnly());
    default:
      return GenVecFourMom_t( pt(), eta(), phi(), m());
    }
  }

  
  void TauJet_v2::setP4(double pt, double eta, double phi, double m)  {
    static const Accessor< float > acc1( "pt" );
    static const Accessor< float > acc2( "eta" );
    static const Accessor< float > acc3( "phi" );
    static const Accessor< float > acc4( "m" );
    acc1( *this )=pt;
    acc2( *this )=eta;
    acc3( *this )=phi;
    acc4( *this )=m;
  }

  bool TauJet_v2::setP4(const TauJetParameters::TauCalibType calib, double pt, double eta, double phi, double m)  {
    TauJetCalibTypeMapper_v1 typeMapper;
    std::string calibString = "";
    bool result = typeMapper.getValue(calib, calibString);
    if (result) {
      Accessor< float > accTauPtCalib( "pt"+calibString );
      Accessor< float > accTauEtaCalib( "eta"+calibString );
      Accessor< float > accTauPhiCalib( "phi"+calibString );
      Accessor< float > accTauMCalib( "m"+calibString );
      accTauPtCalib( *this )=pt;
      accTauEtaCalib( *this )=eta;
      accTauPhiCalib( *this )=phi;
      accTauMCalib( *this )=m;
      return true;
    }
    else {
      return false;
    }
  }


   Type::ObjectType TauJet_v2::type() const {
      return Type::Tau;
   }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v2, unsigned int, ROIWord ,  setROIWord )		    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v2, float, charge ,  setCharge )		    

  //-------------------------------------------------------------------------
  // Has discriminant
  //-------------------------------------------------------------------------
  bool TauJet_v2::hasDiscriminant( TauJetParameters::TauID discID ) const
  {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV2( discID );
    return acc != nullptr;
  }


  //-------------------------------------------------------------------------
  // Get discriminant
  //-------------------------------------------------------------------------
  double TauJet_v2::discriminant( TauJetParameters::TauID discriminant ) const {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV2( discriminant );
    if( ! acc ) return -1111.0;
    
    // Retrieve the discriminant value:
    return ( *acc )( *this );
  }
  
  //-------------------------------------------------------------------------
  // Set discriminant
  //-------------------------------------------------------------------------
  void TauJet_v2::setDiscriminant( TauJetParameters::TauID discriminant, double value ) {
    ( *( xAODTau::discriminantAccessorV2( discriminant ) ) )( *this ) = value;
    return;
  }
    
    //-------------------------------------------------------------------------
    // Accept
    //-------------------------------------------------------------------------
    bool TauJet_v2::isTau(
            TauJetParameters::IsTauFlag flag ) const
    {
      static const Accessor< uint32_t > acc( "isTauFlags" );
      std::bitset<32> isTauFlags( acc( *this ) );
      return isTauFlags[flag];
    }

    void TauJet_v2::setIsTau(
            TauJetParameters::IsTauFlag flag, bool value )
    {
      static const Accessor< uint32_t > acc( "isTauFlags" );
      std::bitset<32> isTauFlags( acc( *this ) );
      isTauFlags[flag] = value;
      acc( *this ) = isTauFlags.to_ulong();
    }

    //-------------------------------------------------------------------------
    // Veto flags accessors
    //-------------------------------------------------------------------------
    void TauJet_v2::setFlag(
            TauJetParameters::VetoFlags flag, bool value )
    {
      static const Accessor< uint32_t > acc( "vetoFlags" );
      std::bitset<32> vetoFlags( acc( *this ) );
      vetoFlags[flag] = value;
      acc( *this ) = vetoFlags.to_ulong();
    }

    bool TauJet_v2::flag(
            TauJetParameters::VetoFlags flag ) const
    {
      static const Accessor< uint32_t > acc( "vetoFlags" );
      std::bitset<32> vetoFlags( acc( *this ) );
      return vetoFlags[flag];
    }



  //-------------------------------------------------------------------------
  // Get int detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v2::detail( TauJetParameters::Detail detail, int& value ) const {
    // Get the detail accessor:
    const Accessor< int >* acc = xAODTau::detailsAccessorV2<int>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v2::detail( TauJetParameters::Detail detail, float& value ) const {
    // Get the detail accessor:
    const Accessor< float >* acc = xAODTau::detailsAccessorV2<float>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v2::setDetail( TauJetParameters::Detail detail, int value ) {
    ( *( xAODTau::detailsAccessorV2<int>( detail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v2::setDetail( TauJetParameters::Detail detail, float value ) {
    ( *( xAODTau::detailsAccessorV2<float>( detail ) ) )( *this ) = value;
  }


  //-------------------------------------------------------------------------
  // Get int panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v2::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, int& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< int >* acc = xAODTau::panTauDetailsAccessorV2<int>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v2::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, float& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< float >* acc = xAODTau::panTauDetailsAccessorV2<float>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v2::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, int value ) {
    ( *( xAODTau::panTauDetailsAccessorV2<int>( panTauDetail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v2::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, float value ) {
    ( *( xAODTau::panTauDetailsAccessorV2<float>( panTauDetail ) ) )( *this ) = value;
  }


  // setters and getters for the track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::TrackParticleLinks_t,
  				     trackLinks,
  				     setTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v2::TrackParticleLinks_t > trackAcc( "trackLinks" );
  
  const TrackParticle* TauJet_v2::track( size_t i ) const {
   return ( *trackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nTracks() const {
    return trackAcc( *this ).size();
  }
  
  void TauJet_v2::addTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   trackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v2::clearTrackLinks() {
   trackAcc( *this ).clear();
   return;
  }


  bool TauJet_v2::trackFlag(const TrackParticle* track, TauJetParameters::TauTrackFlag flag) const
  {
    static const Accessor< std::vector < uint8_t > > trackFlags("trackFlags");
    std::vector<uint8_t> vec = trackFlags(*this);
    unsigned int totalTracks = this->nTracks() + this->nWideTracks() + this->nOtherTracks();
    while (vec.size() < totalTracks) {
      vec.push_back(0);
    }
    for (unsigned int i = 0 ; i < totalTracks ; i++) {
      const TrackParticle* iterTrack;
      if (i < this->nTracks()) {
        iterTrack = this->track(i);
      }
      else if (i < this->nTracks() + this->nWideTracks()) {
        iterTrack = this->wideTrack(i - this->nTracks());
      }
      else {
        iterTrack = this->otherTrack(i - this->nTracks() - this->nWideTracks());
      }
      if (track == iterTrack) {
        std::bitset<8> thisTracksFlags(vec[i]);
        return thisTracksFlags[flag];
      }
    }
    return false;
  }
  
  void TauJet_v2::setTrackFlag(const TrackParticle* track, TauJetParameters::TauTrackFlag flag, bool value)
  {
    static const Accessor< std::vector < uint8_t > > trackFlags("trackFlags");
    std::vector<uint8_t> vec = trackFlags(*this);
    unsigned int totalTracks = this->nTracks() + this->nWideTracks() + this->nOtherTracks();
    while (vec.size() < totalTracks) {
      vec.push_back(0);
    }
    for (unsigned int i = 0 ; i < totalTracks ; i++) {
      const TrackParticle* iterTrack;
      if (i < this->nTracks()) {
        iterTrack = this->track(i);
      }
      else if (i < this->nTracks() + this->nWideTracks()) {
        iterTrack = this->wideTrack(i - this->nTracks());
      }
      else {
        iterTrack = this->otherTrack(i - this->nTracks() - this->nWideTracks());
      }
      if (track == iterTrack) {
        std::bitset<8> thisTracksFlags(vec[i]);
        thisTracksFlags[flag] = value;
        vec[i] = thisTracksFlags.to_ulong();
      }
    }
    trackFlags(*this) = vec;
  }


  //setter and getter for trackfilterprongs
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v2, int, trackFilterProngs ,  setTrackFilterProngs )
  //setter and getter for trackfilterquality
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v2, int, trackFilterQuality ,  setTrackFilterQuality )
  //setter and getter for pi0ConeDR
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v2, float, pi0ConeDR ,  setPi0ConeDR )



  float TauJet_v2::trackEtaStrip(unsigned int numTrack) const
  {
    static const Accessor< std::vector <float > > trackEtaStripAcc("trackEtaStrip");
    const std::vector<float>& vec=trackEtaStripAcc(*this);
    if (vec.size()<numTrack+1)
      return -999.0; 
    else
      return vec[numTrack];
  }
  
  void TauJet_v2::setTrackEtaStrip(unsigned int numTrack, float eta)
  {
    static const Accessor< std::vector <float > > trackEtaStripAcc("trackEtaStrip");
    std::vector<float>& vec=trackEtaStripAcc(*this);
    if (vec.size()<numTrack+1)
      vec.resize(numTrack+1); 
    vec[numTrack]=eta;
  }


  float TauJet_v2::trackPhiStrip(unsigned int numTrack) const
  {
    static const Accessor< std::vector <float > > trackPhiStripAcc("trackPhiStrip");
    const std::vector<float>& vec=trackPhiStripAcc(*this);
    if (vec.size()<numTrack+1)
      return -999.0; 
    else
      return vec[numTrack];
  }
  
  void TauJet_v2::setTrackPhiStrip(unsigned int numTrack, float phi)
  {
    static const Accessor< std::vector <float > > trackPhiStripAcc("trackPhiStrip");
    std::vector<float>& vec=trackPhiStripAcc(*this);
    if (vec.size()<numTrack+1)
      vec.resize(numTrack+1); 
    vec[numTrack]=phi;
  }



  // setters and getters for the wide track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::TrackParticleLinks_t,
  				     wideTrackLinks,
  				     setWideTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v2::TrackParticleLinks_t > wideTrackAcc( "wideTrackLinks" );
  
  const TrackParticle* TauJet_v2::wideTrack( size_t i ) const {
   return ( *wideTrackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nWideTracks() const {
   return wideTrackAcc( *this ).size();
  }
  
  void TauJet_v2::addWideTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   wideTrackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v2::clearWideTrackLinks() {
   wideTrackAcc( *this ).clear();
   return;
  }

  // setters and getters for the other track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::TrackParticleLinks_t,
  				     otherTrackLinks,
  				     setOtherTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v2::TrackParticleLinks_t > otherTrackAcc( "otherTrackLinks" );
  
  const TrackParticle* TauJet_v2::otherTrack( size_t i ) const {
   return ( *otherTrackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nOtherTracks() const {
   return otherTrackAcc( *this ).size();
  }
  
  void TauJet_v2::addOtherTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   otherTrackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v2::clearOtherTrackLinks() {
   otherTrackAcc( *this ).clear();
   return;
  }

  ///setter and getter for the seed jet link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::JetLink_t,
  				     jetLink,
  				     setJetLink )

  static const SG::AuxElement::Accessor< TauJet_v2::JetLink_t > jetAcc( "jetLink" );

  const Jet* TauJet_v2::jet() const {
   return ( *jetAcc( *this ) );
  }


  void TauJet_v2::setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet) {
    if( jetAcc( *this ).isValid() )
      jetAcc( *this ).reset();
    jetAcc( *this ).toContainedElement( *cont, jet );
    }



  ///setter and getter for the vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::VertexLink_t,
  				     vertexLink,
  				     setVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v2::VertexLink_t > vertexAcc( "vertexLink" );

  const Vertex* TauJet_v2::vertex() const {
   return ( *vertexAcc( *this ) );
  }


  void TauJet_v2::setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( vertexAcc( *this ).isValid() )
      vertexAcc( *this ).reset();
    vertexAcc( *this ).toContainedElement( *cont, vertex );
    }



  ///setter and getter for the secondary vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::VertexLink_t,
  				     secondaryVertexLink,
  				     setSecondaryVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v2::VertexLink_t > secondaryVertexAcc( "secondaryVertexLink" );

  const Vertex* TauJet_v2::secondaryVertex() const {
   return ( *secondaryVertexAcc( *this ) );
  }


  void TauJet_v2::setSecondaryVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( secondaryVertexAcc( *this ).isValid() )
      secondaryVertexAcc( *this ).reset();
    secondaryVertexAcc( *this ).toContainedElement( *cont, vertex );
    }




  /////default pfos
  // setters and getters for the hadronic PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     hadronicPFOLinks,
  				     setHadronicPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > hadronicPFOAcc( "hadronicPFOLinks" );
  
  const PFO* TauJet_v2::hadronicPFO( size_t i ) const {
   return ( *hadronicPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nHadronicPFOs() const {
   return hadronicPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addHadronicPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   hadronicPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearHadronicPFOLinks() {
   hadronicPFOAcc( *this ).clear();
   return;
  }

  // setters and getters for the shot PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     shotPFOLinks,
  				     setShotPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > shotPFOAcc( "shotPFOLinks" );
  
  const PFO* TauJet_v2::shotPFO( size_t i ) const {
   return ( *shotPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nShotPFOs() const {
   return shotPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addShotPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   shotPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearShotPFOLinks() {
   shotPFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     chargedPFOLinks,
  				     setChargedPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > chargedPFOAcc( "chargedPFOLinks" );
  
  const PFO* TauJet_v2::chargedPFO( size_t i ) const {
   return ( *chargedPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nChargedPFOs() const {
   return chargedPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addChargedPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   chargedPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearChargedPFOLinks() {
   chargedPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     neutralPFOLinks,
  				     setNeutralPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > neutralPFOAcc( "neutralPFOLinks" );
  
  const PFO* TauJet_v2::neutralPFO( size_t i ) const {
   return ( *neutralPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nNeutralPFOs() const {
   return neutralPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addNeutralPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   neutralPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearNeutralPFOLinks() {
   neutralPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     pi0PFOLinks,
  				     setPi0PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > pi0PFOAcc( "pi0PFOLinks" );
  
  const PFO* TauJet_v2::pi0PFO( size_t i ) const {
   return ( *pi0PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nPi0PFOs() const {
   return pi0PFOAcc( *this ).size();
  }
  
  void TauJet_v2::addPi0PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   pi0PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearPi0PFOLinks() {
   pi0PFOAcc( *this ).clear();
   return;
  }





  /////cellbased pfos
  // setters and getters for the cellbased_charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     protoChargedPFOLinks,
  				     setProtoChargedPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > protoChargedPFOAcc( "protoChargedPFOLinks" );
  
  const PFO* TauJet_v2::protoChargedPFO( size_t i ) const {
   return ( *protoChargedPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nProtoChargedPFOs() const {
   return protoChargedPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addProtoChargedPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoChargedPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearProtoChargedPFOLinks() {
   protoChargedPFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the cellbased_neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     protoNeutralPFOLinks,
  				     setProtoNeutralPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > protoNeutralPFOAcc( "protoNeutralPFOLinks" );
  
  const PFO* TauJet_v2::protoNeutralPFO( size_t i ) const {
   return ( *protoNeutralPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nProtoNeutralPFOs() const {
   return protoNeutralPFOAcc( *this ).size();
  }
  
  void TauJet_v2::addProtoNeutralPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoNeutralPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearProtoNeutralPFOLinks() {
   protoNeutralPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the cellbased_pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v2,
  				     TauJet_v2::PFOLinks_t,
  				     protoPi0PFOLinks,
  				     setProtoPi0PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v2::PFOLinks_t > protoPi0PFOAcc( "protoPi0PFOLinks" );
  
  const PFO* TauJet_v2::protoPi0PFO( size_t i ) const {
   return ( *protoPi0PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v2::nProtoPi0PFOs() const {
   return protoPi0PFOAcc( *this ).size();
  }
  
  void TauJet_v2::addProtoPi0PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoPi0PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v2::clearProtoPi0PFOLinks() {
   protoPi0PFOAcc( *this ).clear();
   return;
  }




  void TauJet_v2::dump() const {
	  /* keep these two lines to create the template methods. Otherwise they are optimzed away
	   * later: move these two lines into a dummy method, which nobody is calling
	   */
	  std::cout << this->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk) << std::endl;
	  std::cout << this->detail<int>(xAOD::TauJetParameters::numTopoClusters) << std::endl;
  }
  
} // namespace xAOD

//  LocalWords:  panTauDetail mJetSeed
