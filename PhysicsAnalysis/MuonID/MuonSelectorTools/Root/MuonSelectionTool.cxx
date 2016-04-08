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
    
    declareProperty( "MaxEta", m_maxEta = 2.7 );
    //xAOD::MuonQuality enum {Tight, Medium, Loose, VeryLoose}
    //corresponds to 0, 1, 2, 3, 4=HighPt
    declareProperty( "MuQuality", m_quality = 1 );
    declareProperty( "ToroidOff", m_toroidOff = false );

    ATH_MSG_DEBUG("Creating MuonSelectionTool named "<<m_name);
  }
  
  
  MuonSelectionTool::MuonSelectionTool( const MuonSelectionTool& toCopy  )
    : asg::AsgTool(toCopy.m_name+"_copy"),
      m_name(toCopy.m_name+"_copy"),
      m_maxEta( toCopy.m_maxEta ),
      m_quality( toCopy.m_quality ),
      m_accept( toCopy.m_accept ){
    
    declareProperty( "MaxEta", m_maxEta = 2.7 );
    //xAOD::MuonQuality enum {Tight, Medium, Loose, VeryLoose}
    //corresponds to 0, 1, 2, 3 , 4=HighPt
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
    if ( m_toroidOff ) ATH_MSG_INFO( "!! CONFIGURED FOR TOROID-OFF COLLISIONS !!" );
    
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
    
    ATH_MSG_VERBOSE( "Muon quality: " << getQuality(mu) << " isHighPt: "<<passedHighPtCuts(mu) );

    if(m_quality<4 && getQuality(mu) > m_quality){
      return m_accept;
    }
    if(m_quality>=4 && !passedHighPtCuts(mu)){
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
    if (mu.muonType()==xAOD::Muon::Combined){
      
      uint8_t nprecisionLayers,nprecisionHoleLayers;      
      if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) || 
	  !mu.summaryValue(nprecisionHoleLayers, xAOD::SummaryType::numberOfPrecisionHoleLayers)){
	ATH_MSG_VERBOSE("getQuality - #precision layers missing in combined muon! Aborting.");
	return xAOD::Muon::VeryLoose;
      }

      // TIGHT WP
      const xAOD::TrackParticle* idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      const xAOD::TrackParticle* metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
      if(idtrack && metrack) {
	
	float qOverPdiff   = fabs(idtrack->qOverP()) - fabs(metrack->qOverP()); 
	float qOverPsigma  = sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) );
	float qOverPsignif = qOverPdiff / qOverPsigma;
	if( fabs(qOverPsignif)  < 5 
	    && nprecisionLayers >= 2 ) {
	  float reducedChi2 = mu.primaryTrackParticle()->chiSquared()/mu.primaryTrackParticle()->numberDoF();
	  if( reducedChi2 < 8 && !m_toroidOff ) {
	    return xAOD::Muon::Tight;
	  }
	}

	// MEDIUM WP
	if( (fabs(qOverPsignif) < 7 || m_toroidOff)
	    && (nprecisionLayers > 1 || ( nprecisionLayers == 1 && nprecisionHoleLayers < 2 && fabs(mu.eta())<0.1) ) ) {
	  return xAOD::Muon::Medium;
	}

      } else {
	// CB muons with missing ID or ME track
	if( (nprecisionLayers > 1 || ( nprecisionLayers == 1 && nprecisionHoleLayers < 2 && fabs(mu.eta())<0.1) ) )
	  // In toroid-off data ME/MS tracks often missing - need special treatment  => flagging as "Medium"
	  // In toroid-on data ME/MS tracks missing only for <1% of CB muons, mostly MuGirl (to be fixed) => flagging as "Loose"
	  return (m_toroidOff ? xAOD::Muon::Medium : xAOD::Muon::Loose);
      }

      //didn't pass the set of requirements for a medium or tight combined muon
      return xAOD::Muon::VeryLoose;
    }
    
    if (mu.author()==xAOD::Muon::STACO){
      return xAOD::Muon::Medium;
    }
    
    // SA muons
    if (mu.author()==xAOD::Muon::MuidSA){

      if(fabs(mu.eta())>2.5){

	uint8_t nprecisionLayers;
	if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers)){
	  ATH_MSG_VERBOSE("getQuality - numberOfPrecisionLayers missing in SA muon! Aborting.");
	  return xAOD::Muon::VeryLoose;
	}
	uint8_t ninnerSmallHits;
	if (!mu.summaryValue(ninnerSmallHits, xAOD::MuonSummaryType::innerSmallHits)){
	  ATH_MSG_VERBOSE("getQuality - innerSmallHits missing in SA muon! Aborting.");
	  //return xAOD::Muon::VeryLoose;
	}
	uint8_t ninnerLargeHits;
	if (!mu.summaryValue(ninnerLargeHits, xAOD::MuonSummaryType::innerLargeHits)){
	  ATH_MSG_VERBOSE("getQuality - innerLargeHits missing in SA muon! Aborting.");
	  //return xAOD::Muon::VeryLoose;
	}

	// 3 station requirement for medium
	if( nprecisionLayers>2 && !m_toroidOff )  return xAOD::Muon::Medium;
		
	return xAOD::Muon::VeryLoose;
      }
      return xAOD::Muon::VeryLoose;
    }
    
    // SegmentTagged muons
    if (mu.muonType()==xAOD::Muon::SegmentTagged){
      if(fabs(mu.eta())<0.1)
    	return xAOD::Muon::Loose;
      else
    	return xAOD::Muon::VeryLoose;
    }
    
    // CaloTagged muons
    if (mu.muonType()==xAOD::Muon::CaloTagged ){
      if(fabs(mu.eta())<0.1 && passedCaloTagQuality(mu))
	return xAOD::Muon::Loose;
    }
    
    return xAOD::Muon::VeryLoose;
  }
  
  void MuonSelectionTool::setPassesIDCuts( xAOD::Muon& mu ) const {    
    mu.setPassesIDCuts(passedIDCuts(mu));
  }
  
  void MuonSelectionTool::setPassesHighPtCuts( xAOD::Muon& mu ) const {    
    mu.setPassesHighPtCuts(passedHighPtCuts(mu));
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::Muon& mu ) const {
    using namespace xAOD;
    //do not apply the ID hit requirements for SA muons for |eta| > 2.5
    if (mu.author()==xAOD::Muon::MuidSA && fabs(mu.eta())>2.5)
      return true;
    else{
      if(mu.primaryTrackParticle()) 
	return passedIDCuts(*mu.primaryTrackParticle());
      else
	return false;
    }
  }
  
  bool MuonSelectionTool::passedHighPtCuts( const xAOD::Muon& mu ) const {
    using namespace xAOD;
    
    //::: Request combined muons
    if( mu.muonType() != xAOD::Muon::Combined ) return false;

    //::: Require 3 station muons
    uint8_t nprecisionLayers;
    if( !mu.summaryValue( nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers ) ){
      ATH_MSG_VERBOSE( "passedHighPtCuts - numberOfPrecisionLayers missing in muon! Aborting." );
      return false;
    }
    if( nprecisionLayers < 3 ) return false;
    
    //::: Apply MS Chamber Vetoes
    // Given according to their eta-phi locations in the muon spectrometer
    // FORM: CHAMBERNAME[ array of four values ] = { eta 1, eta 2, phi 1, phi 2}
    // The vetoes are applied based on the MS track if available. If the MS track is not available, 
    // the vetoes are applied according to the combined track, and runtime warning is printed to
    // the command line.
		const xAOD::TrackParticle* MS_track = NULL;
		if( mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "muonSpectrometerTrackParticleLink" ) && ( mu.muonSpectrometerTrackParticleLink() ).isValid() ) MS_track = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );    
		else{
			ATH_MSG_WARNING( "passedHighPtCuts - No MS track available for muon. Using combined track." );
			MS_track = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
		}

		if( MS_track != NULL ) {
      float etaMS = MS_track->eta();
      float phiMS = MS_track->phi();
      
      float BOE[ 4 ] = { 0.528, 0.618, -1.42, -1.72 }; 
      float BME[ 4 ] = { 0.644, 0.772, -1.42, -1.72 };
      float EE[ 2 ] = { 1.0, 1.33 }; // A-side only; eta bounds given; reject all phi in eta band
      float BEE_eta[ 2 ] = { 1.4, 1.7 };
      float BEE_phi[ 8 ] = { 0.3, 0.5, 1.08, 1.27, 1.85, 2.05, 2.65, 2.85 };
      float BIS78_eta[ 2 ] = { 1.3, 1.5 };
      float BIS78_phi[ 8 ] = { 0.21, 0.57, 1.00, 1.33, 1.78, 2.14, 2.57, 2.93 };

      if( fabs( etaMS ) >= BOE[ 0 ] && fabs( etaMS ) <= BOE[ 1 ] && phiMS >= BOE[ 2 ] && phiMS <= BOE[ 3 ] ) return false;
      else if( fabs( etaMS ) >= BME[ 0 ] && fabs( etaMS ) <= BME[ 1 ] &&  phiMS >= BME[ 2 ] &&  phiMS <= BME[ 3 ] ) return false;
      else if( etaMS >= EE[ 0 ] && etaMS <= EE[ 1 ] ) return false;
      else if( fabs( etaMS ) >= BEE_eta[ 0 ] && fabs( etaMS ) <= BEE_eta[ 1 ] ){
        if( ( fabs( phiMS ) >= BEE_phi[ 0 ] && fabs( phiMS ) <= BEE_phi[ 1 ] ) || ( fabs( phiMS ) >= BEE_phi[ 2 ] && fabs( phiMS ) <= BEE_phi[ 3 ] ) ||( fabs( phiMS ) >= BEE_phi[ 4 ] && fabs( phiMS ) <= BEE_phi[ 5 ] ) || ( fabs( phiMS ) >= BEE_phi[ 6 ] && fabs( phiMS ) <= BEE_phi[ 7 ] ) ) return false;
        else return true;
        }
      else if ( fabs( etaMS ) >= BIS78_eta[ 0 ] && fabs( etaMS ) <= BIS78_eta[ 1 ] ){
        if ( ( fabs( phiMS ) >= BIS78_phi[ 0 ] && fabs( phiMS ) <= BIS78_phi[ 1 ] ) || ( fabs( phiMS ) >= BIS78_phi[ 2 ] && fabs( phiMS ) <= BIS78_phi[ 3 ] ) ||( fabs( phiMS ) >= BIS78_phi[ 4 ] && fabs( phiMS ) <= BIS78_phi[ 5 ] ) || ( fabs( phiMS ) >= BIS78_phi[ 6 ] && fabs( phiMS ) <= BIS78_phi[ 7 ] ) ) return false;
        else return true;
        }
      else return true;
    }
    else{
      ATH_MSG_VERBOSE( "passedHighPtCuts - MS track missing in muon! Aborting." );
      return false;
    }
    
    //::: Apply 1/p significance cut
    const xAOD::TrackParticle* idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    const xAOD::TrackParticle* metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    if( idtrack && metrack ){
      float qOverPdiff   = fabs(idtrack->qOverP()) - fabs(metrack->qOverP());
      float qOverPsigma  = sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) );
      float qOverPsignif = qOverPdiff / qOverPsigma;
      if( fabs( qOverPsignif ) > 7 ) return false;
      else return true;
    }
    else return false;

    return true;
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::TrackParticle & track ) const {
    using namespace xAOD;    
    uint8_t value1=0;
    uint8_t value2=0;

    if (track.summaryValue(value1, xAOD::SummaryType::numberOfPixelHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfPixelDeadSensors) ) {
      if (value1+value2==0) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - pixel values are missing! Aborting.");
      return false;
    }

    if (track.summaryValue(value1, xAOD::SummaryType::numberOfSCTHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfSCTDeadSensors)){
      if (value1+value2<=4) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - SCT values are missing! Aborting.");
      return false;
    }
    
    if (track.summaryValue(value1, xAOD::SummaryType::numberOfPixelHoles) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfSCTHoles)){
      if (value1+value2>=3) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - Si hole values are missing! Aborting.");
      return false;
    }

    float abseta = std::abs(track.eta());
    if (track.summaryValue(value1, xAOD::SummaryType::numberOfTRTHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfTRTOutliers)){
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

    try{
      bool readLR = mu.parameter(CaloLRLikelihood, xAOD::Muon::CaloLRLikelihood);
      bool readID = mu.parameter(CaloMuonIDTag, xAOD::Muon::CaloMuonIDTag);
      
      if (!readLR || !readID){
	ATH_MSG_VERBOSE("Unable to read CaloTag Quality information! Don't do anything for now, accept the calotag muons as they are!");
	//just a hack for now -- this information is not available in the DC14 xAODs
	//return false;
      }
      return (CaloLRLikelihood > 0.9 || CaloMuonIDTag > 10);
    }

    catch (SG::ExcBadAuxVar b){
      return false;
    }
  }
  
} // namespace CP
