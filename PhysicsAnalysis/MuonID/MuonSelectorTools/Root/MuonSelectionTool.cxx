/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.cxx 299883 2014-03-28 17:34:16Z krasznaa $

// Local include(s):
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "PathResolver/PathResolver.h"

namespace CP {
  
  MuonSelectionTool::MuonSelectionTool( const std::string& name )
    : asg::AsgTool( name ),
      m_name(name),
      m_accept( "MuonSelection" ){
    
    declareProperty( "MaxEta", m_maxEta = 2.7 );
    //xAOD::MuonQuality enum {Tight, Medium, Loose, VeryLoose}
    //corresponds to 0, 1, 2, 3, 4=HighPt, 5=LowPtEfficiency
    declareProperty( "MuQuality", m_quality = 1 );
    declareProperty( "ToroidOff", m_toroidOff = false );
    declareProperty( "TurnOffMomCorr", m_TurnOffMomCorr = false );
    declareProperty( "CalibrationRelease", m_calibration_version = "PreRec2016_2016-04-13" );

    // DEVELOPEMENT MODE: EXPERTS ONLY!!! 
    declareProperty( "ExpertDevelopMode", m_developMode = false );
    // these are for debugging / testing, *not* for general use!
    declareProperty( "CustomInputFolder", m_custom_dir = "");
    declareProperty( "TrtCutOff", m_TrtCutOff = false );
    declareProperty( "SctCutOff", m_SctCutOff = false );
    declareProperty( "PixCutOff", m_PixCutOff = false );
    declareProperty( "SiHolesCutOff", m_SiHolesCutOff = false );
    //
    m_tightWP_lowPt_rhoCuts = 0;
    m_tightWP_lowPt_qOverPCuts = 0;
    m_tightWP_mediumPt_rhoCuts = 0;
    m_tightWP_highPt_rhoCuts = 0;
    //
    ATH_MSG_DEBUG("Creating MuonSelectionTool named "<<m_name);
  }
    
  MuonSelectionTool::MuonSelectionTool( const MuonSelectionTool& toCopy  )
    : asg::AsgTool(toCopy.m_name+"_copy"),
      m_name(toCopy.m_name+"_copy"),
      m_maxEta( toCopy.m_maxEta ),
      m_quality( toCopy.m_quality ),
      m_accept( toCopy.m_accept ),
      m_toroidOff( toCopy.m_toroidOff ),
      m_developMode( toCopy.m_developMode ),
      m_TrtCutOff( toCopy.m_TrtCutOff ),
      m_SctCutOff( toCopy.m_SctCutOff ),
      m_PixCutOff( toCopy.m_PixCutOff ),
      m_SiHolesCutOff( toCopy.m_SiHolesCutOff ),
      m_TurnOffMomCorr(  toCopy.m_TurnOffMomCorr ),
      m_calibration_version( toCopy.m_calibration_version ),
      m_custom_dir( toCopy.m_custom_dir )
  {
    //
    m_tightWP_lowPt_rhoCuts = 0;
    m_tightWP_lowPt_qOverPCuts = 0;
    m_tightWP_mediumPt_rhoCuts = 0;
    m_tightWP_highPt_rhoCuts = 0;
    //
    ATH_MSG_DEBUG("Creating copy of MuonSelectionTool named "<<m_name);
  }
  
  MuonSelectionTool::~MuonSelectionTool(){
    ATH_MSG_DEBUG(Form("Deleting MuonSelectionTool named %s",m_name.c_str()));
    //
    if( m_tightWP_lowPt_rhoCuts ){
      delete m_tightWP_lowPt_rhoCuts;
      m_tightWP_lowPt_rhoCuts = 0;
    }
    if( m_tightWP_lowPt_qOverPCuts ){
      delete m_tightWP_lowPt_qOverPCuts;
      m_tightWP_lowPt_qOverPCuts = 0;
    }
    if( m_tightWP_mediumPt_rhoCuts ){
      delete m_tightWP_mediumPt_rhoCuts;
      m_tightWP_mediumPt_rhoCuts = 0;
    }
    if( m_tightWP_highPt_rhoCuts ){
      delete m_tightWP_highPt_rhoCuts;
      m_tightWP_highPt_rhoCuts = 0;
    }
  }
  
