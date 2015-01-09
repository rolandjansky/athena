/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.cxx 299883 2014-03-28 17:34:16Z krasznaa $

// Local include(s):
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace CP {
  
  MuonSelectionTool::MuonSelectionTool( const std::string& name )
    : asg::AsgTool( name ),
      m_name(name),
      m_accept( "MuonSelection" ){
    
    declareProperty( "MaxEta", m_maxEta = 2.5 );
    //xAOD::MuonQuality enum {Tight, Medium, Loose, VeryLoose}
    //corresponds to 0, 1, 2, 3 --> 1 - medium is default
    declareProperty( "MuQuality", m_quality = 1 );

    ATH_MSG_DEBUG("Creating MuonSelectionTool named "<<m_name);
  }
  
  
  MuonSelectionTool::MuonSelectionTool( const MuonSelectionTool& toCopy  )
    : asg::AsgTool(toCopy.m_name+"_copy"),
      m_name(toCopy.m_name+"_copy"),
      m_accept( toCopy.m_accept ){
    
    declareProperty( "MaxEta", m_maxEta = 2.5 );
    //xAOD::MuonQuality enum {Tight, Medium, Loose, VeryLoose}
    //corresponds to 0, 1, 2, 3 --> 1 - medium is default
    declareProperty( "MuQuality", m_quality = 1 );

    ATH_MSG_DEBUG("Creating copy of MuonSelectionTool named "<<m_name);
  }
  
  MuonSelectionTool::~MuonSelectionTool(){
    ATH_MSG_DEBUG(Form("Deleting MuonSelectionTool named %s",m_name.c_str()));
  }
  
  StatusCode MuonSelectionTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initialising..." );
    ATH_MSG_INFO( "Maximum eta: " << m_maxEta );
    ATH_MSG_INFO( "Muon quality: " << m_quality );
    
    // Set up the TAccept object:
    m_accept.addCut( "Eta",
		     "Selection of muons according to their pseudorapidity" );
    m_accept.addCut( "Quality",
		     "Selection of muons according to their tightness" );
    m_accept.addCut( "IDHits",
		     "Selection of muons according to whether they passed the MCP ID Hit cuts" );
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  const Root::TAccept& MuonSelectionTool::getTAccept() const {
    
    return m_accept;
  }
  
  const Root::TAccept&
  MuonSelectionTool::accept( const xAOD::IParticle* p ) const {
    
    // Reset the result:
    m_accept.clear();
    
    // Check if this is a muon:
    if( p->type() != xAOD::Type::Muon ) {
      ATH_MSG_ERROR( "accept(...) Function received a non-muon" );
      return m_accept;
    }
    
    // Cast it to a muon:
    const xAOD::Muon* mu = dynamic_cast< const xAOD::Muon* >( p );
    if( ! mu ) {
      ATH_MSG_FATAL( "accept(...) Failed to cast particle to muon" );
      return m_accept;
    }
    
    // Let the specific function do the work:
    return accept( *mu );
  }
  
  const Root::TAccept& MuonSelectionTool::accept( const xAOD::Muon& mu ) const {
    
    // Reset the result:
    m_accept.clear();
    
    // Do the eta cut:
    ATH_MSG_VERBOSE( "Muon eta: " << mu.eta() );
    if( std::abs( mu.eta() ) > m_maxEta ) {
      return m_accept;
    }
    m_accept.setCutResult( "Eta", true );
    
    // Medium quality doesn't exist yet, hack for now with getQuality method
    // Do the quality cut(s):
    // ATH_MSG_VERBOSE( "Muon quality: " << mu.quality() );
    // if( mu.quality()!=xAOD::Muon::Medium ) {
    //    return m_accept;
    // }
    
    ATH_MSG_VERBOSE( "Muon quality: " << getQuality(mu));
    if(getQuality(mu) > m_quality){
      return m_accept;
    }
    
    m_accept.setCutResult( "Quality", true );
    
    // Passes ID hit cuts
    // ATH_MSG_VERBOSE( "Passes ID Hit cuts " << mu.passesIDCuts () );
    // if( !mu.passesIDCuts () ) {
    //   return m_accept;
    // }
    
    ATH_MSG_VERBOSE( "Passes ID Hit cuts " << passedIDCuts(mu) );
    if( !passedIDCuts (mu) ) {
      return m_accept;
    }
    
    m_accept.setCutResult( "IDHits", true );
    
    // Return the result:
    return m_accept;
  }
  
  void MuonSelectionTool::setQuality( xAOD::Muon& mu ) const {
    mu.setQuality(getQuality(mu)); 
    return;
  }
  
  xAOD::Muon::Quality MuonSelectionTool::getQuality(const xAOD::Muon& mu ) const {
    using namespace xAOD;
    
    // Combined muons
    if (mu.muonType()==Muon_v1::Combined){
      
      uint8_t nprecisionLayers,nprecisionHoleLayers;
      
      if (!mu.primaryTrackParticle()->summaryValue(nprecisionLayers, numberOfPrecisionLayers) || 
	  !mu.primaryTrackParticle()->summaryValue(nprecisionHoleLayers, numberOfPrecisionHoleLayers)){
	ATH_MSG_VERBOSE("getQuality - #precision layers missing in combined muon! Aborting.");
	return Muon_v1::VeryLoose;
      }
      
      float momBalanceSig=0.0;
      
      if (!mu.parameter(momBalanceSig, Muon_v1::momentumBalanceSignificance)){
	ATH_MSG_VERBOSE("getQuality - momentumBalanceSignificance missing in combined muon! Don't abort yet since these variables are not filled!");
       	//momentum balance signficance variable is missing, proceed with a check of the precision layers
       	return Muon_v1::VeryLoose;
      }
      
      if( (momBalanceSig<4.0)
	  && (nprecisionLayers > 1 || ( nprecisionLayers == 1 && nprecisionHoleLayers < 2 ) ) ) {
	return Muon_v1::Medium;
      }
      
      //didn't pass the set of requirements for a medium combined muon
      return Muon_v1::VeryLoose;
    }
    
    if (mu.author()==Muon_v1::STACO){
      return Muon_v1::Medium;
    }
    
    // SA muons
    if (mu.muonType()==Muon_v1::MuonStandAlone){
      
      if(fabs(mu.eta())>2.5){
	uint8_t nprecisionLayers;
	uint8_t ninnerSmallHits;
	if (!mu.primaryTrackParticle()->summaryValue(nprecisionLayers, numberOfPrecisionLayers)){
	  ATH_MSG_VERBOSE("getQuality - numberOfPrecisionLayers missing in SA muon! Aborting.");
	  return Muon_v1::VeryLoose;
	}
	if (!mu.summaryValue(ninnerSmallHits, innerSmallHits)){
	  ATH_MSG_VERBOSE("getQuality - innerSmallHits missing in SA muon! Aborting.");
	  return Muon_v1::VeryLoose;
	}
	
	if( nprecisionLayers>2 || ninnerSmallHits>3 ) {
	  return Muon_v1::Medium; 
	}
	return Muon_v1::VeryLoose;
      }
      return Muon_v1::VeryLoose;
    }
    
    // SegmentTagged muons
    if (mu.muonType()==Muon_v1::SegmentTagged){
      return Muon_v1::Loose;
    }
    
    // CaloTagged muons
    if (mu.muonType()==Muon_v1::CaloTagged ){
      if(fabs(mu.eta())<0.1 && passedCaloTagQuality(mu))
	return Muon_v1::Loose;
    }
    
    return Muon_v1::VeryLoose;
  }
  
  void MuonSelectionTool::setPassesIDCuts( xAOD::Muon& mu ) const {    
    mu.setPassesIDCuts(passedIDCuts(mu));
  }
  
  void MuonSelectionTool::setPassesHighPtCuts( xAOD::Muon& mu ) const {    
    mu.setPassesHighPtCuts(passedHighPtCuts(mu));
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::Muon& mu ) const {
    return passedIDCuts(*mu.primaryTrackParticle());
  }
  
  
  //Placeholder for now
  bool MuonSelectionTool::passedHighPtCuts( const xAOD::Muon& mu ) const {
    using namespace xAOD;
    uint8_t nprecisionLayers;
    //uint8_t ninnerSmallHits;
    if (!mu.primaryTrackParticle()->summaryValue(nprecisionLayers, numberOfPrecisionLayers)){
      ATH_MSG_VERBOSE("passedHighPtCuts - numberOfPrecisionLayers missing in muon! Aborting.");
      return false;
    }
    if (nprecisionLayers<3) return false;
    
    return true;
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::TrackParticle & track ) const {
    using namespace xAOD;
    uint8_t value1=0;
    uint8_t value2=0;
    
    if (track.summaryValue(value1, numberOfPixelHits) &&
	track.summaryValue(value2, numberOfPixelDeadSensors) ) {
      if (value1+value2==0) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - pixel values are missing! Aborting.");
      return false;
    }
    
    if (track.summaryValue(value1, numberOfSCTHits) &&
	track.summaryValue(value2, numberOfSCTDeadSensors)){
      if (value1+value2<=4) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - SCT values are missing! Aborting.");
      return false;
    }
    
    if (track.summaryValue(value1, numberOfPixelHoles) &&
	track.summaryValue(value2, numberOfSCTHoles)){
      if (value1+value2>=3) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - Si hole values are missing! Aborting.");
      return false;
    }
    
    float abseta = std::abs(track.eta());
    if (track.summaryValue(value1, numberOfTRTHits) &&
	track.summaryValue(value2, numberOfTRTOutliers)){
      // Okay, so have values available - now check if the TRT cuts are met...
      uint8_t totTRThits=value1+value2;
      if (!((0.1<abseta && abseta<=1.9 && totTRThits>5 && value2<(0.9 * totTRThits)) || (abseta <= 0.1 || abseta > 1.9)))
	return false;
    } else {
      
      ATH_MSG_WARNING("isPassedIDCuts - TRT values are missing! Aborting.");
      return false;
    }
    // Reached end - all ID hit cuts are passed.
    return true;
  } // passedIDCuts
  
  
  bool MuonSelectionTool::passedCaloTagQuality (const xAOD::Muon& mu) const{
    //initialize with values which would enable the muons to pass the requirement
    //this will be filled correctly if the variables exist in the xAODS
    float CaloLRLikelihood = 1.0;
    int CaloMuonIDTag = 20;

    bool readLR = mu.parameter(CaloLRLikelihood, xAOD::Muon::CaloLRLikelihood);
    bool readID = mu.parameter(CaloMuonIDTag, xAOD::Muon::CaloMuonIDTag);

    if (!readLR || !readID){
      ATH_MSG_VERBOSE("Unable to read CaloTag Quality information! Don't do anything for now, accept the calotag muons as they are!");
      //just a hack for now -- this information is not available in the DC14 xAODs
      //return false;
    }
    return (CaloLRLikelihood > 0.9 || CaloMuonIDTag > 10);
  }
  
} // namespace CP
