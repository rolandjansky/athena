/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */
// **********************************************************************
// AlignmentMonTool.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// **********************************************************************

#include "IDAlignMonGenericTracks.h"

#include <sstream>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"


#include "Particle/TrackParticle.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "xAODEventInfo/EventInfo.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "TrackSelectionTool.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"


// *********************************************************************
// Public Methods
// *********************************************************************

IDAlignMonGenericTracks::IDAlignMonGenericTracks( const std::string & type, const std::string & name, const IInterface* parent )
       :ManagedMonitorToolBase( type, name, parent ),
	m_idHelper(0),
	m_pixelID(0),
	m_sctID(0),
	m_trtID(0),
	m_Pi(3.14156),
	m_events(0),
	m_histosBooked(0),
        m_tracksName("ExtendedTracks"),
        m_triggerChainName("NoTriggerSelection"),
        m_barrelEta(0.8),
	m_vertices(0),
	m_doHitQuality(0),
	m_d0Range(2.0),
	m_d0BsRange(0.2),
	m_z0Range(250.0),
	m_pTRange(100.0),
	m_etaRange(2.5),
	m_NTracksRange(200),
	m_nBinsMuRange(100),
	m_muRangeMin(0.5),
	m_muRangeMax(100.5),
        m_beamCondSvc("BeamCondSvc",name),
        m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator"), 
	m_hWeightInFile(0),
	m_hInputTrackWeight(0),
	m_userInputWeigthMethod(IDAlignMonGenericTracks::TRKETA_TRKPT),
	m_useLowStat(false)

{
  m_trackSelection = ToolHandle< InDetAlignMon::TrackSelectionTool >("InDetAlignMon::TrackSelectionTool");
  m_hitQualityTool = ToolHandle<IInDetAlignHitQualSelTool>("");
 
  InitializeHistograms(); // histograms are now defined as NULL pointers. The actual booking comes later.
  
  declareProperty("tracksName"           , m_tracksName);
  declareProperty("CheckRate"            , m_checkrate=1000);
  declareProperty("triggerChainName"     , m_triggerChainName);
  declareProperty("trackSelection"       , m_trackSelection);
  declareProperty("VxPrimContainerName"  , m_VxPrimContainerName);
  declareProperty("Pixel_Manager"        , m_Pixel_Manager);
  declareProperty("SCT_Manager"          , m_SCT_Manager);
  declareProperty("TRT_Manager"          , m_TRT_Manager);
  declareProperty("HitQualityTool"       , m_hitQualityTool);
  declareProperty("useExtendedPlots"     , m_extendedPlots = false);
  declareProperty("d0Range"              , m_d0Range);
  declareProperty("d0BsRange"            , m_d0BsRange);
  declareProperty("z0Range"              , m_z0Range);
  declareProperty("etaRange"             , m_etaRange);
  declareProperty("pTRange"              , m_pTRange);
  declareProperty("NTracksRange"         , m_NTracksRange);
  declareProperty("beamCondSvc"          , m_beamCondSvc);
  declareProperty("applyHistWeight"      , m_applyHistWeight = false);
  declareProperty("hWeightInFileName"    , m_hWeightInFileName  = "hWeight.root" ); 
  declareProperty("hWeightHistName"      , m_hWeightHistName    = "trk_pT_vs_eta" );
  declareProperty("doIP"                 , m_doIP = false);
  declareProperty("useLowStat"           , m_useLowStat);
}

//////////////////////////////////////////////////////////
IDAlignMonGenericTracks::~IDAlignMonGenericTracks() { }

//////////////////////////////////////////////////////////
void IDAlignMonGenericTracks::InitializeHistograms() {
  
  // set histograms to NULL
  m_summary = 0;
  
  m_trk_chi2oDoF = 0;
  m_trk_chi2Prob = 0;


  //Histo for self beam spot calculation
  m_trk_d0_vs_phi0_z0 = 0;

  // barrel
  m_trk_d0_barrel = 0;
  m_trk_d0_barrel_zoomin = 0;
  m_trk_d0c_barrel = 0;
  m_trk_z0_barrel = 0;
  m_trk_z0_barrel_zoomin = 0;
  m_trk_qopT_vs_phi_barrel = 0;
  m_trk_d0_vs_phi_barrel = 0;
  m_trk_d0bs_vs_phi_barrel = 0;
  m_trk_d0_vs_z0_barrel = 0;
  m_trk_phi0_neg_barrel = 0;
  m_trk_phi0_pos_barrel = 0;
  m_trk_phi0_asym_barrel = 0;
  m_trk_pT_neg_barrel = 0;
  m_trk_pT_pos_barrel = 0;
  m_trk_pT_asym_barrel = 0;
  m_npixhits_per_track_barrel = 0;
  m_nscthits_per_track_barrel = 0;
  m_ntrthits_per_track_barrel = 0;
  m_chi2oDoF_barrel = 0;
  m_phi_barrel = 0;
  m_hitMap_barrel = 0;
  m_hitMap_barrel_zoom = 0;
  m_hitMap_endcapA = 0;
  m_hitMap_endcapC = 0;

  m_trk_d0_vs_phi_vs_eta_barrel = 0;
  m_trk_pT_vs_eta_barrel = 0;
  m_trk_d0_wrtPV_vs_phi_vs_eta_barrel = 0;
  m_trk_z0_wrtPV_vs_phi_vs_eta_barrel = 0;

  // endcap A
  m_trk_d0_eca = 0 ;
  m_trk_d0_eca_zoomin = 0 ;
  m_trk_d0c_eca = 0;
  m_trk_z0_eca = 0;
  m_trk_z0_eca_zoomin = 0;
  m_trk_qopT_vs_phi_eca = 0;
  m_trk_d0_vs_phi_eca = 0;
  m_trk_d0_vs_z0_eca = 0;
  m_trk_phi0_neg_eca = 0;
  m_trk_phi0_pos_eca = 0;
  m_trk_phi0_asym_eca = 0;
  m_trk_pT_neg_eca = 0;
  m_trk_pT_pos_eca = 0;
  m_trk_pT_asym_eca = 0;
  m_npixhits_per_track_eca = 0;
  m_nscthits_per_track_eca = 0;
  m_ntrthits_per_track_eca = 0;
  m_chi2oDoF_eca = 0;
  m_phi_eca = 0;

  m_trk_d0_vs_phi_vs_eta_eca = 0;
  m_trk_pT_vs_eta_eca = 0;
  m_trk_d0_wrtPV_vs_phi_vs_eta_eca = 0;
  m_trk_z0_wrtPV_vs_phi_vs_eta_eca = 0;


  // endcap C
  m_trk_d0_ecc = 0;
  m_trk_d0_ecc_zoomin = 0;
  m_trk_d0c_ecc = 0;
  m_trk_z0_ecc = 0;
  m_trk_z0_ecc_zoomin = 0;
  m_trk_qopT_vs_phi_ecc = 0;
  m_trk_d0_vs_phi_ecc = 0;
  m_trk_d0_vs_z0_ecc = 0;
  m_trk_phi0_neg_ecc = 0;
  m_trk_phi0_pos_ecc = 0;
  m_trk_phi0_asym_ecc = 0;
  m_trk_pT_neg_ecc = 0;
  m_trk_pT_pos_ecc = 0;
  m_trk_pT_asym_ecc = 0;
  m_npixhits_per_track_ecc = 0;
  m_nscthits_per_track_ecc = 0;
  m_ntrthits_per_track_ecc = 0;
  m_chi2oDoF_ecc = 0;
  m_phi_ecc = 0;

  m_trk_d0_vs_phi_vs_eta_ecc = 0;
  m_trk_pT_vs_eta_ecc = 0;

  m_trk_d0_wrtPV_vs_phi_vs_eta_ecc = 0;
  m_trk_z0_wrtPV_vs_phi_vs_eta_ecc = 0;

  // Whole detector


  m_nhits_per_event = 0;
  m_nhits_per_track = 0;
  m_ntrk = 0;
  m_ngtrk = 0;
  m_nsilhits_per_track = 0;
  m_niblhits_per_track = 0;
  m_npixhits_per_track = 0;
  m_nscthits_per_track = 0;
  m_ntrthits_per_track = 0;
  m_chi2oDoF = 0;
  m_eta = 0;
  m_phi = 0;
  m_z0 = 0;
  m_z0sintheta = 0;
  m_z0_pvcorr = 0;
  m_z0sintheta_pvcorr = 0;
  m_d0 = 0;
  m_d0_pvcorr = 0;
  m_d0_bscorr = 0;
  m_pT = 0;
  m_pTRes = 0;
  m_pTResOverP = 0;
  m_P = 0;
  
  m_trk_d0_vs_phi_vs_eta = 0;
  m_trk_pT_vs_eta = 0;
  
  m_trk_d0_wrtPV_vs_phi_vs_eta = 0;
  m_trk_z0_wrtPV_vs_phi_vs_eta = 0;

  // number of interactions per event
  m_eventMu_vs_TrkPt = 0;
  m_eventMu_vs_TrkEta = 0;

  // extended plots
  m_trk_PIXvSCTHits = 0;
  m_trk_PIXHitsvEta = 0;
  m_trk_SCTHitsvEta = 0;
  m_trk_TRTHitsvEta = 0;
  m_trk_chi2oDoF_Phi = 0;
  m_trk_chi2oDoF_Pt = 0;
  m_trk_chi2oDoF_P = 0;
  m_trk_chi2ProbDist = 0;
  m_errCotTheta = 0;
  m_errCotThetaVsD0BS = 0;
  m_errCotThetaVsPt = 0;
  m_errCotThetaVsP = 0;
  m_errCotThetaVsPhi = 0;
  m_errCotThetaVsEta = 0;
  m_errTheta = 0;
  m_errThetaVsD0BS = 0;
  m_errThetaVsPt = 0;
  m_errThetaVsP = 0;
  m_errThetaVsPhi = 0;
  m_errThetaVsEta = 0;
  m_errD0 = 0;
  m_errD0VsD0BS = 0;
  m_errD0VsPt = 0;
  m_errD0VsP = 0;
  m_errD0VsPhi = 0;
  m_errD0VsPhiBarrel = 0;
  m_errD0VsPhiECA = 0;
  m_errD0VsPhiECC = 0;
  m_errD0VsEta = 0;
  m_errPhi0 = 0;
  m_errPhi0VsD0BS = 0;
  m_errPhi0VsPt = 0;
  m_errPhi0VsP = 0;
  m_errPhi0VsPhi0 = 0;
  m_errPhi0VsEta = 0;
  m_errZ0 = 0;
  m_errZ0VsD0BS = 0;
  m_errZ0VsPt = 0;
  m_errZ0VsP = 0;
  m_errZ0VsPhi0 = 0;
  m_errZ0VsEta = 0;
  m_errPt = 0;
  m_PtVsPhi0Pos = 0;
  m_PtVsPhi0Neg = 0;
  m_errPtVsD0BS = 0;
  m_errPtVsPt = 0;
  m_errPtVsP = 0;
  m_errPt_Pt2 = 0;
  m_errPt_Pt2VsPt = 0;
  m_errPt_Pt2VsPhi0 = 0;
  m_errPt_Pt2VsEta = 0;
  m_errPtVsPhi0 = 0;
  m_errPtVsEta = 0;
  
  m_D0VsPhi0 = 0;
  m_Z0VsEta = 0;
  m_QoverPtVsPhi0 = 0;
  m_QoverPtVsEta = 0;
  m_QPtVsPhi0 = 0;
  m_QPtVsEta = 0;
       
  //BeamSpot Plot = 0s

  m_D0bsVsPhi0 = 0;
  m_D0bsVsPhi0ECC = 0;
  m_D0bsVsPhi0ECA = 0;
  m_D0bsVsPhi0Barrel = 0;
  m_D0bsVsEta = 0;
  m_D0bsVsPt = 0;
  m_D0bsVsPtECC = 0;
  m_D0bsVsPtECA = 0;
  m_D0bsVsPtBarrel = 0;


  //BeamSpot Position Plot = 0s

  m_YBs_vs_XBs = 0;
  m_YBs_vs_ZBs = 0;
  m_XBs_vs_ZBs = 0;

  m_XBs = 0;
  m_YBs = 0;
  m_ZBs = 0;
  m_TiltX_Bs = 0;
  m_TiltY_Bs = 0;

  //versus lumibloc = 0k
  m_XBs_vs_LumiBlock = 0;
  m_YBs_vs_LumiBlock = 0;
  m_ZBs_vs_LumiBlock = 0;
  m_BeamSpotTiltX_vs_LumiBlock = 0;
  m_BeamSpotTiltY_vs_LumiBlock = 0;
  
  // End of extended plots
  
  m_phi_barrel_pos_2_5GeV = 0;
  m_phi_barrel_pos_5_10GeV = 0;
  m_phi_barrel_pos_10_20GeV = 0;
  m_phi_barrel_pos_20plusGeV = 0;
  m_phi_barrel_neg_2_5GeV = 0;
  m_phi_barrel_neg_5_10GeV = 0;
  m_phi_barrel_neg_10_20GeV = 0;
  m_phi_barrel_neg_20plusGeV = 0;

  m_phi_eca_pos_2_5GeV = 0;
  m_phi_eca_pos_5_10GeV = 0;
  m_phi_eca_pos_10_20GeV = 0;
  m_phi_eca_pos_20plusGeV = 0;
  m_phi_eca_neg_2_5GeV = 0;
  m_phi_eca_neg_5_10GeV = 0;
  m_phi_eca_neg_10_20GeV = 0;
  m_phi_eca_neg_20plusGeV = 0;

  m_phi_ecc_pos_2_5GeV = 0;
  m_phi_ecc_pos_5_10GeV = 0;
  m_phi_ecc_pos_10_20GeV = 0;
  m_phi_ecc_pos_20plusGeV = 0;
  m_phi_ecc_neg_2_5GeV = 0;
  m_phi_ecc_neg_5_10GeV = 0;
  m_phi_ecc_neg_10_20GeV = 0;
  m_phi_ecc_neg_20plusGeV = 0;

  m_eta_phi_pos_2_5GeV = 0;
  m_eta_phi_pos_5_10GeV = 0;
  m_eta_phi_pos_10_20GeV = 0;
  m_eta_phi_pos_20plusGeV = 0;
  m_eta_phi_neg_2_5GeV = 0;
  m_eta_phi_neg_5_10GeV = 0;
  m_eta_phi_neg_10_20GeV = 0;
  m_eta_phi_neg_20plusGeV = 0;

  m_Zmumu = 0;
  m_Zmumu_barrel = 0;
  m_Zmumu_eca = 0;
  m_Zmumu_ecc = 0;
  m_Zmumu_barrel_eca = 0;
  m_Zmumu_barrel_ecc = 0;
  m_ZpT_n = 0;
  m_ZpT_p = 0;
  m_ZpT_diff = 0;

  m_pT_n = 0;
  m_pT_p = 0;
  m_pT_diff = 0;
  m_trk_pT_asym = 0;

  m_eta_neg = 0;
  m_eta_pos = 0;
  m_eta_asym = 0;

  m_LumiBlock = 0;
  m_Tracks_per_LumiBlock = 0;

  m_trk_d0c_pos = 0;
  m_trk_d0c_neg = 0;
  m_trk_d0c_pos_barrel = 0;
  m_trk_d0c_neg_barrel = 0;
  m_trk_d0c_pos_eca = 0;
  m_trk_d0c_neg_eca = 0;
  m_trk_d0c_pos_ecc = 0;
  m_trk_d0c_neg_ecc = 0;
  m_trk_d0c_asym = 0;
  m_trk_d0c_asym_barrel = 0;
  m_trk_d0c_asym_eca = 0;
  m_trk_d0c_asym_ecc = 0;

  m_trk_z0c_pos = 0;
  m_trk_z0c_neg = 0;
  m_trk_z0c_pos_barrel = 0;
  m_trk_z0c_neg_barrel = 0;
  m_trk_z0c_pos_eca = 0;
  m_trk_z0c_neg_eca = 0;
  m_trk_z0c_pos_ecc = 0;
  m_trk_z0c_neg_ecc = 0;
  m_trk_z0c_asym = 0;
  m_trk_z0c_asym_barrel = 0;
  m_trk_z0c_asym_eca = 0;
  m_trk_z0c_asym_ecc = 0;

  m_hTrackWeight = 0;

  return;
}