  StatusCode MuonSelectionTool::initialize() {
    
    // Greet the user:
    ATH_MSG_INFO( "Initialising..." );
    ATH_MSG_INFO( "Maximum eta: " << m_maxEta );
    ATH_MSG_INFO( "Muon quality: " << m_quality );
    if ( m_toroidOff ) ATH_MSG_INFO( "!! CONFIGURED FOR TOROID-OFF COLLISIONS !!" );
    //if ( m_TrtCutOff ) ATH_MSG_WARNING( "!! SWITCHING TRT REQUIREMENTS OFF !! FOR DEVELOPMENT USE ONLY !!" );
    if ( m_SctCutOff ) ATH_MSG_WARNING( "!! SWITCHING SCT REQUIREMENTS OFF !! FOR DEVELOPMENT USE ONLY !!" );
    if ( m_PixCutOff ) ATH_MSG_WARNING( "!! SWITCHING PIXEL REQUIREMENTS OFF !! FOR DEVELOPMENT USE ONLY !!" );
    if ( m_SiHolesCutOff ) ATH_MSG_WARNING( "!! SWITCHING SILICON HOLES REQUIREMENTS OFF !! FOR DEVELOPMENT USE ONLY !!" );
    if (m_custom_dir!="") ATH_MSG_WARNING("!! SETTING UP WITH USER SPECIFIED INPUT LOCATION \""<<m_custom_dir<<"\"!! FOR DEVELOPMENT USE ONLY !! ");

    // Set up the TAccept object:
    m_accept.addCut( "Eta",
		     "Selection of muons according to their pseudorapidity" );
    m_accept.addCut( "IDHits",
                     "Selection of muons according to whether they passed the MCP ID Hit cuts" );
    m_accept.addCut( "Preselection",
                     "Selection of muons according to their type/author" );
    m_accept.addCut( "Quality",
		     "Selection of muons according to their tightness" );
    // Sanity check
    if(m_quality>5 ){
      ATH_MSG_ERROR( "Invalid quality (i.e. selection WP) set: " << m_quality << " - it must be an integer between 0 and 5! (0=Tight, 1=Medium, 2=Loose, 3=Veryloose, 4=HighPt, 5=LowPtEfficiency)" );
      return StatusCode::FAILURE;
    }

    // Load Tight WP cut-map
    ATH_MSG_INFO( "Initialising tight working point histograms..." );
    std::string tightWP_rootFile_fullPath = "";
    if (m_custom_dir != ""){
    	tightWP_rootFile_fullPath = PathResolverFindCalibFile(m_custom_dir+"/muonSelection_tightWPHisto.root");
    }
    else{
    	tightWP_rootFile_fullPath = PathResolverFindCalibFile(Form("MuonSelectorTools/%s/muonSelection_tightWPHisto.root",
    			m_calibration_version.c_str()));
    }
    // HARD-CODED (TESTING ONLY) !!!
    /*std::string tightWP_rootFile_fullPath = gSystem->ExpandPathName("$ROOTCOREBIN/data/MuonSelectorTools/");
    m_tightWP_rootFile = "muonSelection_tightWPHisto_2016_03_15.root";
    tightWP_rootFile_fullPath.append( m_tightWP_rootFile );*/
    // ! HARD-CODED END

    ATH_MSG_INFO( "Reading muon tight working point histograms from " << tightWP_rootFile_fullPath  );
    // 
    TFile* file = TFile::Open( tightWP_rootFile_fullPath.c_str() ,"READ");

    if( !file->IsOpen() ){
      ATH_MSG_ERROR( "Cannot read tight working point file from " << tightWP_rootFile_fullPath );
      return StatusCode::FAILURE;
    }

    // Retrieve all the relevant histograms 
    ATH_CHECK( getHist( file,"tightWP_lowPt_rhoCuts",m_tightWP_lowPt_rhoCuts) ) ;
    ATH_CHECK( getHist( file,"tightWP_lowPt_qOverPCuts",m_tightWP_lowPt_qOverPCuts) );
    ATH_CHECK( getHist( file,"tightWP_mediumPt_rhoCuts",m_tightWP_mediumPt_rhoCuts) ) ;
    ATH_CHECK( getHist( file,"tightWP_highPt_rhoCuts",m_tightWP_highPt_rhoCuts) ) ;
    // 
    file->Close();
    delete file;
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSelectionTool::getHist( TFile* file, const char* histName, TH2D*& hist ){
    //
    if( !file ) {
      ATH_MSG_ERROR( " getHist(...) NULL TFile! Check that the Tight cut map is loaded correctly"  );
      return StatusCode::FAILURE;
    }
    //
    hist = dynamic_cast<TH2D*>( file->Get( histName ) );
    //
    if( !hist ){
      ATH_MSG_ERROR( "Cannot retrieve histogram " << histName  );
      return StatusCode::FAILURE;
    }
    hist->SetDirectory(0);
    ATH_MSG_INFO( "Successfully read tight working point histogram: " << hist->GetName()  );
    //
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

    // Passes ID hit cuts 
    ATH_MSG_VERBOSE( "Passes ID Hit cuts " << passedIDCuts(mu) );
    if( !passedIDCuts (mu) ) {
      return m_accept;
    }
    m_accept.setCutResult( "IDHits", true );
    
    // Passes muon preselection
    ATH_MSG_VERBOSE( "Passes preselection cuts " << passedMuonCuts(mu) );
    if( !passedMuonCuts (mu) ) {
      return m_accept;
    }
    m_accept.setCutResult( "Preselection", true );

    // Passes quality requirements 
    xAOD::Muon::Quality thisMu_quality = getQuality(mu);
    bool thisMu_highpt = passedHighPtCuts(mu);
    bool thisMu_lowptE = passedLowPtEfficiencyCuts(mu,thisMu_quality);
    ATH_MSG_VERBOSE( "Muon quality: " << thisMu_quality << " passes HighPt: "<< thisMu_highpt << " passes LowPtEfficiency: "<< thisMu_lowptE );
    if(m_quality<4 && thisMu_quality > m_quality){
      return m_accept;
    }
    if(m_quality==4 && !thisMu_highpt){
      return m_accept;
    }
    if(m_quality==5 && !thisMu_lowptE){
      return m_accept;
    }
    m_accept.setCutResult( "Quality", true );
    // Return the result:
    return m_accept;
  }
  
  void MuonSelectionTool::setQuality( xAOD::Muon& mu ) const {
    mu.setQuality(getQuality(mu)); 
    return;
  }
  
  xAOD::Muon::Quality MuonSelectionTool::getQuality( const xAOD::Muon& mu ) const {
    using namespace xAOD;
    
    // Combined muons
    if (mu.muonType()==xAOD::Muon::Combined){

      // formally switching off STACO for Rel 20.7 (preliminary) 
      if (mu.author()==xAOD::Muon::STACO){
	return xAOD::Muon::VeryLoose;
      }

      // rejection muons with out-of-bounds hits
      uint8_t combinedTrackOutBoundsPrecisionHits;
      if(!mu.summaryValue(combinedTrackOutBoundsPrecisionHits, xAOD::MuonSummaryType::combinedTrackOutBoundsPrecisionHits)) {
	ATH_MSG_WARNING("getQuality - # of out-of-bounds hits missing in combined muon! Returning VeryLoose...");
        return xAOD::Muon::VeryLoose;
      }
      if (combinedTrackOutBoundsPrecisionHits>0){
        return xAOD::Muon::VeryLoose;
      }
      
      uint8_t nprecisionLayers,nprecisionHoleLayers;      
      if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) || 
	  !mu.summaryValue(nprecisionHoleLayers, xAOD::SummaryType::numberOfPrecisionHoleLayers)){
	ATH_MSG_WARNING("getQuality - #precision layers missing in combined muon! Returning VeryLoose...");
	return xAOD::Muon::VeryLoose;
      }

      // LOOSE / MEDIUM / TIGHT WP
      const xAOD::TrackParticle* idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
      const xAOD::TrackParticle* metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
      if( idtrack && metrack ) {
        float mePt = -999999., idPt = -999999.;

        if( !m_TurnOffMomCorr ) { // now using corrected ID/MS momenta 
	  try{
	    static SG::AuxElement::Accessor<float> mePt_acc("MuonSpectrometerPt");
	    static SG::AuxElement::Accessor<float> idPt_acc("InnerDetectorPt");
	    mePt = mePt_acc(mu);
	    idPt = idPt_acc(mu);
	  } catch ( const SG::ExcNoAuxStore& b ) {
	    ATH_MSG_FATAL( "No MomentumCorrections decorations available! MuonSelectionTool can not work!!! " <<
			   "Please apply MuonMomentumCorrections before feeding the muon to MuonSelectorTools." );
	    throw std::runtime_error( "No MomentumCorrections decorations available, throwing a runtime error" );
	    return xAOD::Muon::VeryLoose;
	  } 
        } else {
          mePt = metrack->pt();
          idPt = idtrack->pt();
	}

        float cbPt = mu.pt();
        float meP  = 1.0 / ( sin(metrack->theta()) / mePt);
        float idP  = 1.0 / ( sin(idtrack->theta()) / idPt);

        float rho           = fabs( idPt - mePt ) / cbPt;
        float qOverPsigma  = sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) );
        float qOverPsignif  = fabs( (metrack->charge() / meP) - (idtrack->charge() / idP) ) / qOverPsigma;        
        float reducedChi2 = mu.primaryTrackParticle()->chiSquared()/mu.primaryTrackParticle()->numberDoF();

