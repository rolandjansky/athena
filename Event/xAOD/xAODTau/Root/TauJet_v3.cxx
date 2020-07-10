/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v3.cxx 725228 2016-02-19 22:59:42Z griffith $


// System include(s):
#include <cstdint>
#include <bitset>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTau/versions/TauJet_v3.h"
#include "xAODTau/versions/TauJetCalibMapper_v1.h"
//#include "xAODTau/versions/TauJetCalibMapper_v3.h"
#include "TauJetAccessors_v3.h"


namespace{
  bool
  inRange(const double val, const double lo, const double hi){
    return (val>=lo) and (val<=hi);
  }

}

namespace xAOD {
  
  TauJet_v3::TauJet_v3()
    : IParticle() {
  }
  

  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, m)


  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiJetSeed)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mJetSeed)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiDetectorAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mDetectorAxis)

  //primitive setters and getters for IntermediateAxis 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiIntermediateAxis)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mIntermediateAxis)

  //primitive setters and getters for TauEnergyScale 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiTauEnergyScale)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mTauEnergyScale)
  // double TauJet_v3::etaTauEnergyScale() const { return etaIntermediateAxis(); }
  // double TauJet_v3::phiTauEnergyScale() const { return phiIntermediateAxis(); }
  // double TauJet_v3::mTauEnergyScale() const { return 0; }


  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiTauEtaCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mTauEtaCalib)
  // double TauJet_v3::phiTauEtaCalib() const { return phiIntermediateAxis(); }
  // double TauJet_v3::mTauEtaCalib() const { return 0; }

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiPanTauCellBasedProto)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mPanTauCellBasedProto)

  //primitive setters and getters for jetseed 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiPanTauCellBased)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mPanTauCellBased)

  //primitive setters and getters for HLT 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiTrigCaloOnly)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mTrigCaloOnly)

  //primitive setters and getters for HLT 4-vector
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, ptFinalCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, etaFinalCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, phiFinalCalib)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( TauJet_v3, float, double, mFinalCalib)


  double TauJet_v3::e() const {
    return genvecP4().E();
  }


  double TauJet_v3::rapidity() const {
    return genvecP4().Rapidity();
  }

  TauJet_v3::FourMom_t TauJet_v3::p4() const {
    FourMom_t p4{};
    bool validAnswer = inRange(eta(),-10,10);
    validAnswer&= inRange(phi(),-M_PI,+M_PI);
    if (validAnswer) p4.SetPtEtaPhiM( pt(), eta(), phi(),m()); 
    return p4;	
  }

  TauJet_v3::GenVecFourMom_t TauJet_v3::genvecP4() const {
    return GenVecFourMom_t(pt(), eta(), phi(), m());
  }

  TauJet_v3::FourMom_t TauJet_v3::p4(const TauJetParameters::TauCalibType calib) const {
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
    case TauJetParameters::PanTauCellBasedProto:
      p4.SetPtEtaPhiM( ptPanTauCellBasedProto(), etaPanTauCellBasedProto(), phiPanTauCellBasedProto(), mPanTauCellBasedProto());
      break;
    case TauJetParameters::PanTauCellBased:
      p4.SetPtEtaPhiM( ptPanTauCellBased(), etaPanTauCellBased(), phiPanTauCellBased(), mPanTauCellBased());
      break;
    case TauJetParameters::TrigCaloOnly:
      p4.SetPtEtaPhiM( ptTrigCaloOnly(), etaTrigCaloOnly(), phiTrigCaloOnly(), mTrigCaloOnly());
      break;
    case TauJetParameters::FinalCalib:
      p4.SetPtEtaPhiM( ptFinalCalib(), etaFinalCalib(), phiFinalCalib(), mFinalCalib());
      break;
    default:
      p4.SetPtEtaPhiM( pt(), eta(), phi(), m());
    }

    return p4;
  }

  TauJet_v3::GenVecFourMom_t TauJet_v3::genvecP4(const TauJetParameters::TauCalibType calib) const {
   
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
    case TauJetParameters::PanTauCellBasedProto:
      return GenVecFourMom_t( ptPanTauCellBasedProto(), etaPanTauCellBasedProto(), phiPanTauCellBasedProto(), mPanTauCellBasedProto());
    case TauJetParameters::PanTauCellBased:
      return GenVecFourMom_t( ptPanTauCellBased(), etaPanTauCellBased(), phiPanTauCellBased(), mPanTauCellBased());
    case TauJetParameters::TrigCaloOnly:
      return GenVecFourMom_t( ptTrigCaloOnly(), etaTrigCaloOnly(), phiTrigCaloOnly(), mTrigCaloOnly());
    case TauJetParameters::FinalCalib:
      return GenVecFourMom_t( ptFinalCalib(), etaFinalCalib(), phiFinalCalib(), mFinalCalib());
    default:
      return GenVecFourMom_t( pt(), eta(), phi(), m());
    }
  }

  
  void TauJet_v3::setP4(double pt, double eta, double phi, double m)  {
    static const Accessor< float > acc1( "pt" );
    static const Accessor< float > acc2( "eta" );
    static const Accessor< float > acc3( "phi" );
    static const Accessor< float > acc4( "m" );
    acc1( *this )=pt;
    acc2( *this )=eta;
    acc3( *this )=phi;
    acc4( *this )=m;
  }

  bool TauJet_v3::setP4(const TauJetParameters::TauCalibType calib, double pt, double eta, double phi, double m)  {
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

    // TauJetCalibTypeMapper_v3 typeMapper;
    // std::string calibStringPt, calibStringEta, calibStringPhi, calibStringM;
    // bool result = typeMapper.getValue(calib, calibStringPt, calibStringEta, calibStringPhi, calibStringM);
    // if (result) {
    //   if(calibStringPt.size()) {
    // 	Accessor< float > accTauPtCalib( calibStringPt );
    // 	accTauPtCalib( *this )=pt;
    //   }
    //   if(calibStringEta.size()) {
    // 	Accessor< float > accTauEtaCalib( calibStringEta );
    // 	accTauEtaCalib( *this )=eta;
    //   }
    //   if(calibStringPhi.size()) {
    // 	Accessor< float > accTauPhiCalib( calibStringPhi );
    // 	accTauPhiCalib( *this )=phi;
    //   }
    //   if(calibStringM.size()) {
    // 	Accessor< float > accTauMCalib( calibStringM );
    // 	accTauMCalib( *this )=m;
    //   }
    //   return true;
    // }
    // else {
    //   return false;
    // }

    
  }


   Type::ObjectType TauJet_v3::type() const {
      return Type::Tau;
   }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v3, unsigned int, ROIWord ,  setROIWord )		    
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v3, float, charge ,  setCharge )		    

  //-------------------------------------------------------------------------
  // Has discriminant
  //-------------------------------------------------------------------------
  bool TauJet_v3::hasDiscriminant( TauJetParameters::TauID discID ) const
  {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV3( discID );
    if( ! acc ) return false;
    return true;
  }


  //-------------------------------------------------------------------------
  // Get discriminant
  //-------------------------------------------------------------------------
  double TauJet_v3::discriminant( TauJetParameters::TauID discriminant ) const {
    // Get the discriminant accessor:
    const Accessor< float >* acc = xAODTau::discriminantAccessorV3( discriminant );
    if( ! acc ) return -1111.0;
    
    // Retrieve the discriminant value:
    return ( *acc )( *this );
  }
  
  //-------------------------------------------------------------------------
  // Set discriminant
  //-------------------------------------------------------------------------
  void TauJet_v3::setDiscriminant( TauJetParameters::TauID discriminant, double value ) {
    ( *( xAODTau::discriminantAccessorV3( discriminant ) ) )( *this ) = value;
    return;
  }
    
    //-------------------------------------------------------------------------
    // Accept
    //-------------------------------------------------------------------------
    bool TauJet_v3::isTau(
            TauJetParameters::IsTauFlag flag ) const
    {
      static const Accessor< uint32_t > acc( "isTauFlags" );
      std::bitset<32> isTauFlags( acc( *this ) );
      return isTauFlags[flag];
    }

    void TauJet_v3::setIsTau(
            TauJetParameters::IsTauFlag flag, bool value )
    {
      static const Accessor< uint32_t > acc( "isTauFlags" );
      std::bitset<32> isTauFlags( acc( *this ) );
      isTauFlags[flag] = value;
      acc( *this ) = isTauFlags.to_ulong();
    }

  //r21 cleanup
    // //-------------------------------------------------------------------------
    // // Veto flags accessors
    // //-------------------------------------------------------------------------
    // void TauJet_v3::setFlag(
    //         TauJetParameters::VetoFlags flag, bool value )
    // {
    //   static const Accessor< uint32_t > acc( "vetoFlags" );
    //   std::bitset<32> vetoFlags( acc( *this ) );
    //   vetoFlags[flag] = value;
    //   acc( *this ) = vetoFlags.to_ulong();
    // }

    // bool TauJet_v3::flag(
    //         TauJetParameters::VetoFlags flag ) const
    // {
    //   static const Accessor< uint32_t > acc( "vetoFlags" );
    //   std::bitset<32> vetoFlags( acc( *this ) );
    //   return vetoFlags[flag];
    // }



  //-------------------------------------------------------------------------
  // Get int detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v3::detail( TauJetParameters::Detail detail, int& value ) const {
    // Get the detail accessor:
    const Accessor< int >* acc = xAODTau::detailsAccessorV3<int>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v3::detail( TauJetParameters::Detail detail, float& value ) const {
    // Get the detail accessor:
    const Accessor< float >* acc = xAODTau::detailsAccessorV3<float>( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get IParticle detail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v3::detail( TauJetParameters::Detail detail, const IParticle* &value ) const {
    // Get the detail accessor:
    value=0;
    const Accessor< ElementLink<IParticleContainer> >* acc = xAODTau::detailsAccessorV3<ElementLink<IParticleContainer> >( detail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the detail value:
    ElementLink< IParticleContainer > link = ( *acc )( *this );
    if( ! link.isValid() ) return false;
    value = *link;
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v3::setDetail( TauJetParameters::Detail detail, int value ) {
    ( *( xAODTau::detailsAccessorV3<int>( detail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v3::setDetail( TauJetParameters::Detail detail, float value ) {
    ( *( xAODTau::detailsAccessorV3<float>( detail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set IParticle detail via enum
  //-------------------------------------------------------------------------
  void TauJet_v3::setDetail( TauJetParameters::Detail detail, const IParticle* value, const IParticleContainer* cont ) {
    if(value==0){
      ElementLink < IParticleContainer > lParticleLink;
      ( *( xAODTau::detailsAccessorV3<ElementLink < IParticleContainer > >( detail ) ) )( *this ) = lParticleLink;
    }    
    else{      
      if(!cont) cont=static_cast<const IParticleContainer*> (value->container());
      ElementLink < IParticleContainer > lParticleLink(value, *cont);
      ( *( xAODTau::detailsAccessorV3<ElementLink < IParticleContainer > >( detail ) ) )( *this ) = lParticleLink;
    }
  }


  //-------------------------------------------------------------------------
  // Get int panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v3::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, int& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< int >* acc = xAODTau::panTauDetailsAccessorV3<int>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }

  //-------------------------------------------------------------------------
  // Get float panTauDetail via enum
  //-------------------------------------------------------------------------
  bool TauJet_v3::panTauDetail( TauJetParameters::PanTauDetails panTauDetail, float& value ) const {
    // Get the panTauDetail accessor:
    const Accessor< float >* acc = xAODTau::panTauDetailsAccessorV3<float>( panTauDetail );
    if( ! acc ) return false;
    if( ! acc->isAvailable( *this ) ) return false;    

    // Retrieve the panTauDetail value:
    value = ( *acc )( *this );
    return true;
  }
  
  //-------------------------------------------------------------------------
  // Set int panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v3::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, int value ) {
    ( *( xAODTau::panTauDetailsAccessorV3<int>( panTauDetail ) ) )( *this ) = value;
  }

  //-------------------------------------------------------------------------
  // Set float panTauDetail via enum
  //-------------------------------------------------------------------------
  void TauJet_v3::setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, float value ) {
    ( *( xAODTau::panTauDetailsAccessorV3<float>( panTauDetail ) ) )( *this ) = value;
  }




  // implementing the functions by hand in order to maintain tauTrackLinks as
  // persistent name for EL<TauTracks>, but to actually set/retrieve all links using
  // function with name 'all'TauTrackLinks/set'All'TauTrackLinks
  // // setters and getters for the track links
  // AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  // 				     TauJet_v3::TauTrackLinks_t,
  // 				     tauTrackLinks,
  // 				     setTauTrackLinks )

  static const SG::AuxElement::Accessor< TauJet_v3::TauTrackLinks_t > tauTrackAcc( "tauTrackLinks" );

  const TauJet_v3::TauTrackLinks_t& TauJet_v3::allTauTrackLinks() const {
    return tauTrackAcc(*this);
  }

  void TauJet_v3::setAllTauTrackLinks(const TauJet_v3::TauTrackLinks_t& links) {
    tauTrackAcc(*this) = links;
    return;
  }

  const TauJet_v3::TauTrackLinks_t TauJet_v3::tauTrackLinks(TauJetParameters::TauTrackFlag flag/*=TauJetParameters::TauTrackFlag::classifiedCharged*/) const{
    TauTrack::TrackFlagType mask=1<<flag;
    return tauTrackLinksWithMask(mask);
  }

  const TauJet_v3::TauTrackLinks_t TauJet_v3::tauTrackLinksWithMask(unsigned int mask) const{
    TauJet_v3::TauTrackLinks_t links;
    for(const ElementLink< xAOD::TauTrackContainer >& link : tauTrackAcc(*this) ){
      if( (*link)->flagWithMask(mask))
	links.push_back(link);
    }
    return links;
  }


  TauJet_v3::TauTrackLinks_t& TauJet_v3::allTauTrackLinksNonConst(){
    return tauTrackAcc(*this);
  }

  const TauTrack* TauJet_v3::track( size_t i, TauJetParameters::TauTrackFlag flag/*=TauJetParameters::TauTrackFlag::classifiedCharged*/ ,int* container_index) const {
    TauTrack::TrackFlagType mask=1<<flag;
    return trackWithMask(i, mask, container_index);
  }

  const TauTrack* TauJet_v3::trackWithMask( size_t i, unsigned int mask, int* container_index ) const {
    uint tracks_pass_mask=0;

    for(const ElementLink< xAOD::TauTrackContainer >& link : tauTrackAcc(*this) ){
      const TauTrack* trk = *link;
      if(trk->flagWithMask(mask)){
	if(tracks_pass_mask==i) {
	  if(container_index) *container_index=link.index();
	  return trk;
	}
	else tracks_pass_mask++;
      }
    }
    return 0;

  }


#ifndef XAOD_STANDALONE
  TauTrack* TauJet_v3::trackNonConst( size_t i, TauJetParameters::TauTrackFlag flag/*=TauJetParameters::TauTrackFlag::classifiedCharged*/ ) {
    int container_index=-1;
    const TauTrack* c_trk=track(i,flag, &container_index);
    if(c_trk==0 || container_index<0) return 0;
    ElementLink< xAOD::TauTrackContainer > link = tauTrackAcc(*this).at(0);//we don't care about this specific link, just the container
    xAOD::TauTrackContainer* tauTrackContainer = link.getDataNonConstPtr();
    TauTrack* trk=tauTrackContainer->at(container_index);
    if(trk!=c_trk) std::cout << "Did not properly retrieve non-const tauTrack" << std::endl;
    return trk;
    // for( xAOD::TauTrack* trk : *tauTrackContainer){
    //   if(trk==c_trk) return trk;
    // }
    //return 0;
  }
#endif

  /// Get the v<const pointer> to a given tauTrack collection associated with this tau
  std::vector<const TauTrack*> TauJet_v3::tracks(TauJetParameters::TauTrackFlag flag/*=TauJetParameters::TauTrackFlag::classifiedCharged*/ ) const{
    TauTrack::TrackFlagType mask=1<<flag;
    return tracksWithMask( mask );
  }

  /// Get the v<const pointer> to a given tauTrack collection associated with this tau
  std::vector<const TauTrack*> TauJet_v3::tracksWithMask(unsigned int mask )const {
  std::vector<const TauTrack*> trks;
    for(const ElementLink< xAOD::TauTrackContainer >& link : tauTrackAcc(*this) ){
      const TauTrack* trk = *link;
      if(trk->flagWithMask(mask)){
	trks.push_back(trk);
      }
    }
    return trks;
  }


  /// Get the v<const pointer> to all tracks associated with this tau, regardless of classification
  std::vector<const TauTrack*> TauJet_v3::allTracks() const{
    // std::vector<const TauTrack*> trks;
    // for(const ElementLink< xAOD::TauTrackContainer > link : tauTrackAcc(*this) ){
    //   const TauTrack* trk = *link;
    //   trks.push_back(trk);
    // }
    // return trks;
    TauTrack::TrackFlagType mask=0;
    return tracksWithMask( mask );
  }

#ifndef XAOD_STANDALONE
  /// Get the v<pointer> to all tracks associated with this tau, regardless of classification
  std::vector<TauTrack*> TauJet_v3::allTracks() {
    std::vector<TauTrack*> trks;
    for(ElementLink< xAOD::TauTrackContainer >& link : tauTrackAcc(*this) ){
      const TauTrack* c_trk = *link;
      xAOD::TauTrackContainer* tauTrackContainer = link.getDataNonConstPtr();
      TauTrack* trk=tauTrackContainer->at(link.index());
      if(trk!=c_trk) std::cout << "Did not properly retrieve non-const tauTrack" << std::endl;
      trks.push_back(trk);
    }
    return trks;
  }
#endif

  //number of tracks with a given classification
  size_t TauJet_v3::nTracks(TauJetParameters::TauTrackFlag flag/*=TauJetParameters::TauTrackFlag::classifiedCharged*/) const{
    TauTrack::TrackFlagType mask=1<<flag;
    return nTracksWithMask(mask);
  }


  size_t TauJet_v3::nTracksCharged() const{
    return nTracks(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged);
  }

  size_t TauJet_v3::nTracksIsolation() const{
    return nTracks(xAOD::TauJetParameters::TauTrackFlag::classifiedIsolation);
  }


  //number of tracks with a given classification
  size_t TauJet_v3::nTracksWithMask(unsigned int flags) const{
    size_t n(0);
    for(const ElementLink< xAOD::TauTrackContainer >& link : tauTrackAcc(*this) ){
      const TauTrack* trk = *link;
      if(trk->flagWithMask(flags)) n++;
    }    
    return n;
  }

  //all tracks regardless of classification or lack thereof
  size_t TauJet_v3::nAllTracks() const{
    return tauTrackAcc( *this ).size();
  }

  /// add a TauTrack to the tau
  void TauJet_v3::addTauTrackLink( const ElementLink< TauTrackContainer >& tr){
    tauTrackAcc( *this ).push_back(tr);
  }
  /// Remove all tracks from the tau
  void TauJet_v3::clearTauTrackLinks(){
    tauTrackAcc( *this ).clear();
  }
  
  // setters and getters for the cluster links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::IParticleLinks_t,
  				     clusterLinks,
  				     setClusterLinks )

  static const SG::AuxElement::Accessor< TauJet_v3::IParticleLinks_t > clusterAcc( "clusterLinks" );


  const IParticle* TauJet_v3::cluster( size_t i) const {
    return *(clusterAcc(*this).at(i));
  }

  TauJet_v3::FourMom_t TauJet_v3::calibratedCluster( size_t i, xAOD::CaloCluster::State state/*=xAOD::CaloCluster::State::CALIBRATED*/) const{
    const xAOD::IParticle* part = this->cluster(i);
    if(!part) return FourMom_t();
    const xAOD::CaloCluster* clus=dynamic_cast<const xAOD::CaloCluster*> (part);
    if(!clus) return FourMom_t();
    return clus->p4(state);
  }

  //number of clusters
  size_t TauJet_v3::nClusters() const{
    return clusterAcc( *this ).size();
  }

  /// add a cluster to the tau
  void TauJet_v3::addClusterLink( const ElementLink< IParticleContainer >& tr){
    clusterAcc( *this ).push_back(tr);
  }

  /// Remove all clusters from the tau
  void TauJet_v3::clearClusterLinks(){
    clusterAcc( *this ).clear();
  }


  // setters and getters for the pi0 links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::IParticleLinks_t,
  				     pi0Links,
  				     setPi0Links )

  static const SG::AuxElement::Accessor< TauJet_v3::IParticleLinks_t > pi0Acc( "pi0Links" );


  const IParticle* TauJet_v3::pi0( size_t i) const {
    return *(pi0Acc(*this).at(i));
  }

  //number of pi0s
  size_t TauJet_v3::nPi0s() const{
    return pi0Acc( *this ).size();
  }

  /// add a pi0 to the tau
  void TauJet_v3::addPi0Link( const ElementLink< IParticleContainer >& tr){
    pi0Acc( *this ).push_back(tr);
  }

  /// Remove all pi0s from the tau
  void TauJet_v3::clearPi0Links(){
    pi0Acc( *this ).clear();
  }



  //setter and getter for trackfilterprongs
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v3, int, trackFilterProngs ,  setTrackFilterProngs )
  //setter and getter for trackfilterquality
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v3, int, trackFilterQuality ,  setTrackFilterQuality )
  //setter and getter for pi0ConeDR
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TauJet_v3, float, pi0ConeDR ,  setPi0ConeDR )

  ///setter and getter for the seed jet link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::JetLink_t,
  				     jetLink,
  				     setJetLink )

  static const SG::AuxElement::Accessor< TauJet_v3::JetLink_t > jetAcc( "jetLink" );

  const Jet* TauJet_v3::jet() const {
   return ( *jetAcc( *this ) );
  }


  void TauJet_v3::setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet) {
    if( jetAcc( *this ).isValid() )
      jetAcc( *this ).reset();
    jetAcc( *this ).toContainedElement( *cont, jet );
    }



  ///setter and getter for the vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::VertexLink_t,
  				     vertexLink,
  				     setVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v3::VertexLink_t > vertexAcc( "vertexLink" );

  const Vertex* TauJet_v3::vertex() const {
   return ( *vertexAcc( *this ) );
  }


  void TauJet_v3::setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( vertexAcc( *this ).isValid() )
      vertexAcc( *this ).reset();
    vertexAcc( *this ).toContainedElement( *cont, vertex );
    }



  ///setter and getter for the secondary vertex link
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::VertexLink_t,
  				     secondaryVertexLink,
  				     setSecondaryVertexLink )

  static const SG::AuxElement::Accessor< TauJet_v3::VertexLink_t > secondaryVertexAcc( "secondaryVertexLink" );

  const Vertex* TauJet_v3::secondaryVertex() const {
   return ( *secondaryVertexAcc( *this ) );
  }


  void TauJet_v3::setSecondaryVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex) {
    if( secondaryVertexAcc( *this ).isValid() )
      secondaryVertexAcc( *this ).reset();
    secondaryVertexAcc( *this ).toContainedElement( *cont, vertex );
    }




  /////default pfos
  // setters and getters for the hadronic PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     hadronicPFOLinks,
  				     setHadronicPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > hadronicPFOAcc( "hadronicPFOLinks" );
  
  const PFO* TauJet_v3::hadronicPFO( size_t i ) const {
   return ( *hadronicPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nHadronicPFOs() const {
   return hadronicPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addHadronicPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   hadronicPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearHadronicPFOLinks() {
   hadronicPFOAcc( *this ).clear();
   return;
  }

  // setters and getters for the shot PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     shotPFOLinks,
  				     setShotPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > shotPFOAcc( "shotPFOLinks" );
  
  const PFO* TauJet_v3::shotPFO( size_t i ) const {
   return ( *shotPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nShotPFOs() const {
   return shotPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addShotPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   shotPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearShotPFOLinks() {
   shotPFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     chargedPFOLinks,
  				     setChargedPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > chargedPFOAcc( "chargedPFOLinks" );
  
  const PFO* TauJet_v3::chargedPFO( size_t i ) const {
   return ( *chargedPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nChargedPFOs() const {
   return chargedPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addChargedPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   chargedPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearChargedPFOLinks() {
   chargedPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     neutralPFOLinks,
  				     setNeutralPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > neutralPFOAcc( "neutralPFOLinks" );
  
  const PFO* TauJet_v3::neutralPFO( size_t i ) const {
   return ( *neutralPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nNeutralPFOs() const {
   return neutralPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addNeutralPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   neutralPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearNeutralPFOLinks() {
   neutralPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     pi0PFOLinks,
  				     setPi0PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > pi0PFOAcc( "pi0PFOLinks" );
  
  const PFO* TauJet_v3::pi0PFO( size_t i ) const {
   return ( *pi0PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nPi0PFOs() const {
   return pi0PFOAcc( *this ).size();
  }
  
  void TauJet_v3::addPi0PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   pi0PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearPi0PFOLinks() {
   pi0PFOAcc( *this ).clear();
   return;
  }





  /////cellbased pfos
  // setters and getters for the cellbased_charged PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     protoChargedPFOLinks,
  				     setProtoChargedPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > protoChargedPFOAcc( "protoChargedPFOLinks" );
  
  const PFO* TauJet_v3::protoChargedPFO( size_t i ) const {
   return ( *protoChargedPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nProtoChargedPFOs() const {
   return protoChargedPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addProtoChargedPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoChargedPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearProtoChargedPFOLinks() {
   protoChargedPFOAcc( *this ).clear();
   return;
  }


  // setters and getters for the cellbased_neutral PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     protoNeutralPFOLinks,
  				     setProtoNeutralPFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > protoNeutralPFOAcc( "protoNeutralPFOLinks" );
  
  const PFO* TauJet_v3::protoNeutralPFO( size_t i ) const {
   return ( *protoNeutralPFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nProtoNeutralPFOs() const {
   return protoNeutralPFOAcc( *this ).size();
  }
  
  void TauJet_v3::addProtoNeutralPFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoNeutralPFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearProtoNeutralPFOLinks() {
   protoNeutralPFOAcc( *this ).clear();
   return;
  }



  // setters and getters for the cellbased_pi0 PFO links
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TauJet_v3,
  				     TauJet_v3::PFOLinks_t,
  				     protoPi0PFOLinks,
  				     setProtoPi0PFOLinks )
  


  static const SG::AuxElement::Accessor< TauJet_v3::PFOLinks_t > protoPi0PFOAcc( "protoPi0PFOLinks" );
  
  const PFO* TauJet_v3::protoPi0PFO( size_t i ) const {
   return ( *protoPi0PFOAcc( *this )[ i ] );
  }
  
  size_t TauJet_v3::nProtoPi0PFOs() const {
   return protoPi0PFOAcc( *this ).size();
  }
  
  void TauJet_v3::addProtoPi0PFOLink( const ElementLink< PFOContainer >& pfo) {
   
   protoPi0PFOAcc( *this ).push_back( pfo );
   return;
    }
  
  void TauJet_v3::clearProtoPi0PFOLinks() {
   protoPi0PFOAcc( *this ).clear();
   return;
  }




  void TauJet_v3::dump() {
	  /* keep these two lines to create the template methods. Otherwise they are optimzed away
	   * later: move these two lines into a dummy method, which nobody is calling
	   */
	  std::cout << this->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk) << std::endl;
	  std::cout << this->detail<int>(xAOD::TauJetParameters::numTopoClusters) << std::endl;
  }
  
} // namespace xAOD

//  LocalWords:  panTauDetail mJetSeed