//////////////////////////////////////////////////////////
StatusCode IDAlignMonGenericTracks::initialize()
{
  StatusCode sc;  
  m_events=0;  // event count 
  m_histosBooked = 0; // flag to tell us if histograms have been already booked
                                    
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to initialize ManagedMonitorToolBase! (return SUCCESS !!!)" << endmsg;
    return StatusCode::SUCCESS;
  }

  //initialize tools and services                                                                                                                                                  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Calling initialize() to setup tools/services" << endmsg;
  sc = this->SetupTools();
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to initialize tools/services!" << endmsg;
    return StatusCode::SUCCESS;
  }
  else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Successfully initialized tools/services" << endmsg;
  
  // in case of histograms have to be filled with a weight
  // 1) open the root file containing the weights histogram
  // 2) retrieve the weights histogram
  if ( m_applyHistWeight ){
    ATH_MSG_INFO("applying a weight != 1 for this job");
    m_hWeightInFile =  new TFile( m_hWeightInFileName.c_str() ,"read");
    
    if ( m_hWeightInFile->IsZombie() || !(m_hWeightInFile->IsOpen()) ) {
      ATH_MSG_FATAL( " Problem reading TFile " << m_hWeightInFileName );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Succesful openning of file containing the track weight: " << m_hWeightInFileName);
        
    m_hInputTrackWeight = (TH2F*) m_hWeightInFile -> Get( m_hWeightHistName.c_str() );
    if( !m_hInputTrackWeight ){
      ATH_MSG_FATAL( " Problem getting input track weight histogram. Name " << m_hWeightHistName );
      m_hWeightInFile -> Close();
      delete m_hWeightInFile;
      return StatusCode::FAILURE;
    }
    if( m_hInputTrackWeight ){
      // the input histogram exists. Now, make sure the code knows how to deal with it
      if (m_hWeightHistName.compare("trk_pT_vs_eta")    == 0) m_userInputWeigthMethod = IDAlignMonGenericTracks::TRKETA_TRKPT;
      if (m_hWeightHistName.compare("h_eventMuVsTrkPt") == 0) m_userInputWeigthMethod = IDAlignMonGenericTracks::EVENTMU_TRKPT;
      if (m_hWeightHistName.compare("h_eventMuVsTrkEta")== 0) m_userInputWeigthMethod = IDAlignMonGenericTracks::EVENTMU_TRKETA;
    }
    ATH_MSG_INFO( "Track weights histogram is " << m_hWeightHistName << "  m_userInputWeightMethod= " << m_userInputWeigthMethod);
  } // retrieve track weights histogram 

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////
StatusCode IDAlignMonGenericTracks::SetupTools()
{
  //initializing tools
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setupTools()" << endmsg;
  StatusCode sc;

  //ID Helper
  sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get AtlasDetectorID !" << endmsg;
    return StatusCode::SUCCESS;
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found AtlasDetectorID" << endmsg;
  }
  
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Pixel ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized PixelIDHelper" << endmsg;

  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTIDHelper" << endmsg;

  sc = detStore()->retrieve(m_trtID, "TRT_ID");
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get TRT ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized TRTIDHelper" << endmsg;

  if ( m_trackSelection.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSelection << endmsg;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSelection << endmsg;
  }

  if (m_hitQualityTool.empty()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
      "No hit quality tool configured - not hit quality cuts will be imposed"
  << endmsg;
    m_doHitQuality = false;
  } else if (m_hitQualityTool.retrieve().isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_hitQualityTool 
  <<" (to apply hit quality cuts to Si hits) "<< endmsg;
    m_doHitQuality = false;
   } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
  << "Hit quality tool setup "
  << "- hit quality cuts will be applied to Si hits" << endmsg;
    m_doHitQuality = true;
  }
  
  // retrieving trackToVertexIPEstimator
  //if ( m_trackToVertexIPEstimator.retrieve().isFailure() ) {
  //  ATH_MSG_DEBUG( "Failed to retrieve tool " << m_trackToVertexIPEstimator);
  //m_trackToVertexIPEstimator = 0;
  //} else {
  //ATH_MSG_DEBUG( "Retrieved tool " << m_trackToVertexIPEstimator);  
  //}

  if (m_doIP) {
    ATH_CHECK (m_trackToVertexIPEstimator.retrieve());
  }
  
  if ( m_beamCondSvc.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve beamspot service " << m_beamCondSvc << " - will use nominal beamspot at (0,0,0)" << endmsg;
    m_hasBeamCondSvc = false;
  } 
  else {
    m_hasBeamCondSvc = true;
    ATH_MSG_DEBUG("Retrieved service " << m_beamCondSvc);
  }

  return sc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode IDAlignMonGenericTracks::bookHistograms()
{
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "bookHistograms() START ** track collection: " << m_tracksName 
					<< "  trigchain: " << m_triggerChainName 
					<< "  sequence m_histosBooked=" << m_histosBooked
					<< " !!!" << endmsg;

  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Running in online mode "<< endmsg;
  }
  
  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "bookHistograms() Running in cosmic mode "<< endmsg;
  } 
  else {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "bookHistograms() Running in collision mode "<< endmsg;
  }

  std::string outputDirName = "IDAlignMon/" + m_tracksName + "_" + m_triggerChainName + "/GenericTracks";
  MonGroup al_mon ( this, outputDirName, run );

  if ( newLowStatFlag() ) {  
  }
  if ( newLumiBlockFlag() ) {  
  }
  if ( newRunFlag() ) {  
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "newRunFlag() true --> booking GenericTracks histograms for track collection: " << m_tracksName 
					  << "  output dir: " << outputDirName.data()
					  << "  AthenaMonManager::environment()=" << AthenaMonManager::environment()
					  << endmsg;
    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) {
      if (msgLvl(MSG::INFO))msg(MSG::INFO) << "** Beware !! histograms for  track collection: " << m_tracksName 
					   << " trigchain: " << m_triggerChainName
					   << "  ALREADY BOOKED -> return success" << endmsg;
      return StatusCode::SUCCESS;
    }

    // summary histo
    const Int_t nx = 13;
    TString hitSummary[nx] = {"IBLhits #geq 1", "PixHits #geq 3","SCTHits #geq 8","TRTHits #geq 20","PixHitsB #geq 3","SCTHitsB #geq 8","TRTHitsB #geq 20","PixHitsECA #geq 2","SCTHitsECA #geq 2","TRTHitsECA #geq 15","PixHitsECC #geq 2","SCTHitsECC #geq 2","TRTHitsECC #geq 15"};   
    m_summary = new TH1F("summary","summary",13,-0.5,12.5); 
    for (int i=1;i<=m_summary->GetNbinsX();i++) m_summary->GetXaxis()->SetBinLabel(i,hitSummary[i-1]);  
    m_summary->GetYaxis()->SetTitle("Number of Tracks");    
    RegisterHisto(al_mon,m_summary);
    if (msgLvl(MSG::INFO))msg(MSG::INFO) << " summary histogram already booked for histogram sequence: " << m_histosBooked << endmsg;

    // IP vs PV TH3 histograms
    if(m_doIP){
      m_trk_d0_wrtPV_vs_phi_vs_eta       = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta"       , "d0 vs phi vs eta"           , 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -0.5, 0.5);
      m_trk_d0_wrtPV_vs_phi_vs_eta_barrel= new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_barrel", "d0 vs phi vs eta (Barrel)"  , 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -0.5, 0.5);
      m_trk_d0_wrtPV_vs_phi_vs_eta_ecc   = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_ecc"   , "d0 vs phi vs eta (Endcap C)", 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -0.5, 0.5);
      m_trk_d0_wrtPV_vs_phi_vs_eta_eca   = new TH3F("trk_d0_wrtPV_vs_phi_vs_eta_eca"   , "d0 vs phi vs eta (Endcap A)", 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -0.5, 0.5);
  
      m_trk_z0_wrtPV_vs_phi_vs_eta       = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta"       , "d0 vs phi vs eta"           , 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -1, 1);
      m_trk_z0_wrtPV_vs_phi_vs_eta_barrel= new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_barrel", "d0 vs phi vs eta (Barrel)"  , 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -1, 1);
      m_trk_z0_wrtPV_vs_phi_vs_eta_ecc   = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_ecc"   , "d0 vs phi vs eta (Endcap C)", 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -1, 1);
      m_trk_z0_wrtPV_vs_phi_vs_eta_eca   = new TH3F("trk_z0_wrtPV_vs_phi_vs_eta_eca"   , "d0 vs phi vs eta (Endcap A)", 100, -m_etaRange, m_etaRange, 40,0,2*m_Pi, 100, -1, 1);

      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta       );
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_barrel);
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_ecc   );
      RegisterHisto(al_mon, m_trk_d0_wrtPV_vs_phi_vs_eta_eca   );
      
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta       );
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_barrel);
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_ecc   );
      RegisterHisto(al_mon, m_trk_z0_wrtPV_vs_phi_vs_eta_eca   );
    } // m_doIP
    
    
    
    m_nhits_per_event = new TH1F("Nhits_per_event","Number of hits per event",1024, -1., 2047.);  
    RegisterHisto(al_mon,m_nhits_per_event) ;  
    m_nhits_per_event->GetXaxis()->SetTitle("Number of Hits on Tracks per Event"); 
    m_nhits_per_event->GetYaxis()->SetTitle("Number of Events");
 
    
    m_ntrk = new TH1F("ntracks","Number of Tracks",m_NTracksRange+1, -0.5, m_NTracksRange+0.5);
    RegisterHisto(al_mon,m_ntrk);
    m_ngtrk = new TH1F("ngtracks","Number of Good Tracks",m_NTracksRange+1, -0.5, m_NTracksRange+0.5);
    RegisterHisto(al_mon,m_ngtrk);

    m_nhits_per_track = new TH1F("Nhits_per_track","Number of hits per track", 76, -0.5, 75.5);  
    RegisterHisto(al_mon,m_nhits_per_track) ; 
    m_nhits_per_track->GetXaxis()->SetTitle("Number of Hits per Track"); 
    m_nhits_per_track->GetYaxis()->SetTitle("Number of Tracks"); 
 
    m_npixhits_per_track_barrel = new TH1F("Npixhits_per_track_barrel","Number of hits in Pixels and IBL per track (Barrel)",12,-0.5,11.5);  
    RegisterHisto(al_mon,m_npixhits_per_track_barrel) ;  
    m_npixhits_per_track_barrel->GetXaxis()->SetTitle("Number of Pixel hits per Track in Barrel"); 
    m_npixhits_per_track_barrel->GetYaxis()->SetTitle("Number of Tracks"); 

    m_nscthits_per_track_barrel = new TH1F("Nscthits_per_track_barrel","Number of scthits per track (Barrel)",26,-0.5,25.5);  
    RegisterHisto(al_mon,m_nscthits_per_track_barrel) ;  
    m_nscthits_per_track_barrel->GetXaxis()->SetTitle("Number of SCT Hits per Track in Barrel"); 
    m_nscthits_per_track_barrel->GetYaxis()->SetTitle("Number of Tracks"); 

    m_ntrthits_per_track_barrel = new TH1F("Ntrthits_per_track_barrel","Number of TRT hits per track (Barrel)", 81,-0.5,80.5);  
    RegisterHisto(al_mon,m_ntrthits_per_track_barrel) ; 
    m_ntrthits_per_track_barrel->GetXaxis()->SetTitle("Number of TRT Hits per Track in Barrel"); 
    m_ntrthits_per_track_barrel->GetYaxis()->SetTitle("Number of Tracks");   

    m_npixhits_per_track_eca = new TH1F("Npixhits_per_track_eca","Number of pixhits per track (ECA)",12,-0.5,11.5);  
    RegisterHisto(al_mon,m_npixhits_per_track_eca) ;  
    m_npixhits_per_track_eca->GetXaxis()->SetTitle("Number of Pixel Hits per Track in ECA"); 
    m_npixhits_per_track_eca->GetYaxis()->SetTitle("Number of Tracks"); 

    m_nscthits_per_track_eca = new TH1F("Nscthits_per_track_eca","Number of scthits per track (ECA)",26,-0.5,25.5);  
    RegisterHisto(al_mon,m_nscthits_per_track_eca) ;  
    m_nscthits_per_track_eca->GetXaxis()->SetTitle("Number of SCT Hits per Track in ECA"); 
    m_nscthits_per_track_eca->GetYaxis()->SetTitle("Number of Tracks"); 
    m_ntrthits_per_track_eca = new TH1F("Ntrthits_per_track_eca","Number of trthits per track (ECA)", 81,-0.5,80.5);  
    RegisterHisto(al_mon,m_ntrthits_per_track_eca) ;   
    m_ntrthits_per_track_eca->GetXaxis()->SetTitle("Number of TRT Hits per Track in ECA"); 
    m_ntrthits_per_track_eca->GetYaxis()->SetTitle("Number of Tracks");   
 
    m_npixhits_per_track_ecc = new TH1F("Npixhits_per_track_ecc","Number of pixhits per track (ECC)",12,-0.5,11.5);  
    RegisterHisto(al_mon,m_npixhits_per_track_ecc) ;  
    m_npixhits_per_track_ecc->GetXaxis()->SetTitle("Number of Pixel Hits per Track in ECC"); 
    m_npixhits_per_track_ecc->GetYaxis()->SetTitle("Number of Tracks"); 
    m_nscthits_per_track_ecc = new TH1F("Nscthits_per_track_ecc","Number of scthits per track (ECC)",26,-0.5,25.5);  
    RegisterHisto(al_mon,m_nscthits_per_track_ecc) ;  
    m_nscthits_per_track_ecc->GetXaxis()->SetTitle("Number of SCT Hits per Track in ECC"); 
    m_nscthits_per_track_ecc->GetYaxis()->SetTitle("Number of Tracks"); 
    m_ntrthits_per_track_ecc = new TH1F("Ntrthits_per_track_ecc","Number of trthits per track (ECC)", 81,-0.5,80.5);  
    RegisterHisto(al_mon,m_ntrthits_per_track_ecc) ;    
    m_ntrthits_per_track_ecc->GetXaxis()->SetTitle("Number of TRT Hits per Track in ECC"); 
    m_ntrthits_per_track_ecc->GetYaxis()->SetTitle("Number of Tracks");  

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Monitoring plots shown in the dqm web page
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    m_chi2oDoF = new TH1F("chi2oDoF","chi2oDoF",100,0,10);  
    RegisterHisto(al_mon,m_chi2oDoF) ;  
    m_chi2oDoF->GetXaxis()->SetTitle("Track #chi^{2} / NDoF"); 
    m_chi2oDoF->GetYaxis()->SetTitle("Number of Tracks");  
    m_eta = new TH1F("eta","eta",100,-m_etaRange,m_etaRange);  
    RegisterHisto(al_mon,    m_eta) ;  
    m_eta->GetXaxis()->SetTitle("Track #eta"); 
    m_eta->GetYaxis()->SetTitle("Number of Tracks"); 

    m_phi = new TH1F("phi","phi",100,0,2*m_Pi);  m_phi->SetMinimum(0);
    RegisterHisto(al_mon,   m_phi);
    m_phi->GetXaxis()->SetTitle("Track #phi [rad]"); 
    m_phi->GetYaxis()->SetTitle("Number of Tracks");  

    m_d0_bscorr = new TH1F("d0_bscorr","d0 (corrected for beamspot); [mm]",250, -m_d0BsRange,m_d0BsRange);  
    RegisterHisto(al_mon   ,m_d0_bscorr) ;  
    m_d0_bscorr->GetXaxis()->SetTitle("Track d_{0} [mm]"); 
    m_d0_bscorr->GetYaxis()->SetTitle("Number of Tracks");  

    m_z0 = new TH1F("z0","z0;[mm]",100,-m_z0Range,m_z0Range);  
    RegisterHisto(al_mon,    m_z0) ;  
    m_z0->GetXaxis()->SetTitle("Track z_{0} [mm]"); 
    m_z0->GetYaxis()->SetTitle("Number of Tracks");  
    m_z0sintheta = new TH1F("z0sintheta","z0sintheta",100,-m_z0Range,m_z0Range);  
    RegisterHisto(al_mon,m_z0sintheta) ;  
    m_z0sintheta->GetXaxis()->SetTitle("Track z_{0}sin#theta [mm]"); 
    m_z0sintheta->GetYaxis()->SetTitle("Number of Tracks");  

    m_d0 = new TH1F("d0","d0;[mm]",250,-m_d0Range,m_d0Range);  
    RegisterHisto(al_mon,m_d0) ;  

    m_nsilhits_per_track = new TH1F("Nsilhits_per_track","Number of hits in silicon (IBL+Pixels+SCT) per track",26,-0.5,25.5);  
    RegisterHisto(al_mon,    m_nsilhits_per_track) ;  
    m_nsilhits_per_track->GetXaxis()->SetTitle("Number of silicon hits per track"); 
    m_nsilhits_per_track->GetYaxis()->SetTitle("Number of tracks"); 

    m_niblhits_per_track = new TH1F("Niblhits_per_track","Number of hits in IBL per track",6,-0.5,5.5);  
    RegisterHisto(al_mon,    m_niblhits_per_track) ;  
    m_niblhits_per_track->GetXaxis()->SetTitle("Number of IBL hits per Track"); 
    m_niblhits_per_track->GetYaxis()->SetTitle("Number of Tracks"); 
    
    m_npixhits_per_track = new TH1F("Npixhits_per_track","Number of Pixels and IBL hits per track",12,-0.5,11.5);  
    RegisterHisto(al_mon,    m_npixhits_per_track) ;  
    m_npixhits_per_track->GetXaxis()->SetTitle("Number of Pixel and IBL hits per Track"); 
    m_npixhits_per_track->GetYaxis()->SetTitle("Number of Tracks"); 

    m_nscthits_per_track = new TH1F("Nscthits_per_track","Number of SCT hits per track",26,-0.5,25.5);  
    RegisterHisto(al_mon,    m_nscthits_per_track) ;  
    m_nscthits_per_track->GetXaxis()->SetTitle("Number of SCT Hits per Track"); 
    m_nscthits_per_track->GetYaxis()->SetTitle("Number of Tracks"); 

    m_ntrthits_per_track = new TH1F("Ntrthits_per_track","Number of TRT hits per track",81,-0.5,80.5);  
    RegisterHisto(al_mon,    m_ntrthits_per_track) ;  
    m_ntrthits_per_track->GetXaxis()->SetTitle("Number of TRT Hits per Track"); 
    m_ntrthits_per_track->GetYaxis()->SetTitle("Number of Tracks"); 

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        

    m_trk_qopT_vs_phi_barrel = new TProfile("trk_qopT_vs_phi_barrel","Q/pT versus phi0 (Barrel)",20,0,2*m_Pi,-10.0,10.0);  
    RegisterHisto(al_mon,m_trk_qopT_vs_phi_barrel) ;
    m_trk_d0_vs_phi_barrel = new TProfile("trk_d0_vs_phi_barrel","Impact parameter (d_{0})versus phi0 (Barrel)",20,0,2*m_Pi,-5,5);  
    RegisterHisto(al_mon,m_trk_d0_vs_phi_barrel) ;
    m_trk_d0bs_vs_phi_barrel = new TProfile("trk_d0bs_vs_phi_barrel","Impact parameter (d_{0BS}) versus phi0 (Barrel)",20,0,2*m_Pi,-5,5);  
    RegisterHisto(al_mon,m_trk_d0bs_vs_phi_barrel) ;
    m_trk_d0_vs_z0_barrel = new TProfile("trk_d0_vs_z0_barrel","Impact parameter versus z0 (Barrel)",100,-200,200,-5,5);

    m_trk_qopT_vs_phi_eca = new TProfile("trk_qopT_vs_phi_eca","Q/pT versus phi0 (Endcap A)",20,0,2*m_Pi,-10.0,10.0);  
    RegisterHisto(al_mon,m_trk_qopT_vs_phi_eca) ;
    m_trk_d0_vs_phi_eca = new TProfile("trk_d0_vs_phi_eca","Impact parameter versus phi0 (Endcap A)",20,0,2*m_Pi,-5,5);  
    RegisterHisto(al_mon,m_trk_d0_vs_phi_eca) ;
    m_trk_d0_vs_z0_eca = new TProfile("trk_d0_vs_z0_eca","Impact parameter versus z0 (Endcap A)",100,-200,200,-5,5);  
    RegisterHisto(al_mon,m_trk_d0_vs_z0_eca) ;
    m_trk_qopT_vs_phi_ecc = new TProfile("trk_qopT_vs_phi_ecc","Q/pT versus phi0 (Endcap C)",20,0,2*m_Pi,-10.0,10.0);  
    RegisterHisto(al_mon,m_trk_qopT_vs_phi_ecc) ;
    m_trk_d0_vs_phi_ecc = new TProfile("trk_d0_vs_phi_ecc","Impact parameter versus phi0 (Endcap C)",20,0,2*m_Pi,-5,5);  
    RegisterHisto(al_mon,m_trk_d0_vs_phi_ecc) ;
    m_trk_d0_vs_z0_ecc = new TProfile("trk_d0_vs_z0_ecc","Impact parameter versus z0 (Endcap C)",100,-200,200,-5,5);  
    RegisterHisto(al_mon,m_trk_d0_vs_z0_ecc) ;


    //Negative and positive tracks perigee parameters
    RegisterHisto(al_mon,m_trk_d0_vs_z0_barrel) ;
    m_trk_phi0_neg_barrel = new TH1F("trk_phi0_neg_barrel","Phi distribution for negative tracks (Barrel)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_neg_barrel);
    m_trk_phi0_pos_barrel = new TH1F("trk_phi0_pos_barrel","Phi distribution for positive tracks (Barrel)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_pos_barrel);
    m_trk_pT_neg_barrel = new TH1F("trk_pT_neg_barrel","pT distribution for negative tracks (Barrel)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_neg_barrel);
    m_trk_pT_pos_barrel = new TH1F("trk_pT_pos_barrel","pT distribution for positive tracks (Barrel)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_pos_barrel);
    m_trk_phi0_neg_eca = new TH1F("trk_phi0_neg_eca","Phi distribution for negative tracks (Endcap A)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_neg_eca);
    m_trk_phi0_pos_eca = new TH1F("trk_phi0_pos_eca","Phi distribution for positive tracks (Endcap A)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_pos_eca);
    m_trk_pT_neg_eca = new TH1F("trk_pT_neg_eca","pT distribution for negative tracks (Endcap A)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_neg_eca);
    m_trk_pT_pos_eca = new TH1F("trk_pT_pos_eca","pT distribution for positive tracks (Endcap A)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_pos_eca);
    
    m_trk_phi0_neg_ecc = new TH1F("trk_phi0_neg_ecc","Phi distribution for negative tracks (Endcap C)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_neg_ecc);
    m_trk_phi0_pos_ecc = new TH1F("trk_phi0_pos_ecc","Phi distribution for positive tracks (Endcap C)",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_pos_ecc);
    m_trk_pT_neg_ecc = new TH1F("trk_pT_neg_ecc","pT distribution for negative tracks (Endcap C)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_neg_ecc);
    m_trk_pT_pos_ecc = new TH1F("trk_pT_pos_ecc","pT distribution for positive tracks (Endcap C)",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_pos_ecc);
    
    //Asymmetry plots. Useful to spot weak modes
    m_trk_phi0_asym_barrel = new TH1F("trk_phi0_asym_barrel","Track Charge Asymmetry versus phi (Barrel) ",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_asym_barrel);
    m_trk_phi0_asym_eca = new TH1F("trk_phi0_asym_eca","Track Charge Asymmetry versus phi (Endcap A) ",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_asym_eca);
    m_trk_phi0_asym_ecc = new TH1F("trk_phi0_asym_ecc","Track Charge Asymmetry versus phi (Endcap C) ",20,0,2*m_Pi);
    RegisterHisto(al_mon,m_trk_phi0_asym_ecc);
    m_trk_pT_asym_barrel = new TH1F("trk_pT_asym_barrel","Track Charge Asymmetry versus pT (Barrel) ",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_asym_barrel);
    m_trk_pT_asym_eca = new TH1F("trk_pT_asym_eca","Track Charge Asymmetry versus pT (Endcap A) ",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_asym_eca);
    m_trk_pT_asym_ecc = new TH1F("trk_pT_asym_ecc","Track Charge Asymmetry versus pT (Endcap C) ",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_asym_ecc);
    
    
    // 
    // histogram of track Pt vs track eta
    int etaBins = 40;
    int ptBins = 40;
    m_trk_pT_vs_eta = new TH2F("trk_pT_vs_eta", "pT vs eta;#eta;p_{T} [GeV] ", 
			       etaBins, -m_etaRange, m_etaRange,  ptBins, 0., m_pTRange);
    RegisterHisto(al_mon, m_trk_pT_vs_eta);

    // histograms of number of intractions per event vs track pt and vs track eta
    m_eventMu_vs_TrkPt  = new TH2F("h_eventMuVsTrkPt", "Event #mu vs track p_{T};#mu;p_{T} [GeV]", 
				   m_nBinsMuRange, m_muRangeMin, m_muRangeMax,  ptBins, 0., m_pTRange);
    RegisterHisto(al_mon, m_eventMu_vs_TrkPt); 

    m_eventMu_vs_TrkEta = new TH2F("h_eventMuVsTrkEta", "Event #mu vs track #eta;#mu; #eta",        
				   m_nBinsMuRange, m_muRangeMin, m_muRangeMax,  etaBins, -m_etaRange, m_etaRange);
    RegisterHisto(al_mon, m_eventMu_vs_TrkEta); 

    // number of interactions
    m_mu_perEvent = new TH1F("mu_perEvent","#mu per event", m_nBinsMuRange, m_muRangeMin, m_muRangeMax);
    RegisterHisto(al_mon,m_mu_perEvent);
    m_mu_perEvent->GetXaxis()->SetTitle("#mu");
    m_mu_perEvent->GetYaxis()->SetTitle("Events");
    
    if(m_extendedPlots){
      // histo for self beam spot calculation (using the data of the whole run... so nothing to do with the beam spot by Lumiblock).
      m_trk_d0_vs_phi0_z0 = new TH3F("trk_d0_vs_phi0_z0","Track d_{0} vs #phi_{0} and z_{0}", 30, 0., 2*m_Pi, 20, -m_z0Range/2, m_z0Range/2, 30, -m_d0Range, m_d0Range);
      m_trk_d0_vs_phi0_z0->SetXTitle("Track #phi_{0} [rad]");
      m_trk_d0_vs_phi0_z0->SetYTitle("Track z_{0} [mm]");
      RegisterHisto(al_mon, m_trk_d0_vs_phi0_z0);

      m_trk_d0_vs_phi_vs_eta         = new TH3F("trk_d0_vs_phi_vs_eta"       , "d0 vs phi vs eta", 
						etaBins, -m_etaRange, m_etaRange,  40, 0, 2*m_Pi,  100, -m_d0Range, m_d0Range); 
      m_trk_d0_vs_phi_vs_eta_barrel  = new TH3F("trk_d0_vs_phi_vs_eta_barrel", "d0 vs phi vs eta (Barrel)", 
						etaBins, -m_etaRange, m_etaRange,  40, 0, 2*m_Pi,  100, -m_d0Range, m_d0Range);
      m_trk_d0_vs_phi_vs_eta_ecc     = new TH3F("trk_d0_vs_phi_vs_eta_ecc"   , "d0 vs phi vs eta (Endcap C)", 
						etaBins, -m_etaRange, m_etaRange,  40, 0, 2*m_Pi,  100, -m_d0Range, m_d0Range);
      m_trk_d0_vs_phi_vs_eta_eca     = new TH3F("trk_d0_vs_phi_vs_eta_eca"   , "d0 vs phi vs eta (Endcap A)", 
						etaBins, -m_etaRange, m_etaRange,  40, 0, 2*m_Pi,  100, -m_d0Range, m_d0Range);
      m_trk_pT_vs_eta_barrel         = new TH2F("trk_pT_vs_eta_barrel"       , "pT vs eta barrel;#eta;p_{T} [GeV]", 
						etaBins, -m_etaRange, m_etaRange, ptBins, 0., m_pTRange);
      m_trk_pT_vs_eta_eca            = new TH2F("trk_pT_vs_eta_eca"          , "pT vs eta eca;#eta;p_{T} [GeV]", 
						etaBins, -m_etaRange, m_etaRange, ptBins, 0., m_pTRange);
      m_trk_pT_vs_eta_ecc            = new TH2F("trk_pT_vs_eta_ecc"          , "pT vs eta ecc;#eta;p_{T} [GeV]", 
						etaBins, -m_etaRange, m_etaRange, ptBins, 0., m_pTRange);
      
      
      RegisterHisto(al_mon, m_trk_d0_vs_phi_vs_eta        );
      RegisterHisto(al_mon, m_trk_d0_vs_phi_vs_eta_barrel );
      RegisterHisto(al_mon, m_trk_pT_vs_eta_barrel    );
      RegisterHisto(al_mon, m_trk_d0_vs_phi_vs_eta_ecc    );
      RegisterHisto(al_mon, m_trk_pT_vs_eta_ecc           );
      RegisterHisto(al_mon, m_trk_d0_vs_phi_vs_eta_eca    );
      RegisterHisto(al_mon, m_trk_pT_vs_eta_eca     );

      m_trk_d0_barrel  = new TH1F("trk_d0_barrel","Impact parameter: all tracks (Barrel); d_{0} [mm]",100, -m_d0Range, m_d0Range);
      m_trk_d0c_barrel = new TH1F("trk_d0c_barrel","Impact parameter (corrected for vertex): all tracks (Barrel)",100, -5, 5);  
      m_trk_z0_barrel  = new TH1F("trk_z0_barrel","Track z0: all tracks (Barrel)",100, -m_z0Range, m_z0Range);
      m_trk_d0_eca     = new TH1F("trk_d0_eca","Impact parameter: all tracks (Endcap A)",100, -m_d0Range, m_d0Range);
      m_trk_d0c_eca    = new TH1F("trk_d0c_eca","Impact parameter (corrected for vertex): all tracks  (Endcap A)",100, -m_d0Range, m_d0Range);  
      m_trk_z0_eca     = new TH1F("trk_z0_eca","Track z0: all tracks (Endcap A)",100, -m_z0Range, m_z0Range);
      m_trk_d0_ecc     = new TH1F("trk_d0_ecc","Impact parameter: all tracks (Endcap C)",100, -m_d0Range, m_d0Range);
      m_trk_d0c_ecc    = new TH1F("trk_d0c_ecc","Impact parameter (corrected for vertex): all tracks  (Endcap C)",100, -m_d0Range, m_d0Range);  
      m_trk_z0_ecc     = new TH1F("trk_z0_ecc","Track z0: all tracks (Endcap C)",100, -m_z0Range, m_z0Range);


      RegisterHisto(al_mon,m_trk_d0_barrel);
      RegisterHisto(al_mon,m_trk_d0c_barrel);
      RegisterHisto(al_mon,m_trk_z0_barrel) ; 
      RegisterHisto(al_mon,m_trk_d0_eca) ;  
      RegisterHisto(al_mon,m_trk_d0c_eca) ;  
      RegisterHisto(al_mon,m_trk_z0_eca) ;  
      RegisterHisto(al_mon,m_trk_d0_ecc) ;  
      RegisterHisto(al_mon,m_trk_d0c_ecc) ;
      RegisterHisto(al_mon,m_trk_z0_ecc) ;  
  
      

      m_trk_chi2oDoF = new TProfile("trk_chi2oDoF","chi2oDoF versus eta",100,-m_etaRange,m_etaRange,-5,5);  
      RegisterHisto(al_mon,m_trk_chi2oDoF) ;
      m_trk_chi2Prob = new TProfile("trk_chi2Prob","chi2Prob versus eta",100,-m_etaRange,m_etaRange,-5,5);  
      RegisterHisto(al_mon,m_trk_chi2Prob) ;

    

      
      //Detailed IP Plots. All the PV Corrected are broken and need to be fixed.
      m_trk_d0_barrel_zoomin = new TH1F("trk_d0_barrel_zoomin","Impact parameter: all tracks (Barrel)",100,-5,5);
      m_trk_z0_barrel_zoomin = new TH1F("trk_z0_barrel_zoomin","Track z0: all tracks (Barrel)",100,-300,300);  
      m_trk_d0_eca_zoomin = new TH1F("trk_d0_eca_zoomin","Impact parameter: all tracks (Endcap A)",100,-5,5);
      m_trk_z0_eca_zoomin = new TH1F("trk_z0_eca_zoomin","Track z0: all tracks (Endcap A)",100,-300,300);
      m_trk_d0_ecc_zoomin = new TH1F("trk_d0_ecc_zoomin","Impact parameter: all tracks (Endcap C)",100,-5,5);
      m_trk_z0_ecc_zoomin = new TH1F("trk_z0_ecc_zoomin","Track z0: all tracks (Endcap C)",100,-300,300);

      RegisterHisto(al_mon,m_trk_d0_barrel_zoomin);
      RegisterHisto(al_mon,m_trk_z0_barrel_zoomin) ; 
      RegisterHisto(al_mon,m_trk_d0_eca_zoomin) ;  
      RegisterHisto(al_mon,m_trk_z0_eca_zoomin) ; 
      RegisterHisto(al_mon,m_trk_d0_ecc_zoomin) ;  
      RegisterHisto(al_mon,m_trk_z0_ecc_zoomin) ; 
      
      
      
      //############### 
      double z0cRange=5;
      double d0cRange=0.1;
      m_trk_d0c_neg= new TH1F("trk_d0c_neg","Impact parameter: all negative charged tracks" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_neg);
      m_trk_d0c_pos= new TH1F("trk_d0c_pos","Impact parameter: all positive charged tracks" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_pos);
      m_trk_d0c_neg_barrel= new TH1F("trk_d0c_neg_barrel","Impact parameter: all negative charged tracks (Barrel)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_neg_barrel);
      m_trk_d0c_pos_barrel= new TH1F("trk_d0c_pos_barrel","Impact parameter: all positive charged tracks (Barrel)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_pos_barrel);
      m_trk_d0c_neg_eca= new TH1F("trk_d0c_neg_eca","Impact parameter: all negative charged tracks (Endcap A)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_neg_eca);
      m_trk_d0c_pos_eca= new TH1F("trk_d0c_pos_eca","Impact parameter: all positive charged tracks (Endcap A)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_pos_eca);
      m_trk_d0c_neg_ecc= new TH1F("trk_d0c_neg_ecc","Impact parameter: all negative charged tracks (Endcap C)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_neg_ecc);
      m_trk_d0c_pos_ecc= new TH1F("trk_d0c_pos_ecc","Impact parameter: all positive charged tracks (Endcap C)" ,50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_pos_ecc);
      
      m_trk_d0c_asym=new TH1F("trk_d0c_asym","Track Charge Asymmetry versus d0 (corrected for vertex)",50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_asym);
      m_trk_d0c_asym->GetYaxis()->SetTitle("d_0 (mm)");   
      m_trk_d0c_asym->GetYaxis()->SetTitle("(pos-neg)/(pos+neg)");   
      
      m_trk_d0c_asym_barrel=new TH1F("trk_d0c_asym_barrel","Track Charge Asymmetry versus d0 (Barrel, corrected for vertex)",50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_asym_barrel);
      m_trk_d0c_asym_eca=new TH1F("trk_d0c_asym_eca","Track Charge Asymmetry versus d0(Endcap A, corrected for vertex)",50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_asym_eca);
      m_trk_d0c_asym_ecc=new TH1F("trk_d0c_asym_ecc","Track Charge Asymmetry versus d0(Endcap C, corrected for vertex)",50,-d0cRange,d0cRange);
      RegisterHisto(al_mon,m_trk_d0c_asym_ecc);
      
      m_trk_z0c_neg= new TH1F("trk_z0c_neg","z0: all negative charged tracks" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_neg);
      m_trk_z0c_pos= new TH1F("trk_z0c_pos","z0: all positive charged tracks" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_pos);
      m_trk_z0c_neg_barrel= new TH1F("trk_z0c_neg_barrel",":z0 all negative charged tracks (Barrel)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_neg_barrel);
      m_trk_z0c_pos_barrel= new TH1F("trk_z0c_pos_barrel","z0: all positive charged tracks (Barrel)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_pos_barrel);
      m_trk_z0c_neg_eca= new TH1F("trk_z0c_neg_eca","z0: all negative charged tracks (Endcap A)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_neg_eca);
      m_trk_z0c_pos_eca= new TH1F("trk_z0c_pos_eca","z0: all positive charged tracks (Endcap A)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_pos_eca);
      m_trk_z0c_neg_ecc= new TH1F("trk_z0c_neg_ecc","z0: all negative charged tracks (Endcap C)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_neg_ecc);
      m_trk_z0c_pos_ecc= new TH1F("trk_z0c_pos_ecc","z0: all positive charged tracks (Endcap C)" ,50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_pos_ecc);
      m_trk_z0c_asym=new TH1F("trk_z0c_asym","Track Charge Asymmetry versus z0 (corrected for vertex)",50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_asym);
      m_trk_z0c_asym_barrel=new TH1F("trk_z0c_asym_barrel","Track Charge Asymmetry versus z0 (Barrel, corrected for vertex)",50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_asym_barrel);
      m_trk_z0c_asym_eca=new TH1F("trk_z0c_asym_eca","Track Charge Asymmetry versus z0(Endcap A, corrected for vertex)",50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_asym_eca);
      m_trk_z0c_asym_ecc=new TH1F("trk_z0c_asym_ecc","Track Charge Asymmetry versus z0(Endcap C, corrected for vertex)",50,-z0cRange,z0cRange);
      RegisterHisto(al_mon,m_trk_z0c_asym_ecc);
      
      
      
      
      //PV corrected plots. Broken. 
      m_d0_pvcorr = new TH1F("d0_pvcorr","d0 (corrected for primVtx); [mm]",400,-m_d0Range,m_d0Range);  
      RegisterHisto(al_mon,m_d0_pvcorr) ; 
      m_z0_pvcorr = new TH1F("z0_pvcorr","z0 (corrected for primVtx);[mm]",100,-m_z0Range,m_z0Range);  
      RegisterHisto(al_mon,m_z0_pvcorr) ;  
      m_z0sintheta_pvcorr = new TH1F("z0sintheta_pvcorr","z*sintheta (corrected for primVtx); [mm]",100,-m_z0Range,m_z0Range);  
      RegisterHisto(al_mon,m_z0sintheta_pvcorr) ;  
      
      
      //hits
      m_trk_PIXvSCTHits = new TH2F ("PIXvSCTHits", "Hits On track", 30, -0.5, 30-0.5, 11, -0.5,  11-0.5);
      RegisterHisto(al_mon,m_trk_PIXvSCTHits) ;
      m_trk_PIXvSCTHits->GetXaxis()->SetTitle("# SCT hits on track");
      m_trk_PIXvSCTHits->GetYaxis()->SetTitle("# PIX hits on track");
      
      m_trk_PIXHitsvEta = new TH2F ("PIXHitsvEta", "PIX Hits On track", 50, -3., 3., 11, -0.5,  11-0.5);
      RegisterHisto(al_mon,m_trk_PIXHitsvEta) ;
      m_trk_PIXHitsvEta->GetXaxis()->SetTitle("#eta");
      m_trk_PIXHitsvEta->GetYaxis()->SetTitle("# PIX hits on track");
      
      m_trk_SCTHitsvEta = new TH2F ("SCTHitsvEta", "SCT Hits On track", 50, -3., 3., 21, -0.5,  21-0.5);
      RegisterHisto(al_mon,m_trk_SCTHitsvEta) ;
      m_trk_SCTHitsvEta->GetXaxis()->SetTitle("#eta");
      m_trk_SCTHitsvEta->GetYaxis()->SetTitle("# SCT hits on track");
      
      m_trk_TRTHitsvEta= new TH2F ("TRTHitsvEta", "TRT Hits On track", 50, -3., 3., 51, -0.5,  51-0.5);
      RegisterHisto(al_mon,m_trk_TRTHitsvEta) ;
      m_trk_TRTHitsvEta->GetXaxis()->SetTitle("#eta");
      m_trk_TRTHitsvEta->GetYaxis()->SetTitle("# TRT hits on track");
      

      m_trk_chi2oDoF_Phi = new TH2F("trk_chi2oDoFvsPhi","chi2oDoF versus phi",100,0,2*m_Pi,50,-5,5);  
      RegisterHisto(al_mon,m_trk_chi2oDoF_Phi) ;
      m_trk_chi2oDoF_Phi->GetXaxis()->SetTitle("#phi (rad)"); 
      m_trk_chi2oDoF_Phi->GetYaxis()->SetTitle("Chi2"); 
      
      m_trk_chi2oDoF_Pt = new TH2F("trk_chi2oDoFvsPt","chi2oDoF versus Pt",100,-40,40,50,-5,5);  
      RegisterHisto(al_mon,m_trk_chi2oDoF_Pt) ;
      m_trk_chi2oDoF_Pt->GetXaxis()->SetTitle("Pt (GeV)"); 
      m_trk_chi2oDoF_Pt->GetYaxis()->SetTitle("Chi2"); 
      
      m_trk_chi2oDoF_P = new TH2F("trk_chi2oDoFvsP","chi2oDoF versus P",100,-40,40,50,-5,5);  
      RegisterHisto(al_mon,m_trk_chi2oDoF_P) ;
      m_trk_chi2oDoF_P->GetXaxis()->SetTitle("P (GeV)"); 
      m_trk_chi2oDoF_P->GetYaxis()->SetTitle("Chi2"); 
      
      m_trk_chi2ProbDist = new TH1F("trk_chi2ProbDist","chi2Prob distribution",50,0,1);  
      m_trk_chi2ProbDist->GetXaxis()->SetTitle("Track #chi^{2} prob"); 
      m_trk_chi2ProbDist->GetYaxis()->SetTitle("Number of Tracks");
      RegisterHisto(al_mon,m_trk_chi2ProbDist) ;
      
      m_errCotTheta = new TH1F("errCotTheta","Error of CotTheta",50,0,0.02);
      RegisterHisto(al_mon,m_errCotTheta);  
      m_errCotTheta->GetXaxis()->SetTitle("Track #Delta(cot(#theta))"); 
      m_errCotTheta->GetYaxis()->SetTitle("Number of Tracks"); 
      
      m_errCotThetaVsD0BS = new TH2F("errCotThetaVsD0BS","Error of CotTheta vs d0BS",-m_d0BsRange,m_d0BsRange,40,50,0 ,0.02);
      RegisterHisto(al_mon,m_errCotThetaVsD0BS) ;  
      m_errCotThetaVsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errCotThetaVsD0BS->GetYaxis()->SetTitle("Track #Delta(cot(#theta))"); 

      m_errCotThetaVsPt = new TH2F("errCotThetaVsPt","Error of CotTheta vs Pt",50,0,40,50,0 ,0.02);
      RegisterHisto(al_mon,m_errCotThetaVsPt) ;  
      m_errCotThetaVsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errCotThetaVsPt->GetYaxis()->SetTitle("Track #Delta(cot(#theta))"); 
       
      m_errCotThetaVsP = new TH2F("errCotThetaVsP","Error of CotTheta vs P",50,0,40,50,0 ,0.02);
      RegisterHisto(al_mon,m_errCotThetaVsP) ;  
      m_errCotThetaVsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errCotThetaVsP->GetYaxis()->SetTitle("Track #Delta(cot(#theta))"); 
      
      m_errCotThetaVsPhi = new TH2F("errCotThetaVsPhi","Error of CotTheta vs Phi",50,0,2*m_Pi,50, 0 ,0.02);
      RegisterHisto(al_mon,m_errCotThetaVsPhi) ;  
      m_errCotThetaVsPhi->GetXaxis()->SetTitle("#phi0"); 
      m_errCotThetaVsPhi->GetYaxis()->SetTitle("Track #Delta(cot(#theta))"); 
      
      m_errCotThetaVsEta = new TH2F("errCotThetaVsEta","Error of CotTheta vs Eta",50,-3., 3.,50, 0 ,0.02);
      RegisterHisto(al_mon,m_errCotThetaVsEta) ;  
      m_errCotThetaVsEta->GetXaxis()->SetTitle("#eta"); 
      m_errCotThetaVsEta->GetYaxis()->SetTitle("Track #Delta(cot(#theta))"); 
      
      m_errTheta = new TH1F("errTheta","Error of Theta",50,0,0.02);
      RegisterHisto(al_mon,m_errTheta);  
      m_errTheta->GetXaxis()->SetTitle("Track #Delta(#theta)"); 
      m_errTheta->GetYaxis()->SetTitle("Number of Tracks"); 
    
      m_errThetaVsD0BS = new TH2F("errThetaVsD0BS","Error of Theta vs d0BS",50,-m_d0BsRange,m_d0BsRange,50,0 ,0.02);
      RegisterHisto(al_mon,m_errThetaVsD0BS) ;  
      m_errThetaVsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errThetaVsD0BS->GetYaxis()->SetTitle("Track #delta(#theta)");
      
      m_errThetaVsPt = new TH2F("errThetaVsPt","Error of Theta vs Pt",50,0,50,50,0 ,0.02);
      RegisterHisto(al_mon,m_errThetaVsPt) ;  
      m_errThetaVsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errThetaVsPt->GetYaxis()->SetTitle("Track #delta(#theta)"); 
       
      m_errThetaVsP = new TH2F("errThetaVsP","Error of Theta vs P",50,0,50,50,0 ,0.02);
      RegisterHisto(al_mon,m_errThetaVsP) ;  
      m_errThetaVsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errThetaVsP->GetYaxis()->SetTitle("Track #delta(#theta)"); 
      
      m_errThetaVsPhi = new TH2F("errThetaVsPhi","Error of Theta vs Phi",50,0,2*m_Pi,50, 0 ,0.02);
      RegisterHisto(al_mon,m_errThetaVsPhi) ;  
      m_errThetaVsPhi->GetXaxis()->SetTitle("#phi0"); 
      m_errThetaVsPhi->GetYaxis()->SetTitle("Track #delta(#theta)"); 
        
      m_errThetaVsEta = new TH2F("errThetaVsEta","Error of Theta vs Eta",50,-3., 3.,50, 0 ,0.02);
      RegisterHisto(al_mon,m_errThetaVsEta) ;  
      m_errThetaVsEta->GetXaxis()->SetTitle("#eta"); 
      m_errThetaVsEta->GetYaxis()->SetTitle("Track #delta(#theta)"); 
      
      m_errD0 = new TH1F("errD0", "Error of d0", 60,0,0.30);
      RegisterHisto(al_mon,m_errD0);
      m_errD0->GetXaxis()->SetTitle("d0 error (mm)"); 
      
      m_errD0VsD0BS = new TH2F("errD0VsD0BS", "Error of d0 vs d0BS", 50,-m_d0BsRange,m_d0BsRange,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsD0BS);
      m_errD0VsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errD0VsD0BS->GetYaxis()->SetTitle("d0 error (mm)");
      
      m_errD0VsPt = new TH2F("errD0VsPt", "Error of d0 vs Pt", 50,0,40,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsPt);
      m_errD0VsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errD0VsPt->GetYaxis()->SetTitle("d0 error (mm)");
      
      m_errD0VsP = new TH2F("errD0VsP", "Error of d0 vs P", 50,0,40,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsP);
      m_errD0VsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errD0VsP->GetYaxis()->SetTitle("d0 error (mm)");
        
      m_errD0VsPhi = new TH2F("errD0VsPhi", "Error of d0 vs Phi", 90,0,2*m_Pi,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsPhi);
      m_errD0VsPhi->GetXaxis()->SetTitle("#phi0 (rad)");  
      m_errD0VsPhi->GetYaxis()->SetTitle("d0 error (mm)");   
      
      m_errD0VsPhiBarrel = new TH2F("errD0VsPhiBarrel", "Error of d0 vs Phi (Barrel)", 90,0,2*m_Pi,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsPhiBarrel);
      m_errD0VsPhiBarrel->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errD0VsPhiBarrel->GetYaxis()->SetTitle("d0 error (mm)");  
      
      m_errD0VsPhiECA = new TH2F("errD0VsPhiECA", "Error of d0 vs Phi (ECA)", 90,0,2*m_Pi,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsPhiECA);
      m_errD0VsPhiECA->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errD0VsPhiECA->GetYaxis()->SetTitle("d0 error (mm)");  
      
      m_errD0VsPhiECC = new TH2F("errD0VsPhiECC", "Error of d0 vs Phi (ECC)", 90,0,2*m_Pi,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsPhiECC);
      m_errD0VsPhiECC->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errD0VsPhiECC->GetYaxis()->SetTitle("d0 error (mm)");  
      
      m_errD0VsEta = new TH2F("errD0VsEta", "Error of d0 vs Eta", 50,-3.,3.,100,0 ,0.50);
      RegisterHisto(al_mon,m_errD0VsEta);
      m_errD0VsEta->GetXaxis()->SetTitle("#eta"); 
      m_errD0VsEta->GetYaxis()->SetTitle("d0 error (mm)");         
      
      m_errPhi0 = new TH1F("errPhi0", "Error of Phi0", 50,0,0.010);
      RegisterHisto(al_mon,m_errPhi0);
      m_errPhi0->GetXaxis()->SetTitle("#phi0 error (rad)"); 

      m_errPhi0VsD0BS = new TH2F("errPhi0VsD0BS", "Error of Phi0 vs d0BS", 50,-m_d0BsRange,m_d0BsRange, 50,0,0.010);
      RegisterHisto(al_mon,m_errPhi0VsD0BS);
      m_errPhi0VsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errPhi0VsD0BS->GetYaxis()->SetTitle("#phi0 error (rad)");
        
      m_errPhi0VsPt = new TH2F("errPhi0VsPt", "Error of Phi0 vs Pt", 50,0,40, 50,0,0.010);
      RegisterHisto(al_mon,m_errPhi0VsPt);
      m_errPhi0VsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errPhi0VsPt->GetYaxis()->SetTitle("#phi0 error (rad)");
        
      m_errPhi0VsP = new TH2F("errPhi0VsP", "Error of Phi0 vs P", 50,0,40, 50,0,0.010);
      RegisterHisto(al_mon,m_errPhi0VsP);
      m_errPhi0VsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errPhi0VsP->GetYaxis()->SetTitle("#phi0 error (rad)");
        
      m_errPhi0VsPhi0 = new TH2F("errPhi0VsPhi0", "Error of Phi0 vs Phi0", 90,0,2*m_Pi,50, 0,0.010);
      RegisterHisto(al_mon,m_errPhi0VsPhi0);
      m_errPhi0VsPhi0->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errPhi0VsPhi0->GetYaxis()->SetTitle("#phi0 error (rad)");
      
      m_errPhi0VsEta = new TH2F("errPhi0VsEta", "Error of Phi0 vs Eta", 50,-3.,3.,50, 0,0.010);
      RegisterHisto(al_mon,m_errPhi0VsEta);
      m_errPhi0VsEta->GetXaxis()->SetTitle("#eta"); 
      m_errPhi0VsEta->GetYaxis()->SetTitle("#phi0 error (rad)");  
        
      m_errZ0 = new TH1F("errZ0", "Error of Z0", 50,0,0.3);
      RegisterHisto(al_mon,m_errZ0);
      m_errZ0->GetXaxis()->SetTitle("z0 error (mm)"); 
      
      m_errZ0VsD0BS = new TH2F("errZ0VsD0BS", "Error of Z0 vs D0BS", 50,-m_d0BsRange,m_d0BsRange, 50,0,0.3);
      RegisterHisto(al_mon,m_errZ0VsD0BS);
      m_errZ0VsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errZ0VsD0BS->GetYaxis()->SetTitle("z0 error (mm)");
        
      m_errZ0VsPt = new TH2F("errZ0VsPt", "Error of Z0 vs Pt", 50,0,40, 50,0,0.3);
      RegisterHisto(al_mon,m_errZ0VsPt);
      m_errZ0VsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errZ0VsPt->GetYaxis()->SetTitle("z0 error (mm)");
        
      m_errZ0VsP = new TH2F("errZ0VsP", "Error of Z0 vs P", 50,0,40, 50,0,0.3);
      RegisterHisto(al_mon,m_errZ0VsP);
      m_errZ0VsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errZ0VsP->GetYaxis()->SetTitle("z0 error (mm)");
      
      m_errZ0VsPhi0 = new TH2F("errZ0VsPhi0", "Error of Z0 vs Phi0", 50,0,2*m_Pi,50, 0,0.3);
      RegisterHisto(al_mon,m_errZ0VsPhi0);
      m_errZ0VsPhi0->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errZ0VsPhi0->GetYaxis()->SetTitle("z0 error (mm)");
        
      m_errZ0VsEta = new TH2F("errZ0VsEta", "Error of Z0 vs Eta", 50,-3.,3.,50, 0,0.3);
      RegisterHisto(al_mon,m_errZ0VsEta);
      m_errZ0VsEta->GetXaxis()->SetTitle("#eta"); 
      m_errZ0VsEta->GetYaxis()->SetTitle("z0 error (mm)");  
        
      m_errPt = new TH1F("errPt", "Error of Pt", 50 ,0., 1.);
      RegisterHisto(al_mon,m_errPt);
      m_errPt->GetXaxis()->SetTitle("Pt err (GeV/c)");
        
      m_errPtVsD0BS = new TH2F("errPtVsD0BS", "Error of Pt Vs doBS", 50, -m_d0BsRange, m_d0BsRange, 50,0., 0.5);
      RegisterHisto(al_mon,m_errPtVsD0BS);
      m_errPtVsD0BS->GetXaxis()->SetTitle("d0 (mm)"); 
      m_errPtVsD0BS->GetYaxis()->SetTitle("Pt error (GeV/c)");  

      m_errPtVsPt = new TH2F("errPtVsPt", "Error of Pt Vs Pt", 50, 0, 40., 50,0., 0.5);
      RegisterHisto(al_mon,m_errPtVsPt);
      m_errPtVsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errPtVsPt->GetYaxis()->SetTitle("Pt error (GeV/c)");  
        
      m_errPtVsP = new TH2F("errPtVsP", "Error of Pt Vs P", 50, 0, 40., 50,0., 0.5);
      RegisterHisto(al_mon,m_errPtVsP);
      m_errPtVsP->GetXaxis()->SetTitle("P (GeV/c)"); 
      m_errPtVsP->GetYaxis()->SetTitle("P error (GeV/c)");  
      
      m_errPtVsPhi0 = new TH2F("errPtVsPhi0", "Error of Pt Vs Phi0", 50, 0, 2*m_Pi, 50,0., 0.5);
      RegisterHisto(al_mon,m_errPtVsPhi0);
      m_errPtVsPhi0->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_errPtVsPhi0->GetYaxis()->SetTitle("Pt error (GeV/c)");     
        
      m_PtVsPhi0Pos = new TH2F("errPtVsPhi0Pos", "Pt Vs Phi0 (q>0)", 50, 0, 2*m_Pi,50, 0., 40.);
      RegisterHisto(al_mon,m_PtVsPhi0Pos);
      m_PtVsPhi0Pos->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_PtVsPhi0Pos->GetYaxis()->SetTitle("Pt (GeV/c)");   
      
      m_PtVsPhi0Neg = new TH2F("errPtVsPhi0Neg", "Pt Vs Phi0 (q<0)", 50, 0, 2*m_Pi,50, 0., 40.);
      RegisterHisto(al_mon,m_PtVsPhi0Neg);
      m_PtVsPhi0Neg->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_PtVsPhi0Neg->GetYaxis()->SetTitle("Pt (GeV/c)");  
      
      m_errPtVsEta = new TH2F("errPtVsEta", "Error of Pt Vs Eta", 50, -3., 3.,50, 0., 0.5);
      RegisterHisto(al_mon,m_errPtVsEta);
      m_errPtVsEta->GetXaxis()->SetTitle("#eta"); 
      m_errPtVsEta->GetYaxis()->SetTitle("Pt error (GeV/c)");       
        
      m_errPt_Pt2 = new TH1F("errPt_Pt2", "Error of Pt/Pt^{2}", 50 ,0., 0.015);
      RegisterHisto(al_mon,m_errPt_Pt2);
      m_errPt_Pt2->GetXaxis()->SetTitle("#sigma(Pt)/Pt^{2} (GeV/c)^{-1}");        
  
      m_errPt_Pt2VsPt= new TH2F("errPt_Pt2VsPt", "Error of Pt / Pt^2 Vs Pt", 50, 0, 40., 50,0., 0.015);
      RegisterHisto(al_mon,m_errPt_Pt2VsPt);
      m_errPt_Pt2VsPt->GetXaxis()->SetTitle("Pt (GeV/c)"); 
      m_errPt_Pt2VsPt->GetYaxis()->SetTitle("#sigma(Pt)/Pt^{2} [GeV/c]^{-1}");

      m_errPt_Pt2VsEta= new TH2F("errPt_Pt2VsEta", "Error of Pt / Pt^2 Vs Eta", 50, -3., 3., 50,0., 0.015);
      RegisterHisto(al_mon,m_errPt_Pt2VsEta);
      m_errPt_Pt2VsEta->GetXaxis()->SetTitle("#eta"); 
      m_errPt_Pt2VsEta->GetYaxis()->SetTitle("#sigma(Pt)/Pt^{2} [GeV/c]^{-1}");

      m_errPt_Pt2VsPhi0= new TH2F("errPt_Pt2VsPhi0", "Error of Pt / Pt^2 Vs #phi0", 100, 0, 2*m_Pi, 50,0., 0.015);
      RegisterHisto(al_mon,m_errPt_Pt2VsPhi0);
      m_errPt_Pt2VsPhi0->GetXaxis()->SetTitle("#eta"); 
      m_errPt_Pt2VsPhi0->GetYaxis()->SetTitle("#sigma(Pt)/Pt^{2} [GeV/c]^{-1}");
  
      m_D0VsPhi0 = new TH2F("D0VsPhi0", "d0 Vs #phi0 ", 100, 0, 2*m_Pi, 400, -m_d0Range, m_d0Range);
      RegisterHisto(al_mon,m_D0VsPhi0);
      m_D0VsPhi0->GetXaxis()->SetTitle("#phi0 [rad]"); 
      m_D0VsPhi0->GetYaxis()->SetTitle("d0 [mm]"); 
      
      m_Z0VsEta = new TH2F("Z0VsEta", "z0 Vs #eta ", 50, -3., 3., 100, -m_z0Range, m_z0Range);
      RegisterHisto(al_mon,m_Z0VsEta);
      m_Z0VsEta->GetXaxis()->SetTitle("#eta"); 
      m_Z0VsEta->GetYaxis()->SetTitle("z0 [mm]"); 
        
      m_QoverPtVsPhi0 = new TH2F("QoverPtVsPhi0", "q/Pt Vs #phi0 ", 100, 0, 2*m_Pi, 100, -0.5,0.5);
      RegisterHisto(al_mon,m_QoverPtVsPhi0);
      m_QoverPtVsPhi0->GetXaxis()->SetTitle("#phi0 [rad]"); 
      m_QoverPtVsPhi0->GetYaxis()->SetTitle("q/Pt [GeV^{-1}]"); 
        
      m_QPtVsPhi0 = new TH2F("QPtVsPhi0", "qPt Vs #phi0 ", 100, 0, 2*m_Pi, 100, -40.,40.);
      RegisterHisto(al_mon,m_QPtVsPhi0);
      m_QPtVsPhi0->GetXaxis()->SetTitle("#phi0 (rad)"); 
      m_QPtVsPhi0->GetYaxis()->SetTitle("qPt (GeV)");   
      
      m_QoverPtVsEta = new TH2F("QoverPtVsEta", "q/Pt Vs #eta ", 50, -3., 3., 100, -0.5,0.5);
      RegisterHisto(al_mon,m_QoverPtVsEta);
      m_QoverPtVsEta->GetXaxis()->SetTitle("#eta"); 
      m_QoverPtVsEta->GetYaxis()->SetTitle("q/Pt (GeV^{-1})"); 
        
      m_QPtVsEta = new TH2F("QPtVsEta", "qPt Vs #eta ", 50, -3., 3., 100, -40.,40.);
      m_QPtVsEta->GetXaxis()->SetTitle("#eta"); 
      RegisterHisto(al_mon,m_QPtVsEta);
      m_QPtVsEta->GetYaxis()->SetTitle("qPt (GeV)");  
      
      //Plots to check the BeamSpot
      // versus Phi0
      m_D0bsVsPhi0 = new TH2F("D0bsVsPhi0", "d0_{bs} Vs #phi0 ", 50, 0, 2*m_Pi, 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPhi0->GetXaxis()->SetTitle("#phi0"); 
      RegisterHisto(al_mon,m_D0bsVsPhi0);
      m_D0bsVsPhi0->GetYaxis()->SetTitle("d0_{bs} (mm)"); 

      m_D0bsVsPhi0ECC = new TH2F("D0bsVsPhi0_ECC", "d0_{bs} Vs #phi0 (ECC) ", 50, 0, 2*m_Pi, 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPhi0ECC->GetXaxis()->SetTitle("#phi0"); 
      RegisterHisto(al_mon,m_D0bsVsPhi0ECC);
      m_D0bsVsPhi0ECC->GetYaxis()->SetTitle("d0_{bs} (mm)"); 
      
      m_D0bsVsPhi0ECA = new TH2F("D0bsVsPhi0_ECA", "d0_{bs} Vs #phi0 (ECA)", 50, 0, 2*m_Pi, 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPhi0ECA->GetXaxis()->SetTitle("#phi0"); 
      RegisterHisto(al_mon,m_D0bsVsPhi0ECA);
      m_D0bsVsPhi0ECA->GetYaxis()->SetTitle("d0_{bs} (mm)"); 
      
      m_D0bsVsPhi0Barrel = new TH2F("D0bsVsPhi0_Barrel", "d0_{bs} Vs #phi0 (BA)", 50, 0, 2*m_Pi, 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPhi0Barrel->GetXaxis()->SetTitle("#phi0"); 
      RegisterHisto(al_mon,m_D0bsVsPhi0Barrel);
      m_D0bsVsPhi0Barrel->GetYaxis()->SetTitle("d0_{bs} (mm)"); 
        
      // versus Eta
      m_D0bsVsEta = new TH2F("D0bsVsEta", "d0_{bs} Vs #eta", 50, -3., 3., 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsEta->GetXaxis()->SetTitle("#eta"); 
      RegisterHisto(al_mon,m_D0bsVsEta);
      m_D0bsVsEta->GetYaxis()->SetTitle("d0_{bs} (mm)");  
      
      //versus Pt
      m_D0bsVsPt = new TH2F("D0bsVsPt", "d0_{bs} Vs qPt ",  100, -40.,40., 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPt->GetXaxis()->SetTitle("qPt (GeV)"); 
      RegisterHisto(al_mon,m_D0bsVsPt);
      m_D0bsVsPt->GetYaxis()->SetTitle("d0_{bs} (mm)");  

      m_D0bsVsPtECC = new TH2F("D0bsVsPt_ECC", "d0_{bs} Vs qPt (ECC)",  100, -40.,40.,400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPtECC->GetXaxis()->SetTitle("qPt (GeV)"); 
      RegisterHisto(al_mon,m_D0bsVsPtECC);
      m_D0bsVsPtECC->GetYaxis()->SetTitle("d0_{bs} (mm)");  
      
      m_D0bsVsPtECA = new TH2F("D0bsVsPt_ECA", "d0_{bs} Vs qPt (ECA)",  100, -40.,40., 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPtECA->GetXaxis()->SetTitle("qPt (GeV)"); 
      RegisterHisto(al_mon,m_D0bsVsPtECA);
      m_D0bsVsPtECA->GetYaxis()->SetTitle("d0_{bs} (mm)");  
      
      m_D0bsVsPtBarrel = new TH2F("D0bsVsPt_Barrel", "d0_{bs} Vs qPt (BA)",  100, -40.,40., 400, -m_d0BsRange, m_d0BsRange);
      m_D0bsVsPtBarrel->GetXaxis()->SetTitle("qPt (GeV)"); 
      RegisterHisto(al_mon,m_D0bsVsPtBarrel);
      m_D0bsVsPtBarrel->GetYaxis()->SetTitle("d0_{bs} mm )");

      //BeamSpot Position histos
      m_YBs_vs_XBs = new TH2F("YBs_vs_XBs","BeamSpot Position: y vs x",100, -0.9,-0.1, 100, -0.9,-0.1);
      RegisterHisto(al_mon,m_YBs_vs_XBs);
      m_YBs_vs_XBs->GetXaxis()->SetTitle("x coordinate (mm)");
      m_YBs_vs_XBs->GetYaxis()->SetTitle("y coordinate (mm)");
      
      m_YBs_vs_ZBs = new TH2F("YBs_vs_ZBs","BeamSpot Position: y vs z",100,-25., -5., 100, -0.9,-0.1);
      RegisterHisto(al_mon,m_YBs_vs_ZBs);
      m_YBs_vs_ZBs->GetXaxis()->SetTitle("z coordinate (mm)");
      m_YBs_vs_ZBs->GetYaxis()->SetTitle("y coordinate (mm)");
      
      m_XBs_vs_ZBs = new TH2F("XBs_vs_ZBs","BeamSpot Position: x vs z",100,-25., -5., 100, -0.9,-0.1);
      RegisterHisto(al_mon,m_XBs_vs_ZBs);
      m_XBs_vs_ZBs->GetXaxis()->SetTitle("z coordinate (mm)");
      m_XBs_vs_ZBs->GetYaxis()->SetTitle("x coordinate (mm)");
      
      
      m_XBs = new TH1F("XBs","BeamSpot Position: x",100,-1,0.);
      RegisterHisto(al_mon,m_XBs);
      m_XBs->GetXaxis()->SetTitle("x (mm)");
      m_XBs->GetYaxis()->SetTitle("#events");
      
      m_YBs= new TH1F("YBs","BeamSpot Position: y",100,-1,0.);
      RegisterHisto(al_mon,m_YBs);
      m_YBs->GetXaxis()->SetTitle("y (mm)");
      m_YBs->GetYaxis()->SetTitle("#events");
      
      m_ZBs = new TH1F("ZBs","BeamSpot Position: z",100,-50,50);
      RegisterHisto(al_mon,m_ZBs);
      m_ZBs->GetXaxis()->SetTitle("z (mm)");
      m_ZBs->GetYaxis()->SetTitle("#events");
      
      m_TiltX_Bs = new TH1F("TiltX_Bs","Beam spot tile angle: x-z plane",100,-1e3,1e3);
      RegisterHisto(al_mon,m_TiltX_Bs);
      m_TiltX_Bs->GetXaxis()->SetTitle("Tilt angle (#murad)");
      
      m_TiltY_Bs = new TH1F("TiltY_Bs","Beam spot tile angle: y-z plane",100,-1e3,1e3);
      RegisterHisto(al_mon,m_TiltY_Bs);
      m_TiltY_Bs->GetXaxis()->SetTitle("Tilt angle (#murad)");
      
      //versus lumiblock
      m_XBs_vs_LumiBlock = new TProfile("XBs_vs_LumiBlock","Profile of BeamSpot x Coordinate vs LumiBlock",1024,-0.5,1023.5);
      RegisterHisto(al_mon,m_XBs_vs_LumiBlock);
      m_XBs_vs_LumiBlock->GetXaxis()->SetTitle("LumiBlock");
      m_XBs_vs_LumiBlock->GetYaxis()->SetTitle("x coordinate (mm)");
      
      m_YBs_vs_LumiBlock = new TProfile("YBs_vs_LumiBlock","Profile of BeamSpot y Coordinate vs LumiBlock",1024,-0.5,1023.5);
      RegisterHisto(al_mon,m_YBs_vs_LumiBlock);
      m_YBs_vs_LumiBlock->GetXaxis()->SetTitle("LumiBlock");
      m_YBs_vs_LumiBlock->GetYaxis()->SetTitle("y coordinate (mm)");
      
      m_ZBs_vs_LumiBlock = new TProfile("ZBs_vs_LumiBlock","Profile of BeamSpot z Coordinate vs LumiBlock",1024,-0.5,1023.5);
      RegisterHisto(al_mon,m_ZBs_vs_LumiBlock);
      m_ZBs_vs_LumiBlock->GetXaxis()->SetTitle("LumiBlock");
      m_ZBs_vs_LumiBlock->GetYaxis()->SetTitle("z coordinate (mm)");
      
      m_BeamSpotTiltX_vs_LumiBlock = new TProfile("BeamSpotTiltX_vs_LumiBlock","Profile of BeamSpot TiltX vs LumiBlock",1024,-0.5,1023.5,-2,2);
      RegisterHisto(al_mon,m_BeamSpotTiltX_vs_LumiBlock);
      m_BeamSpotTiltX_vs_LumiBlock->GetXaxis()->SetTitle("LumiBlock");
      m_BeamSpotTiltX_vs_LumiBlock->GetYaxis()->SetTitle("TiltX (mrad)");
      m_BeamSpotTiltX_vs_LumiBlock->GetYaxis()->SetRangeUser(-1,1);
      
      m_BeamSpotTiltY_vs_LumiBlock = new TProfile("BeamSpotTiltY_vs_LumiBlock","Profile of BeamSpot TiltY vs LumiBlock",1024,-0.5,1023.5,-2,2);
      RegisterHisto(al_mon,m_BeamSpotTiltY_vs_LumiBlock);
      m_BeamSpotTiltY_vs_LumiBlock->GetXaxis()->SetTitle("LumiBlock");
      m_BeamSpotTiltY_vs_LumiBlock->GetYaxis()->SetTitle("TiltY (mrad)");
      m_BeamSpotTiltY_vs_LumiBlock->GetYaxis()->SetRangeUser(-1,1);
    }
    
    
    //These plots are broken. Have to be passed to the PVbiases tool
    m_trk_d0_wrtPV = new TH1F("d0_pvcorr_est","d0 (corrected for primVtx v2); [mm]",400,-0.2,0.2);  
    RegisterHisto(al_mon,m_trk_d0_wrtPV) ; 
    m_trk_z0_wrtPV = new TH1F("z0_pvcorr_est","z0 (corrected for primVtx v2); [mm]",100,-1,1);  
    RegisterHisto(al_mon,m_trk_z0_wrtPV ) ; 
 
    
    
    m_phi_barrel_pos_2_5GeV = new TH1F("phi_barrel_pos_2_5GeV","phi_barrel_pos_2_5GeV",100,0,2*m_Pi);  m_phi_barrel_pos_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_pos_2_5GeV) ;  
    m_phi_barrel_pos_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_pos_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 2-5 GeV"); 
    m_phi_barrel_pos_5_10GeV = new TH1F("phi_barrel_pos_5_10GeV","phi_barrel_pos_5_10GeV",100,0,2*m_Pi);  m_phi_barrel_pos_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_pos_5_10GeV) ;  
    m_phi_barrel_pos_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_pos_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 5-10 GeV");  
    m_phi_barrel_pos_10_20GeV = new TH1F("phi_barrel_pos_10_20GeV","phi_barrel_pos_10_20GeV",100,0,2*m_Pi);  m_phi_barrel_pos_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_pos_10_20GeV) ;  
    m_phi_barrel_pos_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_pos_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 10-20 GeV");   
    m_phi_barrel_pos_20plusGeV = new TH1F("phi_barrel_pos_20plusGeV","phi_barrel_pos_20plusGeV",100,0,2*m_Pi);  m_phi_barrel_pos_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_pos_20plusGeV) ;  
    m_phi_barrel_pos_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_pos_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in barrel, >20 GeV");    

    m_phi_barrel_neg_2_5GeV = new TH1F("phi_barrel_neg_2_5GeV","phi_barrel_neg_2_5GeV",100,0,2*m_Pi);  m_phi_barrel_neg_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_neg_2_5GeV) ;  
    m_phi_barrel_neg_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_neg_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 2-5 GeV"); 
    m_phi_barrel_neg_5_10GeV = new TH1F("phi_barrel_neg_5_10GeV","phi_barrel_neg_5_10GeV",100,0,2*m_Pi);  m_phi_barrel_neg_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_neg_5_10GeV) ;  
    m_phi_barrel_neg_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_neg_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 5-10 GeV");  
    m_phi_barrel_neg_10_20GeV = new TH1F("phi_barrel_neg_10_20GeV","phi_barrel_neg_10_20GeV",100,0,2*m_Pi);  m_phi_barrel_neg_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_neg_10_20GeV) ;  
    m_phi_barrel_neg_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_neg_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in barrel, 10-20 GeV");   
    m_phi_barrel_neg_20plusGeV = new TH1F("phi_barrel_neg_20plusGeV","phi_barrel_neg_20plusGeV",100,0,2*m_Pi);  m_phi_barrel_neg_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel_neg_20plusGeV) ;  
    m_phi_barrel_neg_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_barrel_neg_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in barrel, >20 GeV");  

    m_phi_eca_pos_2_5GeV = new TH1F("phi_eca_pos_2_5GeV","phi_eca_pos_2_5GeV",100,0,2*m_Pi);  m_phi_eca_pos_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_pos_2_5GeV) ;  
    m_phi_eca_pos_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_pos_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 2-5 GeV"); 
    m_phi_eca_pos_5_10GeV = new TH1F("phi_eca_pos_5_10GeV","phi_eca_pos_5_10GeV",100,0,2*m_Pi);  m_phi_eca_pos_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_pos_5_10GeV) ;  
    m_phi_eca_pos_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_pos_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 5-10 GeV");  
    m_phi_eca_pos_10_20GeV = new TH1F("phi_eca_pos_10_20GeV","phi_eca_pos_10_20GeV",100,0,2*m_Pi);  m_phi_eca_pos_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_pos_10_20GeV) ;  
    m_phi_eca_pos_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_pos_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 10-20 GeV");   
    m_phi_eca_pos_20plusGeV = new TH1F("phi_eca_pos_20plusGeV","phi_eca_pos_20plusGeV",100,0,2*m_Pi);  m_phi_eca_pos_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_pos_20plusGeV) ;  
    m_phi_eca_pos_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_pos_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in eca, >20 GeV");    

    m_phi_eca_neg_2_5GeV = new TH1F("phi_eca_neg_2_5GeV","phi_eca_neg_2_5GeV",100,0,2*m_Pi);  m_phi_eca_neg_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_neg_2_5GeV) ;  
    m_phi_eca_neg_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_neg_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 2-5 GeV"); 
    m_phi_eca_neg_5_10GeV = new TH1F("phi_eca_neg_5_10GeV","phi_eca_neg_5_10GeV",100,0,2*m_Pi);  m_phi_eca_neg_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_neg_5_10GeV) ;  
    m_phi_eca_neg_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_neg_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 5-10 GeV");  
    m_phi_eca_neg_10_20GeV = new TH1F("phi_eca_neg_10_20GeV","phi_eca_neg_10_20GeV",100,0,2*m_Pi);  m_phi_eca_neg_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_neg_10_20GeV) ;  
    m_phi_eca_neg_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_neg_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in eca, 10-20 GeV");   
    m_phi_eca_neg_20plusGeV = new TH1F("phi_eca_neg_20plusGeV","phi_eca_neg_20plusGeV",100,0,2*m_Pi);  m_phi_eca_neg_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca_neg_20plusGeV) ;  
    m_phi_eca_neg_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_eca_neg_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in eca, >20 GeV");   

    m_phi_ecc_pos_2_5GeV = new TH1F("phi_ecc_pos_2_5GeV","phi_ecc_pos_2_5GeV",100,0,2*m_Pi);  m_phi_ecc_pos_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_pos_2_5GeV) ;  
    m_phi_ecc_pos_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_pos_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 2-5 GeV"); 
    m_phi_ecc_pos_5_10GeV = new TH1F("phi_ecc_pos_5_10GeV","phi_ecc_pos_5_10GeV",100,0,2*m_Pi);  m_phi_ecc_pos_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_pos_5_10GeV) ;  
    m_phi_ecc_pos_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_pos_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 5-10 GeV");  
    m_phi_ecc_pos_10_20GeV = new TH1F("phi_ecc_pos_10_20GeV","phi_ecc_pos_10_20GeV",100,0,2*m_Pi);  m_phi_ecc_pos_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_pos_10_20GeV) ;  
    m_phi_ecc_pos_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_pos_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 10-20 GeV");   
    m_phi_ecc_pos_20plusGeV = new TH1F("phi_ecc_pos_20plusGeV","phi_ecc_pos_20plusGeV",100,0,2*m_Pi);  m_phi_ecc_pos_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_pos_20plusGeV) ;  
    m_phi_ecc_pos_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_pos_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in ecc, >20 GeV");    

    m_phi_ecc_neg_2_5GeV = new TH1F("phi_ecc_neg_2_5GeV","phi_ecc_neg_2_5GeV",100,0,2*m_Pi);  m_phi_ecc_neg_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_neg_2_5GeV) ;  
    m_phi_ecc_neg_2_5GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_neg_2_5GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 2-5 GeV"); 
    m_phi_ecc_neg_5_10GeV = new TH1F("phi_ecc_neg_5_10GeV","phi_ecc_neg_5_10GeV",100,0,2*m_Pi);  m_phi_ecc_neg_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_neg_5_10GeV) ;  
    m_phi_ecc_neg_5_10GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_neg_5_10GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 5-10 GeV");  
    m_phi_ecc_neg_10_20GeV = new TH1F("phi_ecc_neg_10_20GeV","phi_ecc_neg_10_20GeV",100,0,2*m_Pi);  m_phi_ecc_neg_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_neg_10_20GeV) ;  
    m_phi_ecc_neg_10_20GeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_neg_10_20GeV->GetYaxis()->SetTitle("Number of Tracks in ecc, 10-20 GeV");   
    m_phi_ecc_neg_20plusGeV = new TH1F("phi_ecc_neg_20plusGeV","phi_ecc_neg_20plusGeV",100,0,2*m_Pi);  m_phi_ecc_neg_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc_neg_20plusGeV) ;  
    m_phi_ecc_neg_20plusGeV->GetXaxis()->SetTitle("Track #phi"); 
    m_phi_ecc_neg_20plusGeV->GetYaxis()->SetTitle("Number of Tracks in ecc, >20 GeV");    

    m_eta_phi_pos_2_5GeV = new TH2F("eta_phi_pos_2_5GeV","eta_phi_pos_2_5GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_pos_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_pos_2_5GeV) ;  
    m_eta_phi_pos_2_5GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_pos_2_5GeV->GetYaxis()->SetTitle("Track #phi"); 
    m_eta_phi_pos_5_10GeV = new TH2F("eta_phi_pos_5_10GeV","eta_phi_pos_5_10GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_pos_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_pos_5_10GeV) ;  
    m_eta_phi_pos_5_10GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_pos_5_10GeV->GetYaxis()->SetTitle("Track #phi");  
    m_eta_phi_pos_10_20GeV = new TH2F("eta_phi_pos_10_20GeV","eta_phi_pos_10_20GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_pos_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_pos_10_20GeV) ;  
    m_eta_phi_pos_10_20GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_pos_10_20GeV->GetYaxis()->SetTitle("Track #phi");   
    m_eta_phi_pos_20plusGeV = new TH2F("eta_phi_pos_20plusGeV","eta_phi_pos_20plusGeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_pos_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_pos_20plusGeV) ;  
    m_eta_phi_pos_20plusGeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_pos_20plusGeV->GetYaxis()->SetTitle("Track #phi");    

    m_eta_phi_neg_2_5GeV = new TH2F("eta_phi_neg_2_5GeV","eta_phi_neg_2_5GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_neg_2_5GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_neg_2_5GeV) ;  
    m_eta_phi_neg_2_5GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_neg_2_5GeV->GetYaxis()->SetTitle("Track #phi");
    m_eta_phi_neg_5_10GeV = new TH2F("eta_phi_neg_5_10GeV","eta_phi_neg_5_10GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_neg_5_10GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_neg_5_10GeV) ;  
    m_eta_phi_neg_5_10GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_neg_5_10GeV->GetYaxis()->SetTitle("Track #phi");
    m_eta_phi_neg_10_20GeV = new TH2F("eta_phi_neg_10_20GeV","eta_phi_neg_10_20GeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_neg_10_20GeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_neg_10_20GeV) ;  
    m_eta_phi_neg_10_20GeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_neg_10_20GeV->GetYaxis()->SetTitle("Track #phi");
    m_eta_phi_neg_20plusGeV = new TH2F("eta_phi_neg_20plusGeV","eta_phi_neg_20plusGeV",20,-m_etaRange,m_etaRange,20,0,2*m_Pi);  m_eta_phi_neg_20plusGeV->SetMinimum(0);
    RegisterHisto(al_mon,m_eta_phi_neg_20plusGeV) ;  
    m_eta_phi_neg_20plusGeV->GetXaxis()->SetTitle("Track #eta"); 
    m_eta_phi_neg_20plusGeV->GetYaxis()->SetTitle("Track #phi");

    m_chi2oDoF_barrel = new TH1F("chi2oDoF_barrel","chi2oDoF (Barrel)",100,0,10);  
    RegisterHisto(al_mon,m_chi2oDoF_barrel) ; 
    m_chi2oDoF_barrel->GetXaxis()->SetTitle("Track in Barrel #chi^{2} / NDoF"); 
    m_chi2oDoF_barrel->GetYaxis()->SetTitle("Number of Tracks");  
    m_chi2oDoF_eca = new TH1F("chi2oDoF_eca","chi2oDoF (Eca)",100,0,10);  
    RegisterHisto(al_mon,m_chi2oDoF_eca) ; 
    m_chi2oDoF_eca->GetXaxis()->SetTitle("Track in ECA #chi^{2} / NDoF"); 
    m_chi2oDoF_eca->GetYaxis()->SetTitle("Number of Tracks");  
    m_chi2oDoF_ecc = new TH1F("chi2oDoF_ecc","chi2oDoF (Ecc)",100,0,10);  
    RegisterHisto(al_mon,m_chi2oDoF_ecc) ; 
    m_chi2oDoF_ecc->GetXaxis()->SetTitle("Track in ECC #chi^{2} / NDoF"); 
    m_chi2oDoF_ecc->GetYaxis()->SetTitle("Number of Tracks");      
    
    m_phi_barrel = new TH1F("phi_barrel","phi (Barrel)",100,0,2*m_Pi); m_phi_barrel->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_barrel) ;  
    m_phi_barrel->GetXaxis()->SetTitle("Track in Barrel #phi"); 
    m_phi_barrel->GetYaxis()->SetTitle("Number of Tracks");  
    m_phi_eca = new TH1F("phi_eca","phi (Eca)",100,0,2*m_Pi); m_phi_eca->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_eca) ; 
    m_phi_eca->GetXaxis()->SetTitle("Track in ECA #phi"); 
    m_phi_eca->GetYaxis()->SetTitle("Number of Tracks");  
    m_phi_ecc = new TH1F("phi_ecc","phi (Ecc)",100,0,2*m_Pi); m_phi_ecc->SetMinimum(0);
    RegisterHisto(al_mon,m_phi_ecc) ; 
    m_phi_ecc->GetXaxis()->SetTitle("Track in ECC #phi"); 
    m_phi_ecc->GetYaxis()->SetTitle("Number of Tracks");   

    m_pT = new TH1F("pT","signed track p_{T}",200,-m_pTRange,m_pTRange);  
    RegisterHisto(al_mon,   m_pT);   
    m_pT->GetXaxis()->SetTitle("Signed track p_{T} [GeV]"); 
    m_pT->GetYaxis()->SetTitle("Number of Tracks");   

    m_pTabs = new TH1F("pTabs","Track p_{T}",100, 0., m_pTRange);  
    RegisterHisto(al_mon,   m_pTabs) ;   
    m_pTabs->GetXaxis()->SetTitle("Track p_{T} [GeV]"); 
    m_pTabs->GetYaxis()->SetTitle("Number of Tracks");   

    m_pTRes = new TH1F("pTRes","pTRes",100,0,1.0);  
    RegisterHisto(al_mon,m_pTRes) ;  
    m_pTResOverP = new TH1F("pTResOverP","Momentum resolution / Momentum",100,0,0.05);  
    RegisterHisto(al_mon,m_pTResOverP) ;  

    m_P = new TH1F("P","Track Momentum P",200,-m_pTRange,m_pTRange);  
    RegisterHisto(al_mon,m_P) ;   
    m_P->GetXaxis()->SetTitle("Signed Track P [GeV]"); 
    m_P->GetYaxis()->SetTitle("Number of Tracks");   
    
    m_Zmumu = new TH1F("Zmumu","Zmumu Inv. Mass",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu) ;  
    m_Zmumu_barrel = new TH1F("Zmumu_barrel","Zmumu Both Legs Barrel",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu_barrel) ;  
    m_Zmumu_eca = new TH1F("Zmumu_eca","Zmumu Both Legs ECA",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu_eca) ;  
    m_Zmumu_ecc = new TH1F("Zmumu_ecc","Zmumu Both Legs ECC",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu_ecc) ;  
    m_Zmumu_barrel_eca = new TH1F("Zmumu_barrel_eca","Zmumu One Barrel One ECA",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu_barrel_eca) ;  
    m_Zmumu_barrel_ecc = new TH1F("Zmumu_barrel_ecc","Zmumu One Barrel One ECC",60,60,120);  
    RegisterHisto(al_mon,m_Zmumu_barrel_ecc) ; 

    m_ZpT_n = new TH1F("ZpT_n","pT of negative tracks from Z",100,0,100);  
    RegisterHisto(al_mon,m_ZpT_n) ;
    m_ZpT_p = new TH1F("ZpT_p","pT of positive tracks from Z",100,0,100);  
    RegisterHisto(al_mon,m_ZpT_p) ;  
    m_ZpT_diff = new TH1F("ZpT_diff","pT difference for negative-positive tracks from Z",100,0,100);  
    RegisterHisto(al_mon,m_ZpT_diff) ;    

    m_pT_n = new TH1F("pT_n","pT of negative tracks",50,0,100); 
    RegisterHisto(al_mon,m_pT_n) ;
    m_pT_p = new TH1F("pT_p","pT of positive tracks",50,0,100); 
    RegisterHisto(al_mon,m_pT_p) ;  
    m_pT_diff = new TH1F("pT_diff","pT difference for negative-positive tracks",50,0,100);   
    RegisterHisto(al_mon,m_pT_diff);

    m_trk_pT_asym = new TH1F("trk_pT_asym","Track Charge Asymmetry versus pT",50,0,100);
    RegisterHisto(al_mon,m_trk_pT_asym);
    m_trk_pT_asym->GetXaxis()->SetTitle("#pt (GeV)"); 
    m_trk_pT_asym->GetYaxis()->SetTitle("(pos-neg)/(pos+neg)");   
    
    m_hitMap_barrel = new TH2F("hitMap_barrel","Hit Map for Barrel",125,-1100,1100,125,-1100,1100);
    RegisterHisto(al_mon,m_hitMap_barrel);

    m_hitMap_barrel_zoom = new TH2F("hitMap_barrel_zoom","Hit Map for Barrel",125,-150.,150., 125,-150.,150.);
    RegisterHisto(al_mon,m_hitMap_barrel_zoom);

    m_hitMap_endcapA = new TH2F("hitMap_endcapA","Hit Map for Endcap A",100,800,2800,100,-3.14,3.14);
    RegisterHisto(al_mon,m_hitMap_endcapA);
    
    m_hitMap_endcapC = new TH2F("hitMap_endcapC","Hit Map for Endcap C",100,-2800,-800,100,-3.14,3.14);
    RegisterHisto(al_mon,m_hitMap_endcapC);

    //charge asymmetry vs.eta
    m_eta_neg = new TH1F("eta_neg","eta for negative tracks; #eta(-)",25,-m_etaRange,m_etaRange);   
    RegisterHisto(al_mon,m_eta_neg);
    m_eta_neg->GetXaxis()->SetTitle("#eta"); 
    m_eta_neg->GetYaxis()->SetTitle("# tracks");   

    m_eta_pos = new TH1F("eta_pos","eta for positive tracks; #eta(+)",25,-m_etaRange,m_etaRange);   
    RegisterHisto(al_mon,m_eta_pos);
    m_eta_pos->GetXaxis()->SetTitle("#eta"); 
    m_eta_pos->GetYaxis()->SetTitle("# tracks");   

    m_eta_asym = new TH1F("eta_asym","Track Charge Asymmetry versus eta",25, -m_etaRange,m_etaRange);
    RegisterHisto(al_mon,m_eta_asym);
    m_eta_asym->GetXaxis()->SetTitle("#eta"); 
    m_eta_asym->GetYaxis()->SetTitle("(pos-neg)/(pos+neg)");   


    
    // msg(MSG::INFO) << "lumiblock histos done " <<endmsg;

 


    
    // lumiblock histos 
    m_LumiBlock = new TH1F("LumiBlock","Lumi block",1024,-0.5,1023.5); 
    RegisterHisto(al_mon,m_LumiBlock) ;
    m_LumiBlock->GetXaxis()->SetTitle("Lumi block ID"); 
    m_LumiBlock->GetYaxis()->SetTitle("# events");   

    m_Tracks_per_LumiBlock = new TH1F("TracksPerLumiBlock","Tracks per Lumi block",1024,-0.5,1023.5); 
    RegisterHisto(al_mon,m_Tracks_per_LumiBlock) ;
    m_Tracks_per_LumiBlock->GetXaxis()->SetTitle("Lumi block ID"); 
    m_Tracks_per_LumiBlock->GetYaxis()->SetTitle("# tracks");   

    m_NPIX_per_LumiBlock = new TH1F("NPixPerLumiBlock","N pixel hits per Lumi block",1024,-0.5,1023.5); 
    RegisterHisto(al_mon, m_NPIX_per_LumiBlock) ;
    m_NPIX_per_LumiBlock->GetXaxis()->SetTitle("Lumi block ID"); 
    m_NPIX_per_LumiBlock->GetYaxis()->SetTitle("# pixel hits");   

    m_NSCT_per_LumiBlock = new TH1F("NSCTPerLumiBlock","N SCT hits per Lumi block",1024,-0.5,1023.5); 
    RegisterHisto(al_mon, m_NSCT_per_LumiBlock) ;
    m_NSCT_per_LumiBlock->GetXaxis()->SetTitle("Lumi block ID"); 
    m_NSCT_per_LumiBlock->GetYaxis()->SetTitle("# SCT hits");   
    

    m_NTRT_per_LumiBlock = new TH1F("NTRTPerLumiBlock","N TRT hits per Lumi block",1024,-0.5,1023.5); 
    RegisterHisto(al_mon, m_NTRT_per_LumiBlock) ;
    m_NTRT_per_LumiBlock->GetXaxis()->SetTitle("Lumi block ID"); 
    m_NTRT_per_LumiBlock->GetYaxis()->SetTitle("# TRT hits");   

    // track weight histo
    int theNbins = 50;
    float theLowRange = 0.;
    float theUppRange = 5.;
    float theBinWidth= (theUppRange-theLowRange)/theNbins;
    m_hTrackWeight = new TH1F("hUsedWeight","Weight per track", theNbins+1, theLowRange-theBinWidth/2, theUppRange+theBinWidth/2); 
    RegisterHisto(al_mon, m_hTrackWeight) ;
    m_hTrackWeight->GetXaxis()->SetTitle("weight"); 
    m_hTrackWeight->GetYaxis()->SetTitle("# tracks");   
    //
    m_histosBooked++;
  }

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << " ** IDAlignMonGenericTracks::bookHistograms() ** COMPLETED ** track collection: " << m_tracksName 
					<< "  trigchain: " << m_triggerChainName 
					<< "  m_histosBooked : " << m_histosBooked << std::endl;
  
  return StatusCode::SUCCESS; 
}

//////////////////////////////////////////////////////////
void IDAlignMonGenericTracks::RegisterHisto(MonGroup& mon, TH1* histo) {

  //histo->Sumw2();
  histo->SetOption("e");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDAlignMonGenericTracks::RegisterHisto(MonGroup& mon, TProfile* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile Histogram:" << endmsg;
  }
}

void IDAlignMonGenericTracks::RegisterHisto(MonGroup& mon, TH2* histo) {
  
  //histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH2 Histogram:" << endmsg;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode IDAlignMonGenericTracks::fillHistograms()
{
  m_events++;
 
  // Get EventInfo
  const DataHandle<xAOD::EventInfo> eventInfo;
  if (StatusCode::SUCCESS != evtStore()->retrieve( eventInfo ) ){
    msg(MSG::ERROR) << "Cannot get event info." << endmsg;
    return StatusCode::FAILURE;
  }
  //EventID* eventID = eventInfo->event_ID();

  //if (!evtStore()->contains<TrackCollection>(m_tracksName)) {
  if (!evtStore()->contains<TrackCollection>(m_tracksName)) {
    if(m_events == 1) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_tracksName << " TrackCollection" << endmsg;}
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_tracksName << " TrackCollection" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Code is able to get a weight from an external file and appy it to all histograms
  double TrkWeight = 1.;
  // NB the weight is a "per track" weight, so histograms such as BS info are never weighted
  
  // interactions per event
  m_mu = eventInfo->averageInteractionsPerCrossing();
  if(m_mu_perEvent) m_mu_perEvent->Fill(m_mu);
  
  //get tracks
  DataVector<Trk::Track>* trks = m_trackSelection->selectTracks(m_tracksName);
  //const DataVector<xAOD::TrackParticle>* trkPs = m_trackSelection->selectTracksParticle(m_tracksName);


  

  float xv=-999;
  float yv=-999;
  float zv=-999;

  if (evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)) {
    StatusCode scv = evtStore()->retrieve(m_vertices,m_VxPrimContainerName);
    if (scv.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name  "<<m_VxPrimContainerName<<" found in StoreGate" << endmsg;
      return StatusCode::SUCCESS;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name  "<<m_VxPrimContainerName<< " with size " << m_vertices->size() <<" found  in StoreGate" << endmsg;
  
      xAOD::VertexContainer::const_iterator vxItr  = m_vertices->begin();
      xAOD::VertexContainer::const_iterator vxItrE = m_vertices->end();    
      int ntrkMax=0;
      for (; vxItr != vxItrE; ++vxItr) {
	const xAOD::Vertex* theVertex = (*vxItr);
	if(!theVertex->vxTrackAtVertexAvailable()) continue;
	const std::vector< Trk::VxTrackAtVertex >& theTrackAtVertex = theVertex->vxTrackAtVertex();
        int numTracksPerVertex = theTrackAtVertex.size();
	ATH_MSG_DEBUG("Size of TrackAtVertex: " << numTracksPerVertex);
        if (numTracksPerVertex>ntrkMax) {
          ntrkMax=numTracksPerVertex;
          xv=theVertex->position()[0];
          yv=theVertex->position()[1];
          zv=theVertex->position()[2];
	}
      }
    }
    //std::cout << "xv, yv, zv: " << xv << ", " << yv << ", " << zv << std::endl;
  } else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "StoreGate does not contain VxPrimaryCandidate Container" << endmsg;

  if (xv==-999 || yv==-999 || zv==-999) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No vertex found => setting it to 0"<<endmsg;
    xv=0;yv=0;zv=0;
  }


 
  if (m_doIP) fillVertexInformation();

  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  float beamTiltX = 0.;
  float beamTiltY = 0.;
  if (m_hasBeamCondSvc) {
    Amg::Vector3D bpos = m_beamCondSvc->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
    beamTiltX = m_beamCondSvc->beamTilt(0);
    beamTiltY = m_beamCondSvc->beamTilt(1);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Beamspot from " << m_beamCondSvc << ": x0 = " << beamSpotX << ", y0 = " << beamSpotY
          << ", z0 = " << beamSpotZ << ", tiltX = " << beamTiltX
          << ", tiltY = " << beamTiltY <<endmsg;
  }
  
  // extract lumiblock
  unsigned int LumiBlock = eventInfo->lumiBlock();

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " LumiBlock = " <<  LumiBlock << endmsg;
  m_LumiBlock->Fill(float(LumiBlock), TrkWeight);
  
  if (m_extendedPlots) { // warning: these histograms are filled once per event, not for every track --> TrkWeight = 1;
    
    //Fill BeamSpot Position histos
    


    m_YBs_vs_XBs->Fill(beamSpotX,beamSpotY, TrkWeight);
    m_YBs_vs_ZBs->Fill(beamSpotZ,beamSpotY, TrkWeight);
    m_XBs_vs_ZBs->Fill(beamSpotZ,beamSpotX, TrkWeight);
    
    m_XBs->Fill(beamSpotX, TrkWeight);
    m_YBs->Fill(beamSpotY, TrkWeight);
    m_ZBs->Fill(beamSpotZ, TrkWeight);
    m_TiltX_Bs->Fill(1e6*beamTiltX, TrkWeight);
    m_TiltY_Bs->Fill(1e6*beamTiltY, TrkWeight);
    
    //Fill BeamSpot Position versus lumiblock histos
    m_XBs_vs_LumiBlock->Fill(float(LumiBlock),beamSpotX, TrkWeight);
    m_YBs_vs_LumiBlock->Fill(float(LumiBlock),beamSpotY, TrkWeight);
    m_ZBs_vs_LumiBlock->Fill(float(LumiBlock),beamSpotZ, TrkWeight);
    m_BeamSpotTiltX_vs_LumiBlock->Fill(float(LumiBlock),1e3*beamTiltX, TrkWeight);
    m_BeamSpotTiltY_vs_LumiBlock->Fill(float(LumiBlock),1e3*beamTiltY, TrkWeight);
  } // end of m_extendedPlots
  

  int nHits=0;
  int nTracks=0;
  int ngTracks=0;
  
  bool z_true = false;
  float z_E[2]={0.}, z_px[2]={0.}, z_py[2]={0.}, z_pz[2]={0.}, z_pT[2]={0.}; 
  float z_eta[2]={0.};
  float ptlast = 0;
  int chargefirst = 0;

  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
  
  //DataVector<xAOD::TrackParticle>::const_iterator trkPsItr  = trkPs->begin();
  //DataVector<xAOD::TrackParticle>::const_iterator trkPsItrE = trkPs->end();
  
  if (m_doIP) {
    const xAOD::VertexContainer* vxContainer(0);
    StatusCode sc = evtStore()->retrieve(vxContainer, m_VxPrimContainerName);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Could not retrieve primary vertex info: " << m_VxPrimContainerName);
      return false;
    }
    if(vxContainer) {
      ATH_MSG_VERBOSE("Nb of reco primary vertex for coll "
		      << " = " << vxContainer->size() );
      
      
      xAOD::VertexContainer::const_iterator vxI = vxContainer->begin();
      xAOD::VertexContainer::const_iterator vxE = vxContainer->end();
      for(; vxI!=vxE; ++vxI) {
	//int nbtk = 0;
	//const std::vector<Trk::VxTrackAtVertex*>* tracks = (*vxI)->vxTrackAtVertex();
	if ((*vxI)->type()==1) {
	  m_pvtx=(*vxI);
	}
      }
    }
  } // end m_doIP
  
  
  for (; trksItr != trksItrE; ++trksItr) {
    
    nTracks++;  
    
    
    double chi2Prob=0.;
    float chisquared     = 0.;
    int DoF              = 0;
    float chi2oDoF       = -999;
    float trkd0          = -999;
    float Err_d0         = -999;
    float trkz0          = -999;
    float Err_z0         = -999;
    float trkphi         = -999;
    float Err_phi        = -999;
    float trktheta       = -999;
    float Err_cottheta   = -999;
    float Err_theta      = -999;
    float trketa         = -999;
    float qOverP         = -999;
    float qOverPt        = -999;
    float Err_qOverP     = -999;
    float Err_Pt   = -999;
    float trkpt          = -999;
    float trkP           = -999;
    float charge         = 0;
    float trkd0c         = -999;
    float trkz0c         = -999;
    float beamX          = 0;
    float beamY          = 0;
    float d0bscorr   = -999;
    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();
    
    const Trk::Perigee* measPer = (*trksItr)->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : NULL;

    const Trk::ImpactParametersAndSigma* myIPandSigma=0;
    
    
    if (m_doIP){

      //Get unbiased impact parameter
      
      if (m_pvtx) myIPandSigma = m_trackToVertexIPEstimator->estimate((*trksItr)->perigeeParameters(), m_pvtx, true);
    } 
    
    if (covariance == NULL) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endmsg; 
    }
    else{  
      AmgVector(5) perigeeParams = measPer->parameters(); 
      trkd0        = perigeeParams[Trk::d0];  
      trkz0        = perigeeParams[Trk::z0];    
      trkphi       = perigeeParams[Trk::phi0];  
      trktheta     = perigeeParams[Trk::theta];
      trketa       = measPer->eta(); 
      qOverP       = perigeeParams[Trk::qOverP]*1000.;  
      if(qOverP) trkP = 1/qOverP;
      qOverPt      = qOverP/sin(trktheta);  
      trkpt        = measPer->pT()/1000.;  
      Err_qOverP   = Amg::error(*measPer->covariance(),Trk::qOverP)*1000;
      Err_Pt       = sin(trktheta)*Err_qOverP/pow(qOverP,2);
      Err_d0       = Amg::error(*measPer->covariance(),Trk::d0);
      Err_z0       = Amg::error(*measPer->covariance(),Trk::z0);
      Err_phi      = Amg::error(*measPer->covariance(),Trk::phi0);
      Err_cottheta = Amg::error(*measPer->covariance(),Trk::theta)/pow(TMath::Sin(trktheta),2);
      Err_theta    = Amg::error(*measPer->covariance(),Trk::theta);
      if (qOverP < 0) charge = -1;
      else charge=+1; 

      // correct the track d0 for the vertex position
      // would rather corrected for the beamline but could not find beamline
      trkd0c=trkd0-(yv*cos(trkphi)-xv*sin(trkphi));
      trkz0c=trkz0-zv;
      ATH_MSG_DEBUG("trkd0, trkd0c" << trkd0 << ", " << trkd0c);

      // correct the track parameters for the beamspot position
      beamX = beamSpotX + tan(beamTiltX) * (trkz0-beamSpotZ);
      beamY = beamSpotY + tan(beamTiltY) * (trkz0-beamSpotZ);
      d0bscorr = trkd0 - ( -sin(trkphi)*beamX + cos(trkphi)*beamY );

      // per track weight, if required
      if ( m_applyHistWeight ){
	TrkWeight = 1.; // default
	int binNumber = 0;
	// different weights can be considered
	switch (m_userInputWeigthMethod) {
	case IDAlignMonGenericTracks::TRKETA_TRKPT : // map of trk pt vs eta
	  binNumber = m_hInputTrackWeight->FindBin(trketa, trkpt);
	  TrkWeight = m_hInputTrackWeight->GetBinContent(binNumber);
	  break;
	case IDAlignMonGenericTracks::EVENTMU_TRKPT : // map of event mu vs eta
	  binNumber = m_hInputTrackWeight->FindBin(m_mu, trkpt);
	  TrkWeight = m_hInputTrackWeight->GetBinContent(binNumber);
	  break;
	case IDAlignMonGenericTracks::EVENTMU_TRKETA : // map of event mu vs eta
	  binNumber = m_hInputTrackWeight->FindBin(m_mu, trketa);
	  TrkWeight = m_hInputTrackWeight->GetBinContent(binNumber);
	  break;
	default:
	  // map of trk pt vs eta
	  binNumber = m_hInputTrackWeight->FindBin(trketa, trkpt);
	  TrkWeight = m_hInputTrackWeight->GetBinContent(binNumber);
	} // end switch for trk weight calculation

	m_hTrackWeight->Fill(TrkWeight);   
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track weight calculation method " << m_userInputWeigthMethod << " TrkWeight: " << TrkWeight << endmsg;
      } // end of track weight
    } // end of else (covariance == NULL)  

    if (fitQual==0) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No fit quality assigned to the track" << endmsg; 
      chi2Prob = -1e12; // return big value
    }
    else {
      if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0) {
        Genfun::CumulativeChiSquare probabilityFunction( fitQual->numberDoF() );
        chi2Prob = 1 - probabilityFunction( fitQual->chiSquared() );
      }
    }            
    
    chisquared = (fitQual) ? fitQual->chiSquared() : -1.;
    DoF        = (fitQual) ? fitQual->numberDoF() : -1;
    if(DoF>0) chi2oDoF = chisquared/(float)DoF;

    if (trkphi<0) trkphi+=2*m_Pi;
    
    ngTracks++;    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << nTracks << " is a good track!" << endmsg;  

   
    // variables for number of hits
    int nhpixB=0, nhibl=0, nhpixECA=0, nhpixECC=0, nhsctB=0, nhsctECA=0, nhsctECC=0, nhtrtB=0, nhtrtECA=0, nhtrtECC=0;

    // loop over all hits on track
    const DataVector<const Trk::TrackStateOnSurface>* TSOS;
    TSOS = (*trksItr)->trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr  = TSOS->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItrE = TSOS->end();

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"starting to loop over TSOS"<<endmsg;
 
    for (; TSOSItr != TSOSItrE; ++TSOSItr) {
     
      //check that we have track parameters defined for the surface (pointer is not null)
      if(!((*TSOSItr)->trackParameters())) {
        if (msgLvl(MSG::DEBUG)) msg() << "hit skipped because no associated track parameters" << endmsg;
        continue;
      }
      
      Identifier surfaceID;
      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
      // hits, outliers
      if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement()!=NULL) surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();

      // holes, perigee 
      else continue; 


      if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){

        //hit quality cuts for Si hits if tool is configured - default is NO CUTS
        if (m_idHelper->is_pixel(surfaceID) ||  m_idHelper->is_sct(surfaceID)){
          
          if(m_doHitQuality) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "applying hit quality cuts to Silicon hit..." << endmsg;
            
            const Trk::RIO_OnTrack* hit = m_hitQualityTool->getGoodHit(*TSOSItr);
            if(hit==NULL) {
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit failed quality cuts and is rejected." << endmsg;
              continue;
            }
            else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit passed quality cuts" << endmsg;
          }
          else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit quality cuts NOT APPLIED to Silicon hit." << endmsg;
        }
      
        const Trk::Surface& hitSurface  = mesb->associatedSurface();
        float hitSurfaceX = hitSurface.center().x();
        float hitSurfaceY = hitSurface.center().y();
      
        const Amg::Vector3D&  gp  = mesb->globalPosition();
        
        // --- pixel
        if (m_idHelper->is_pixel(surfaceID)){
          if(m_pixelID->barrel_ec(surfaceID)      ==  0){
            nhpixB++;
            m_hitMap_barrel   -> Fill( hitSurfaceX,  hitSurfaceY, TrkWeight );
            m_hitMap_barrel_zoom-> Fill( hitSurfaceX,  hitSurfaceY, TrkWeight );

	    if (m_pixelID->layer_disk(surfaceID)      ==  0){
	      nhibl++;
	    }
          }
          else if(m_pixelID->barrel_ec(surfaceID) ==  2)  nhpixECA++;
          else if(m_pixelID->barrel_ec(surfaceID) == -2) nhpixECC++;
        }
        // --- sct
        else if (m_idHelper->is_sct(surfaceID)){
          if(m_sctID->barrel_ec(surfaceID)      ==  0){
            nhsctB++;
            m_hitMap_barrel   -> Fill( hitSurfaceX,  hitSurfaceY, TrkWeight );
          }
          else if(m_sctID->barrel_ec(surfaceID) ==  2) nhsctECA++;
          else if(m_sctID->barrel_ec(surfaceID) == -2) nhsctECC++;
        }
        // --- trt
        if (m_idHelper->is_trt(surfaceID)){
          int m_barrel_ec      = m_trtID->barrel_ec(surfaceID);
          if(m_barrel_ec == 1 || m_barrel_ec == -1 ) {
            nhtrtB++;
            m_hitMap_barrel  -> Fill( hitSurfaceX,   hitSurfaceY, TrkWeight );
          }
          else if(m_barrel_ec ==  2){
            nhtrtECA++;
            m_hitMap_endcapA -> Fill( float(gp.z()), float(gp.phi()), TrkWeight );
          }else if(m_barrel_ec == -2){
            nhtrtECC++;
            m_hitMap_endcapC -> Fill( float(gp.z()), float(gp.phi()), TrkWeight );
          }
        }
      }
    }

    int nhpix=nhpixB+nhpixECA+nhpixECC;
    int nhsct=nhsctB+nhsctECA+nhsctECC;
    int nhtrt=nhtrtB+nhtrtECA+nhtrtECC;
    int nhits=nhpix+nhsct+nhtrt;
    nHits += nhits;

    m_nhits_per_track -> Fill(nhits, TrkWeight);
    m_nsilhits_per_track -> Fill(nhpix+nhsct);
    m_niblhits_per_track -> Fill(nhibl, TrkWeight);
    m_npixhits_per_track -> Fill(nhpix, TrkWeight); // including IBL and old_Pixels
    m_nscthits_per_track -> Fill(nhsct, TrkWeight);
    m_ntrthits_per_track -> Fill(nhtrt, TrkWeight);
    // barrel
    m_npixhits_per_track_barrel -> Fill(nhpixB, TrkWeight);
    m_nscthits_per_track_barrel -> Fill(nhsctB, TrkWeight);
    m_ntrthits_per_track_barrel -> Fill(nhtrtB, TrkWeight);
    // eca
    m_npixhits_per_track_eca -> Fill(nhpixECA, TrkWeight);
    m_nscthits_per_track_eca -> Fill(nhsctECA, TrkWeight);
    m_ntrthits_per_track_eca -> Fill(nhtrtECA, TrkWeight);
    // ecc  
    m_npixhits_per_track_ecc -> Fill(nhpixECC, TrkWeight);
    m_nscthits_per_track_ecc -> Fill(nhsctECC, TrkWeight);
    m_ntrthits_per_track_ecc -> Fill(nhtrtECC, TrkWeight);

    if (nhibl   >= 1) m_summary -> Fill(0., TrkWeight);
    if (nhpix   >= 3) m_summary -> Fill(1., TrkWeight); 
    if (nhsct   >= 8) m_summary -> Fill(2., TrkWeight);
    if (nhtrt   >=20) m_summary -> Fill(3., TrkWeight);
    if (nhpixB  >= 3) m_summary -> Fill(4., TrkWeight);
    if (nhsctB  >= 8) m_summary -> Fill(5., TrkWeight);
    if (nhtrtB  >=20) m_summary -> Fill(6., TrkWeight);
    if (nhpixECA>= 2) m_summary -> Fill(7., TrkWeight);
    if (nhsctECA>= 2) m_summary -> Fill(8., TrkWeight);
    if (nhtrtECA>=15) m_summary -> Fill(9., TrkWeight);
    if (nhpixECC>= 2) m_summary -> Fill(10., TrkWeight);
    if (nhsctECC>= 2) m_summary -> Fill(11., TrkWeight);
    if (nhtrtECC>=15) m_summary -> Fill(12., TrkWeight);

    bool hasECAhits = false;
    if(nhpixECA+nhsctECA+nhtrtECA > 0) hasECAhits = true;
    bool hasECChits = false;
    if(nhpixECC+nhsctECC+nhtrtECC > 0) hasECChits = true;

    
    if (!hasECAhits && !hasECChits) { //filling barrel histograms
      m_chi2oDoF_barrel             -> Fill(chi2oDoF, TrkWeight);
      m_phi_barrel                  -> Fill(trkphi  , TrkWeight);
            

      if(m_doIP && myIPandSigma){
		
        m_trk_d0_wrtPV_vs_phi_vs_eta_barrel->Fill(trketa, trkphi, myIPandSigma->IPd0, TrkWeight);
        m_trk_z0_wrtPV_vs_phi_vs_eta_barrel->Fill(trketa, trkphi, myIPandSigma->IPz0, TrkWeight);
      }


      if(charge > 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_barrel_pos_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_barrel_pos_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_barrel_pos_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_barrel_pos_20plusGeV-> Fill(trkphi, TrkWeight);
      } else if(charge < 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_barrel_neg_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_barrel_neg_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_barrel_neg_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_barrel_neg_20plusGeV-> Fill(trkphi, TrkWeight);
      }
    }
    else if (hasECAhits) {//filling endcap A histograms
      m_chi2oDoF_eca -> Fill(chi2oDoF, TrkWeight);
      m_phi_eca      -> Fill(trkphi  , TrkWeight);  
      
      
      
      if(m_doIP && myIPandSigma){
        m_trk_d0_wrtPV_vs_phi_vs_eta_eca -> Fill(trketa, trkphi, myIPandSigma->IPd0, TrkWeight);
        m_trk_z0_wrtPV_vs_phi_vs_eta_eca -> Fill(trketa, trkphi, myIPandSigma->IPz0, TrkWeight);
      }

      if(charge > 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_eca_pos_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_eca_pos_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_eca_pos_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_eca_pos_20plusGeV-> Fill(trkphi, TrkWeight);
      } else if(charge < 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_eca_neg_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_eca_neg_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_eca_neg_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_eca_neg_20plusGeV-> Fill(trkphi, TrkWeight);
      }
    }
    else if (hasECChits) {//filling endcap C histograms
      m_chi2oDoF_ecc -> Fill(chi2oDoF, TrkWeight);
      m_phi_ecc      -> Fill(trkphi  , TrkWeight);
      

      if(m_doIP && myIPandSigma){
        m_trk_d0_wrtPV_vs_phi_vs_eta_ecc -> Fill(trketa, trkphi, myIPandSigma->IPd0, TrkWeight);
        m_trk_z0_wrtPV_vs_phi_vs_eta_ecc -> Fill(trketa, trkphi, myIPandSigma->IPz0, TrkWeight);
      }


      if(charge > 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_ecc_pos_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_ecc_pos_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_ecc_pos_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_ecc_pos_20plusGeV-> Fill(trkphi, TrkWeight);
      } else if(charge < 0){
        if(trkpt> 2. && trkpt< 5.)     m_phi_ecc_neg_2_5GeV   -> Fill(trkphi, TrkWeight);
        else if(trkpt> 5. && trkpt<10.)m_phi_ecc_neg_5_10GeV  -> Fill(trkphi, TrkWeight);
        else if(trkpt>10. && trkpt<20.)m_phi_ecc_neg_10_20GeV -> Fill(trkphi, TrkWeight);
        else if(trkpt>20.)             m_phi_ecc_neg_20plusGeV-> Fill(trkphi, TrkWeight);
      }
    }
  
    m_chi2oDoF -> Fill(chi2oDoF, TrkWeight);
    m_eta      -> Fill(trketa  , TrkWeight);
    if (charge>0) m_eta_pos -> Fill (trketa, TrkWeight);
    else          m_eta_neg -> Fill (trketa, TrkWeight);
    m_phi -> Fill(trkphi, TrkWeight);
    if(charge > 0){
      if(trkpt> 2. && trkpt< 5.)     m_eta_phi_pos_2_5GeV   -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt> 5. && trkpt<10.)m_eta_phi_pos_5_10GeV  -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt>10. && trkpt<20.)m_eta_phi_pos_10_20GeV -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt>20.)             m_eta_phi_pos_20plusGeV-> Fill(trketa,trkphi, TrkWeight);
    } else if(charge < 0){
      if(trkpt> 2. && trkpt< 5.)     m_eta_phi_neg_2_5GeV   -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt> 5. && trkpt<10.)m_eta_phi_neg_5_10GeV  -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt>10. && trkpt<20.)m_eta_phi_neg_10_20GeV -> Fill(trketa,trkphi, TrkWeight);
      else if(trkpt>20.)             m_eta_phi_neg_20plusGeV-> Fill(trketa,trkphi, TrkWeight);
    }
    m_z0 -> Fill(trkz0, TrkWeight);
    m_z0sintheta -> Fill(trkz0*(sin(trktheta)), TrkWeight);
    m_d0        -> Fill(trkd0, TrkWeight);
    m_d0_bscorr -> Fill(d0bscorr    , TrkWeight);
    
    if(m_doIP && myIPandSigma){
      m_trk_d0_wrtPV -> Fill(myIPandSigma->IPd0, TrkWeight);
      m_trk_z0_wrtPV -> Fill(myIPandSigma->IPz0, TrkWeight);
      }
    m_pT        -> Fill(charge*trkpt, TrkWeight);
    m_pTabs     -> Fill(trkpt, TrkWeight);
    m_P         -> Fill(trkP        , TrkWeight);
    if(charge>0) m_pT_p -> Fill(trkpt, TrkWeight);
    if(charge<0) m_pT_n -> Fill(trkpt, TrkWeight);
    
    m_pTRes      -> Fill(fabs(Err_qOverP/qOverP)       , TrkWeight);
    m_pTResOverP -> Fill(fabs(Err_qOverP/qOverP*qOverP), TrkWeight);

    

    m_Tracks_per_LumiBlock->Fill(float(LumiBlock), TrkWeight);
    m_NPIX_per_LumiBlock->Fill(float(LumiBlock), nhpix*TrkWeight);
    m_NSCT_per_LumiBlock->Fill(float(LumiBlock), nhsct*TrkWeight);
    m_NTRT_per_LumiBlock->Fill(float(LumiBlock), nhtrt*TrkWeight);

    
    if(m_doIP && myIPandSigma){
      m_trk_d0_wrtPV_vs_phi_vs_eta-> Fill(trketa, trkphi, myIPandSigma->IPd0, TrkWeight);
      m_trk_z0_wrtPV_vs_phi_vs_eta-> Fill(trketa, trkphi, myIPandSigma->IPz0, TrkWeight);
    }


    if(m_extendedPlots){
      m_d0_pvcorr -> Fill(trkd0c      , TrkWeight);
      m_z0_pvcorr  -> Fill(trkz0c, TrkWeight);
      
      if(charge>0) {
	m_trk_d0c_pos -> Fill(trkd0c, TrkWeight);
	m_trk_z0c_pos -> Fill(trkz0c, TrkWeight);
      }
      if(charge<0) {
	m_trk_d0c_neg -> Fill(trkd0c, TrkWeight);
	m_trk_z0c_neg -> Fill(trkz0c, TrkWeight);
      }
      
      m_trk_d0_vs_phi0_z0->Fill (trkphi, trkz0, trkd0);
      m_z0sintheta_pvcorr -> Fill(trkz0c*(sin(trktheta)), TrkWeight);
      m_trk_chi2oDoF -> Fill(trketa,chi2oDoF, TrkWeight);
      m_trk_chi2Prob -> Fill(trketa,chi2Prob, TrkWeight);
      m_trk_d0_vs_phi_vs_eta -> Fill(trketa, trkphi, trkd0c, TrkWeight);

      m_trk_PIXvSCTHits  -> Fill( nhsct            , nhpix   , TrkWeight);
      m_trk_PIXHitsvEta  -> Fill( trketa           , nhpix   , TrkWeight);    
      m_trk_SCTHitsvEta  -> Fill( trketa           , nhsct   , TrkWeight);
      m_trk_TRTHitsvEta  -> Fill( trketa           , nhtrt   , TrkWeight);
      m_trk_chi2oDoF_Phi -> Fill( trkphi           , chi2oDoF, TrkWeight);
      m_trk_chi2oDoF_Pt  -> Fill( charge*trkpt     , chi2oDoF, TrkWeight);
      m_trk_chi2oDoF_P   -> Fill( charge*fabs(trkP), chi2oDoF, TrkWeight);
      m_trk_chi2ProbDist -> Fill( chi2Prob                   , TrkWeight);
      m_errCotTheta      -> Fill( Err_cottheta               , TrkWeight);
      m_errCotThetaVsD0BS-> Fill( d0bscorr         , Err_cottheta, TrkWeight);
      m_errCotThetaVsPt  -> Fill( fabs(trkpt)      , Err_cottheta, TrkWeight);
      m_errCotThetaVsP   -> Fill( fabs(trkP)       , Err_cottheta, TrkWeight);
      m_errCotThetaVsPhi -> Fill( trkphi           , Err_cottheta, TrkWeight);
      m_errCotThetaVsEta -> Fill( trketa           , Err_cottheta, TrkWeight);
      m_errTheta         -> Fill( Err_theta                   , TrkWeight); 
      m_errThetaVsD0BS   -> Fill( d0bscorr         , Err_theta, TrkWeight);
      m_errThetaVsPt     -> Fill( fabs(trkpt)      , Err_theta, TrkWeight);
      m_errThetaVsP      -> Fill( fabs(trkP)       , Err_theta, TrkWeight);
      m_errThetaVsPhi    -> Fill( trkphi           , Err_theta, TrkWeight);
      m_errThetaVsEta    -> Fill( trketa           , Err_theta, TrkWeight);
      m_errD0            -> Fill( Err_d0                      , TrkWeight);
      m_errD0VsD0BS      -> Fill( d0bscorr         , Err_d0   , TrkWeight);
      m_errD0VsPt        -> Fill( fabs(trkpt)      , Err_d0   , TrkWeight);
      m_errD0VsP         -> Fill( fabs(trkP)       , Err_d0   , TrkWeight);
      m_errD0VsPhi       -> Fill( trkphi           , Err_d0   , TrkWeight);
      m_errD0VsEta       -> Fill( trketa           , Err_d0   , TrkWeight);
      m_errPhi0          -> Fill( Err_phi                     , TrkWeight);
      m_errPhi0VsD0BS    -> Fill( d0bscorr         , Err_phi  , TrkWeight);
      m_errPhi0VsPt      -> Fill( fabs(trkpt)      , Err_phi  , TrkWeight);
      m_errPhi0VsP       -> Fill( fabs(trkP)       , Err_phi  , TrkWeight);
      m_errPhi0VsPhi0    -> Fill( trkphi           , Err_phi  , TrkWeight);
      m_errPhi0VsEta     -> Fill( trketa           , Err_phi  , TrkWeight);
      m_errZ0            -> Fill( Err_z0                      , TrkWeight);
      //m_errZ0VsD0BS      -> Fill( d0bscorr         , Err_z0   , TrkWeight);
      m_errZ0VsPt        -> Fill( fabs(trkpt)      , Err_z0   , TrkWeight);
      m_errZ0VsP         -> Fill( fabs(trkP)       , Err_z0   , TrkWeight);
      m_errZ0VsPhi0      -> Fill( trkphi           , Err_z0   , TrkWeight);
      m_errZ0VsEta       -> Fill( trketa           , Err_z0   , TrkWeight);
      m_errPt            -> Fill( Err_Pt                      , TrkWeight);
      m_errPtVsD0BS      -> Fill( d0bscorr         , Err_Pt   , TrkWeight);
      m_errPtVsPt        -> Fill( trkpt            , Err_Pt   , TrkWeight);
      m_errPtVsP         -> Fill( fabs(trkP)       , Err_Pt   , TrkWeight);
      m_errPt_Pt2        -> Fill( Err_Pt/(trkpt*trkpt)        , TrkWeight);
      m_errPt_Pt2VsPt    -> Fill( trkpt , Err_Pt/(trkpt*trkpt), TrkWeight);
      m_errPt_Pt2VsEta   -> Fill( trketa, Err_Pt/(trkpt*trkpt), TrkWeight);
      m_errPt_Pt2VsPhi0  -> Fill( trkphi, Err_Pt/(trkpt*trkpt), TrkWeight);
      m_errPtVsPhi0      -> Fill( trkphi, Err_Pt              , TrkWeight);
      m_errPtVsEta       -> Fill( trketa, Err_Pt              , TrkWeight);
      m_D0VsPhi0         -> Fill( trkphi, trkd0               , TrkWeight);
      m_Z0VsEta          -> Fill( trketa, trkz0               , TrkWeight);
      m_QoverPtVsPhi0    -> Fill( trkphi, qOverPt             , TrkWeight);
      m_QoverPtVsEta     -> Fill( trketa, qOverPt             , TrkWeight);
      m_QPtVsPhi0        -> Fill( trkphi, charge*trkpt        , TrkWeight);
      m_QPtVsEta         -> Fill( trketa, charge*trkpt        , TrkWeight);
      // interaction per event histograms
      m_eventMu_vs_TrkPt -> Fill( m_mu, trkpt, TrkWeight);
      m_eventMu_vs_TrkEta-> Fill( m_mu, trketa, TrkWeight); 

      // pt vs eta
      m_trk_pT_vs_eta        -> Fill(trketa, trkpt         , TrkWeight);
      

      //bs plots
      m_D0bsVsPhi0 -> Fill( trkphi      , d0bscorr, TrkWeight);
      m_D0bsVsEta  -> Fill( trketa      , d0bscorr, TrkWeight);
      m_D0bsVsPt   -> Fill( charge*trkpt, d0bscorr, TrkWeight);
      
      if (!hasECAhits && !hasECChits) {//filling barrel histograms
        m_errD0VsPhiBarrel -> Fill(trkphi      , Err_d0  , TrkWeight);
        m_D0bsVsPhi0Barrel -> Fill(trkphi      , d0bscorr, TrkWeight);
        m_D0bsVsPtBarrel   -> Fill(charge*trkpt, d0bscorr, TrkWeight);
	m_trk_d0_vs_phi_vs_eta_barrel -> Fill(trketa, trkphi, trkd0c, TrkWeight); 
	m_trk_pT_vs_eta_barrel        -> Fill(trketa, trkpt         , TrkWeight);
	m_trk_d0_barrel  -> Fill(trkd0 , TrkWeight); 
	m_trk_d0_barrel_zoomin -> Fill(trkd0, TrkWeight);
	m_trk_d0c_barrel -> Fill(trkd0c, TrkWeight);            
	m_trk_z0_barrel  -> Fill(trkz0 , TrkWeight); 
	m_trk_z0_barrel_zoomin -> Fill(trkz0, TrkWeight);
	
	if (charge <0)
	  {
	    m_trk_d0c_neg_barrel  -> Fill( trkd0c , TrkWeight);
	    m_trk_z0c_neg_barrel  -> Fill( trkz0c , TrkWeight);
	  }
	else
	  {
	    m_trk_d0c_pos_barrel  -> Fill(trkd0c, TrkWeight);
	    m_trk_z0c_pos_barrel  -> Fill(trkz0c, TrkWeight);
	  }
      
	
      }
      else if (hasECAhits) {//filling ECA histograms
        m_errD0VsPhiECA -> Fill(trkphi      , Err_d0  , TrkWeight);
        m_D0bsVsPhi0ECA -> Fill(trkphi      , d0bscorr, TrkWeight);
        m_D0bsVsPtECA   -> Fill(charge*trkpt, d0bscorr, TrkWeight);
	m_trk_d0_vs_phi_vs_eta_eca -> Fill(trketa, trkphi, trkd0c, TrkWeight);
	m_trk_pT_vs_eta_eca        -> Fill(trketa, trkpt         , TrkWeight);
	m_trk_d0_eca  -> Fill(trkd0 , TrkWeight); m_trk_d0_eca_zoomin -> Fill(trkd0, TrkWeight);
	m_trk_d0c_eca -> Fill(trkd0c, TrkWeight);              
	m_trk_z0_eca  -> Fill(trkz0 , TrkWeight); m_trk_z0_eca_zoomin -> Fill(trkz0, TrkWeight);
	
	if (charge<0)
	  {
	    m_trk_d0c_neg_eca  -> Fill(trkd0c, TrkWeight);
	    m_trk_z0c_neg_eca  -> Fill(trkz0c, TrkWeight);
	  }
	else
	  {
	    m_trk_d0c_pos_eca  -> Fill(trkd0c, TrkWeight);
	    m_trk_z0c_pos_eca  -> Fill(trkz0c, TrkWeight);
	  }
	  

      }
      else if (hasECChits) {//filling ECA histograms
        m_errD0VsPhiECC -> Fill(trkphi      , Err_d0  , TrkWeight);
        m_D0bsVsPhi0ECC -> Fill(trkphi      , d0bscorr, TrkWeight);
        m_D0bsVsPtECC   -> Fill(charge*trkpt, d0bscorr, TrkWeight);
	m_trk_d0_vs_phi_vs_eta_ecc -> Fill(trketa, trkphi, trkd0c, TrkWeight);
	m_trk_pT_vs_eta_ecc        -> Fill(trketa, trkpt         , TrkWeight);
	m_trk_d0_ecc  -> Fill(trkd0 , TrkWeight); m_trk_d0_ecc_zoomin -> Fill(trkd0, TrkWeight);
	m_trk_d0c_ecc -> Fill(trkd0c, TrkWeight);              
	m_trk_z0_ecc  -> Fill(trkz0 , TrkWeight); m_trk_z0_ecc_zoomin -> Fill(trkz0, TrkWeight);
	
	if (charge <0)
	  {
	    m_trk_d0c_neg_ecc  -> Fill(trkd0c, TrkWeight);
	    m_trk_z0c_neg_ecc  -> Fill(trkz0c, TrkWeight);
	  }
	else
	  {
	    m_trk_d0c_pos_ecc  -> Fill(trkd0c, TrkWeight);
	    m_trk_z0c_pos_ecc  -> Fill(trkz0c, TrkWeight);
	  }
	  
	
      }
      if(charge>0)
        m_PtVsPhi0Pos->Fill(trkphi, trkpt, TrkWeight);
      else
        m_PtVsPhi0Neg->Fill(trkphi, trkpt, TrkWeight);
    }//Closing extended plots

    if (!hasECAhits && !hasECChits) {//filling barrel histograms
      m_trk_qopT_vs_phi_barrel -> Fill(trkphi, qOverPt, TrkWeight);
      m_trk_d0_vs_phi_barrel   -> Fill(trkphi, trkd0  , TrkWeight);
      m_trk_d0bs_vs_phi_barrel -> Fill(trkphi, d0bscorr, TrkWeight); 
      m_trk_d0_vs_z0_barrel    -> Fill(trkz0 , trkd0  , TrkWeight);
      if (charge<0) {
        m_trk_phi0_neg_barrel -> Fill( trkphi , TrkWeight);
        m_trk_pT_neg_barrel   -> Fill( trkpt  , TrkWeight);
        
      }
      else {
        m_trk_phi0_pos_barrel -> Fill(trkphi, TrkWeight);
        m_trk_pT_pos_barrel   -> Fill(trkpt , TrkWeight);
        
      }
    } else if (hasECAhits) {//filling endcap A histograms
      m_trk_qopT_vs_phi_eca -> Fill(trkphi,qOverPt, TrkWeight);
      m_trk_d0_vs_phi_eca   -> Fill(trkphi,trkd0  , TrkWeight);
      m_trk_d0_vs_z0_eca    -> Fill(trkz0 ,trkd0  , TrkWeight);
      if (charge<0) {
        m_trk_phi0_neg_eca -> Fill(trkphi, TrkWeight);
        m_trk_pT_neg_eca   -> Fill(trkpt , TrkWeight);
      }
      else {
        m_trk_phi0_pos_eca -> Fill(trkphi, TrkWeight);
        m_trk_pT_pos_eca   -> Fill(trkpt , TrkWeight);
      }
    } else if (hasECChits) {//filling endcap C histograms
      m_trk_qopT_vs_phi_ecc -> Fill(trkphi, qOverPt, TrkWeight);
      m_trk_d0_vs_phi_ecc   -> Fill(trkphi, trkd0  , TrkWeight);
      m_trk_d0_vs_z0_ecc    -> Fill(trkz0 , trkd0  , TrkWeight);
      if (charge<0) {
        m_trk_phi0_neg_ecc -> Fill(trkphi, TrkWeight);
        m_trk_pT_neg_ecc   -> Fill(trkpt , TrkWeight);
      }
      else {
        m_trk_phi0_pos_ecc -> Fill(trkphi, TrkWeight);
        m_trk_pT_pos_ecc   -> Fill(trkpt , TrkWeight);
        
      }
    } 
  
    if(trkpt > ptlast){
      z_E[0]   = fabs(trkpt / sin(trktheta));
      z_pz[0]  = trkpt / tan(trktheta);
      z_px[0]  = trkpt * sin(trkphi);
      z_py[0]  = trkpt * cos(trkphi);
      z_pT[0]  = charge*trkpt;
      z_eta[0] = trketa;
      ptlast   = trkpt;
      chargefirst = (int)charge;
    }


    // 
    

  } // end of loop on trks

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of good tracks from TrackCollection: "<< ngTracks<< endmsg;

  m_nhits_per_event -> Fill(nHits   , TrkWeight);
  m_ntrk            -> Fill(nTracks , TrkWeight);
  m_ngtrk           -> Fill(ngTracks, TrkWeight);

  float ptfirst = ptlast;
  ptlast = 0;
  trksItr  = trks->begin();
  for (; trksItr != trksItrE; ++trksItr) {     
    //const Trk::Track* trksItr = (*trkPsItr)->track();
    
    float trkphi   = -999;
    float trktheta = -999;
    float trkpt    = -999;
    float trketa   = -999;
    float qOverP   = -999;
    float charge   = 0;
    
  
    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
 
    const Trk::Perigee* measPer = (*trksItr)->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : NULL;
    
    if (covariance == 0) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No measured perigee parameters assigned to the track or no covariance matrix associated to the perigee" << endmsg; 
    }
    else{  
      AmgVector(5)  perigeeParams = measPer->parameters(); 
      trkphi   = perigeeParams[Trk::phi0];  
      trktheta = perigeeParams[Trk::theta];
      trkpt    = measPer->pT()/1000.;  
      qOverP   = perigeeParams[Trk::qOverP]*1000.;
      trketa   = measPer->eta(); 
      if (qOverP < 0) charge = -1;
      else charge =+ 1; 
    }   
    if(trkpt > ptlast && trkpt < ptfirst && chargefirst*charge < 0 && trkpt > 20){
      z_E[1]   = fabs(trkpt / sin(trktheta));
      z_pz[1]  = trkpt / tan(trktheta);
      z_px[1]  = trkpt * sin(trkphi);
      z_py[1]  = trkpt * cos(trkphi);
      z_pT[1]  = charge*trkpt;
      z_eta[1] = trketa;
      ptlast   = trkpt;
      z_true   = true;
    } 
  }

  if(z_true){
    // build invariant mass of two highest pT tracks
    // M = sqrt(E1*E2 - px1*px2 - py1*py2 - pz1*pz2);
    float M = (z_E[0]+z_E[1])*(z_E[0]+z_E[1]) - (z_px[0]+z_px[1])*(z_px[0]+z_px[1]) - (z_py[0]+z_py[1])*(z_py[0]+z_py[1]) - (z_pz[0]+z_pz[1])*(z_pz[0]+z_pz[1]);
    if( M <= 0 ) 
      M =0;
    else 
      M = sqrt(M);
    

    m_Zmumu -> Fill(M, TrkWeight);

    if(z_pT[0] > 0){
      m_ZpT_p -> Fill(z_pT[0] , TrkWeight);
      m_ZpT_n -> Fill(-z_pT[1], TrkWeight);
    } else {
      m_ZpT_p -> Fill(z_pT[1] , TrkWeight);
      m_ZpT_n -> Fill(-z_pT[0], TrkWeight);
    }

    if(fabs(z_eta[0]) < m_barrelEta && fabs(z_eta[1]) < m_barrelEta) m_Zmumu_barrel-> Fill(M, TrkWeight);
    if(z_eta[0] >=  m_barrelEta && z_eta[1] >=  m_barrelEta)         m_Zmumu_eca   -> Fill(M, TrkWeight);
    if(z_eta[0] <= -m_barrelEta && z_eta[1] <= -m_barrelEta)         m_Zmumu_ecc   -> Fill(M, TrkWeight);
    
    if((fabs(z_eta[0]) < m_barrelEta && z_eta[1] >= m_barrelEta) ||
       (z_eta[0] >= m_barrelEta && fabs(z_eta[1]) < m_barrelEta))    m_Zmumu_barrel_eca -> Fill(M, TrkWeight);

    if((fabs(z_eta[0]) < m_barrelEta && z_eta[1] <= -m_barrelEta) ||
       (z_eta[0] <= -m_barrelEta && fabs(z_eta[1]) < m_barrelEta))   m_Zmumu_barrel_ecc -> Fill(M, TrkWeight);
  } 

  delete trks;

  return StatusCode::SUCCESS;
}