	//---- FIX FOR CSC ----
	if( fabs(mu.eta()) > 2.0 ) {
	  nprecisionLayers = 0;
	  uint8_t innerSmallHits, innerLargeHits, middleSmallHits, middleLargeHits, outerSmallHits, outerLargeHits;
	  if ( !mu.summaryValue(innerSmallHits, xAOD::MuonSummaryType::innerSmallHits) ||
	       !mu.summaryValue(innerLargeHits, xAOD::MuonSummaryType::innerLargeHits) ||
	       !mu.summaryValue(middleSmallHits, xAOD::MuonSummaryType::middleSmallHits) ||
	       !mu.summaryValue(middleLargeHits, xAOD::MuonSummaryType::middleLargeHits) ||
	       !mu.summaryValue(outerSmallHits, xAOD::MuonSummaryType::outerSmallHits) ||
	       !mu.summaryValue(outerLargeHits, xAOD::MuonSummaryType::outerLargeHits) ){

	    ATH_MSG_WARNING("getQuality - Muon in CSC region and MS hits information missing!!! Returning VeryLoose...");
	    return xAOD::Muon::VeryLoose;
	  }
	  if( innerSmallHits>1  || innerLargeHits>1  ) nprecisionLayers += 1;
	  if( middleSmallHits>2 || middleLargeHits>2 ) nprecisionLayers += 1;
	  if( outerSmallHits>2  || outerLargeHits>2  ) nprecisionLayers += 1;
	}

