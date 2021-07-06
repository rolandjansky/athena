/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v1.cxx 740338 2016-04-14 16:03:58Z griffith $


// System include(s):
extern "C" {
#   include <cstdint>
}
#include <bitset>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v1.h"
#include "xAODTau/versions/TauJetCalibMapper_v1.h"
#include "TauJetAccessors_v1.h"

namespace xAOD {
  
  TauJet_v1::TauJet_v1()
    : IParticle() {
  }
  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, m)


  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mJetSeed)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mDetectorAxis)

  //primitive setters and getters for IntermediateAxis 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mIntermediateAxis)

  //primitive setters and getters for TauEnergyScale 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mTauEnergyScale)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mTauEtaCalib)

  // //primitive setters and getters for jetseed 4-vector
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiPanTauEFlowRecProto)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mPanTauEFlowRecProto)

  // //primitive setters and getters for jetseed 4-vector
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiPanTauEFlowRec)
  // AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mPanTauEFlowRec)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mPanTauCellBasedProto)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, ptPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, etaPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, phiPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v1, float, double, mPanTauCellBased)


  double TauJet_v1::e() const {
    return genvecP4().E();
  }


  double TauJet_v1::rapidity() const {
    return genvecP4().Rapidity();
  }

  TauJet_v1::FourMom_t TauJet_v1::p4() const {
    FourMom_t p4;
    p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    return p4;	
  }

  TauJet_v1::GenVecFourMom_t TauJet_v1::genvecP4() const {
    return GenVecFourMom_t(pt(), eta(), phi(), m());
  }

  TauJet_v1::FourMom_t TauJet_v1::p4(const TauJetParameters::TauCalibType calib) const {
    
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
    default:
      p4.SetPtEtaPhiM( pt(), eta(), phi(), m());
    }
    return p4;
  }

  TauJet_v1::GenVecFourMom_t TauJet_v1::genvecP4(const TauJetParameters::TauCalibType calib) const {
    
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
    default:
      return GenVecFourMom_t( pt(), eta(), phi(), m());
    }
  }

  
  void TauJet_v1::setP4(double pt, double eta, double phi, double m)  {
    static const Accessor< float > acc1( "pt" );
    static const Accessor< float > acc2( "eta" );
    static const Accessor< float > acc3( "phi" );
    static const Accessor< float > acc4( "m" );
    acc1( *this )=pt;
    acc2( *this )=eta;
    acc3( *this )=phi;
    acc4( *this )=m;
  }

  bool TauJet_v1::setP4(const TauJetParameters::TauCalibType calib, double pt, double eta, double phi, double m)  {
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


   Type::ObjectType TauJet_v1::type() const {
      return Type::Tau;
   }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v1, unsigned int, ROIWord ,  setROIWord )		    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v1, float, charge ,  setCharge )		    

  //-------------------------------------------------------------------------
  // Has discriminant
  //-------------------------------------------------------------------------
  bool TauJet_v1::hasDiscriminant( TauJetParameters::TauID discID ) const
  {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV1( discID );
    return acc != nullptr;
  }


  //-------------------------------------------------------------------------
  // Get discriminant
  //-------------------------------------------------------------------------
  double TauJet_v1::discriminant( TauJetParameters::TauID discriminant ) const {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV1( discriminant );
    if( ! acc ) return -1111.0;
    
    // Retrieve the discriminant value:
    return ( *acc )( *this );
  }
  
  //-------------------------------------------------------------------------
  // Set discriminant
  //-------------------------------------------------------------------------
  void TauJet_v1::setDiscriminant( TauJetParameters::TauID discriminant, double value ) {
    ( *( xAODTau::discriminantAccessorV1( discriminant ) ) )( *this ) = value;
    return;
  }
    
    //-------------------------------------------------------------------------
    // Accept
    //-------------------------------------------------------------------------
    bool TauJet_v1::isTau(
            TauJetParameters::IsTauFlag flag ) const
    {
      static const Accessor< uint32_t > acc( "isTauFlags" );
      std::bitset<32> isTauFlags( acc( *this ) );
      return isTauFlags[flag];
    }

    void TauJet_v1::setIsTau(
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
    void TauJet_v1::setFlag(
            TauJetParameters::VetoFlags flag, bool value )
    {
      static const Accessor< uint32_t > acc( "vetoFlags" );
      std::bitset<32> vetoFlags( acc( *this ) );
      vetoFlags[flag] = value;
      acc( *this ) = vetoFlags.to_ulong();
    }

    bool TauJet_v1::flag(
            TauJetParameters::VetoFlags flag ) const
    {
      static const Accessor< uint32_t > acc( "vetoFlags" );
      std::bitset<32> vetoFlags( acc( *this ) );
      return vetoFlags[flag];
    }



  //-------------------------------------------------------------------------
  // Get int detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v1::detail( TauJetParameters::Detail detail, int& value ) const {
    // Get the detail accessor:
    const Accessor< int >* acc = xAODTau::detailsAccessorV1<int>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v1::detail( TauJetParameters::Detail detail, float& value ) const {
    // Get the detail accessor:
    const Accessor< float >* acc = xAODTau::detailsAccessorV1<float>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v1::setDetail( TauJetParameters::Detail detail, int value ) {
    ( *( xAODTau::detailsAccessorV1<int>( detail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v1::setDetail( TauJetParameters::Detail detail, float value ) {
    ( *( xAODTau::detailsAccessorV1<float>( detail ) ) )( *this ) = value;
  }


  //-------------------------------------------------------------------------
  // Get int panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v1::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, int& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< int >* acc = xAODTau::panTauDetailsAccessorV1<int>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v1::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, float& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< float >* acc = xAODTau::panTauDetailsAccessorV1<float>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v1::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, int value ) {
    ( *( xAODTau::panTauDetailsAccessorV1<int>( panTauDetail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v1::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, float value ) {
    ( *( xAODTau::panTauDetailsAccessorV1<float>( panTauDetail ) ) )( *this ) = value;
  }






  // setters and getters for the track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::TrackParticleLinks_t,
  				     trackLinks,
  				     setTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v1::TrackParticleLinks_t > trackAcc( "trackLinks" );
  
  const TrackParticle* TauJet_v1::track( size_t i ) const {
   return ( *trackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nTracks() const {
   return trackAcc( *this ).size();
  }
  
  void TauJet_v1::addTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   trackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v1::clearTrackLinks() {
   trackAcc( *this ).clear();
   return;
  }


  int TauJet_v1::trackFilterPass(unsigned int numTrack) const
  {
    static const Accessor< std::vector <int > > trackPassAcc("trackFilterPass");
    const std::vector<int>& vec=trackPassAcc(*this);
    if (vec.size()<numTrack+1)
      return -999; 
    else
      return vec[numTrack];
  }
  
  void TauJet_v1::setTrackFilterPass(unsigned int numTrack, int pass)
  {
    static const Accessor< std::vector <int > > trackPassAcc("trackFilterPass");
    std::vector<int>& vec=trackPassAcc(*this);
    if (vec.size()<numTrack+1)
      vec.resize(numTrack+1); 
    vec[numTrack]=pass;
  }


  //setter and getter for trackfilterprongs
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v1, int, trackFilterProngs ,  setTrackFilterProngs )
  //setter and getter for trackfilterquality
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v1, int, trackFilterQuality ,  setTrackFilterQuality )
  //setter and getter for pi0ConeDR
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v1, float, pi0ConeDR ,  setPi0ConeDR )



  float TauJet_v1::trackEtaStrip(unsigned int numTrack) const
  {
    static const Accessor< std::vector <float > > trackEtaStripAcc("trackEtaStrip");
    const std::vector<float>& vec=trackEtaStripAcc(*this);
    if (vec.size()<numTrack+1)
      return -999.0; 
    else
      return vec[numTrack];
  }
  
  void TauJet_v1::setTrackEtaStrip(unsigned int numTrack, float eta)
  {
    static const Accessor< std::vector <float > > trackEtaStripAcc("trackEtaStrip");
    std::vector<float>& vec=trackEtaStripAcc(*this);
    if (vec.size()<numTrack+1)
      vec.resize(numTrack+1); 
    vec[numTrack]=eta;
  }


  float TauJet_v1::trackPhiStrip(unsigned int numTrack) const
  {
    static const Accessor< std::vector <float > > trackPhiStripAcc("trackPhiStrip");
    const std::vector<float>& vec=trackPhiStripAcc(*this);
    if (vec.size()<numTrack+1)
      return -999.0; 
    else
      return vec[numTrack];
  }
  
  void TauJet_v1::setTrackPhiStrip(unsigned int numTrack, float phi)
  {
    static const Accessor< std::vector <float > > trackPhiStripAcc("trackPhiStrip");
    std::vector<float>& vec=trackPhiStripAcc(*this);
    if (vec.size()<numTrack+1)
      vec.resize(numTrack+1); 
    vec[numTrack]=phi;
  }



  // setters and getters for the conversion track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::TrackParticleLinks_t,
  				     conversionTrackLinks,
  				     setConversionTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v1::TrackParticleLinks_t > convTrackAcc( "conversionTrackLinks" );
  
  const TrackParticle* TauJet_v1::conversionTrack( size_t i ) const {
   return ( *convTrackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nConversionTracks() const {
   return convTrackAcc( *this ).size();
  }
  
  void TauJet_v1::addConversionTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   convTrackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v1::clearConversionTrackLinks() {
   convTrackAcc( *this ).clear();
   return;
  }

  // setters and getters for the wide track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::TrackParticleLinks_t,
  				     wideTrackLinks,
  				     setWideTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v1::TrackParticleLinks_t > wideTrackAcc( "wideTrackLinks" );
  
  const TrackParticle* TauJet_v1::wideTrack( size_t i ) const {
   return ( *wideTrackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nWideTracks() const {
   return wideTrackAcc( *this ).size();
  }
  
  void TauJet_v1::addWideTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   wideTrackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v1::clearWideTrackLinks() {
   wideTrackAcc( *this ).clear();
   return;
  }

  // setters and getters for the other track links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::TrackParticleLinks_t,
  				     otherTrackLinks,
  				     setOtherTrackLinks )
  
  static const SG::AuxElement::Accessor< TauJet_v1::TrackParticleLinks_t > otherTrackAcc( "otherTrackLinks" );
  
  const TrackParticle* TauJet_v1::otherTrack( size_t i ) const {
   return ( *otherTrackAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nOtherTracks() const {
   return otherTrackAcc( *this ).size();
  }
  
  void TauJet_v1::addOtherTrackLink( const ElementLink< TrackParticleContainer >& tr) {
   
   otherTrackAcc( *this ).push_back( tr );
   return;
    }
  
  void TauJet_v1::clearOtherTrackLinks() {
   otherTrackAcc( *this ).clear();
   return;
  }

  ///setter and getter for the seed jet link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::JetLink_t,
  				     jetLink,
  				     setJetLink )

  static const SG::AuxElement::Accessor< TauJet_v1::JetLink_t > jetAcc( "jetLink" );

  const Jet* TauJet_v1::jet() const {
   return ( *jetAcc( *this ) );
  }


  void TauJet_v1::setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet) {
    if( jetAcc( *this ).isValid() )
      jetAcc( *this ).reset();
    jetAcc( *this ).toContainedElement( *cont, jet );
    }



  ///setter and getter for the vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::VertexLink_t,
  				     vertexLink,
  				     setVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v1::VertexLink_t > vertexAcc( "vertexLink" );

  const Vertex* TauJet_v1::vertex() const {
   return ( *vertexAcc( *this ) );
  }


  void TauJet_v1::setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( vertexAcc( *this ).isValid() )
      vertexAcc( *this ).reset();
    vertexAcc( *this ).toContainedElement( *cont, vertex );
    }



  ///setter and getter for the secondary vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::VertexLink_t,
  				     secondaryVertexLink,
  				     setSecondaryVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v1::VertexLink_t > secondaryVertexAcc( "secondaryVertexLink" );

  const Vertex* TauJet_v1::secondaryVertex() const {
   return ( *secondaryVertexAcc( *this ) );
  }


  void TauJet_v1::setSecondaryVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( secondaryVertexAcc( *this ).isValid() )
      secondaryVertexAcc( *this ).reset();
    secondaryVertexAcc( *this ).toContainedElement( *cont, vertex );
    }




  /////default pfos
  // setters and getters for the hadronic PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     hadronic_PFOLinks,
  				     setHadronic_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > hadronic_PFOAcc( "hadronic_PFOLinks" );
  
  const PFO* TauJet_v1::hadronic_PFO( size_t i ) const {
   return ( *hadronic_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nHadronic_PFOs() const {
   return hadronic_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addHadronic_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   hadronic_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearHadronic_PFOLinks() {
   hadronic_PFOAcc( *this ).clear();
   return;
  }

  // setters and getters for the shot PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     shot_PFOLinks,
  				     setShot_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > shot_PFOAcc( "shot_PFOLinks" );
  
  const PFO* TauJet_v1::shot_PFO( size_t i ) const {
   return ( *shot_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nShot_PFOs() const {
   return shot_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addShot_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   shot_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearShot_PFOLinks() {
   shot_PFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     charged_PFOLinks,
  				     setCharged_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > charged_PFOAcc( "charged_PFOLinks" );
  
  const PFO* TauJet_v1::charged_PFO( size_t i ) const {
   return ( *charged_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nCharged_PFOs() const {
   return charged_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addCharged_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   charged_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearCharged_PFOLinks() {
   charged_PFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     neutral_PFOLinks,
  				     setNeutral_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > neutral_PFOAcc( "neutral_PFOLinks" );
  
  const PFO* TauJet_v1::neutral_PFO( size_t i ) const {
   return ( *neutral_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nNeutral_PFOs() const {
   return neutral_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addNeutral_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   neutral_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearNeutral_PFOLinks() {
   neutral_PFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     pi0_PFOLinks,
  				     setPi0_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > pi0_PFOAcc( "pi0_PFOLinks" );
  
  const PFO* TauJet_v1::pi0_PFO( size_t i ) const {
   return ( *pi0_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nPi0_PFOs() const {
   return pi0_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addPi0_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   pi0_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearPi0_PFOLinks() {
   pi0_PFOAcc( *this ).clear();
   return;
  }





  /////cellbased pfos
  // setters and getters for the cellbased_charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     cellBased_Charged_PFOLinks,
  				     setCellBased_Charged_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > cellBased_Charged_PFOAcc( "cellBased_Charged_PFOLinks" );
  
  const PFO* TauJet_v1::cellBased_Charged_PFO( size_t i ) const {
   return ( *cellBased_Charged_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nCellBased_Charged_PFOs() const {
   return cellBased_Charged_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addCellBased_Charged_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   cellBased_Charged_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearCellBased_Charged_PFOLinks() {
   cellBased_Charged_PFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the cellbased_neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     cellBased_Neutral_PFOLinks,
  				     setCellBased_Neutral_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > cellBased_Neutral_PFOAcc( "cellBased_Neutral_PFOLinks" );
  
  const PFO* TauJet_v1::cellBased_Neutral_PFO( size_t i ) const {
   return ( *cellBased_Neutral_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nCellBased_Neutral_PFOs() const {
   return cellBased_Neutral_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addCellBased_Neutral_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   cellBased_Neutral_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearCellBased_Neutral_PFOLinks() {
   cellBased_Neutral_PFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the cellbased_pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     cellBased_Pi0_PFOLinks,
  				     setCellBased_Pi0_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > cellBased_Pi0_PFOAcc( "cellBased_Pi0_PFOLinks" );
  
  const PFO* TauJet_v1::cellBased_Pi0_PFO( size_t i ) const {
   return ( *cellBased_Pi0_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nCellBased_Pi0_PFOs() const {
   return cellBased_Pi0_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addCellBased_Pi0_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   cellBased_Pi0_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearCellBased_Pi0_PFOLinks() {
   cellBased_Pi0_PFOAcc( *this ).clear();
   return;
  }






  /////eflowRec pfos
  // setters and getters for the eflowRec_charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     eflowRec_Charged_PFOLinks,
  				     setEflowRec_Charged_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > eflowRec_Charged_PFOAcc( "eflowRec_Charged_PFOLinks" );
  
  const PFO* TauJet_v1::eflowRec_Charged_PFO( size_t i ) const {
   return ( *eflowRec_Charged_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nEflowRec_Charged_PFOs() const {
   return eflowRec_Charged_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addEflowRec_Charged_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   eflowRec_Charged_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearEflowRec_Charged_PFOLinks() {
   eflowRec_Charged_PFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the eflowRec_neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     eflowRec_Neutral_PFOLinks,
  				     setEflowRec_Neutral_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > eflowRec_Neutral_PFOAcc( "eflowRec_Neutral_PFOLinks" );
  
  const PFO* TauJet_v1::eflowRec_Neutral_PFO( size_t i ) const {
   return ( *eflowRec_Neutral_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nEflowRec_Neutral_PFOs() const {
   return eflowRec_Neutral_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addEflowRec_Neutral_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   eflowRec_Neutral_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearEflowRec_Neutral_PFOLinks() {
   eflowRec_Neutral_PFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the eflowRec_pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v1,
  				     TauJet_v1::PFOLinks_t,
  				     eflowRec_Pi0_PFOLinks,
  				     setEflowRec_Pi0_PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v1::PFOLinks_t > eflowRec_Pi0_PFOAcc( "eflowRec_Pi0_PFOLinks" );
  
  const PFO* TauJet_v1::eflowRec_Pi0_PFO( size_t i ) const {
   return ( *eflowRec_Pi0_PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v1::nEflowRec_Pi0_PFOs() const {
   return eflowRec_Pi0_PFOAcc( *this ).size();
  }
  
  void TauJet_v1::addEflowRec_Pi0_PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   eflowRec_Pi0_PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v1::clearEflowRec_Pi0_PFOLinks() {
   eflowRec_Pi0_PFOAcc( *this ).clear();
   return;
  }


  void TauJet_v1::dump() const {
	  /* keep these two lines to create the template methods. Otherwise they are optimzed away
	   * later: move these two lines into a dummy method, which nobody is calling
	   */
	  std::cout << this->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk) << std::endl;
	  std::cout << this->detail<int>(xAOD::TauJetParameters::numTopoClusters) << std::endl;
  }
  
} // namespace xAOD

//  LocalWords:  panTauDetail mJetSeed