StatusCode IDAlignMonGenericTracks::procHistograms()
{
  if( endOfLowStatFlag() ) {
  }
  if( endOfLumiBlockFlag() ) {
  }
  if( endOfRunFlag() ) {

    m_ZpT_diff->Add(m_ZpT_p,m_ZpT_n,1.,-1);
    m_pT_diff->Add(m_pT_p,m_pT_n,1.,-1);
    
    ProcessAsymHistograms(m_eta_neg,             m_eta_pos,             m_eta_asym);
    ProcessAsymHistograms(m_pT_n,m_pT_p,m_trk_pT_asym);
    
    if (m_extendedPlots) {
      ProcessAsymHistograms(m_trk_phi0_neg_barrel, m_trk_phi0_pos_barrel, m_trk_phi0_asym_barrel);
      ProcessAsymHistograms(m_trk_phi0_neg_eca,    m_trk_phi0_pos_eca,    m_trk_phi0_asym_eca);
      ProcessAsymHistograms(m_trk_phi0_neg_ecc,    m_trk_phi0_pos_ecc,    m_trk_phi0_asym_ecc);
      ProcessAsymHistograms(m_trk_pT_neg_barrel,   m_trk_pT_pos_barrel,   m_trk_pT_asym_barrel);
      ProcessAsymHistograms(m_trk_pT_neg_eca,      m_trk_pT_pos_eca,      m_trk_pT_asym_eca);
      ProcessAsymHistograms(m_trk_pT_neg_ecc,      m_trk_pT_pos_ecc,      m_trk_pT_asym_ecc);
      ProcessAsymHistograms(m_trk_d0c_neg,m_trk_d0c_pos,m_trk_d0c_asym);
      ProcessAsymHistograms(m_trk_z0c_neg,m_trk_z0c_pos,m_trk_z0c_asym);
      ProcessAsymHistograms(m_trk_d0c_neg_barrel,m_trk_d0c_pos_barrel,m_trk_d0c_asym_barrel);
      ProcessAsymHistograms(m_trk_z0c_neg_barrel,m_trk_z0c_pos_barrel,m_trk_z0c_asym_barrel);
      ProcessAsymHistograms(m_trk_d0c_neg_eca,m_trk_d0c_pos_eca,m_trk_d0c_asym_eca);
      ProcessAsymHistograms(m_trk_z0c_neg_eca,m_trk_z0c_pos_eca,m_trk_z0c_asym_eca);
      ProcessAsymHistograms(m_trk_d0c_neg_ecc,m_trk_d0c_pos_ecc,m_trk_d0c_asym_ecc);
      ProcessAsymHistograms(m_trk_z0c_neg_ecc,m_trk_z0c_pos_ecc,m_trk_z0c_asym_ecc);
    }
  }
  
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonGenericTracks::ProcessAsymHistograms(TH1F* m_neg, TH1F* m_pos, TH1F* m_asym) 
{
  if (m_neg->GetNbinsX()==m_pos->GetNbinsX()&& m_neg->GetNbinsX()==m_asym->GetNbinsX()) {
    for (int i=1;i<=m_neg->GetNbinsX();i++) {
      float nneg=m_neg->GetBinContent(i);
      float npos=m_pos->GetBinContent(i);
      float asym=0;
      if (nneg+npos>0) asym=(npos-nneg)/(nneg+npos);
      m_asym->SetBinContent(i,asym);
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
                              << ">>ProcessAsymHistograms>> " << m_asym->GetTitle() 
                              << "  bin: " << i 
                              << "  npos=" << npos
                              << "  nneg=" << nneg
                              << "  asym=" << asym
                              << endmsg;
    }
  }

}


const xAOD::Vertex* IDAlignMonGenericTracks::findAssociatedVertexTP(const xAOD::TrackParticle *track) const
{

  std::map<const xAOD::TrackParticle*, const xAOD::Vertex* >::iterator tpVx =  m_trackVertexMapTP.find( track);

  if (tpVx == m_trackVertexMapTP.end() ){
    ATH_MSG_VERBOSE("Did not find the vertex. Returning 0");
    return 0;
  } 
  return (*tpVx).second;

}


//const Trk::VxCandidate* IDAlignMonGenericTracks::findAssociatedVertex(const Trk::Track *track) const
//{

//std::map<const Trk::Track*, const Trk::VxCandidate* >::iterator tpVx =  m_trackVertexMap.find( track);

//if (tpVx == m_trackVertexMap.end() ){
//  return 0;
//} 
//return (*tpVx).second;

//}

const Trk::Track* IDAlignMonGenericTracks::getTrkTrack(const Trk::VxTrackAtVertex *trkAtVx)const
{

  //find the link to the TrackParticleBase
  const Trk::ITrackLink* trkLink = trkAtVx->trackOrParticleLink();
  const Trk::TrackParticleBase* trkPB(0);
  if(0!= trkLink){
    const Trk::LinkToTrackParticleBase* linktrkPB = dynamic_cast<const Trk::LinkToTrackParticleBase *>(trkLink);
    if(0!= linktrkPB){
      if(linktrkPB->isValid()) trkPB = linktrkPB->cachedElement();
    }//end of dynamic_cast check
  }//end of ITrackLink existance check

  //cast to TrackParticle
  if(trkPB){
    if ( trkPB->trackElementLink()->isValid() ) {      
      // retrieve and refit original track
      const Trk::Track* trktrk = trkPB->originalTrack();
      return trktrk;
    } 
  } 
  return 0;
}



bool IDAlignMonGenericTracks::fillVertexInformation() const
{
  ATH_MSG_DEBUG("Generic Tracks: fillVertexInformation(): Checking ");
  m_trackVertexMapTP.clear();
  const xAOD::VertexContainer* vxContainer(0);
  StatusCode sc = evtStore()->retrieve(vxContainer, m_VxPrimContainerName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Could not retrieve primary vertex info: " << m_VxPrimContainerName);
    return false;
  }
  
  
  ATH_MSG_DEBUG( "Found primary vertex info: " << m_VxPrimContainerName );
  if(vxContainer) {
    ATH_MSG_VERBOSE("Nb of reco primary vertex for coll "
                      << " = " << vxContainer->size() );


    xAOD::VertexContainer::const_iterator vxI = vxContainer->begin();
    xAOD::VertexContainer::const_iterator vxE = vxContainer->end();
    for(; vxI!=vxE; ++vxI) {
      //int nbtk = 0;
      //const std::vector<Trk::VxTrackAtVertex*>* tracks = (*vxI)->vxTrackAtVertex();
      auto tpLinks = (*vxI)->trackParticleLinks();
      ATH_MSG_DEBUG("tpLinks size " << tpLinks.size());

      if (tpLinks.size() > 4 ) {
	for(auto link: tpLinks) {
	  const xAOD::TrackParticle *TP = *link;
          if(TP) {
            m_trackVertexMapTP.insert( std::make_pair( TP, *vxI )  );
          }
        }
      }
    }
  }
 
  return true;
}