	// NEW TIGHT WP  
        if( nprecisionLayers>1 && reducedChi2<8 && fabs(qOverPsignif)<7 ) {
	  if( passTight(mu,rho,qOverPsignif) ) {
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

      // Improvement for Loose targeting low-pT muons
      /*if (fabs(mu.eta())>0.1 && fabs(mu.eta())<1.05 && nprecisionLayers==1 && 
	  (mu.author()==xAOD::Muon::MuidCo || 
	   (mu.author()==xAOD::Muon::MuGirl && mu.isAuthor(xAOD::Muon::MuTagIMO)) || 
	   mu.author()==xAOD::Muon::MuTagIMO ) ) 
	   return xAOD::Muon::Loose;*/

      // didn't pass the set of requirements for a medium or tight combined muon
      return xAOD::Muon::VeryLoose;
    }
    
    // SA muons
    if ( mu.author()==xAOD::Muon::MuidSA ) {

      if(fabs(mu.eta())>2.5){

	uint8_t nprecisionLayers = 0;
	uint8_t innerSmallHits(0), innerLargeHits(0), middleSmallHits(0), middleLargeHits(0), outerSmallHits(0), outerLargeHits(0); //, nGoodPrecLayers(0);
	if ( !mu.summaryValue(innerSmallHits, xAOD::MuonSummaryType::innerSmallHits) ||
	     !mu.summaryValue(innerLargeHits, xAOD::MuonSummaryType::innerLargeHits) ||
	     !mu.summaryValue(middleSmallHits, xAOD::MuonSummaryType::middleSmallHits) ||
	     !mu.summaryValue(middleLargeHits, xAOD::MuonSummaryType::middleLargeHits) ||
	     !mu.summaryValue(outerSmallHits, xAOD::MuonSummaryType::outerSmallHits) ||
	     !mu.summaryValue(outerLargeHits, xAOD::MuonSummaryType::outerLargeHits) //||
	     //mu.summaryValue(nGoodPrecLayers, xAOD::numberOfGoodPrecisionLayers) 
	   ) {
	  ATH_MSG_WARNING("getQuality - SA muon with missing MS hits information!!! Returning VeryLoose...");
	  return xAOD::Muon::VeryLoose;
	}

	// requiring at least 3 good precision layers (for future improvements)                                                                                                    
	// if( nGoodPrecLayers <3 )                                                                                                                                                
	//return xAOD::Muon::VeryLoose;                                                                                                                                            
	// ---- require 3 MS stations ----                                                                                                                                         
	if( innerSmallHits>1  || innerLargeHits>1  ) nprecisionLayers += 1;
	if( middleSmallHits>2 && outerSmallHits>2  ) nprecisionLayers += 2;
	if( middleLargeHits>2 && outerLargeHits>2  ) nprecisionLayers += 2;
	
	/*if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) ) {
	  ATH_MSG_VERBOSE("getQuality - #precision layers missing in Standalone muon! Aborting.");
	  return xAOD::Muon::VeryLoose;
	  }*/

	// 3 station requirement for medium
	if( nprecisionLayers>2 && !m_toroidOff )  return xAOD::Muon::Medium;
      }

      // didn't pass the set of requirements for a medium SA muon
      return xAOD::Muon::VeryLoose;
    }

    // SiliconAssociatedForward (SAF) muons
    if ( mu.muonType()==xAOD::Muon::SiliconAssociatedForwardMuon ){

      const xAOD::TrackParticle* cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
      const xAOD::TrackParticle* metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );

      if( cbtrack && metrack ) {
	if( fabs(cbtrack->eta()) > 2.5 ) { 
	  uint8_t nprecisionLayers = 0;
          uint8_t innerSmallHits(0), innerLargeHits(0), middleSmallHits(0), middleLargeHits(0), outerSmallHits(0), outerLargeHits(0); //, nGoodPrecLayers(0);
          if ( !mu.summaryValue(innerSmallHits, xAOD::MuonSummaryType::innerSmallHits) ||
               !mu.summaryValue(innerLargeHits, xAOD::MuonSummaryType::innerLargeHits) ||
               !mu.summaryValue(middleSmallHits, xAOD::MuonSummaryType::middleSmallHits) ||
               !mu.summaryValue(middleLargeHits, xAOD::MuonSummaryType::middleLargeHits) ||
               !mu.summaryValue(outerSmallHits, xAOD::MuonSummaryType::outerSmallHits) ||
               !mu.summaryValue(outerLargeHits, xAOD::MuonSummaryType::outerLargeHits)  
	       //!mu.summaryValue(nGoodPrecLayers, xAOD::numberOfGoodPrecisionLayers) 
	       ) {
            ATH_MSG_WARNING("getQuality - SAF muon with missing MS hits information!!! Returning VeryLoose...");
            return xAOD::Muon::VeryLoose;
          }
	  // requiring at least 3 good precision layers (for future improvements)
	  // if( nGoodPrecLayers <3 ) 
	  //return xAOD::Muon::VeryLoose;
	  // ---- require 3 MS stations ----
          if( innerSmallHits>1  || innerLargeHits>1  ) nprecisionLayers += 1;
          if( middleSmallHits>2 && outerSmallHits>2  ) nprecisionLayers += 2;
          if( middleLargeHits>2 && outerLargeHits>2  ) nprecisionLayers += 2;

	  /*if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers) ) {
	    ATH_MSG_VERBOSE("getQuality - #precision layers missing in SiliconAssociatedForward muon! Aborting.");
	    return xAOD::Muon::VeryLoose;
	    }*/
	  
	  if( nprecisionLayers >2 && !m_toroidOff  ) {
	    if (mu.trackParticle( xAOD::Muon::Primary )  == mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) && !m_developMode ){
	      ATH_MSG_FATAL("SiliconForwardAssociated muon has ID track as primary track particle. "<<
			    "This is a bug fixed starting with xAODMuon-00-17-07, which should be present in this release. "<<
			    "Please report this to the Muon CP group!");
	    }
	    // xAOD::Muon& mymu = const_cast<xAOD::Muon&>(mu);
	    // fixing the muon 4-vector to return the desired quantities
	    // mymu.setP4( metrack->pt(), cbtrack->eta(), cbtrack->phi() );
	    return xAOD::Muon::Medium;
	  }
	}
      }

      // didn't pass the set of requirements for a medium SAF muon
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

  /*void MuonSelectionTool::setPassesLowPtEfficiencyCuts( xAOD::Muon& mu ) const {
    mu.setPassesLowPtEfficiencyCuts(passedLowPtEfficiencyCuts(mu));
    }*/
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::Muon& mu ) const {
    //using namespace xAOD;
    //do not apply the ID hit requirements for SA muons for |eta| > 2.5
    if ( mu.author()==xAOD::Muon::MuidSA && fabs(mu.eta())>2.5 ) {
      return true;
    } else if( mu.muonType()==xAOD::Muon::SiliconAssociatedForwardMuon ) {
      const xAOD::TrackParticle* cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
      if( cbtrack ) {
	if( fabs(cbtrack->eta()) >2.5 ) {
	  //uint8_t value=0;
	  // ---
	  // Postponing this to post-ICHEP recommendations 
	  // ---
	  /*if( cbtrack->summaryValue(value, xAOD::SummaryType::numberOfInnermostPixelLayerHits) ) {
	    // at least one b-layer hit 
	    if( value>0 ) return true;
	    }*/
	  // TODO -COMMENT THIS RETURN FOR ICHEP!!!
	  return true;
	}
      }
      return false;
    } else {
      if(mu.primaryTrackParticle()) 
	return passedIDCuts(*mu.primaryTrackParticle());
      else
	return false;
    }
  }
  
  bool MuonSelectionTool::isBadMuon( const xAOD::Muon& mu ) const {
   
    if( mu.muonType() != xAOD::Muon::Combined ) return false;
    // ::
    const xAOD::TrackParticle*       idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    const xAOD::TrackParticle*       metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    const xAOD::TrackParticle*       cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
    // ::
    bool IsBadMuon = false;
    if( idtrack && metrack && cbtrack ) {
      // ::
      double qOverP_ID = idtrack->qOverP();
      double qOverPerr_ID = sqrt( idtrack->definingParametersCovMatrix()(4,4) );
      double qOverP_ME = metrack->qOverP();
      double qOverPerr_ME = sqrt( metrack->definingParametersCovMatrix()(4,4) );
      double qOverP_CB = cbtrack->qOverP();
      double qOverPerr_CB = sqrt( cbtrack->definingParametersCovMatrix()(4,4) );
      // ::
      if( m_quality==4 ) { 
	// recipe for high-pt selection
	IsBadMuon = !passedErrorCutCB(mu);
      } else {
	// recipe for other WP
	double IdCbRatio = fabs( (qOverPerr_ID/qOverP_ID) / (qOverPerr_CB/qOverP_CB) );
	double MeCbRatio = fabs( (qOverPerr_ME/qOverP_ME) / (qOverPerr_CB/qOverP_CB) );
	IsBadMuon = ( IdCbRatio<0.8 || MeCbRatio<0.8 );
      }	
    } else {
      return true;
    }
    return IsBadMuon;
  }

  bool MuonSelectionTool::passedLowPtEfficiencyCuts( const xAOD::Muon& mu ) const {
    xAOD::Muon::Quality thisMu_quality = getQuality(mu);
    return passedLowPtEfficiencyCuts(mu,thisMu_quality);
  }

  bool MuonSelectionTool::passedLowPtEfficiencyCuts( const xAOD::Muon& mu, xAOD::Muon::Quality thisMu_quality ) const {

    // requiring combined muons
    if( mu.muonType() != xAOD::Muon::Combined ) return false;
    if( mu.author()!=xAOD::Muon::MuGirl && mu.author()!=xAOD::Muon::MuidCo ) return false;

    // applying Medium selection above pT = 18 GeV 
    if( mu.pt()/1000.>18. ) {
      if( thisMu_quality <= xAOD::Muon::Medium ) return true;
      else return false;
    }

    // requiring Medium in forward regions
    if( fabs(mu.eta())>1.55 && thisMu_quality > xAOD::Muon::Medium ) return false;
    
    // rejection of muons with out-of-bounds hits 
    uint8_t combinedTrackOutBoundsPrecisionHits;
    if(!mu.summaryValue(combinedTrackOutBoundsPrecisionHits, xAOD::MuonSummaryType::combinedTrackOutBoundsPrecisionHits)) {
      ATH_MSG_WARNING("passedLowPtEfficiencyCuts - # of out-of-bounds hits missing in combined muon! Failing selection...");
      return false;
    }
    if (combinedTrackOutBoundsPrecisionHits>0) {
      return false;
    }

    // requiring explicitely >=1 station (2 in the |eta|>1.3 region when Medium selection is not explicitely required)
    uint8_t nprecisionLayers;
    if (!mu.summaryValue(nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers)){
      ATH_MSG_WARNING("passedLowPtEfficiencyCuts - #precision layers missing in combined muon! Failing selection...");
      return false;
    }
    uint nStationsCut = (fabs(mu.eta())>1.3&&fabs(mu.eta())<1.55) ? 2 : 1;
    if ( nprecisionLayers<nStationsCut ) {
      return false;
    }

    // reject MuGirl muon if not found also by MuTagIMO 
    if( mu.author()==xAOD::Muon::MuGirl && !mu.isAuthor(xAOD::Muon::MuTagIMO) ) {
      return false;
    }

    // apply some loose quality requirements 
    float momentumBalanceSignificance(0.), scatteringCurvatureSignificance(0.), scatteringNeighbourSignificance(0.);
    if( !mu.parameter(momentumBalanceSignificance,xAOD::Muon::momentumBalanceSignificance) 
	|| !mu.parameter(scatteringCurvatureSignificance,xAOD::Muon::scatteringCurvatureSignificance) 
	|| !mu.parameter(scatteringNeighbourSignificance,xAOD::Muon::scatteringNeighbourSignificance) ) {
      ATH_MSG_WARNING("passedLowPtEfficiencyCuts - momentum balance, scatternig curvature or neighbour significances missing in combined muon! Failing selection...");
      return false;
    }
    if( fabs(momentumBalanceSignificance)>3. || fabs(scatteringCurvatureSignificance)>3. || fabs(scatteringNeighbourSignificance)>3. ) {
      return false;
    }

    // passed low pt selection 
    return true;
  }

  bool MuonSelectionTool::passedHighPtCuts( const xAOD::Muon& mu ) const {
    using namespace xAOD;
    
    // :: Request combined muons
    if( mu.muonType() != xAOD::Muon::Combined ) return false;
    if( mu.author()==xAOD::Muon::STACO ) return false;

    // :: Reject muons with out-of-bounds hits
    uint8_t combinedTrackOutBoundsPrecisionHits;
    if(!mu.summaryValue(combinedTrackOutBoundsPrecisionHits, xAOD::MuonSummaryType::combinedTrackOutBoundsPrecisionHits)) {
      ATH_MSG_WARNING("passedHighPtCuts - # of out-of-bounds hits missing in combined muon! Failing selection...");
      return false;
    }
    if (combinedTrackOutBoundsPrecisionHits>0){
      return false;
    }

    // :: Access MS hits information 
    uint8_t nprecisionLayers(0), nGoodPrecLayers(0), innerSmallHits(0), innerLargeHits(0), middleSmallHits(0), middleLargeHits(0), 
      outerSmallHits(0), outerLargeHits(0), extendedSmallHits(0), extendedLargeHits(0), extendedSmallHoles(0), isSmallGoodSectors(0);
    if ( !mu.summaryValue( nprecisionLayers, xAOD::SummaryType::numberOfPrecisionLayers ) || 
	 !mu.summaryValue( nGoodPrecLayers, xAOD::numberOfGoodPrecisionLayers ) ||
	 !mu.summaryValue(innerSmallHits, xAOD::MuonSummaryType::innerSmallHits) ||
	 !mu.summaryValue(innerLargeHits, xAOD::MuonSummaryType::innerLargeHits) ||
	 !mu.summaryValue(middleSmallHits, xAOD::MuonSummaryType::middleSmallHits) ||
	 !mu.summaryValue(middleLargeHits, xAOD::MuonSummaryType::middleLargeHits) ||
	 !mu.summaryValue(outerSmallHits, xAOD::MuonSummaryType::outerSmallHits) ||
	 !mu.summaryValue(outerLargeHits, xAOD::MuonSummaryType::outerLargeHits) ||
	 !mu.summaryValue(extendedSmallHits, xAOD::MuonSummaryType::extendedSmallHits) ||
	 !mu.summaryValue(extendedLargeHits, xAOD::MuonSummaryType::extendedLargeHits) ||
	 !mu.summaryValue(extendedSmallHoles, xAOD::MuonSummaryType::extendedSmallHoles) ||
	 !mu.summaryValue(isSmallGoodSectors, xAOD::MuonSummaryType::isSmallGoodSectors)
       ){
      ATH_MSG_WARNING("passedHighPtCuts - MS hits information missing!!! Failing High-pT selection...");
      return false;
    }

    //::: Require 3 (good) station muons
    if( nprecisionLayers < 3 ) return false;
    //if( nGoodPrecLayers < 3 ) return false; // postponed (further studies needed)

    //::: Apply MS Chamber Vetoes
    // Given according to their eta-phi locations in the muon spectrometer
    // FORM: CHAMBERNAME[ array of four values ] = { eta 1, eta 2, phi 1, phi 2}
    // The vetoes are applied based on the MS track if available. If the MS track is not available, 
    // the vetoes are applied according to the combined track, and runtime warning is printed to
    // the command line.
    const xAOD::TrackParticle* CB_track = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
    const xAOD::TrackParticle* MS_track = NULL;
    if( mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "muonSpectrometerTrackParticleLink" )
	&& ( mu.muonSpectrometerTrackParticleLink() ).isValid() 
      ) MS_track = mu.trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );    
    else{
      ATH_MSG_VERBOSE( "passedHighPtCuts - No MS track available for muon. Using combined track." );
      MS_track = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
    }
    
    if( MS_track != NULL && CB_track !=NULL ) {

      float etaMS = MS_track->eta();
      float phiMS = MS_track->phi();
      float etaCB = CB_track->eta();

      //::: Barrel/Endcap overlap region
      if( 1.01 < fabs( etaMS ) && fabs( etaMS ) < 1.1 ) return false;
      if( 1.01 < fabs( etaCB ) && fabs( etaCB ) < 1.1 ) return false;

      //::: BIS78
      float BIS78_eta[ 2 ] = { 1.05, 1.3 };
      float BIS78_phi[ 8 ] = { 0.21, 0.57, 1.00, 1.33, 1.78, 2.14, 2.57, 2.93 };

      if ( fabs( etaMS ) >= BIS78_eta[ 0 ] && fabs( etaMS ) <= BIS78_eta[ 1 ] ) {
        if ( ( fabs( phiMS ) >= BIS78_phi[ 0 ] && fabs( phiMS ) <= BIS78_phi[ 1 ] ) 
	     || ( fabs( phiMS ) >= BIS78_phi[ 2 ] && fabs( phiMS ) <= BIS78_phi[ 3 ] ) 
	     || ( fabs( phiMS ) >= BIS78_phi[ 4 ] && fabs( phiMS ) <= BIS78_phi[ 5 ] ) 
	     || ( fabs( phiMS ) >= BIS78_phi[ 6 ] && fabs( phiMS ) <= BIS78_phi[ 7 ] ) 
	   ) return false;
      }

      //::: BEE
      float BEE_eta[ 2 ] = { 1.440, 1.692 };
      float BEE_phi[ 8 ] = { 0.301, 0.478, 1.086, 1.263, 1.872, 2.049, 2.657, 2.834 };     
      if ( fabs( etaMS ) >= BEE_eta[ 0 ] && fabs( etaMS ) <= BEE_eta[ 1 ] ) {
	if ( ( fabs( phiMS ) >= BEE_phi[ 0 ] && fabs( phiMS ) <= BEE_phi[ 1 ] ) 
	     || ( fabs( phiMS ) >= BEE_phi[ 2 ] && fabs( phiMS ) <= BEE_phi[ 3 ] ) 
	     || ( fabs( phiMS ) >= BEE_phi[ 4 ] && fabs( phiMS ) <= BEE_phi[ 5 ] ) 
	     || ( fabs( phiMS ) >= BEE_phi[ 6 ] && fabs( phiMS ) <= BEE_phi[ 7 ] ) 
	     ) {
	  // Muon falls in the BEE eta-phi region: asking for 4 good precision layers
	  //if( nGoodPrecLayers < 4 ) return false; // postponed (further studies needed) 
	  if( nprecisionLayers < 4 ) return false;
	}  
      }
      if( fabs(etaCB)>1.4 ) {
	// Veto residual 3-station muons in BEE region due to MS eta/phi resolution effects
	//if( nGoodPrecLayers<4 && (extendedSmallHits>0||extendedSmallHoles>0) ) return false; // postponed (further studies needed)
	if( nprecisionLayers<4 && (extendedSmallHits>0||extendedSmallHoles>0) ) return false;
      }
    } else {
      ATH_MSG_WARNING( "passedHighPtCuts - MS or CB track missing in muon! Failing High-pT selection..." );
      return false;
    }

    // Remove 3-station muons with small-large sectors overlap
    if( isSmallGoodSectors ) {
      if( !(innerSmallHits > 2 && middleSmallHits > 2 && (outerSmallHits>2||extendedSmallHits>2)) ) return false;
    } else {
      if( !(innerLargeHits > 2 && middleLargeHits > 2 && (outerLargeHits>2||extendedLargeHits>2)) ) return false;
    }
    
    //::: Apply 1/p significance cut
    const xAOD::TrackParticle* idtrack = mu.trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
    const xAOD::TrackParticle* metrack = mu.trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
    if( idtrack && metrack ){
      float mePt = -999999., idPt = -999999.;
      
      if( !m_TurnOffMomCorr ) { // now using corrected ID/MS momenta
	try{
	  static SG::AuxElement::Accessor<float> mePt_acc("MuonSpectrometerPt");//A bit of optimization
	  static SG::AuxElement::Accessor<float> idPt_acc("InnerDetectorPt");
	  mePt = mePt_acc(mu);
	  idPt = idPt_acc(mu);
	} catch ( const SG::ExcNoAuxStore& b ) {
	  ATH_MSG_FATAL( "No MomentumCorrections decorations available! MuonSelectionTool can not work!!! " <<
			 "Please apply MuonMomentumCorrections before feeding the muon to MuonSelectorTools." );
	  throw std::runtime_error( "No MomentumCorrections decorations available, throwing a runtime error" );
	  return false;
	}
      } else {
	mePt = metrack->pt();
	idPt = idtrack->pt();
      }
      
      float meP  = 1.0 / ( sin(metrack->theta()) / mePt);
      float idP  = 1.0 / ( sin(idtrack->theta()) / idPt);
      
      float qOverPsigma  = sqrt( idtrack->definingParametersCovMatrix()(4,4) + metrack->definingParametersCovMatrix()(4,4) );
      float qOverPsignif  = fabs( (metrack->charge() / meP) - (idtrack->charge() / idP) ) / qOverPsigma;        
      if( fabs(qOverPsignif) > 7 ) return false;

    }
    else return false;

    return true;
  }

  bool MuonSelectionTool::passedErrorCutCB( const xAOD::Muon& mu ) const {
    // ::
    if( mu.muonType() != xAOD::Muon::Combined ) return false;
    // :: 
    float fabs_eta = fabs(mu.eta());
    float p0(8.0), p1(0.034), p2(0.00011);
    if( fabs_eta>1.05 && fabs_eta<1.3 ) {
      p1=0.036;
      p2=0.00012;
    } else if( fabs_eta>1.3 && fabs_eta<1.7 ) {
      p1=0.051;
      p2=0.00014;
    } else if( fabs_eta>1.7 && fabs_eta<2.0 ) {
      p1=0.042;
      p2=0.00010;
    } else if( fabs_eta>2.0) {
      p1=0.034;
      p2=0.00013;
    }
    // :: 
    bool passErrorCutCB = false;
    const xAOD::TrackParticle* cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
    if( cbtrack ) {
      // ::
      double pt_CB = (cbtrack->pt() / 1000. < 5000.) ? cbtrack->pt() / 1000. : 5000.; // GeV
      double qOverP_CB = cbtrack->qOverP();
      double qOverPerr_CB = sqrt( cbtrack->definingParametersCovMatrix()(4,4) );
      // sigma represents the average expected error at the muon's pt/eta 
      double sigma = sqrt( pow(p0/pt_CB,2) + pow(p1,2) + pow(p2*pt_CB,2) );
      // cuttting at 1.8*sigma for pt <=1 TeV, then linearly tightening untill 1*sigma is reached at pt >= 5TeV. 
      double coefficient = (pt_CB > 1000.) ? (2.0-0.0002*pt_CB) : 1.8;
      // ::
      if( fabs(qOverPerr_CB/qOverP_CB) < coefficient*sigma ) {
        passErrorCutCB = true;
      }
    }
    // :: 
    return passErrorCutCB;
  }

  bool MuonSelectionTool::passedMuonCuts( const xAOD::Muon& mu ) const {
    // ::
    if( mu.muonType() == xAOD::Muon::Combined ) { 
      if( mu.author()==xAOD::Muon::STACO ) return false;
      else return true;
    }
    // ::
    if( mu.muonType() == xAOD::Muon::CaloTagged && fabs(mu.eta())<0.105 ) return true;
    // ::
    if( mu.muonType() == xAOD::Muon::SegmentTagged && fabs(mu.eta())<0.105 ) return true;
    // ::
    if( mu.author()==xAOD::Muon::MuidSA && fabs(mu.eta())>2.4 ) return true;
    // ::
    if( mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
      const xAOD::TrackParticle* cbtrack = mu.trackParticle( xAOD::Muon::CombinedTrackParticle );
      if( cbtrack ) {
        if( fabs(cbtrack->eta()) >2.4 ) return true; 
      }
    }
    // ::
    return false;
  }
  
  bool MuonSelectionTool::passedIDCuts( const xAOD::TrackParticle & track ) const {
    using namespace xAOD;    
    uint8_t value1=0;
    uint8_t value2=0;

    //if( (m_PixCutOff || m_SctCutOff || m_SiHolesCutOff || m_TrtCutOff) && !m_developMode )
    if( (m_PixCutOff || m_SctCutOff || m_SiHolesCutOff) && !m_developMode )
      ATH_MSG_WARNING( " !! Tool configured with some of the ID hits requirements changed... FOR DEVELOPMENT ONLY: muon efficiency SF won't be valid !! " );

    if (track.summaryValue(value1, xAOD::SummaryType::numberOfPixelHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfPixelDeadSensors) ) {
      //if (value1+value2==0) return false;
      if ( (value1+value2==0) && !m_PixCutOff ) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - pixel values are missing! Aborting.");
      return false;
    }

    if (track.summaryValue(value1, xAOD::SummaryType::numberOfSCTHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfSCTDeadSensors)){
      //if (value1+value2<=4) return false;
      if ( (value1+value2<=4) && !m_SctCutOff ) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - SCT values are missing! Aborting.");
      return false;
    }
    
    if (track.summaryValue(value1, xAOD::SummaryType::numberOfPixelHoles) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfSCTHoles)){
      //if (value1+value2>=3) return false;
      if ( (value1+value2>=3) && !m_SiHolesCutOff ) return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - Si hole values are missing! Aborting.");
      return false;
    }

    float abseta = std::abs(track.eta());
    if (track.summaryValue(value1, xAOD::SummaryType::numberOfTRTHits) &&
	track.summaryValue(value2, xAOD::SummaryType::numberOfTRTOutliers)){
      // Okay, so have values available - now check if the TRT cuts are met...
      uint8_t totTRThits=value1+value2;
      //if (!((0.1<abseta && abseta<=1.9 && totTRThits>5 && value2<(0.9 * totTRThits)) || (abseta <= 0.1 || abseta > 1.9)))
      if ( (!((0.1<abseta && abseta<=1.9 && totTRThits>5 && value2<(0.9 * totTRThits)) || (abseta <= 0.1 || abseta > 1.9))) && !m_TrtCutOff )
	return false;
    } else {
      ATH_MSG_WARNING("isPassedIDCuts - TRT values are missing! Aborting.");
      return false;
    }

    // Reached end - all ID hit cuts are passed.
    return true;
  } // passedIDCuts
  
  
  bool MuonSelectionTool::passedCaloTagQuality (const xAOD::Muon& mu) const{

    //float CaloLRLikelihood = 1.0;
    int CaloMuonIDTag = -20;

    // Rel. 20.1
    /*try{
      bool readLR = mu.parameter(CaloLRLikelihood, xAOD::Muon::CaloLRLikelihood);
      bool readID = mu.parameter(CaloMuonIDTag, xAOD::Muon::CaloMuonIDTag);
      
      if (!readLR || !readID){
	ATH_MSG_VERBOSE("Unable to read CaloTag Quality information! Don't do anything for now, accept the calotag muons as they are!");
	//just a hack for now -- this information is not available in the DC14 xAODs
	//return false;
      }
      return ( CaloLRLikelihood > 0.9 || CaloMuonIDTag > 10 );
    }

    catch (SG::ExcBadAuxVar b) {
      return false;
    }*/

    // Rel 20.7
    try{                                                                                                                                                                                                                                                                                                                    
      bool readID = mu.parameter(CaloMuonIDTag, xAOD::Muon::CaloMuonIDTag);       
      if ( !readID ) {
	ATH_MSG_VERBOSE( "Unable to read CaloTag Quality information! Rejection the CALO muon!" ); 
        return false;
      }
      return ( CaloMuonIDTag > 10 ); 
    }
    catch (const SG::ExcBadAuxVar& b) {
      return false; 
    }
  }

  bool MuonSelectionTool::passTight( const xAOD::Muon& mu, float rho, float oneOverPSig ) const
  {
    float symmetric_eta = fabs( mu.eta() );
    float pt = mu.pt() / 1000.0; // GeV                                                                                                                                                                                                                                                                                                                                     
    // Impose pT and eta cuts; the bounds of the cut maps  
    if( pt < 4.0 || symmetric_eta>2.5 ) return false;
    ATH_MSG_DEBUG( "Muon is passing tight WP kinemartic cuts with pT,eta " << mu.pt() << "  ,  " << mu.eta()  );

    // ** Low pT specific cuts ** //  
    if( pt > 4.0 && pt <= 20.0 ){

      double rhoCut    = m_tightWP_lowPt_rhoCuts->Interpolate( pt, symmetric_eta );
      double qOverPCut = m_tightWP_lowPt_qOverPCuts->Interpolate( pt , symmetric_eta );

      ATH_MSG_DEBUG( "Applying tight WP cuts to a low pt muon with (pt,eta) ( " << pt << " , " << mu.eta() << " ) " );
      ATH_MSG_DEBUG( "Rho value " << rho << ", required to be less than " << rhoCut  );
      ATH_MSG_DEBUG( "Momentum significance value " << oneOverPSig << ", required to be less than " << qOverPCut );

      if( rho > rhoCut ) return false;
      ATH_MSG_DEBUG( "Muon passed tight WP, low pT rho cut!" );

      if( oneOverPSig > qOverPCut ) return false;
      ATH_MSG_DEBUG( "Muon passed tight WP, low pT momentum significance cut" );

      // Tight muon!
      return true;

    }

    // ** Medium pT specific cuts ** //  
    else if ( pt > 20.0 && pt <= 100.0 ) {
      double rhoCut = m_tightWP_mediumPt_rhoCuts->Interpolate( pt , symmetric_eta );
      // 
      ATH_MSG_DEBUG( "Applying tight WP cuts to a medium pt muon with (pt,eta) (" << pt << "," << mu.eta() << ")" );
      ATH_MSG_DEBUG( "Rho value " << rho << " required to be less than " << rhoCut );

      // Apply cut 
      if( rho > rhoCut ) return false;
      ATH_MSG_DEBUG( "Muon passed tight WP, medium pT rho cut!" );

      // Tight muon! 
      return true;
    }

    // ** High pT specific cuts  
    else if ( pt > 100.0 && pt <= 500.0 ){
      // 
      ATH_MSG_DEBUG( "Applying tight WP cuts to a high pt muon with (pt,eta) (" << pt << "," << mu.eta() << ")" );
      // No interpolation, since bins with -1 mean we should cut really loose       
      double rhoCut = m_tightWP_highPt_rhoCuts->GetBinContent( m_tightWP_highPt_rhoCuts->FindBin(pt, symmetric_eta) );
      ATH_MSG_DEBUG( "Rho value " << rho << ", required to be less than " << rhoCut << " unless -1, in which no cut is applied" );
      //
      if( rhoCut <  0.0 ) return true;
      if( rho > rhoCut  ) return false;
      ATH_MSG_DEBUG( "Muon passd tight WP, high pT rho cut!" );


      return true;
    }
    // For muons with pT > 500 GeV, no extra cuts                    
    else{
      ATH_MSG_DEBUG( "Not applying any tight WP cuts to a very high pt muon with (pt,eta) (" << pt << "," << mu.eta() << ")" );
      return true;
    }
    
    // you should never reach this point
    return false;
  }

  
} // namespace CP




