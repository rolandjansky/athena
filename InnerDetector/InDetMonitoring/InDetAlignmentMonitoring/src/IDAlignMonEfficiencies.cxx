/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDAlignMonEfficiencies.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, John Alison, Lauren Tompkins
// **********************************************************************
#include <sstream>
#include <math.h>

#include "xAODEventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "TrkParameters/TrackParameters.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

//#include "Particle/TrackParticleContainer.h"
//#include "Particle/TrackParticle.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
// #include "VxVertex/VxContainer.h"
// #include "VxVertex/VxCandidate.h"
// #include "VxVertex/RecVertex.h"
// #include "VxVertex/Vertex.h"
// #include "VxVertex/VxTrackAtVertex.h"

//#include "AthenaMonitoring/AthenaMonManager.h"
//#include "InDetAlignmentMonitoring/IDAlignMonEfficiencies.h"
#include "IDAlignMonEfficiencies.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

//#include "TrkTrackSummary/TrackSummary.h"
//#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetAlignGenTools/IInDetAlignHitQualSelTool.h"
#include "TrackSelectionTool.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"

// *********************************************************************
// Public Methods
// *********************************************************************

struct IDAlignMonEfficiencies::TRTBarrelHistograms{
  /** TubeHits

  When there are hits in the TRT which lie beyond a certian sigma from the track fit
  (currently 2 sigma), but are not far enough away to be called outliers and not used
  in the fit, they are marked as "tube" hits.  When a hit becomes a tube hit the drift
  time information is ignored, the center of the straw is taken as the position of the hit,
  and an error of 4mm/sqrt(12) is assigned.  */

  /** TRT Barrel */
  /**  total hits (hits + tubeHits + outliers) in the TRT barel modules(0-2) vs phi sector */
  TH1F_LW* totHits_vs_phiSector[3];

  /** total hits (hits + tubeHits + outliers) in the TRT barel modules(0-2) vs phi sector */
  TH1F_LW* totHits_vs_StrawLay;

  /** number of (hits/tubeHits/outliers) in the TRT barel modules(0-2) vs phi sector */
  TH1F_LW* outliers_vs_phiSector[3];
  TH1F_LW* hits_vs_phiSector[3];
  TH1F_LW* tubeHits_vs_phiSector[3];

  /** number of (hits/tubeHits/outliers) in the TRT barel modules(0-2) vs straw layer (0-72) */
  TH1F_LW* outliers_vs_StrawLay;
  TH1F_LW* hits_vs_StrawLay;
  TH1F_LW* tubeHits_vs_StrawLay;

  /** ratio of (hits/tubeHits/outliers) to total measurements (outl + hits + tubeHits) in the TRT barrel
      for the three modules (0-2) */
  TProfile* outliers_eff_vs_phiSector[3];
  TProfile* hits_eff_vs_phiSector[3];
  TProfile* tubeHits_eff_vs_phiSector[3];

  /** ratio of (hits/tubeHits/outliers) to total measurements (outl + hits + tubeHits) vs phi sector
      in the TRT Barrel for the three modules (0-2) */
  TProfile* outliers_eff_vs_StrawLay;
  TProfile* hits_eff_vs_StrawLay;
  TProfile* tubeHits_eff_vs_StrawLay;

  TRTBarrelHistograms(){

    totHits_vs_StrawLay = 0;
    outliers_vs_StrawLay = 0;
    hits_vs_StrawLay = 0;
    tubeHits_vs_StrawLay = 0;
    outliers_eff_vs_StrawLay = 0;
    hits_eff_vs_StrawLay = 0;
    tubeHits_eff_vs_StrawLay = 0;

    for(unsigned int lay=0; lay<3; ++lay){
      totHits_vs_phiSector[lay] = 0;
      outliers_vs_phiSector[lay] =0;
      hits_vs_phiSector[lay] =0;
      tubeHits_vs_phiSector[lay] =0;

      outliers_eff_vs_phiSector[lay] = 0;
      hits_eff_vs_phiSector[lay] = 0;
      tubeHits_eff_vs_phiSector[lay] = 0;


    }


  };

};

struct IDAlignMonEfficiencies::TRTEndcapHistograms{

  /** TRT Endcap Histograms */
  /** total hits (measurements+outliers+tubeHits) in the trt endcap vs ring layer */
  TH1F_LW* totHits_vs_ring[2];

  /** outliers/hits/tubeHits in the trt endcap vs ring layer */
  TH1F_LW* outliers_vs_ring[2];
  TH1F_LW* hits_vs_ring[2];
  TH1F_LW* tubeHits_vs_ring[2];

  /** ratio of outliers/hits/tubeHits  to total measurements (outl + hits) vs ring in the TRT Endcap */
  TProfile* outliers_eff_vs_ring[2];
  TProfile* hits_eff_vs_ring[2];
  TProfile* tubeHits_eff_vs_ring[2];

  /** total hits (measurements+outliers+tubeHits) in the trt endcap vs phiSector layer */
  TH1F_LW* totHits_vs_phiSector[2];

  /** outliers/hits/tubeHits in the trt endcap vs phiSector layer */
  TH1F_LW* outliers_vs_phiSector[2];
  TH1F_LW* hits_vs_phiSector[2];
  TH1F_LW* tubeHits_vs_phiSector[2];

  /** ratio of outliers/hits/tubeHits  to total measurements (outl + hits) vs phiSector in the TRT Endcap */
  TProfile* outliers_eff_vs_phiSector[2];
  TProfile* hits_eff_vs_phiSector[2];
  TProfile* tubeHits_eff_vs_phiSector[2];

  TRTEndcapHistograms(){

    for(unsigned int side=0; side<2; ++side){
      totHits_vs_ring[side] = 0;
      outliers_vs_ring[side] = 0;
      hits_vs_ring[side] = 0;
      tubeHits_vs_ring[side] = 0;
      outliers_eff_vs_ring[side] = 0;
      hits_eff_vs_ring[side] = 0;
      tubeHits_eff_vs_ring[side] = 0;

      totHits_vs_phiSector[side] = 0;
      outliers_vs_phiSector[side] = 0;
      hits_vs_phiSector[side] = 0;
      tubeHits_vs_phiSector[side] = 0;
      outliers_eff_vs_phiSector[side] = 0;
      hits_eff_vs_phiSector[side] = 0;
      tubeHits_eff_vs_phiSector[side] = 0;
    }

  };

};



IDAlignMonEfficiencies::IDAlignMonEfficiencies( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_trt_b_hist(new TRTBarrelHistograms),
   m_trt_ec_hist(new TRTEndcapHistograms),
   m_idHelper(0),
   m_pixelID(0),
   m_sctID(0),
   m_trtID(0),
   m_PIX_Mgr(0),
   m_SCT_Mgr(0),
   m_Pi(3.14156),
   m_NPixLayers(0),
   m_events(0),
   m_histosBooked(0),
   m_doHitQuality(false)
{
  m_minLB=-0.5;
  m_maxLB=3023.5;
  m_nLB = 3024;
  m_trackSelection = ToolHandle<InDetAlignMon::TrackSelectionTool>("InDetAlignMon::TrackSelectionTool");
  m_hitQualityTool = ToolHandle<IInDetAlignHitQualSelTool>("");
  m_holeSearchTool = ToolHandle<Trk::ITrackHoleSearchTool>("InDetHoleSearchTool");
  m_trackSumTool        = ToolHandle<Trk::ITrackSummaryTool>("Trk::TrackSummaryTool/InDetTrackSummaryTool");

  m_tracksName = "ExtendedTracks";
  m_minSiliconEffWindow = 0.8;
  m_maxSiliconEffWindow = 1.05;
  m_triggerChainName = "NoTriggerSelection";
  m_doHoleSearch = true;
  m_extendedPlots= false;
  m_mapSplit=1;
  m_useLowStat = true;

  InitializeHistograms();

  declareProperty("tracksName"             , m_tracksName);
  declareProperty("CheckRate"              , m_checkrate=1000);
  declareProperty("HoleSearch"             , m_holeSearchTool);
  declareProperty("doHoleSearch"           , m_doHoleSearch);
  declareProperty("trackSelection"         , m_trackSelection);
  declareProperty("minSiliconEffWindow"    , m_minSiliconEffWindow);
  declareProperty("maxSiliconEffWindow"    , m_maxSiliconEffWindow);
  declareProperty("HitQualityTool"         , m_hitQualityTool);
  declareProperty("triggerChainName"       , m_triggerChainName);
  declareProperty("Pixel_Manager"          , m_Pixel_Manager);
  declareProperty("SCT_Manager"            , m_SCT_Manager);
  declareProperty("TRT_Manager"            , m_TRT_Manager);
  declareProperty("trackSumTool"           , m_trackSumTool);
  declareProperty("NSplitMap"              , m_mapSplit);
  declareProperty("useExtendedPlots"       , m_extendedPlots);
  declareProperty("useLowStat"             , m_useLowStat);
}


IDAlignMonEfficiencies::~IDAlignMonEfficiencies() {
  delete m_trt_b_hist;
  delete m_trt_ec_hist;
}

void IDAlignMonEfficiencies::InitializeHistograms()
{

  m_hits_vs_layer_barrel = 0;
  m_hits_vs_layer_eca = 0;
  m_hits_vs_layer_ecc = 0;

  m_measurements_vs_layer_barrel = 0;
  m_measurements_vs_layer_eca = 0;
  m_measurements_vs_layer_ecc = 0;

  m_outliers_vs_layer_barrel = 0;
  m_outliers_vs_layer_eca = 0;
  m_outliers_vs_layer_ecc = 0;

  m_holes_vs_layer_barrel = 0;
  m_holes_vs_layer_eca = 0;
  m_holes_vs_layer_ecc = 0;

  m_noholes_vs_layer_barrel = 0;
  m_noholes_vs_layer_eca = 0;
  m_noholes_vs_layer_ecc = 0;

  m_overlapX_vs_layer_barrel = 0;
  m_overlapX_vs_layer_eca = 0;
  m_overlapX_vs_layer_ecc = 0;
  m_overlapY_vs_layer_barrel = 0;
  m_overlapY_vs_layer_eca = 0;
  m_overlapY_vs_layer_ecc = 0;

  m_measurements_eff_vs_layer_barrel = 0;
  m_measurements_eff_vs_layer_eca = 0;
  m_measurements_eff_vs_layer_ecc = 0;

  m_outliers_eff_vs_layer_barrel = 0;
  m_outliers_eff_vs_layer_eca = 0;
  m_outliers_eff_vs_layer_ecc = 0;

  m_holes_eff_vs_layer_barrel = 0;
  m_holes_eff_vs_layer_eca = 0;
  m_holes_eff_vs_layer_ecc = 0;

  m_noholes_eff_vs_layer_barrel = 0;
  m_noholes_eff_vs_layer_eca = 0;
  m_noholes_eff_vs_layer_ecc = 0;

  m_overlapX_eff_vs_layer_barrel = 0;
  m_overlapX_eff_vs_layer_eca = 0;
  m_overlapX_eff_vs_layer_ecc = 0;
  m_overlapY_eff_vs_layer_barrel = 0;
  m_overlapY_eff_vs_layer_eca = 0;
  m_overlapY_eff_vs_layer_ecc = 0;

  m_measurements_eff_vs_Eta_Phi_pix_eca = 0;
  m_measurements_eff_vs_Eta_Phi_pix_ecc = 0;
  m_measurements_vs_Eta_Phi_pix_eca = 0;
  m_measurements_vs_Eta_Phi_pix_ecc = 0;
  m_hits_vs_Eta_Phi_pix_eca = 0;
  m_hits_vs_Eta_Phi_pix_ecc = 0;
  m_outliers_vs_Eta_Phi_pix_eca = 0;
  m_outliers_vs_Eta_Phi_pix_ecc = 0;
  m_holes_vs_Eta_Phi_pix_eca = 0;
  m_holes_vs_Eta_Phi_pix_ecc = 0;

  m_measurements_eff_vs_Eta_Phi_sct_eca = 0;
  m_measurements_eff_vs_Eta_Phi_sct_ecc = 0;
  m_measurements_vs_Eta_Phi_sct_eca = 0;
  m_measurements_vs_Eta_Phi_sct_ecc = 0;
  m_measurements_vs_Eta_Phi_sct_eca_3d_s0 = 0;
  m_measurements_vs_Eta_Phi_sct_eca_3d_s1 = 0;
  m_measurements_vs_Eta_Phi_sct_ecc_3d_s0 = 0;
  m_measurements_vs_Eta_Phi_sct_ecc_3d_s1 = 0;
  m_hits_vs_Eta_Phi_sct_eca = 0;
  m_hits_vs_Eta_Phi_sct_ecc = 0;
  m_outliers_vs_Eta_Phi_sct_eca = 0;
  m_outliers_vs_Eta_Phi_sct_ecc = 0;
  m_holes_vs_Eta_Phi_sct_eca = 0;
  m_holes_vs_Eta_Phi_sct_ecc = 0;
}



StatusCode IDAlignMonEfficiencies::initialize()
{
  StatusCode sc;

  m_events=0;
  m_histosBooked = 0;

  //ID Helper
  sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get AtlasDetectorID !" << endmsg;
    return StatusCode::SUCCESS;
  }else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found AtlasDetectorID" << endmsg;
  }

  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get Pixel ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized PixelIDHelper" << endmsg;

  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTIDHelper" << endmsg;

  sc = detStore()->retrieve(m_trtID, "TRT_ID");
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get TRT ID helper !" << endmsg;
    return StatusCode::SUCCESS;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized TRTIDHelper" << endmsg;

  //Pixel Manager
  sc = detStore()->retrieve(m_PIX_Mgr,m_Pixel_Manager);
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get PIX_Manager !" << endmsg;
    return StatusCode::FAILURE;
   }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized PixelManager" << endmsg;
  //SCT Manager

  sc = detStore()->retrieve(m_SCT_Mgr, m_SCT_Manager);
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not get SCT_Manager !" << endmsg;
    return StatusCode::FAILURE;
  }
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialized SCTManager" << endmsg;


  if ( m_holeSearchTool.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve tool " << m_holeSearchTool << endmsg;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endmsg;
  }

  if ( m_trackSelection.retrieve().isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSelection << endmsg;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_trackSelection << endmsg;
  }

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to initialize ManagedMonitorToolBase!" << endmsg;
    return StatusCode::SUCCESS;
  }

  if (m_hitQualityTool.empty()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<
      "No hit quality tool configured - not hit quality cuts will be imposed"
	<< endmsg;
    m_doHitQuality = false;
  } else if (m_hitQualityTool.retrieve().isFailure()) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve "<< m_hitQualityTool
	<<" (to apply hit quality cuts to Si hits) "<< endmsg;
    m_doHitQuality = false;
   } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	<< "Hit quality tool setup "
	<< "- hit quality cuts will be applied to Si hits" << endmsg;
    m_doHitQuality = true;
  }

  // get TrackSummaryTool
  if ( m_trackSumTool.retrieve().isFailure() ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to retrieve tool " << m_trackSumTool << endmsg;
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSumTool << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode IDAlignMonEfficiencies::bookHistograms()
{
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  std::string outputDirName = "IDAlignMon/" + m_tracksName + "_" + m_triggerChainName + "/HitEfficiencies";
  MonGroup al_mon ( this, outputDirName, run );
  MonGroup al_mon_ls ( this, outputDirName, lowStat );

  if ( newLowStatFlag() ) {
  }
  if ( newLumiBlockFlag() ) {
  }
  if ( newRunFlag() ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    const Int_t nx = 22;
    TString siliconLayers[nx] = {"IBL", "Pix L0", "Pix L1","Pix L2","SCT L0 S0","S1","SCT L1 S0","S1","SCT L2 S0","S1","SCT L3 S0","S1","SCT L4 S0","S1","SCT L5 S0","S1","SCT L6 S0","S1","SCT L7 S0","S1","SCT L8 S0","S1"};

    // do plots by layers

    m_hits_vs_layer_barrel = new TH1F("hits_vs_layer_barrel","possible hits vs. layer in the barrel", 12,-0.5, 11.5); // 12
    for (int i=1;i<=12;i++) m_hits_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon, m_hits_vs_layer_barrel);

    m_hits_vs_layer_eca = new TH1F("hits_vs_layer_eca","possible hits vs. layer in the barrel eca",21,-0.5,20.5);
    for (int i=2;i<=nx;i++) m_hits_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon, m_hits_vs_layer_eca) ;
    m_hits_vs_layer_ecc = new TH1F("hits_vs_layer_ecc","possible hits vs. layer in the barrel ecc",21,-0.5,20.5);
    for (int i=2;i<=nx;i++) m_hits_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon, m_hits_vs_layer_ecc) ;

    m_measurements_vs_layer_barrel = new TH1F("measurements_vs_layer_barrel","measurements per possible hit vs. layer in the barrel",12,-0.5,11.5);
    for (int i=1; i<=12;i++) m_measurements_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_measurements_vs_layer_barrel) ;
    m_measurements_vs_layer_eca = new TH1F("measurements_vs_layer_eca","measurements per possible hit vs. layer in the eca",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_measurements_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_measurements_vs_layer_eca) ;
    m_measurements_vs_layer_ecc = new TH1F("measurements_vs_layer_ecc","measurements per possible hit vs. layer in the ecc",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_measurements_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_measurements_vs_layer_ecc) ;

    m_outliers_vs_layer_barrel = new TH1F("outliers_vs_layer_barrel","outliers per possible hit vs. layer in the barrel",12,-0.5,11.5);
    for (int i=1; i<=12;i++) m_outliers_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_vs_layer_barrel) ;
    m_outliers_vs_layer_eca = new TH1F("outliers_vs_layer_eca","outliers per possible hit vs. layer in the eca",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_outliers_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_vs_layer_eca) ;
    m_outliers_vs_layer_ecc = new TH1F("outliers_vs_layer_ecc","outliers per possible hit vs. layer in the ecc",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_outliers_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_vs_layer_ecc) ;

    m_holes_vs_layer_barrel = new TH1F("holes_vs_layer_barrel","holes per possible hit vs. layer in the barrel",12,-0.5,11.5);
    for (int i=1; i<=12;i++) m_holes_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_vs_layer_barrel) ;
    m_holes_vs_layer_eca = new TH1F("holes_vs_layer_eca","holes per possible hit vs. layer in the eca",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_holes_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_vs_layer_eca) ;
    m_holes_vs_layer_ecc = new TH1F("holes_vs_layer_ecc","holes per possible hit vs. layer in the ecc",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_holes_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_vs_layer_ecc) ;

    m_noholes_vs_layer_barrel = new TH1F("noholes_vs_layer_barrel","noholes per possible hit vs. layer in the barrel",12,-0.5,11.5);
    for (int i=1; i<=12;i++) m_noholes_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_vs_layer_barrel) ;
    m_noholes_vs_layer_eca = new TH1F("noholes_vs_layer_eca","noholes per possible hit vs. layer in the eca",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_noholes_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_vs_layer_eca) ;
    m_noholes_vs_layer_ecc = new TH1F("noholes_vs_layer_ecc","noholes per possible hit vs. layer in the ecc",21,-0.5,20.5);
    for (int i=2; i<=nx;i++) m_noholes_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_vs_layer_ecc) ;

    if (m_extendedPlots)
      {
	m_overlapX_vs_layer_barrel = new TH1F("overlapX_vs_layer_barrel","X Overlap Hits (one per overlap) vs. layer in the barrel",12,-0.5,11.5);
	for (int i=1;i<=12;i++) m_overlapX_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_vs_layer_barrel);
	m_overlapX_vs_layer_eca = new TH1F("overlapX_vs_layer_eca","X Overlap Hits (one per overlap) vs. layer in the barrel eca",21,-0.5,20.5);
	for (int i=2; i<=nx;i++) m_overlapX_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_vs_layer_eca) ;
	m_overlapX_vs_layer_ecc = new TH1F("overlapX_vs_layer_ecc","X Overlap Hits (one per overlap) vs. layer in the barrel ecc",21,-0.5,20.5);
	for (int i=2; i<=nx;i++) m_overlapX_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_vs_layer_ecc) ;

	m_overlapY_vs_layer_barrel = new TH1F("overlapY_vs_layer_barrel","Y Overlap Hits (one per overlap) vs. layer in the barrel",12,-0.5,11.5);
	for (int i=1; i<=12;i++) m_overlapY_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_vs_layer_barrel);
	m_overlapY_vs_layer_eca = new TH1F("overlapY_vs_layer_eca","Y Overlap Hits (one per overlap) vs. layer in the barrel eca",21,-0.5,20.5);
	for (int i=2; i<=nx;i++) m_overlapY_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_vs_layer_eca) ;
	m_overlapY_vs_layer_ecc = new TH1F("overlapY_vs_layer_ecc","Y Overlap Hits (one per overlap) vs. layer in the barrel ecc",21,-0.5,20.5);
	for (int i=2; i<=nx;i++) m_overlapY_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_vs_layer_ecc) ;

	m_overlapX_eff_vs_layer_barrel = new TProfile("overlapX_eff_vs_layer_barrel","fraction of OverlapX hits vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
	for (int i=1; i<=12;i++) m_overlapX_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_eff_vs_layer_barrel) ;
	m_overlapX_eff_vs_layer_eca = new TProfile("overlapX_eff_vs_layer_eca","fraction of OverlapX hits vs. layer in the eca",21,-0.5,20.5, 0., 1.);
	for (int i=2; i<=nx;i++) m_overlapX_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_eff_vs_layer_eca) ;
	m_overlapX_eff_vs_layer_ecc = new TProfile("overlapX_eff_vs_layer_ecc","fraction of OverlapX hits vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
	for (int i=2; i<=nx;i++) m_overlapX_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapX_eff_vs_layer_ecc) ;

	m_overlapY_eff_vs_layer_barrel = new TProfile("overlapY_eff_vs_layer_barrel","fraction of OverlapY hits vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
	for (int i=1; i<=12;i++) m_overlapY_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_eff_vs_layer_barrel) ;
	m_overlapY_eff_vs_layer_eca = new TProfile("overlapY_eff_vs_layer_eca","fraction of OverlapY hits vs. layer in the eca",21,-0.5,20.5, 0., 1.);
	for (int i=2; i<=nx;i++) m_overlapY_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_eff_vs_layer_eca) ;
	m_overlapY_eff_vs_layer_ecc = new TProfile("overlapY_eff_vs_layer_ecc","fraction of OverlapY hits vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
	for (int i=2; i<=nx;i++) m_overlapY_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
	RegisterHisto(al_mon,m_overlapY_eff_vs_layer_ecc) ;

      }


    //book efficiencie by layers
    m_measurements_eff_vs_layer_barrel = new TProfile("measurements_eff_vs_layer_barrel","measurements per possible hit vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
    for (int i=1; i<=12;i++) m_measurements_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    if ( m_useLowStat) RegisterHisto(al_mon_ls, m_measurements_eff_vs_layer_barrel) ;
    if (!m_useLowStat) RegisterHisto(al_mon,    m_measurements_eff_vs_layer_barrel) ;
    m_measurements_eff_vs_layer_eca = new TProfile("measurements_eff_vs_layer_eca","measurements per possible hit vs. layer in the eca",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_measurements_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    if ( m_useLowStat) RegisterHisto(al_mon_ls, m_measurements_eff_vs_layer_eca) ;
    if (!m_useLowStat) RegisterHisto(al_mon,    m_measurements_eff_vs_layer_eca) ;

    m_measurements_eff_vs_layer_ecc = new TProfile("measurements_eff_vs_layer_ecc","measurements per possible hit vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_measurements_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    if ( m_useLowStat) RegisterHisto(al_mon_ls, m_measurements_eff_vs_layer_ecc) ;
    if (!m_useLowStat) RegisterHisto(al_mon,    m_measurements_eff_vs_layer_ecc) ;

    m_outliers_eff_vs_layer_barrel = new TProfile("outliers_eff_vs_layer_barrel","outliers per possible hit vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
    for (int i=1; i<=12;i++) m_outliers_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_eff_vs_layer_barrel) ;
    m_outliers_eff_vs_layer_eca = new TProfile("outliers_eff_vs_layer_eca","outliers per possible hit vs. layer in the eca",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_outliers_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_eff_vs_layer_eca) ;
    m_outliers_eff_vs_layer_ecc = new TProfile("outliers_eff_vs_layer_ecc","outliers per possible hit vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_outliers_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_outliers_eff_vs_layer_ecc) ;

    m_holes_eff_vs_layer_barrel = new TProfile("holes_eff_vs_layer_barrel","holes per possible hit vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
    for (int i=1;i<=12;i++) m_holes_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_eff_vs_layer_barrel) ;
    m_holes_eff_vs_layer_eca = new TProfile("holes_eff_vs_layer_eca","holes per possible hit vs. layer in the eca",21,-0.5,20.5, 0., 1.);
    for (int i=2;i<=nx;i++) m_holes_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_eff_vs_layer_eca) ;
    m_holes_eff_vs_layer_ecc = new TProfile("holes_eff_vs_layer_ecc","holes per possible hit vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
    for (int i=2;i<=nx;i++) m_holes_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_holes_eff_vs_layer_ecc) ;

    m_noholes_eff_vs_layer_barrel = new TProfile("noholes_eff_vs_layer_barrel","noholes per possible hit vs. layer in the barrel",12,-0.5,11.5, 0., 1.);
    for (int i=1;i<=11;i++) m_noholes_eff_vs_layer_barrel->GetXaxis()->SetBinLabel(i,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_eff_vs_layer_barrel) ;
    m_noholes_eff_vs_layer_eca = new TProfile("noholes_eff_vs_layer_eca","noholes per possible hit vs. layer in the eca",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_noholes_eff_vs_layer_eca->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_eff_vs_layer_eca) ;
    m_noholes_eff_vs_layer_ecc = new TProfile("noholes_eff_vs_layer_ecc","noholes per possible hit vs. layer in the ecc",21,-0.5,20.5, 0., 1.);
    for (int i=2; i<=nx;i++) m_noholes_eff_vs_layer_ecc->GetXaxis()->SetBinLabel(i-1,siliconLayers[i-1]);
    RegisterHisto(al_mon,m_noholes_eff_vs_layer_ecc) ;

    m_measurements_eff_vs_Eta_Phi_pix_eca = new TProfile2D("measurements_eff_vs_Eta_Phi_pix_eca","hit eff. vs. Eta-Phi-ID in Pixel ECA",3,-0.5,2.5,48,-0.5,47.5,0.,1.);
    RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_pix_eca);
    m_measurements_eff_vs_Eta_Phi_pix_ecc = new TProfile2D("measurements_eff_vs_Eta_Phi_pix_ecc","hit eff. vs. Eta-Phi-ID in Pixel ECC",3,-0.5,2.5,48,-0.5,47.5,0.,1.);
    RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_pix_ecc);
    m_measurements_vs_Eta_Phi_pix_eca = new TH2F("measurements_vs_Eta_Phi_pix_eca","measurements vs. Eta-Phi-ID in Pixel ECA",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_pix_eca);
    m_measurements_vs_Eta_Phi_pix_ecc = new TH2F("measurements_vs_Eta_Phi_pix_ecc","measurements vs. Eta-Phi-ID in Pixel ECC",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_pix_ecc);
    m_hits_vs_Eta_Phi_pix_eca = new TH2F("hits_vs_Eta_Phi_pix_eca","hits vs. Eta-Phi-ID in Pixel ECA",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_hits_vs_Eta_Phi_pix_eca);
    m_hits_vs_Eta_Phi_pix_ecc = new TH2F("hits_vs_Eta_Phi_pix_ecc","hits vs. Eta-Phi-ID in Pixel ECC",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_hits_vs_Eta_Phi_pix_ecc);
    m_outliers_vs_Eta_Phi_pix_eca = new TH2F("outliers_vs_Eta_Phi_pix_eca","outliers vs. Eta-Phi-ID in Pixel ECA",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_pix_eca);
    m_outliers_vs_Eta_Phi_pix_ecc = new TH2F("outliers_vs_Eta_Phi_pix_ecc","outliers vs. Eta-Phi-ID in Pixel ECC",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_pix_ecc);
    m_holes_vs_Eta_Phi_pix_eca = new TH2F("holes_vs_Eta_Phi_pix_eca","holes vs. Eta-Phi-ID in Pixel ECA",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_holes_vs_Eta_Phi_pix_eca);
    m_holes_vs_Eta_Phi_pix_ecc = new TH2F("holes_vs_Eta_Phi_pix_ecc","holes vs. Eta-Phi-ID in Pixel ECC",3,-0.5,2.5,48,-0.5,47.5);
    RegisterHisto(al_mon,m_holes_vs_Eta_Phi_pix_ecc);

    m_measurements_eff_vs_Eta_Phi_sct_eca = new TProfile2D("measurements_eff_vs_Eta_Phi_sct_eca","hit eff. vs. Eta-Phi-ID in SCT ECA",9,-0.5,8.5,52,-0.5,51.5,0.,1.);
    RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_sct_eca);
    m_measurements_eff_vs_Eta_Phi_sct_ecc = new TProfile2D("measurements_eff_vs_Eta_Phi_sct_ecc","hit eff. vs. Eta-Phi-ID in SCT ECC",9,-0.5,8.5,52,-0.5,51.5,0.,1.);
    RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_sct_ecc);


    m_measurements_vs_Eta_Phi_sct_eca = new TH2F("measurements_vs_Eta_Phi_sct_eca","measurements vs. Eta-Phi-ID in SCT ECA",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_sct_eca);
    m_measurements_vs_Eta_Phi_sct_ecc = new TH2F("measurements_vs_Eta_Phi_sct_ecc","measurements vs. Eta-Phi-ID in SCT ECC",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_sct_ecc);

    if (m_extendedPlots)
      {
	// 3d histograms of measurements in SCT end caps
	m_measurements_vs_Eta_Phi_sct_eca_3d_s0 = new TH3F("measurements_vs_Eta_Phi_sct_eca_3d_s0","measurements vs. Eta-Phi-ID in SCT ECA (side 0)"
							   ,9,-0.5,8.5, 3,-0.5,2.5, 52,-0.5,51.5);
	m_measurements_vs_Eta_Phi_sct_eca_3d_s0->SetXTitle("Disk");
	m_measurements_vs_Eta_Phi_sct_eca_3d_s0->SetYTitle("#eta ring");
	m_measurements_vs_Eta_Phi_sct_eca_3d_s0->SetZTitle("#phi sector");
	RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_eca_3d_s0);

	m_measurements_vs_Eta_Phi_sct_eca_3d_s1 = new TH3F("measurements_vs_Eta_Phi_sct_eca_3d_s1","measurements vs. Eta-Phi-ID in SCT ECA (side 1)"
							   ,9,-0.5,8.5, 3,-0.5,2.5, 52,-0.5,51.5);
	m_measurements_vs_Eta_Phi_sct_eca_3d_s1->SetXTitle("Disk");
	m_measurements_vs_Eta_Phi_sct_eca_3d_s1->SetYTitle("#eta ring");
	m_measurements_vs_Eta_Phi_sct_eca_3d_s1->SetZTitle("#phi sector");
	RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_eca_3d_s1);

	m_measurements_vs_Eta_Phi_sct_ecc_3d_s0 = new TH3F("measurements_vs_Eta_Phi_sct_ecc_3d_s0","measurements vs. Eta-Phi-ID in SCT ECC (side 0)"
							   ,9,-0.5,8.5, 3,-0.5,2.5, 52,-0.5,51.5);
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s0->SetXTitle("Disk");
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s0->SetYTitle("#eta ring");
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s0->SetZTitle("#phi sector");
	RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_ecc_3d_s0);

	m_measurements_vs_Eta_Phi_sct_ecc_3d_s1 = new TH3F("measurements_vs_Eta_Phi_sct_ecc_3d_s1","measurements vs. Eta-Phi-ID in SCT ECC (side 1)"
							   ,9,-0.5,8.5, 3,-0.5,2.5, 52,-0.5,51.5);
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s1->SetXTitle("Disk");
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s1->SetYTitle("#eta ring");
	m_measurements_vs_Eta_Phi_sct_ecc_3d_s1->SetZTitle("#phi sector");
	RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_ecc_3d_s1);

      }

    m_hits_vs_Eta_Phi_sct_eca = new TH2F("hits_vs_Eta_Phi_sct_eca","hits vs. Eta-Phi-ID in SCT ECA",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_hits_vs_Eta_Phi_sct_eca);
    m_hits_vs_Eta_Phi_sct_ecc = new TH2F("hits_vs_Eta_Phi_sct_ecc","hits vs. Eta-Phi-ID in SCT ECC",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_hits_vs_Eta_Phi_sct_ecc);
    m_outliers_vs_Eta_Phi_sct_eca = new TH2F("outliers_vs_Eta_Phi_sct_eca","outliers vs. Eta-Phi-ID in SCT ECA",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_sct_eca);
    m_outliers_vs_Eta_Phi_sct_ecc = new TH2F("outliers_vs_Eta_Phi_sct_ecc","outliers vs. Eta-Phi-ID in SCT ECC",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_sct_ecc);
    m_holes_vs_Eta_Phi_sct_eca = new TH2F("holes_vs_Eta_Phi_sct_eca","holes vs. Eta-Phi-ID in SCT ECA",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_holes_vs_Eta_Phi_sct_eca);
    m_holes_vs_Eta_Phi_sct_ecc = new TH2F("holes_vs_Eta_Phi_sct_ecc","holes vs. Eta-Phi-ID in SCT ECC",9,-0.5,8.5,52,-0.5,51.5);
    RegisterHisto(al_mon,m_holes_vs_Eta_Phi_sct_ecc);

    /**=============================================
    //  TRT histograms
    //============================================= */
    makePIXBarrelHistograms(al_mon);
    makePIXEndCapsHistograms(al_mon);
    makeSCTBarrelHistograms(al_mon);
    makeSCTEndCapsHistograms(al_mon);
    makeTRTHistograms(al_mon, al_mon_ls);

    m_histosBooked++;
  }

  return StatusCode::SUCCESS;
}

/** Convert from an int to a string */
std::string IDAlignMonEfficiencies::intToString(int input){
  std::ostringstream stm;
  stm << input;
  return stm.str();
}

/** Make Histograms */
TH1F_LW* IDAlignMonEfficiencies::MakeHist(const std::string & name, const std::string & title, int nBins, float xLow, float xUp, const std::string & xAxisTitle, const std::string & yAxisTitle) {
  TH1F_LW* hist = TH1F_LW::create(name.c_str(), title.c_str(), nBins, xLow, xUp);
  hist->SetMinimum(0);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->SetTitle(xAxisTitle.c_str());
  hist->GetYaxis()->SetTitle(yAxisTitle.c_str());
  return hist;
}

/** Make Profiles */
TProfile* IDAlignMonEfficiencies::MakeProfile(const std::string & name, const std::string & title, int nBins, float xLow, float xUp, float yLow, float yUp, const std::string & xAxisTitle, const std::string & yAxisTitle, bool setMinZero) {
  TProfile* profile = new TProfile(name.c_str(),title.c_str(),nBins,xLow, xUp, yLow, yUp);
  if(setMinZero)
    profile->SetMinimum(0);
  profile->GetXaxis()->SetLabelSize(0.03);
  profile->GetYaxis()->SetLabelSize(0.03);
  profile->GetXaxis()->SetTitle(xAxisTitle.c_str());
  profile->GetYaxis()->SetTitle(yAxisTitle.c_str());

  return profile;
}

void IDAlignMonEfficiencies::RegisterHisto(MonGroup& mon, TH1* histo) {

  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1 Histogram:" << endmsg;
  }
  histo->GetYaxis()->SetTitle("Number of Hits on Tracks");
  const char* HistName = histo->GetName();
  if (strncmp (HistName,"outlier",7) == 0) histo->GetYaxis()->SetTitle("Number of Outlier");
  if (strncmp (HistName,"hole",4) == 0) histo->GetYaxis()->SetTitle("Number of Holes");
  if (strncmp (HistName,"hit",3) == 0) histo->GetYaxis()->SetTitle("Number of Possible Hits on Tracks");

  const char * pch;
  pch = strstr (HistName,"Phi"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Phi Identifier");
  pch = strstr (HistName,"Eta"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Eta Identifier");
  pch = strstr (HistName,"pT"); if (pch != NULL) histo->GetXaxis()->SetTitle("Signed Track pT [GeV]");
}

void IDAlignMonEfficiencies::RegisterHisto(MonGroup& mon, TH1F_LW* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH1 Histogram:" << endmsg;
  }
  histo->GetYaxis()->SetTitle("Number of Hits on Tracks");
  const char* HistName = histo->GetName();
  if (strncmp (HistName,"outlier",7) == 0) histo->GetYaxis()->SetTitle("Number of Outlier");
  if (strncmp (HistName,"hole",4) == 0) histo->GetYaxis()->SetTitle("Number of Holes");
  if (strncmp (HistName,"hit",3) == 0) histo->GetYaxis()->SetTitle("Number of Possible Hits on Tracks");

  const char * pch;
  pch = strstr (HistName,"Phi"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Phi Identifier");
  pch = strstr (HistName,"Eta"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Eta Identifier");
  pch = strstr (HistName,"pT"); if (pch != NULL) histo->GetXaxis()->SetTitle("Signed Track pT [GeV]");
}

void IDAlignMonEfficiencies::RegisterHisto(MonGroup& mon, TProfile* histo, const std::string & yAxisName) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile Histogram:" << endmsg;
  }

  if(yAxisName != ""){
    histo->GetYaxis()->SetTitle(yAxisName.c_str());
  }else{
    histo->GetYaxis()->SetTitle("Hit Efficiency");
  }

  const char* HistName = histo->GetName();
  if (strncmp (HistName,"outlier",7) == 0) histo->GetYaxis()->SetTitle("Outlier Fraction");
  if (strncmp (HistName,"hole",4) == 0) histo->GetYaxis()->SetTitle("Hole Fraction");

  const char * pch;
  pch = strstr (HistName,"Phi"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Phi Identifier");
  pch = strstr (HistName,"Eta"); if (pch != NULL) histo->GetXaxis()->SetTitle("Module Eta Identifier");
  pch = strstr (HistName,"pT"); if (pch != NULL) histo->GetXaxis()->SetTitle("Signed Track pT [GeV]");
}

void IDAlignMonEfficiencies::RegisterHisto(MonGroup& mon, TH2* histo) {

  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TH2 Histogram:" << endmsg;
  }
  histo->GetXaxis()->SetTitle("Module Eta Identifier");
  histo->GetYaxis()->SetTitle("Module Phi Identifier");
}

void IDAlignMonEfficiencies::RegisterHisto(MonGroup& mon, TProfile2D* histo) {

  //histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Cannot book TProfile2D Histogram:" << endmsg;
  }
  histo->GetXaxis()->SetTitle("Module Eta Identifier");
  histo->GetYaxis()->SetTitle("Module Phi Identifier");

  const char* HistName = histo->GetName();
  const char * pch;
  pch = strstr (HistName,"pix_eca"); if (pch != NULL) histo->GetXaxis()->SetTitle("Pixel ECA Disk");
  pch = strstr (HistName,"pix_ecc"); if (pch != NULL) histo->GetXaxis()->SetTitle("Pixel ECC Disk");
  pch = strstr (HistName,"sct_eca"); if (pch != NULL) histo->GetXaxis()->SetTitle("SCT ECA Disk");
  pch = strstr (HistName,"sct_ecc"); if (pch != NULL) histo->GetXaxis()->SetTitle("SCT ECC Disk");
}


StatusCode IDAlignMonEfficiencies::fillHistograms()
{
  m_events++;


  const DataHandle<xAOD::EventInfo> eventInfo;
  if (StatusCode::SUCCESS != evtStore()->retrieve( eventInfo ) ){
    msg(MSG::ERROR) << "Cannot get event info." << endmsg;
    return StatusCode::FAILURE;
  }
  unsigned int LumiBlock = eventInfo->lumiBlock();

  if (!evtStore()->contains<TrackCollection>(m_tracksName)) {
    if(m_events == 1) {if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to get " << m_tracksName << " TrackCollection" << endmsg;}
    else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to get " << m_tracksName << " TrackCollection" << endmsg;
    return StatusCode::SUCCESS;
  }

  // get TrackCollection
  DataVector<Trk::Track>* trks = m_trackSelection->selectTracks(m_tracksName);
  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
  for (; trksItr != trksItrE; ++trksItr) {

    //float trkd0          = -999;
    //float trkz0          = -999;
    //float trkphi         = -999;
    //float trktheta       = -999;
    //float trketa         = -999;
    float qOverP         = -999;
    float trkpt          = -999;
    float charge         = 0;
    float abs_trkpt      = -999;     //charge*trkpt

    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    //const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );

    //if (measPer==0) {
    //  if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endmsg;
    //}
    //else{
    //  CLHEP::HepVector perigeeParams = measPer->parameters();
    //  trkd0    = perigeeParams[Trk::d0];
    //  trkz0    = perigeeParams[Trk::z0];
    //  trkphi   = perigeeParams[Trk::phi0];
    //  trktheta = perigeeParams[Trk::theta];
    //  trketa   = measPer->eta();
    //  qOverP   = perigeeParams[Trk::qOverP]*1000.;
    //  trkpt    = measPer->pT()/1000.;
    //  if (qOverP<0) charge=-1;
    //  else charge=+1;
    //  abs_trkpt = charge*trkpt;
    //}


    const Trk::Perigee* measPer = (*trksItr)->perigeeParameters();
    const AmgSymMatrix(5)* covariance = measPer ? measPer->covariance() : NULL;

    if (measPer && covariance) {
      AmgVector(5) perigeeParams = measPer->parameters();
      //trkd0    = perigeeParams(Trk::d0);
      //trkz0    = perigeeParams(Trk::z0);
      //trkphi   = perigeeParams(Trk::phi0);
      //trktheta = perigeeParams(Trk::theta);
      //trketa   = measPer->eta();
      qOverP   = perigeeParams(Trk::qOverP)*1000.;
      trkpt    = measPer->pT()/1000.;
      if (qOverP<0) charge=-1;
      else charge=+1;
      abs_trkpt = charge*trkpt;
    }
    else
      {
	if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endmsg;
      }
    //if (trkphi<0) trkphi+=2*m_Pi;

//     //some good quality requirements (will need to refine them later)
//     bool good=(fabs(trketa<2.5)&&fabs(trkz0)<150 && trkpt>1);

//     if (!good) continue;


    // holes + outliers + measurements + etc.
    // these two are identical in the ID:
    ////const Trk::Track* trackWithHoles = m_holeSearchTool->getTrackWithHoles(**trksItr);
    //const Trk::Track* trackWithHoles = m_holeSearchTool->getTrackWithHolesAndOutliers(**trksItr);
    ////if (trackWithHoles!=NULL) TSOS = trackWithHoles->trackStateOnSurfaces();

    // loop over all hits on track
    const DataVector<const Trk::TrackStateOnSurface>* TSOS;
    TSOS = (*trksItr)->trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr  = TSOS->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItrE = TSOS->end();

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"starting to loop over TSOS"<<endmsg;

    for (; TSOSItr != TSOSItrE; ++TSOSItr) {

      //check that we have track parameters defined for the surface (pointer is not null)
      if(!((*TSOSItr)->trackParameters())) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit skipped because no associated track parameters" << endmsg;
	continue;
      }

      Identifier surfaceID;
      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();

      // hits, outliers
      if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement()!=NULL) surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();

      // holes, perigee
      else surfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();

      bool isPseudoMeasurement = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(mesb);
      const Trk::RIO_OnTrack *rio   = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
      if(!rio) isPseudoMeasurement = true;

      if(isPseudoMeasurement){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "found a pseudomeasurement skipping it!" << endmsg;
	continue;
      }

      int detType = 99;
      int barrelEC = 99;
      int layerDisk = 99;
      int sctSide   = 99;
      int modEta = 9999;
      int modPhi = 9999;
      if (m_idHelper->is_trt(surfaceID)) detType = 2;
      else if (m_idHelper->is_sct(surfaceID)) detType = 1;
      else if (m_idHelper->is_pixel(surfaceID)) detType = 0;


      //hit quality cuts for Si hits if tool is configured - default is NO CUTS
      if (detType ==0 ||  detType==1){
	if (detType==0){//pixel

	  barrelEC  = m_pixelID -> barrel_ec(surfaceID);
	  layerDisk = m_pixelID -> layer_disk(surfaceID);
	  modEta    = m_pixelID -> eta_module(surfaceID);
	  modPhi    = m_pixelID -> phi_module(surfaceID);
	}
	else {          //sct. Since detType == 0 or detType == 1 here

	  barrelEC  = m_sctID->barrel_ec(surfaceID);
	  layerDisk = m_sctID->layer_disk(surfaceID);
	  modEta    = m_sctID->eta_module(surfaceID);
	  modPhi    = m_sctID->phi_module(surfaceID);
	  sctSide   = m_sctID->side(surfaceID);
	}


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

      /** Get information for the TRT hits */
      int m_barrel_ec      = 99;
      int m_layer_or_wheel = 99;
      int m_straw_layer    = 99;
      int m_phi_module     = 99;
      bool isTubeHit = false;
      if (detType==2){
	m_barrel_ec      = m_trtID->barrel_ec(surfaceID);
	m_layer_or_wheel = m_trtID->layer_or_wheel(surfaceID);
	m_straw_layer    = m_trtID->straw_layer(surfaceID);
	m_phi_module     = m_trtID->phi_module(surfaceID);
	if((mesb->localCovariance()(Trk::locX,Trk::locX) > 1.0))
	  isTubeHit=true;
      }

      // fill histograms with all posible hit types: measurements, outliers & holes
      if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) || (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) || (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole) ){

	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"have a measurement/outlier/hole"<<endmsg;

	// all possible hits
	// dealing with PIXEL tsos
	if (detType==0){
	  // barrel
	  if (barrelEC==0) {
	    m_hits_vs_layer_barrel -> Fill(layerDisk);
	    m_hits_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta, modPhi);
	    m_hits_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_hits_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_pix_b[layerDisk]  -> Fill(float(LumiBlock));
	    m_hits_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);
	  } //barrel
	  else if (barrelEC==2){
	    m_hits_vs_layer_eca -> Fill(layerDisk);
	    m_hits_vs_Phi_pix_eca[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    m_hits_vs_Eta_Phi_pix_eca -> Fill(layerDisk,modPhi);
	  }
	  else if (barrelEC == -2){
	    m_hits_vs_layer_ecc -> Fill(layerDisk);
	    m_hits_vs_Phi_pix_ecc[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    m_hits_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  }
	} // end of pix
	// Let's treat SCT tsos
	else if (detType==1){
	  if (barrelEC==0) {
	    m_hits_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);
	    m_hits_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	    if (sctSide == 0) m_hits_vs_Eta_Phi_sct_s0_b[layerDisk] -> Fill(modEta,modPhi);
	    if (sctSide == 1) m_hits_vs_Eta_Phi_sct_s1_b[layerDisk] -> Fill(modEta,modPhi);
	    m_hits_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	    m_hits_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_sct_b[layerDisk] -> Fill(float(LumiBlock));

	    m_hits_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);
	  }//barrel
	  else if (barrelEC == 2){
	    m_hits_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_hits_vs_Phi_sct_eca[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_sct_eca -> Fill(float(LumiBlock));
	    m_hits_vs_Eta_Phi_sct_eca -> Fill(layerDisk,modPhi);
	  }
	  else if (barrelEC == -2){
	    m_hits_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_hits_vs_Phi_sct_ecc[layerDisk] -> Fill(modPhi);

	    m_hits_vs_Eta_Phi_sct_ecc -> Fill(layerDisk,modPhi);
	    m_hits_vs_LB_sct_ecc -> Fill(float(LumiBlock));
	  }
	}// end of sct
	else if (detType==2){
	  fillTRTTotalMeasurements(m_barrel_ec,m_layer_or_wheel,m_phi_module,m_straw_layer);
	}
      } // end of all type of hits

      // now, let's fill histograms only for hits on track = measurements
      if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){

	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"starting to find overlaps"<<endmsg;

	bool foundXOverlap = false;
	bool foundYOverlap = false;
	//look for overlap hits if hit is pixel or SCT
	//overlaps are defined such that the overlapping module has to be at a greater radius - avoids double counting
	//for SCT module overlaps there will be two overlaps corresponding to the two different sides
	//this shouldn't effect the overlap efficiency since an SCT module has two hits, one for each side
	if (detType==0 || detType==1){
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit is pixel or SCT - looking for overlaps" << endmsg;
	  //const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(mesb);
	  //std::pair<const Trk::TrackStateOnSurface*,const Trk::TrackStateOnSurface*> overlap = findOverlapHit(*trksItr,hitOnTrack);
	  std::pair<const Trk::TrackStateOnSurface*,const Trk::TrackStateOnSurface*> overlap = findOverlapHit(*trksItr,mesb);
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap search complete" << endmsg;
	  //these will be NULL if no overlap is found
	  if(overlap.first) foundXOverlap = true;
	  if(overlap.second) foundYOverlap = true;
	}


	// measurements per possible hit
	// --- pixel
	if (detType==0){
	  if(barrelEC == 0){
	    m_measurements_vs_layer_barrel -> Fill(layerDisk);
	    m_noholes_vs_layer_barrel -> Fill(layerDisk);




	    //msg(MSG::WARNING) <<"Pix barrel, layer_disk=" << m_pixelID->layer_disk(surfaceID) << ", eta=" << m_pixelID->eta_module(surfaceID) << ", phi=" << m_pixelID->phi_module(surfaceID) <<endmsg;

	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found pixel barrel hit"<<endmsg;
	    m_measurements_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta, modPhi);
	    m_measurements_vs_LB_pix_b[layerDisk]      -> Fill(float(LumiBlock));
	    m_measurements_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_measurements_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);

	    if (m_extendedPlots)
	      {
		if(foundXOverlap){
		  m_overlapX_vs_layer_barrel-> Fill(layerDisk);
		  m_overlapX_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
		  m_overlapX_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
		}
		if(foundYOverlap){
		  m_overlapY_vs_layer_barrel-> Fill(layerDisk);
		  m_overlapY_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
		  m_overlapY_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
		}
	      }
	  } // end of PIX barrel

	  else if (barrelEC == 2){
	    //msg(MSG::WARNING) <<"Pix eca, layer_disk=" << m_pixelID->layer_disk(surfaceID) << ", eta=" << m_pixelID->eta_module(surfaceID) << ", phi=" << m_pixelID->phi_module(surfaceID) <<endmsg;
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found pixel eca hit"<<endmsg;
	    m_measurements_vs_layer_eca -> Fill(layerDisk);
	    m_noholes_vs_layer_eca -> Fill(layerDisk);
	    m_measurements_vs_Phi_pix_eca[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    if (m_extendedPlots) {
	      if(foundXOverlap){
		m_overlapX_vs_layer_eca-> Fill(layerDisk);
		m_overlapX_vs_Phi_pix_eca[layerDisk]-> Fill(modPhi);
	      }
	      if(foundYOverlap){
		m_overlapY_vs_layer_eca-> Fill(layerDisk);
		m_overlapY_vs_Phi_pix_eca[layerDisk] -> Fill(modPhi);
	      }
	    }

	    m_measurements_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	  } // ECA
	  else if (barrelEC == -2){

	    //msg(MSG::WARNING) <<"Pix ecc, layer_disk=" << layerDisk << ", eta=" << m_pixelID->eta_module(surfaceID) << ", phi=" << m_pixelID->phi_module(surfaceID) <<endmsg;
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found pixel ecc hit"<<endmsg;
	    m_measurements_vs_layer_ecc -> Fill(layerDisk);
	    m_noholes_vs_layer_ecc -> Fill(layerDisk);
	    m_measurements_vs_Phi_pix_ecc[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    if (m_extendedPlots){
	      if(foundXOverlap){
		m_overlapX_vs_Phi_pix_ecc[layerDisk] -> Fill(modPhi);
		m_overlapX_vs_layer_ecc-> Fill(layerDisk);
	      }
	      if(foundYOverlap){
		m_overlapY_vs_layer_ecc-> Fill(layerDisk);
		m_overlapY_vs_Phi_pix_ecc[layerDisk] -> Fill(modPhi);
	      }
	    }
	    m_measurements_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  } //ECC
	}// End of PIXELS

	// --- SCT ---- now dealing with SCT
	else if (detType==1){
	  if(barrelEC == 0){

	    //msg(MSG::WARNING) <<"SCT barrel, layer_disk=" << m_sctID->layer_disk(surfaceID) << ", eta=" << m_sctID->eta_module(surfaceID) << ", phi=" << m_sctID->phi_module(surfaceID) << ", side=" << m_sctID->side(surfaceID) <<endmsg;
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found sct barrel hit"<<endmsg;
	    m_measurements_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);
	    m_measurements_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);

	    if (sctSide == 0) m_measurements_vs_Eta_Phi_sct_s0_b[layerDisk] -> Fill(modEta,modPhi);
	    if (sctSide == 1) m_measurements_vs_Eta_Phi_sct_s1_b[layerDisk] -> Fill(modEta,modPhi);
	    m_measurements_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	    m_measurements_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_LB_sct_b[layerDisk] -> Fill(float(LumiBlock));
	    m_measurements_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);
	    if (m_extendedPlots) {
	      if(foundXOverlap){
		m_overlapX_vs_layer_barrel-> Fill(m_NPixLayers + 2*layerDisk + sctSide);
		if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found sct barrel hit 1"<<endmsg;
		m_overlapX_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
		m_overlapX_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      }
	      if(foundYOverlap){
		m_overlapY_vs_layer_barrel-> Fill(m_NPixLayers + 2*layerDisk + sctSide);
		if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found sct barrel hit 2"<<endmsg;
		m_overlapY_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
		m_overlapY_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      }
	    }
	  }//barrel
	  else if (barrelEC == 2){

	    //msg(MSG::WARNING) <<"SCT eca, layer_disk=" << m_sctID->layer_disk(surfaceID) << ", eta=" << m_sctID->eta_module(surfaceID) << ", phi=" << m_sctID->phi_module(surfaceID) << ", side=" << m_sctID->side(surfaceID) <<endmsg;
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found sct eca hit"<<endmsg;
	    m_measurements_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_measurements_vs_Phi_sct_eca[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_LB_sct_eca -> Fill(float(LumiBlock));
	    m_measurements_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	    if (m_extendedPlots)
	      {
		if(foundXOverlap)
		  {
		    m_overlapX_vs_layer_eca-> Fill(3 + 2*layerDisk + sctSide);
		    m_overlapX_vs_Phi_sct_eca[layerDisk] -> Fill(modPhi);
		  }
		if(foundYOverlap)
		  {
		    m_overlapY_vs_layer_eca-> Fill(3 + 2*layerDisk + sctSide);
		    m_overlapY_vs_Phi_sct_eca[layerDisk] -> Fill(modPhi);
		  }
		if (sctSide == 0) m_measurements_vs_Eta_Phi_sct_eca_3d_s0 -> Fill(layerDisk, modEta, modPhi);
		if (sctSide == 1) m_measurements_vs_Eta_Phi_sct_eca_3d_s1 -> Fill(layerDisk, modEta, modPhi);
	      }
	  }//ECA
	  else if (barrelEC == -2){

	    //msg(MSG::WARNING) <<"SCT ecc, layer_disk=" << layerDisk << ", eta=" << m_sctID->eta_module(surfaceID) << ", phi=" << m_sctID->phi_module(surfaceID) << ", side=" << sctSide <<endmsg;
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"found sct ecc hit"<<endmsg;
	    m_measurements_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_measurements_vs_Phi_sct_ecc[layerDisk] -> Fill(modPhi);
	    m_measurements_vs_LB_sct_ecc -> Fill(float(LumiBlock));
	    m_measurements_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	    if (m_extendedPlots)
	      {
		if(foundXOverlap){
		  m_overlapX_vs_layer_ecc-> Fill(3 + 2*layerDisk + sctSide);
		  m_overlapX_vs_Phi_sct_ecc[layerDisk] -> Fill(modPhi);
		}
		if(foundYOverlap){
		  m_overlapY_vs_layer_ecc-> Fill(3 + 2*layerDisk + sctSide);
		  m_overlapY_vs_Phi_sct_ecc[layerDisk] -> Fill(modPhi);
		}
		if (sctSide == 0) m_measurements_vs_Eta_Phi_sct_ecc_3d_s0 -> Fill(layerDisk, modEta, modPhi);
		if (sctSide == 1) m_measurements_vs_Eta_Phi_sct_ecc_3d_s1 -> Fill(layerDisk, modEta, modPhi);
	      }

	  }//ECC
	}//SCT

	if (detType==2){
	  fillTRTHits(m_barrel_ec,m_layer_or_wheel,m_phi_module,m_straw_layer,isTubeHit);
	}

      } // end of measurements (hits on track)

      // Now filling outliers histograms
      if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ){

	// outlier per layer and per hit
	// --- pixel
	if (detType==0){
	  if (barrelEC == 0){
	    // msg(MSG::WARNING) << " ** OULIER FOUND ** Pix barrel layer_disk=" << layerDisk << ", eta=" << modEta << ", phi=" << modPhi <<endmsg;
	    m_outliers_vs_layer_barrel -> Fill(layerDisk);
	    m_noholes_vs_layer_barrel -> Fill(layerDisk);
	    m_outliers_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta, modPhi);
	    m_outliers_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_outliers_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_outliers_vs_LB_pix_b[layerDisk] -> Fill(float(LumiBlock));
	    m_outliers_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);
	  }//barrel

	  else if (barrelEC == 2){
	    m_outliers_vs_layer_eca -> Fill(layerDisk);
	    m_noholes_vs_layer_eca -> Fill(layerDisk);
	    m_outliers_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    m_outliers_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	  }//ECA

	  // ECC
	  else if (barrelEC == -2){
	    m_outliers_vs_layer_ecc -> Fill(layerDisk);
	    m_outliers_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    m_noholes_vs_layer_ecc -> Fill(layerDisk);
	    m_outliers_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  }//ecc
	}//pixels

	// --- sct -- outliers in SCT
	else if (detType==1){
	  if (barrelEC == 0){
	    //msg(MSG::WARNING) <<"SCT barrel outlier, layer_disk=" << layerDisk << ", eta=" << m_sctID->eta_module(surfaceID) << ", phi=" << m_sctID->phi_module(surfaceID) << ", side=" << sctSide <<endmsg;

	    m_outliers_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);
	    m_outliers_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	    m_outliers_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	    m_outliers_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	    m_outliers_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);
	  }//barrel
	  else if (barrelEC == 2){
	    m_outliers_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_outliers_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	  }

	  else if (barrelEC == -2){
	    m_outliers_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_noholes_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_outliers_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	  }
	} // SCT

	// TRT hits
	if (detType==2){
	  fillTRTOutliers(m_barrel_ec,m_layer_or_wheel,m_phi_module,m_straw_layer);
	}

      } // end of outliers
    } // TSOS on track

    const Trk::TrackSummary* summary = m_trackSumTool->createSummary(**trksItr);
    if( !summary->get(Trk::numberOfPixelHits) && !summary->get(Trk::numberOfSCTHits) && (summary->get(Trk::numberOfPixelHoles)==0) && (summary->get(Trk::numberOfSCTHoles)==0) && (m_doHoleSearch)){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Pixel or SCT hits skip hole search" << endmsg;
      continue;
    }
    delete summary;

    const DataVector<const Trk::TrackStateOnSurface>* HTSOS;
    HTSOS = m_holeSearchTool->getHolesOnTrack(**trksItr);
    DataVector<const Trk::TrackStateOnSurface>::const_iterator HTSOSItr  = HTSOS->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator HTSOSItrE = HTSOS->end();

    for (; HTSOSItr != HTSOSItrE; ++HTSOSItr) { // Loop on Holes on Track (HTSOS)
      Identifier surfaceID;
      const Trk::MeasurementBase* mesb=(*HTSOSItr)->measurementOnTrack();
      // hits, outliers
      if (mesb != 0 && mesb->associatedSurface().associatedDetectorElement()!=NULL) surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
      // holes, perigee
      else surfaceID = (*HTSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();

      int detType = 99;
      int barrelEC = 99;
      int layerDisk = 99;
      int sctSide   = 99;
      int modEta = 9999;
      int modPhi = 9999;
      if (m_idHelper->is_trt(surfaceID)) detType = 2;
      else if (m_idHelper->is_sct(surfaceID)) detType = 1;
      else if (m_idHelper->is_pixel(surfaceID)) detType = 0;
      //hit quality cuts for Si hits if tool is configured - default is NO CUTS
      if (detType==0 ||  detType==1){
	if (detType==0){//pixel

	  barrelEC  = m_pixelID -> barrel_ec(surfaceID);
	  layerDisk = m_pixelID -> layer_disk(surfaceID);
	  modEta    = m_pixelID -> eta_module(surfaceID);
	  modPhi    = m_pixelID -> phi_module(surfaceID);
	}
	else {          //sct. Since detType == 0 or detType == 1 here

	  barrelEC  = m_sctID->barrel_ec(surfaceID);
	  layerDisk = m_sctID->layer_disk(surfaceID);
	  modEta    = m_sctID->eta_module(surfaceID);
	  modPhi    = m_sctID->phi_module(surfaceID);
	  sctSide   = m_sctID->side(surfaceID);
	}
	if(m_doHitQuality) {
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "applying hole quality cuts to Silicon hole..." << endmsg;

	  if(!m_hitQualityTool->getGoodHole(*HTSOSItr)) {
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hole failed quality cuts and is rejected." << endmsg;
	    continue;
	  }
	  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hole passed quality cuts" << endmsg;
	}
	else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hole quality cuts NOT APPLIED to Silicon hole." << endmsg;
      }


      if ( (*HTSOSItr)->type(Trk::TrackStateOnSurface::Measurement) || (*HTSOSItr)->type(Trk::TrackStateOnSurface::Outlier) || (*HTSOSItr)->type(Trk::TrackStateOnSurface::Hole) ){

	// all possible type of holes
	// --- pixel
	if (detType==0){
	  if (barrelEC == 0){
	    // msg(MSG::WARNING) << " ** HOLE FOUND ** Pixel barrel ** layer_disk=" << m_pixelID->layer_disk(surfaceID) << ", eta=" << m_pixelID->eta_module(surfaceID) << ", phi=" << m_pixelID->phi_module(surfaceID) << endmsg;


	    bool knownType = false;
	    if ((*HTSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
	      knownType = true;
	      //std::cout << " Type: Measurement ";
	    }
	    if ((*HTSOSItr)->type(Trk::TrackStateOnSurface::Outlier)) {
	      knownType = true;
	      //std::cout << " Type: Outlier ";
	    }
	    if ((*HTSOSItr)->type(Trk::TrackStateOnSurface::Hole)) {
	      knownType = true;
	      //std::cout << " Type: Hole ";
	    }
	    if (!knownType)  std::cout << "IDAlignment Monitoring HitEfficiencies Type: -- UNKNOWN -- "<<std::endl;


	     m_hits_vs_layer_barrel -> Fill(layerDisk);
	    m_hits_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta,modPhi);
	    m_hits_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_hits_vs_LB_pix_b[layerDisk]  -> Fill(float(LumiBlock));
	    m_hits_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_hits_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);

	  }//barrel
	  // ----------- eca
	  else if (barrelEC == 2){
	    m_hits_vs_layer_eca -> Fill(layerDisk);
	    m_hits_vs_Phi_pix_eca[layerDisk] -> Fill(modPhi);
	    m_hits_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	    m_hits_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	  } // eca
	  // ----------- ecc
	  else if (barrelEC == -2){
	    m_hits_vs_layer_ecc -> Fill(layerDisk);
	    m_hits_vs_Phi_pix_ecc[layerDisk] -> Fill(modPhi);
	    m_hits_vs_LB_pix_ecc[layerDisk]  -> Fill(float(LumiBlock));
	    m_hits_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  } // ecc
	} // pixels

	// --- sct
	else if (detType==1){
	  if (barrelEC == 0)
	    {
	      //msg(MSG::WARNING) <<"SCT barrel hole, layer_disk=" << m_sctID->layer_disk(surfaceID) << ", eta=" << m_sctID->eta_module(surfaceID) << ", phi=" << modPhi << ", side=" << m_sctID->side(surfaceID) << endmsg;

	      m_hits_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk + sctSide);

	      m_hits_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	      if (sctSide == 0) m_hits_vs_Eta_Phi_sct_s0_b[layerDisk] -> Fill(modEta,modPhi);
	      if (sctSide == 1) m_hits_vs_Eta_Phi_sct_s1_b[layerDisk] -> Fill(modEta,modPhi);
	      m_hits_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      m_hits_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	      m_hits_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);
	      m_hits_vs_LB_sct_b[layerDisk] -> Fill(float(LumiBlock));
	    }
	  else if (barrelEC == 2){
	    m_hits_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_hits_vs_Phi_sct_eca[layerDisk] -> Fill(modPhi);

	    m_hits_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	  }
	  else if (barrelEC == -2){
	    m_hits_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    //if(layerDisk == 0)   ???? (PF: why this is here?)
	    m_hits_vs_Phi_sct_ecc[layerDisk] -> Fill(modPhi);
	    m_hits_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	  }
	}//sct
      } // all possible holes

      // filling histograms for holes being holes
      if ( (*HTSOSItr)->type(Trk::TrackStateOnSurface::Hole) ){

	// holes per layer and per hit
	// --- pixel
	if (detType==0){
	  if (barrelEC == 0){
	    m_holes_vs_layer_barrel -> Fill(layerDisk);
	    m_holes_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta,modPhi);
	    m_holes_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_holes_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_holes_vs_LB_pix_b[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);
	  }
	  // ----------- eca
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(layerDisk);
	    m_holes_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	  }
	  // ----------- ecc
	  else if (barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(layerDisk);
	    m_holes_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  }
	} // pix
	// --- sct holes
	else if (detType==1){
	  if (barrelEC==0) {
	    m_holes_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk +sctSide);

	      m_holes_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	      m_holes_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      m_holes_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	      m_holes_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);

	  }
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	  }

	  else if (detType==1 && barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	  }
	} // sct
      } // holes as holes

      // filling histograms for holes being outliers
      if ( (*HTSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ){

	//This section has to be removed. There is no hole flagged as outlier. I've checked it (PF)

	// holes per layer and per hit
	// --- pixel
	if (detType==0){
	  if (barrelEC == 0){
	    m_holes_vs_layer_barrel -> Fill(layerDisk);
	    m_holes_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta,modPhi);
	    m_holes_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_holes_vs_LB_pix_b[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_holes_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);
	  }
	  // ----------- eca
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(layerDisk);
	    m_holes_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	  }
	  // ----------- ecc
	  else if (barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(layerDisk);
	    m_holes_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  }
	} // pix

	// --- sct holes
	else if (detType==1){
	  if (barrelEC==0) {
	    m_holes_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk +sctSide);

	      m_holes_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	      m_holes_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      m_holes_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	      m_holes_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);

	  }
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	  }

	  else if (detType==1 && barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	  }
	} // sct
      } // holes as outliers

      // filling histograms for holes being measurements
      if ( (*HTSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){

	//This has to be removed. There are no holes flagged as measurements. Checked it. PF

	// holes per layer and per hit
	// --- pixel
	if (detType==0){
	  if (barrelEC == 0){
	    m_holes_vs_layer_barrel -> Fill(layerDisk);
	    m_holes_vs_Eta_Phi_pix_b[layerDisk] -> Fill(modEta,modPhi);
	    m_holes_vs_Eta_pix_b[layerDisk] -> Fill(modEta);
	    m_holes_vs_Phi_pix_b[layerDisk] -> Fill(modPhi);
	    m_holes_vs_LB_pix_b[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_pT_pix_b[layerDisk] -> Fill(abs_trkpt);
	  }
	  // ----------- eca
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(layerDisk);
	    m_holes_vs_LB_pix_eca[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_eca -> Fill(layerDisk, modPhi);
	  }
	  // ----------- ecc
	  else if (barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(layerDisk);
	    m_holes_vs_LB_pix_ecc[layerDisk] -> Fill(float(LumiBlock));
	    m_holes_vs_Eta_Phi_pix_ecc -> Fill(layerDisk, modPhi);
	  }
	} // pix

	// --- sct holes
	else if (detType==1){
	  if (barrelEC==0) {
	    m_holes_vs_layer_barrel -> Fill(m_NPixLayers + 2*layerDisk +sctSide);

	      m_holes_vs_Eta_Phi_sct_b[layerDisk] -> Fill(modEta,modPhi);
	      m_holes_vs_Eta_sct_b[layerDisk] -> Fill(modEta);
	      m_holes_vs_Phi_sct_b[layerDisk] -> Fill(modPhi);
	      m_holes_vs_pT_sct_b[layerDisk] -> Fill(abs_trkpt);

	  }
	  else if (barrelEC == 2){
	    m_holes_vs_layer_eca -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_eca -> Fill(layerDisk, modPhi);
	  }

	  else if (detType==1 && barrelEC == -2){
	    m_holes_vs_layer_ecc -> Fill(3 + 2*layerDisk + sctSide);
	    m_holes_vs_Eta_Phi_sct_ecc -> Fill(layerDisk, modPhi);
	  }
	} // sct
      } // holes as measurements (?)
    }
    delete HTSOS;
  }
  delete trks;
  return StatusCode::SUCCESS;
}

void IDAlignMonEfficiencies::makeEffHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  int Nbins;
  Nbins = h_num->GetNbinsX();

  for (int bin=0; bin!=Nbins; ++bin) {
    int Npass = int(h_num->GetBinContent(bin+1));
    int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;
    double x = h_denom->GetBinCenter(bin+1);
    for (int pass=0; pass<Npass; ++pass) {
      h_eff->Fill(x,1.);
    }
    for (int fail=0; fail<Nfail; ++fail) {
      h_eff->Fill(x,0.);
    }
  }
  SetMinWindow(h_eff, m_minSiliconEffWindow, m_maxSiliconEffWindow);
}

void IDAlignMonEfficiencies::makeEffHisto(TH1F_LW* h_num, TH1F_LW* h_denom, TProfile* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in makeEffHisto  " << endmsg;
  int Nbins;
  Nbins = h_num->GetNbinsX();
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Nbins is  " << Nbins << endmsg;
  for (int bin=0; bin!=Nbins; ++bin) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "bin is  " << bin << endmsg;
    int Npass = int(h_num->GetBinContent(bin+1));
    int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;

    float binSize = (h_denom->getXMax() - h_denom->getXMin())/h_denom->GetNbinsX();
    //double x = h_denom->GetBinCenter(bin+1);
    double x = h_denom->getXMin() + binSize * bin + binSize/2;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Npass is  " << Npass << endmsg;
    for (int pass=0; pass<Npass; ++pass) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "pass is  " << pass << endmsg;
      h_eff->Fill(x,1.);
    }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Nfail is  " << Nfail << endmsg;
    for (int fail=0; fail<Nfail; ++fail) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "fail is  " << fail << endmsg;
      h_eff->Fill(x,0.);
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Out of loop  " << endmsg;
  SetMinWindow(h_eff, m_minSiliconEffWindow, m_maxSiliconEffWindow);

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "leaving makeEffHisto  " << Nbins << endmsg;
  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonEfficiencies::makeOverlapFracHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  int Nbins;
  Nbins = h_num->GetNbinsX();
  for (int bin=0; bin!=Nbins; ++bin) {
    int Npass = int(h_num->GetBinContent(bin+1));
    int Nfail = int(h_denom->GetBinContent(bin+1)) - Npass;
    float x = h_denom->GetBinCenter(bin+1);
    for (int pass=0; pass<Npass; ++pass) {
      h_eff->Fill(x,1.);
    }
    for (int fail=0; fail<Nfail; ++fail) {
      h_eff->Fill(x,0.);
    }
  }
  SetMinWindow(h_eff, 0., .3);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonEfficiencies::makeEffHisto(TH2F* h_num, TH2F* h_denom, TProfile2D* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  int Nbins;
  int NbinsY;
  Nbins = h_num->GetNbinsX();
  NbinsY = h_num->GetNbinsY();
  for (int bin=0; bin!=Nbins; ++bin) {
    for (int binY=0; binY!=NbinsY; ++binY) {
      int Npass = int(h_num->GetBinContent(bin+1,binY+1));
      int Nfail = int(h_denom->GetBinContent(bin+1,binY+1)) - Npass;
      float x = h_denom->GetXaxis()->GetBinCenter(bin+1);
      float y = h_denom->GetYaxis()->GetBinCenter(binY+1);
      for (int pass=0; pass<Npass; ++pass) {
	h_eff->Fill(x,y,1.);
      }
      for (int fail=0; fail<Nfail; ++fail) {
	h_eff->Fill(x,y,0.);
      }
    }
  }
  SetMinWindow(h_eff, m_minSiliconEffWindow, m_maxSiliconEffWindow);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonEfficiencies::makeEffHisto(TH2F* h_num, TH2F* h_denom, TH2F* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  int Nbins = h_num->GetNbinsX();
  int NbinsY = h_num->GetNbinsY();
  float NSeenHits, NExpected;
  float myEff = 0.; // default is 0%
  bool doComputeEff = false;
  for (int bin=0; bin!=Nbins; ++bin) {
    for (int binY=0; binY!=NbinsY; ++binY) {
      NSeenHits = h_num->GetBinContent(bin+1,binY+1);
      NExpected = h_denom->GetBinContent(bin+1,binY+1);
      doComputeEff = false; // check that there is a mimimum of entries
      if (NExpected >  10) doComputeEff = true;
      if (NSeenHits >  5) doComputeEff = true;
      if (doComputeEff) {
	myEff = NSeenHits / NExpected;
	if (myEff < 0.01) myEff = 0.01; //trick to distinguish between few entries and not working modules or too low statistics (doComputeEff = false --> Eff = 0 % )
	h_eff->SetBinContent(bin+1,binY+1, myEff);
      }
      // std::cout << "  ComputeEff= " << doComputeEff;
      if (doComputeEff) std::cout << "  Eff= " << myEff;
      std::cout << std::endl;
    }
  }
  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void IDAlignMonEfficiencies::makeEffHistoWithCut(TH2F* h_num, TH2F* h_denom, TProfile2D* h_eff) {
  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      h_eff->Reset("ICE");
  }

  int Nbins = h_num->GetNbinsX();
  int NbinsY = h_num->GetNbinsY();
  float NSeenHits, NExpected;
  float myEff = 0.; // default is 0%
  bool doComputeEff = false;

  for (int bin=0; bin!=Nbins; ++bin) {
    for (int binY=0; binY!=NbinsY; ++binY) {
      NSeenHits = h_num->GetBinContent(bin+1,binY+1);
      NExpected = h_denom->GetBinContent(bin+1,binY+1);
      doComputeEff = false; // check that there is a mimimum of entries
      if (NExpected > 10) doComputeEff = true;
      if (NSeenHits >  3) doComputeEff = true;
      if (doComputeEff) {
	myEff = NSeenHits / NExpected;
	if (myEff < 0.01) myEff = 0.01; // trick to distinguish between few entries and not working modules (dead modules).
	// h_eff->SetBinContent(bin+1,binY+1, myEff);
	float this_x = h_denom->GetXaxis()->GetBinCenter(bin+1);
	float this_y = h_denom->GetYaxis()->GetBinCenter(binY+1);

	for (int i=0; i<NExpected; i++) {
	  if (i < NSeenHits) {
	    h_eff->Fill(this_x, this_y, 1.);
	  }
	  else {
	    h_eff->Fill(this_x, this_y, 0.);
	  }
	}
      }
      // if (doComputeEff) std::cout << "  Eff= " << myEff;
      if (doComputeEff) std::cout << "  Eff= " << h_eff->GetBinContent(bin, binY);
      std::cout << std::endl;
    }
  }
  return;
}


StatusCode IDAlignMonEfficiencies::procHistograms()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In procHistograms" << endmsg;
  if( endOfLowStatFlag() ) {
  }
  if( endOfLumiBlockFlag() ) {
  }
  if( endOfRunFlag() || ( ( AthenaMonManager::environment() == AthenaMonManager::online ) && endOfLumiBlockFlag() ) ) {
    // -----------------------------------------------------------------------
    //
    // normalize: divide measurement / outliers / holes by number of possible hits
    //

    for (int iLayer=0; iLayer < m_PIX_Mgr->numerology().numLayers();++iLayer) {
      if (!m_PIX_Mgr->numerology().useLayer(iLayer)){
	msg(MSG::WARNING) << "Layer "<<iLayer<<" Not in Use. No efficiencies plots"<<endmsg;
	continue;}
      makeEffHisto(m_measurements_vs_Eta_Phi_pix_b[iLayer], m_hits_vs_Eta_Phi_pix_b[iLayer], m_measurements_eff_vs_Eta_Phi_pix_b[iLayer]);
      makeEffHisto(m_measurements_vs_Eta_pix_b[iLayer],m_hits_vs_Eta_pix_b[iLayer],m_measurements_eff_vs_Eta_pix_b[iLayer]);
      makeEffHisto(m_measurements_vs_Phi_pix_b[iLayer],m_hits_vs_Phi_pix_b[iLayer],m_measurements_eff_vs_Phi_pix_b[iLayer]);
      makeEffHisto(m_measurements_vs_pT_pix_b[iLayer],m_hits_vs_pT_pix_b[iLayer],m_measurements_eff_vs_pT_pix_b[iLayer]);
      makeEffHisto(m_outliers_vs_Eta_pix_b[iLayer],m_hits_vs_Eta_pix_b[iLayer],m_outliers_eff_vs_Eta_pix_b[iLayer]);
      makeEffHisto(m_holes_vs_Eta_pix_b[iLayer],m_hits_vs_Eta_pix_b[iLayer],m_holes_eff_vs_Eta_pix_b[iLayer]);
      makeEffHisto(m_outliers_vs_Phi_pix_b[iLayer],m_hits_vs_Phi_pix_b[iLayer],m_outliers_eff_vs_Phi_pix_b[iLayer]);
      makeEffHisto(m_holes_vs_Phi_pix_b[iLayer],m_hits_vs_Phi_pix_b[iLayer],m_holes_eff_vs_Phi_pix_b[iLayer]);
      makeEffHisto(m_outliers_vs_pT_pix_b[iLayer],m_hits_vs_pT_pix_b[iLayer],m_outliers_eff_vs_pT_pix_b[iLayer]);
      makeEffHisto(m_holes_vs_pT_pix_b[iLayer],m_hits_vs_pT_pix_b[iLayer],m_holes_eff_vs_pT_pix_b[iLayer]);

      if (m_extendedPlots)
	{
	  makeOverlapFracHisto(m_overlapX_vs_Phi_pix_b[iLayer],m_measurements_vs_Phi_pix_b[iLayer],m_overlapX_eff_vs_Phi_pix_b[iLayer]);
	  makeOverlapFracHisto(m_overlapY_vs_Phi_pix_b[iLayer],m_measurements_vs_Phi_pix_b[iLayer],m_overlapY_eff_vs_Phi_pix_b[iLayer]);
	  makeOverlapFracHisto(m_overlapX_vs_Eta_pix_b[iLayer],m_measurements_vs_Eta_pix_b[iLayer],m_overlapX_eff_vs_Eta_pix_b[iLayer]);
	  makeOverlapFracHisto(m_overlapY_vs_Eta_pix_b[iLayer],m_measurements_vs_Eta_pix_b[iLayer],m_overlapY_eff_vs_Eta_pix_b[iLayer]);
	}



      //LB plots - Barrel
      makeEffHisto(m_measurements_vs_LB_pix_b[iLayer],m_hits_vs_LB_pix_b[iLayer],m_measurements_eff_vs_LB_pix_b[iLayer]);
      makeEffHisto(m_outliers_vs_LB_pix_b[iLayer],m_hits_vs_LB_pix_b[iLayer],m_outliers_eff_vs_LB_pix_b[iLayer]);
      makeEffHisto(m_holes_vs_LB_pix_b[iLayer],m_hits_vs_LB_pix_b[iLayer],m_holes_eff_vs_LB_pix_b[iLayer]);




    }// Loop on barrel layers

    for (int iECIndex = 0; iECIndex < m_PIX_Mgr->numerology().numEndcaps(); ++iECIndex) {
      int iSide = m_PIX_Mgr->numerology().endcapId(iECIndex);
      for (int iDisk=0; iDisk < m_PIX_Mgr->numerology().numDisks();++iDisk)
	{
	  if (!m_PIX_Mgr->numerology().useDisk(iDisk)){//To check if the Disk is in use.
	    msg(MSG::WARNING) << "Disk "<<iDisk<<" Not in Use"<<endmsg;
	    continue;}

	  if (iSide>0)
	    {
	      makeEffHisto(m_measurements_vs_Phi_pix_eca[iDisk],m_hits_vs_Phi_pix_eca[iDisk],m_measurements_eff_vs_Phi_pix_eca[iDisk]);
	      makeEffHisto(m_measurements_vs_LB_pix_eca[iDisk],m_hits_vs_LB_pix_eca[iDisk],m_measurements_eff_vs_LB_pix_eca[iDisk]);
	      makeEffHisto(m_holes_vs_LB_pix_eca[iDisk],m_hits_vs_LB_pix_eca[iDisk],m_holes_eff_vs_LB_pix_eca[iDisk]);
	      makeEffHisto(m_outliers_vs_LB_pix_eca[iDisk],m_hits_vs_LB_pix_eca[iDisk],m_outliers_eff_vs_LB_pix_eca[iDisk]);

	    }
	  if (iSide<0)
	    {
	      makeEffHisto(m_measurements_vs_Phi_pix_ecc[iDisk],m_hits_vs_Phi_pix_ecc[iDisk],m_measurements_eff_vs_Phi_pix_ecc[iDisk]);
	      makeEffHisto(m_measurements_vs_LB_pix_ecc[iDisk],m_hits_vs_LB_pix_ecc[iDisk],m_measurements_eff_vs_LB_pix_ecc[iDisk]);
	      makeEffHisto(m_holes_vs_LB_pix_ecc[iDisk],m_hits_vs_LB_pix_ecc[iDisk],m_holes_eff_vs_LB_pix_ecc[iDisk]);
	      makeEffHisto(m_outliers_vs_LB_pix_ecc[iDisk],m_hits_vs_LB_pix_ecc[iDisk],m_outliers_eff_vs_LB_pix_ecc[iDisk]);

	    }







	}//Disk loop

    }//Endcap loop


    for (int iLayer=0; iLayer < m_SCT_Mgr->numerology().numLayers();++iLayer)
      {
	if (!m_SCT_Mgr->numerology().useLayer(iLayer)){
	  msg(MSG::WARNING) << "Layer "<<iLayer<<" Not in Use. No efficiencies plots"<<endmsg;
	  continue;}
	makeEffHisto(m_measurements_vs_Eta_Phi_sct_b[iLayer], m_hits_vs_Eta_Phi_sct_b[iLayer], m_measurements_eff_vs_Eta_Phi_sct_b[iLayer]);
	makeEffHisto(m_measurements_vs_Eta_sct_b[iLayer],m_hits_vs_Eta_sct_b[iLayer],m_measurements_eff_vs_Eta_sct_b[iLayer]);
	makeEffHisto(m_outliers_vs_Eta_sct_b[iLayer],m_hits_vs_Eta_sct_b[iLayer],m_outliers_eff_vs_Eta_sct_b[iLayer]);
	makeEffHisto(m_holes_vs_Eta_sct_b[iLayer],m_hits_vs_Eta_sct_b[iLayer],m_holes_eff_vs_Eta_sct_b[iLayer]);
	makeEffHisto(m_measurements_vs_Phi_sct_b[iLayer],m_hits_vs_Phi_sct_b[iLayer],m_measurements_eff_vs_Phi_sct_b[iLayer]);
	makeEffHisto(m_outliers_vs_Phi_sct_b[iLayer],m_hits_vs_Phi_sct_b[iLayer],m_outliers_eff_vs_Phi_sct_b[iLayer]);
	makeEffHisto(m_holes_vs_Phi_sct_b[iLayer],m_hits_vs_Phi_sct_b[iLayer],m_holes_eff_vs_Phi_sct_b[iLayer]);
	makeEffHisto(m_measurements_vs_pT_sct_b[iLayer],m_hits_vs_pT_sct_b[iLayer],m_measurements_eff_vs_pT_sct_b[iLayer]);
	makeEffHisto(m_outliers_vs_pT_sct_b[iLayer],m_hits_vs_pT_sct_b[iLayer],m_outliers_eff_vs_pT_sct_b[iLayer]);
	makeEffHisto(m_holes_vs_pT_sct_b[iLayer],m_hits_vs_pT_sct_b[iLayer],m_holes_eff_vs_pT_sct_b[iLayer]);

	// SCT barrel: module by module & plane by plane
	makeEffHisto (m_measurements_vs_Eta_Phi_sct_s0_b[iLayer], m_hits_vs_Eta_Phi_sct_s0_b[iLayer], m_measurements_eff_vs_Eta_Phi_sct_s0_b[iLayer]);
	makeEffHisto (m_measurements_vs_Eta_Phi_sct_s1_b[iLayer], m_hits_vs_Eta_Phi_sct_s1_b[iLayer], m_measurements_eff_vs_Eta_Phi_sct_s1_b[iLayer]);
	makeEffHisto (m_measurements_vs_LB_sct_b[iLayer], m_hits_vs_LB_sct_b[iLayer], m_measurements_eff_vs_LB_sct_b[iLayer]);

	if (m_extendedPlots)
	  {
	    makeOverlapFracHisto(m_overlapX_vs_Phi_sct_b[iLayer],m_measurements_vs_Phi_sct_b[iLayer],m_overlapX_eff_vs_Phi_sct_b[iLayer]);
	    makeOverlapFracHisto(m_overlapY_vs_Phi_sct_b[iLayer],m_measurements_vs_Phi_sct_b[iLayer],m_overlapY_eff_vs_Phi_sct_b[iLayer]);
	    makeOverlapFracHisto(m_overlapX_vs_Eta_sct_b[iLayer],m_measurements_vs_Eta_sct_b[iLayer],m_overlapX_eff_vs_Eta_sct_b[iLayer]);
	    makeOverlapFracHisto(m_overlapY_vs_Eta_sct_b[iLayer],m_measurements_vs_Eta_sct_b[iLayer],m_overlapY_eff_vs_Eta_sct_b[iLayer]);
	  }


      }

    for (int iECIndex = 0; iECIndex < m_SCT_Mgr->numerology().numEndcaps(); ++iECIndex)
      {
	int iSide = m_SCT_Mgr->numerology().endcapId(iECIndex);
	for (int iWheel=0; iWheel < m_SCT_Mgr->numerology().numDisks();++iWheel)
	  {
	    if (!m_SCT_Mgr->numerology().useDisk(iWheel)){//To check if the Wheel is in use.
	      msg(MSG::WARNING) << "Wheel "<<iWheel<<" Not in Use"<<endmsg;
	      continue;}

	    if (iSide>0)
	      {
		makeEffHisto(m_measurements_vs_Phi_sct_eca[iWheel],m_hits_vs_Phi_sct_eca[iWheel],m_measurements_eff_vs_Phi_sct_eca[iWheel]);

	      }
	    if (iSide<0)
	      {
		makeEffHisto(m_measurements_vs_Phi_sct_ecc[iWheel],m_hits_vs_Phi_sct_ecc[iWheel],m_measurements_eff_vs_Phi_sct_ecc[iWheel]);

	      }

	  }//Wheel loop
      }//Endcap loop

    makeEffHisto(m_measurements_vs_Eta_Phi_pix_eca, m_hits_vs_Eta_Phi_pix_eca, m_measurements_eff_vs_Eta_Phi_pix_eca);
    makeEffHisto(m_measurements_vs_Eta_Phi_pix_ecc, m_hits_vs_Eta_Phi_pix_ecc, m_measurements_eff_vs_Eta_Phi_pix_ecc);
    makeEffHisto(m_measurements_vs_Eta_Phi_sct_eca, m_hits_vs_Eta_Phi_sct_eca, m_measurements_eff_vs_Eta_Phi_sct_eca);
    makeEffHisto(m_measurements_vs_Eta_Phi_sct_ecc, m_hits_vs_Eta_Phi_sct_ecc, m_measurements_eff_vs_Eta_Phi_sct_ecc);

    makeEffHisto(m_measurements_vs_LB_sct_eca, m_hits_vs_LB_sct_eca, m_measurements_eff_vs_LB_sct_eca);
    makeEffHisto(m_measurements_vs_LB_sct_ecc, m_hits_vs_LB_sct_ecc, m_measurements_eff_vs_LB_sct_ecc);

    makeEffHisto(m_measurements_vs_layer_barrel,m_hits_vs_layer_barrel,m_measurements_eff_vs_layer_barrel);
    makeEffHisto(m_measurements_vs_layer_eca,m_hits_vs_layer_eca,m_measurements_eff_vs_layer_eca);
    makeEffHisto(m_measurements_vs_layer_ecc,m_hits_vs_layer_ecc,m_measurements_eff_vs_layer_ecc);
    makeEffHisto(m_outliers_vs_layer_barrel,m_hits_vs_layer_barrel,m_outliers_eff_vs_layer_barrel);
    makeEffHisto(m_outliers_vs_layer_eca,m_hits_vs_layer_eca,m_outliers_eff_vs_layer_eca);
    makeEffHisto(m_outliers_vs_layer_ecc,m_hits_vs_layer_ecc,m_outliers_eff_vs_layer_ecc);
    makeEffHisto(m_holes_vs_layer_barrel,m_hits_vs_layer_barrel,m_holes_eff_vs_layer_barrel);
    makeEffHisto(m_holes_vs_layer_eca,m_hits_vs_layer_eca,m_holes_eff_vs_layer_eca);
    makeEffHisto(m_holes_vs_layer_ecc,m_hits_vs_layer_ecc,m_holes_eff_vs_layer_ecc);
    makeEffHisto(m_noholes_vs_layer_barrel,m_hits_vs_layer_barrel,m_noholes_eff_vs_layer_barrel);
    makeEffHisto(m_noholes_vs_layer_eca,m_hits_vs_layer_eca,m_noholes_eff_vs_layer_eca);
    makeEffHisto(m_noholes_vs_layer_ecc,m_hits_vs_layer_ecc,m_noholes_eff_vs_layer_ecc);

    if (m_extendedPlots)
      {
	makeEffHisto(m_overlapX_vs_layer_barrel,m_measurements_vs_layer_barrel,m_overlapX_eff_vs_layer_barrel);
	makeEffHisto(m_overlapX_vs_layer_eca,m_measurements_vs_layer_eca,m_overlapX_eff_vs_layer_eca);
	makeEffHisto(m_overlapX_vs_layer_ecc,m_measurements_vs_layer_ecc,m_overlapX_eff_vs_layer_ecc);
	makeEffHisto(m_overlapY_vs_layer_barrel,m_measurements_vs_layer_barrel,m_overlapY_eff_vs_layer_barrel);
	makeEffHisto(m_overlapY_vs_layer_eca,m_measurements_vs_layer_eca,m_overlapY_eff_vs_layer_eca);
	makeEffHisto(m_overlapY_vs_layer_ecc,m_measurements_vs_layer_ecc,m_overlapY_eff_vs_layer_ecc);
      }

    /**  TRT Processing */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Doint the TRT pocessing" << endmsg;
    /** TRT Barrel */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Barrel" << endmsg;
    for(int i=0; i<3;i++) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "layer is " << i << endmsg;
      makeEffHisto(m_trt_b_hist->outliers_vs_phiSector[i], m_trt_b_hist->totHits_vs_phiSector[i], m_trt_b_hist->outliers_eff_vs_phiSector[i]);
      makeEffHisto(m_trt_b_hist->hits_vs_phiSector[i], m_trt_b_hist->totHits_vs_phiSector[i], m_trt_b_hist->hits_eff_vs_phiSector[i]);
      makeEffHisto(m_trt_b_hist->tubeHits_vs_phiSector[i], m_trt_b_hist->totHits_vs_phiSector[i], m_trt_b_hist->tubeHits_eff_vs_phiSector[i]);
    }
    makeEffHisto(m_trt_b_hist->outliers_vs_StrawLay, m_trt_b_hist->totHits_vs_StrawLay, m_trt_b_hist->outliers_eff_vs_StrawLay);
    makeEffHisto(m_trt_b_hist->hits_vs_StrawLay, m_trt_b_hist->totHits_vs_StrawLay, m_trt_b_hist->hits_eff_vs_StrawLay);
    makeEffHisto(m_trt_b_hist->tubeHits_vs_StrawLay, m_trt_b_hist->totHits_vs_StrawLay, m_trt_b_hist->tubeHits_eff_vs_StrawLay);

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Endcap" << endmsg;
    /**  TRT Endcap */
    for(unsigned int endcap = 0; endcap<2; ++endcap){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "endcap is " << endcap << endmsg;
      makeEffHisto(m_trt_ec_hist->outliers_vs_ring[endcap],
		   m_trt_ec_hist->totHits_vs_ring[endcap],
		   m_trt_ec_hist->outliers_eff_vs_ring[endcap]);

      makeEffHisto(m_trt_ec_hist->hits_vs_ring[endcap],
		   m_trt_ec_hist->totHits_vs_ring[endcap],
		   m_trt_ec_hist->hits_eff_vs_ring[endcap]);

      makeEffHisto(m_trt_ec_hist->tubeHits_vs_ring[endcap],
		   m_trt_ec_hist->totHits_vs_ring[endcap],
		   m_trt_ec_hist->tubeHits_eff_vs_ring[endcap]);

      makeEffHisto(m_trt_ec_hist->outliers_vs_phiSector[endcap],
		   m_trt_ec_hist->totHits_vs_phiSector[endcap],
		   m_trt_ec_hist->outliers_eff_vs_phiSector[endcap]);

      makeEffHisto(m_trt_ec_hist->hits_vs_phiSector[endcap],
		   m_trt_ec_hist->totHits_vs_phiSector[endcap],
		   m_trt_ec_hist->hits_eff_vs_phiSector[endcap]);

      makeEffHisto(m_trt_ec_hist->tubeHits_vs_phiSector[endcap],
		   m_trt_ec_hist->totHits_vs_phiSector[endcap],
		   m_trt_ec_hist->tubeHits_eff_vs_phiSector[endcap]);
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Done TRT Processing" << endmsg;
    //========================



//     // normalize: divide outliers by number of hits
//     for (int i=1;i<=m_hits_vs_Eta_Phi_pix_b0->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_pix_b0->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_pix_b0->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_pix_b0->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_pix_b0->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_pix_b0->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_pix_b0->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_pix_b0->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_pix_b0->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_pix_b0->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_pix_b0->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_pix_b0->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_pix_b1->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_pix_b1->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_pix_b1->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_pix_b1->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_pix_b1->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_pix_b1->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_pix_b1->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_pix_b1->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_pix_b1->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_pix_b1->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_pix_b1->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_pix_b1->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_pix_b2->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_pix_b2->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_pix_b2->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_pix_b2->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_pix_b2->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_pix_b2->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_pix_b2->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_pix_b2->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_pix_b2->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_pix_b2->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_pix_b2->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_pix_b2->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_sct_b0->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_sct_b0->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_sct_b0->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_sct_b0->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_sct_b0->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_sct_b0->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_sct_b0->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_sct_b0->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_sct_b0->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_sct_b0->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_sct_b0->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_sct_b0->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_sct_b1->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_sct_b1->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_sct_b1->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_sct_b1->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_sct_b1->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_sct_b1->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_sct_b1->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_sct_b1->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_sct_b1->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_sct_b1->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_sct_b1->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_sct_b1->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_sct_b2->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_sct_b2->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_sct_b2->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_sct_b2->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_sct_b2->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_sct_b2->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_sct_b2->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_sct_b2->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_sct_b2->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_sct_b2->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_sct_b2->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_sct_b2->SetBinContent(i,j,0.);
// 	}
//       }
//     }

//     for (int i=1;i<=m_hits_vs_Eta_Phi_sct_b3->GetNbinsX();i++){
//       for (int j=1;j<=m_hits_vs_Eta_Phi_sct_b3->GetNbinsY();j++){
// 	float measurements=m_measurements_vs_Eta_Phi_sct_b3->GetBinContent(i,j);
// 	float outliers=m_outliers_vs_Eta_Phi_sct_b3->GetBinContent(i,j);
// 	float holes=m_holes_vs_Eta_Phi_sct_b3->GetBinContent(i,j);
// 	float hits=m_hits_vs_Eta_Phi_sct_b3->GetBinContent(i,j);
// 	if(hits > 0){
// 	  m_measurements_vs_Eta_Phi_sct_b3->SetBinContent(i,j,measurements / hits);
// 	  m_outliers_vs_Eta_Phi_sct_b3->SetBinContent(i,j,outliers / hits);
// 	  m_holes_vs_Eta_Phi_sct_b3->SetBinContent(i,j,holes / hits);
// 	} else {
// 	  m_measurements_vs_Eta_Phi_sct_b3->SetBinContent(i,j,0.);
// 	  m_outliers_vs_Eta_Phi_sct_b3->SetBinContent(i,j,0.);
// 	  m_holes_vs_Eta_Phi_sct_b3->SetBinContent(i,j,0.);
// 	}
//       }
//     }

  }

  return StatusCode::SUCCESS;
}

//================================================================
// Establishes a minimim window for the TProfile
//================================================================
void IDAlignMonEfficiencies::SetMinWindow(TProfile* hProf, float windowMin, float windowMax){

  float min=hProf->GetMinimum();
  float max=hProf->GetMaximum();
  float margin=0.05*(max-min);
  if(min > windowMin) min=windowMin-margin;
  if(max < windowMax) max=windowMax+margin;
  hProf->SetMinimum(min);
  hProf->SetMaximum(max);
}

void IDAlignMonEfficiencies::SetMinWindow(TProfile2D* hProf, float windowMin, float windowMax){

  float min=hProf->GetMinimum();
  float max=hProf->GetMaximum();
  float margin=0.05*(max-min);
  if(min > windowMin) min=windowMin-margin;
  if(max < windowMax) max=windowMax+margin;
  hProf->SetMinimum(min);
  hProf->SetMaximum(max);

}

//---------------------------------------------------------------------------------------------------------------

std::pair<const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> IDAlignMonEfficiencies::findOverlapHit(const Trk::Track* trk, const Trk::MeasurementBase* hit)
{

  // this method identifies which hits on modules for Trk::Track* trk overlap with Trk::RIO_OnTrack* hit
  // - only one overlapping module should be returned
  // - the overlapping module must be at a greater global radius than Trk::RIO_OnTrack* hit
  //   this avoids double counting when you are trying to find all the unique overlaps for a track
  // - for SCT overlaps, the overlapping module must have the same axial/stereo orientation as the Trk::RIO_OnTrack* hit
  //   otherwise the computation of the overlap residual is using two different coordinate frames and will be distorted
  // returns two Trk::TrackStateOnSurface, one for a potential x overlaps and one for a potential y overlap
  // if no overlap is found the corresponding Trk::TrackStateOnSurface will be null

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in  IDAlignMonEfficiencies::findOverlapHit()"<< endmsg;

  const Trk::TrackStateOnSurface* xOverlap = NULL;
  const Trk::TrackStateOnSurface* yOverlap = NULL;

  if(!hit) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit is NULL; abandoning overlap search"<< endmsg;
    std::pair <const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> result(xOverlap, yOverlap);
    return result;
  }

  Identifier hitId;
  if (hit->associatedSurface().associatedDetectorElement()!=NULL) hitId = hit->associatedSurface().associatedDetectorElement()->identify();
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit doesn't have an associated detector element; abandoning overlap search"<< endmsg;
    std::pair <const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> result(xOverlap, yOverlap);
    return result;
  }

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "obtained hit identifier"<< endmsg;
  int detType = 99;
  int barrelEC = 99;
  int layerDisk = 99;
  int modEta = 99;
  int modPhi = 99;
  if (m_idHelper->is_pixel(hitId)) detType = 0;
  if (m_idHelper->is_sct(hitId)) detType = 1;
  if (m_idHelper->is_trt(hitId)) detType = 2;

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "determined hit detector type = "<< detType << endmsg;

  //determining Si module physical position
  if (detType==0){//pixel
    const Identifier& id = m_pixelID->wafer_id(hitId);
    barrelEC = m_pixelID->barrel_ec(id);
    layerDisk = m_pixelID->layer_disk(id);
    modEta = m_pixelID->eta_module(id);
    modPhi = m_pixelID->phi_module(id);
  }
  if (detType==1){//sct
    const Identifier& id = m_sctID->wafer_id(hitId);
    barrelEC = m_sctID->barrel_ec(id);
    layerDisk = m_sctID->layer_disk(id);
    modEta = m_sctID->eta_module(id);
    modPhi = m_sctID->phi_module(id);
  }

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "**************looking for overlaps for new hit detType = " << detType
	    << ", modEta = " << modEta << ", modPhi = " << modPhi << endmsg;


  int nHits = 0;
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos2=trk->trackStateOnSurfaces()->begin();tsos2!=trk->trackStateOnSurfaces()->end(); ++tsos2) {

    int detType2 = -99;
    int barrelEC2 = -99;
    int layerDisk2 = -99;
    int modEta2 = -99;
    int modPhi2 = -99;
    const Trk::MeasurementBase* hit2 =(*tsos2)->measurementOnTrack();
    if (hit2== NULL) continue;//the first hit on the track never has associated hit information - just stores track parameters
    nHits++;

    Identifier hitId2;
    if (hit2->associatedSurface().associatedDetectorElement()!=NULL) hitId2 = hit2->associatedSurface().associatedDetectorElement()->identify();
    else {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "hit tested for overlap doesn't have an associated detector element"<< endmsg;
      continue;
    }

    if (m_idHelper->is_pixel(hitId2)) detType2 = 0;
    if (m_idHelper->is_sct(hitId2)) detType2 = 1;
    if (m_idHelper->is_trt(hitId2)) detType2 = 2;

    //determining Si module physical position
    if (detType2==0){//pixel
      const Identifier& id = m_pixelID->wafer_id(hitId2);
      barrelEC2 = m_pixelID->barrel_ec(id);
      layerDisk2 = m_pixelID->layer_disk(id);
      modEta2 = m_pixelID->eta_module(id);
      modPhi2 = m_pixelID->phi_module(id);
    }
    if (detType2==1){//sct
      const Identifier& id = m_sctID->wafer_id(hitId2);
      barrelEC2 = m_sctID->barrel_ec(id);
      layerDisk2 = m_sctID->layer_disk(id);
      modEta2 = m_sctID->eta_module(id);
      modPhi2 = m_sctID->phi_module(id);
    }

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "testing hit " << nHits << " for overlap, detType2 = " << detType2
	       << ", modEta2 = " << modEta2 << ", modPhi2 = " << modPhi2 << endmsg;

    if(!(*tsos2)->type(Trk::TrackStateOnSurface::Measurement)) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because hit is an outlier" << endmsg;
      continue;
    }

    if(detType!=detType2) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same detector" << endmsg;
      continue;
    }
    if(barrelEC!=barrelEC2) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same barrel/endcap" << endmsg;
      continue;
    }
    if(layerDisk!=layerDisk2) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because not the same layer/disk" << endmsg;
      continue;
    }

    if(modEta==modEta2 && modPhi==modPhi2){
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "overlap rejected because this is the original hit (or the opposite side for SCT)" << endmsg;
      continue;
    }

    if (detType==1){
      //const InDetDD::SiDetectorElement *siDet = dynamic_cast<const InDetDD::SiDetectorElement*>(hit->detectorElement());
      const InDetDD::SiDetectorElement *siDet = dynamic_cast<const InDetDD::SiDetectorElement*>(hit->associatedSurface().associatedDetectorElement());
      bool stereo = siDet->isStereo();
      const InDetDD::SiDetectorElement *siDet2 = dynamic_cast<const InDetDD::SiDetectorElement*>(hit2->associatedSurface().associatedDetectorElement());
      bool stereo2 = siDet2->isStereo();
      if(stereo!=stereo2){
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "SCT overlap rejected because the modules are not both stereo/radial modules" << endmsg;
	continue;
      }
    }

    //const Trk::TrackParameters* trackParameter = (*tsos2)->trackParameters();
    //const Trk::MeasuredTrackParameters* measuredTrackParameter = dynamic_cast<const Trk::MeasuredTrackParameters*>(trackParameter);

    const Trk::TrackParameters* TrackParameters = (*tsos2)->trackParameters();
    if (!TrackParameters)
      continue;
    const AmgSymMatrix(5)* covariance = TrackParameters->covariance();


    if(covariance==NULL) {

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap rejected because overlap hit does not have associated measuredTrackParameters" << endmsg;
      continue;
    }

    if((modEta==modEta2 && modPhi!=modPhi2) || (modEta-modEta2 == 1 && modPhi==modPhi2)){
      //potentially an overlap hit - apply hit quality cuts if tool configured
      if((detType2==0 || detType2==1) && m_doHitQuality) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "applying hit quality cuts to overlap hit..." << endmsg;

	hit2 = m_hitQualityTool->getGoodHit(*tsos2);
	if(hit2==NULL) {
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap rejected because failed hit quality cuts." << endmsg;
	  continue;
	}
	else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "overlap hit passed quality cuts" << endmsg;
      }
      else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "hit quality cuts NOT APPLIED to overlap hit." << endmsg;
    }


    bool close = false; //added by LT

    if(modEta==modEta2 && modPhi!=modPhi2){

      // begin added by TG
      // Pixel barrel #phi: 22, 38, 52
      // Pixel EC #phi: 48
      // SCT barrel #phi: 32, 40, 48, 56
      // SCT EC #phi: 52 ???
      if(modPhi-modPhi2 == 1){
	close = true;
	// Pix barrel special cases
      } else if( (detType==0 && barrelEC==0 && layerDisk==2) && modPhi-modPhi2 == -51){
	close = true;
      } else if ( (detType==0 && barrelEC==0 && layerDisk==1) && modPhi-modPhi2 == -37){
	close = true;
      } else if ( (detType==0 && barrelEC==0 && layerDisk==0) && modPhi-modPhi2 == -21){
	close = true;
	// SCT barrel special cases
      } else if( (detType==1 && barrelEC==0 && layerDisk==3) && modPhi-modPhi2 == -55){
	close = true;
      } else if( (detType==1 && barrelEC==0 && layerDisk==2) && modPhi-modPhi2 == -47){
	close = true;
      } else if ( (detType==1 && barrelEC==0 && layerDisk==1) && modPhi-modPhi2 == -39){
	close = true;
      } else if ( (detType==1 && barrelEC==0 && layerDisk==0) && modPhi-modPhi2 == -31){
	close = true;
	// Pix EC special cases
      } else if( (detType==0 && abs(barrelEC)==2) && modPhi-modPhi2 == -47){
	close = true;
	// SCT EC special cases: check!!!
      } else if( (detType==1 && abs(barrelEC)==2) && modPhi-modPhi2 == -51){
	close = true;
      }
      if(close){  //end add by TG
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "***** identified local X overlap" << endmsg;
	//	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "original module radius = " << radius << endmsg;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "original module phi, eta  = " << modEta <<", "<<modPhi<< endmsg;
	//if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "overlap module radius = " << radius2 << endmsg;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "second module phi, eta  = " << modEta2 <<", "<<modPhi2<< endmsg;
	xOverlap = (*tsos2);
      } //added by LT

    }
    if(modEta-modEta2 == 1 && modPhi==modPhi2){

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "***** identified local Y overlap" << endmsg;
      //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "original module radius = " << radius << endmsg;
      //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "overlap module radius = " << radius2 << endmsg;
      yOverlap = (*tsos2);
    }
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "modEta2 = " << modEta2 << endmsg;
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "modPhi2 = " << modPhi2 << endmsg;
  }




  std::pair <const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> result(xOverlap, yOverlap);
  return result;

}

void IDAlignMonEfficiencies::makeTRTHistograms(MonGroup& al_mon, MonGroup& al_mon_ls){

  //Make the Barrel histograms
  makeTRTBarrelHistograms(al_mon, al_mon_ls);

  //Make the Endcap histograms
  makeTRTEndcapHistograms(al_mon, al_mon_ls);

}

void IDAlignMonEfficiencies::makePIXBarrelHistograms(MonGroup& al_mon){
  m_NPixLayers = m_PIX_Mgr->numerology().numLayers();
  for (int iLayer=0; iLayer < m_PIX_Mgr->numerology().numLayers();++iLayer)
    {
      if (!m_PIX_Mgr->numerology().useLayer(iLayer)){
	msg(MSG::WARNING) << "Layer "<<iLayer<<" Not in Use"<<endmsg;
	continue;}
      float EtaModules= m_PIX_Mgr->numerology().endEtaModuleForLayer(iLayer) - m_PIX_Mgr->numerology().beginEtaModuleForLayer(iLayer); //(i put float in order to divide by 2)
      float EtaModulesMin;// = -EtaModules/2-0.5;
      float EtaModulesMax;// =  EtaModules/2.+0.5; if (iLayer==0) EtaModulesMax--; // IBL eta rings rang from -10 to +9
      // pixel layers
      EtaModulesMin = -(EtaModules-1)/2-0.5;  // so from -6.5 to +6.5 (13 modules)
      EtaModulesMax =  (EtaModules-1)/2+0.5;
      // IBL modules
      if (iLayer == 0) {
	EtaModules--; // IBL has one ring less total 20: from -10 to +9 (including 0)
	EtaModulesMin = -EtaModules/2.-0.5; // -10.5
	EtaModulesMax =  EtaModules/2.+0.5; EtaModulesMax--; // +9.5
      }

      int maxPhiModulesPerLayer = m_PIX_Mgr->numerology().numPhiModulesForLayer(iLayer);
      m_measurements_eff_vs_Eta_Phi_pix_b.push_back(new TProfile2D(("measurements_eff_vs_Eta_Phi_pix_b"+intToString(iLayer)).c_str(),("hit eff. vs. Eta-Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5,0.,1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_pix_b[iLayer]);
      //all hits
      m_hits_vs_Eta_Phi_pix_b.push_back(new TH2F(("hits_vs_Eta_Phi_pix_b"+intToString(iLayer)).c_str(),("possible hits vs. Eta-Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_hits_vs_Eta_Phi_pix_b[iLayer]);
      //hits on track
      m_measurements_vs_Eta_Phi_pix_b.push_back(new TH2F(("measurements_vs_Eta_Phi_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_pix_b[iLayer]);
      //outliers
      m_outliers_vs_Eta_Phi_pix_b.push_back(new TH2F(("outliers_vs_Eta_Phi_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_pix_b[iLayer]);



      //holes
      m_holes_vs_Eta_Phi_pix_b.push_back(new TH2F(("holes_vs_Eta_Phi_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_holes_vs_Eta_Phi_pix_b[iLayer]);



      //hits in barrel by layer
      m_hits_vs_Phi_pix_b.push_back(new TH1F(("hits_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_hits_vs_Phi_pix_b[iLayer]);
      //hits on track by layer
      m_measurements_vs_Phi_pix_b.push_back(new TH1F(("measurements_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_measurements_vs_Phi_pix_b[iLayer]);
      //hits on track by layer Overlap

      if (m_extendedPlots)
	{
	  m_overlapX_vs_Phi_pix_b.push_back(new TH1F(("overlapX_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon,m_overlapX_vs_Phi_pix_b[iLayer]);
	  m_overlapY_vs_Phi_pix_b.push_back(new TH1F(("overlapY_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon,m_overlapY_vs_Phi_pix_b[iLayer]);
	}
      //outliers by layer
      m_outliers_vs_Phi_pix_b.push_back(new TH1F(("outliers_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_outliers_vs_Phi_pix_b[iLayer]);
      //holes by layer vs phi
      m_holes_vs_Phi_pix_b.push_back(new TH1F(("holes_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_holes_vs_Phi_pix_b[iLayer]);
      //hit efficiency by layer
      m_measurements_eff_vs_Phi_pix_b.push_back(new TProfile(("measurements_eff_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0.,1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Phi_pix_b[iLayer]);

      //hit efficiency vs LB by layer

      m_hits_vs_LB_pix_b.push_back(new TH1F(("hits_vs_LB_pix_b"+intToString(iLayer)).c_str(),("possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_hits_vs_LB_pix_b[iLayer]);

      m_measurements_vs_LB_pix_b.push_back(new TH1F(("measurements_vs_LB_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_measurements_vs_LB_pix_b[iLayer]);

      m_measurements_eff_vs_LB_pix_b.push_back(new TProfile(("measurements_eff_vs_LB_pix_b"+intToString(iLayer)).c_str(),("measurements eff per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_LB_pix_b[iLayer]);

      //holes vs LB by layer
      m_holes_vs_LB_pix_b.push_back(new TH1F(("holes_vs_LB_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_holes_vs_LB_pix_b[iLayer]);

      m_holes_eff_vs_LB_pix_b.push_back(new TProfile(("holes_eff_vs_LB_pix_b"+intToString(iLayer)).c_str(),("holes eff per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
      RegisterHisto(al_mon,m_holes_eff_vs_LB_pix_b[iLayer]);

      //outliers vs LB by layer

      m_outliers_vs_LB_pix_b.push_back(new TH1F(("outliers_vs_LB_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_outliers_vs_LB_pix_b[iLayer]);

      m_outliers_eff_vs_LB_pix_b.push_back(new TProfile(("outliers_eff_vs_LB_pix_b"+intToString(iLayer)).c_str(),("outliers eff per possible hits vs. LB-ID in PIX barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_LB_pix_b[iLayer]);


      //overlap efficiency by layer
      if (m_extendedPlots)
	{
	  m_overlapX_eff_vs_Phi_pix_b.push_back(new TProfile(("overlapX_eff_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
	  RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_pix_b[iLayer]);
	  //overlap efficiency by layer
	  m_overlapY_eff_vs_Phi_pix_b.push_back(new TProfile(("overlapY_eff_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Phi-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
	  RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_pix_b[iLayer]);
	}
      //outlier eff by layer
      m_outliers_eff_vs_Phi_pix_b.push_back(new TProfile(("outliers_eff_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_Phi_pix_b[iLayer]);
      //hole frac by layer
      m_holes_eff_vs_Phi_pix_b.push_back(new TProfile(("holes_eff_vs_Phi_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Phi-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_Phi_pix_b[iLayer]) ;
      //hits vs Eta Layer
      m_hits_vs_Eta_pix_b.push_back(new TH1F(("hits_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules,  EtaModulesMin, EtaModulesMax));
      RegisterHisto(al_mon,m_hits_vs_Eta_pix_b[iLayer]);
      //measured hits vs eta
      m_measurements_vs_Eta_pix_b.push_back( new TH1F(("measurements_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax));
      RegisterHisto(al_mon,m_measurements_vs_Eta_pix_b[iLayer]);
      if (m_extendedPlots)
	{
	  //hits overlap Eta
	  m_overlapX_vs_Eta_pix_b.push_back(new TH1F(("overlapX_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax));
	  RegisterHisto(al_mon,m_overlapX_vs_Eta_pix_b[iLayer]);
	  m_overlapY_vs_Eta_pix_b.push_back(new TH1F(("overlapY_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax));
	  RegisterHisto(al_mon,m_overlapY_vs_Eta_pix_b[iLayer]);
	}
      //outliers vs Eta
      m_outliers_vs_Eta_pix_b.push_back(new TH1F(("outliers_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax));
      RegisterHisto(al_mon,m_outliers_vs_Eta_pix_b[iLayer]);
      //holes vs eta
      m_holes_vs_Eta_pix_b.push_back(new TH1F(("holes_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-ID in Pixel barrel layer"+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax));
      RegisterHisto(al_mon,m_holes_vs_Eta_pix_b[iLayer]) ;
      //copy to SCT
      //measurements eff vs Eta
      m_measurements_eff_vs_Eta_pix_b.push_back( new TProfile(("measurements_eff_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, 0., 1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Eta_pix_b[iLayer]) ;
      if (m_extendedPlots)
	{
	  //OverlapX eff vs Eta
	  m_overlapX_eff_vs_Eta_pix_b.push_back(new TProfile(("overlapX_eff_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules,EtaModules, 0., 1.));
	  RegisterHisto(al_mon,m_overlapX_eff_vs_Eta_pix_b[iLayer]) ;
	  m_overlapY_eff_vs_Eta_pix_b.push_back(new TProfile(("overlapY_eff_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules,EtaModules, 0., 1.));
	  RegisterHisto(al_mon,m_overlapY_eff_vs_Eta_pix_b[iLayer]) ;
	}
      //Outliers eff vs Eta
      m_outliers_eff_vs_Eta_pix_b.push_back(new TProfile(("outliers_eff_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_Eta_pix_b[iLayer]) ;
      //Holes vs Eta
      m_holes_eff_vs_Eta_pix_b.push_back( new TProfile(("holes_eff_vs_Eta_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-ID in Pixel barrel layer "+intToString(iLayer)).c_str(),EtaModules, EtaModulesMin, EtaModulesMax, 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_Eta_pix_b[iLayer]) ;
      //hits vs PT
      m_hits_vs_pT_pix_b.push_back( new TH1F(("hits_vs_pT_pix_b"+intToString(iLayer)).c_str(),("possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_hits_vs_pT_pix_b[iLayer]);
      //measurements vs Pt
      m_measurements_vs_pT_pix_b.push_back(new TH1F(("measurements_vs_pT_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_measurements_vs_pT_pix_b[iLayer]) ;
      //outliers vs Pt
      m_outliers_vs_pT_pix_b.push_back(new TH1F(("outliers_vs_pT_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_outliers_vs_pT_pix_b[iLayer]) ;
      //holes vs Pt
      m_holes_vs_pT_pix_b.push_back(new TH1F(("holes_vs_pT_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_holes_vs_pT_pix_b[iLayer]) ;
      //measurement eff vs pT
      m_measurements_eff_vs_pT_pix_b.push_back( new TProfile(("measurements_eff_vs_pT_pix_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_pT_pix_b[iLayer]) ;
      //outliers eff vs pT
      m_outliers_eff_vs_pT_pix_b.push_back(new TProfile(("outliers_eff_vs_pT_pix_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_pT_pix_b[iLayer]) ;
      //holes eff vs pT
      m_holes_eff_vs_pT_pix_b.push_back(new TProfile(("holes_eff_vs_pT_pix_b"+intToString(iLayer)).c_str(),("holes per possible hits vs.pT in Pixel barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_pT_pix_b[iLayer]) ;
    } //Layer loop
}

void IDAlignMonEfficiencies::makePIXEndCapsHistograms(MonGroup& al_mon){

  for (int iECIndex = 0; iECIndex < m_PIX_Mgr->numerology().numEndcaps(); ++iECIndex)
    {
      int iSide = m_PIX_Mgr->numerology().endcapId(iECIndex);
      for (int iWheel=0; iWheel < m_PIX_Mgr->numerology().numDisks();++iWheel)
	{
	  int rings=0;
	  if (!m_PIX_Mgr->numerology().useDisk(iWheel)){//To check if the Wheel is in use.
	    msg(MSG::WARNING) << "Wheel "<<iWheel<<" Not in Use"<<endmsg;
	    continue;}
	  else
	    {
	      rings=m_PIX_Mgr->numerology().numRingsForDisk(iWheel);
	    }
	  int maxPhiModulesPerRing(0);
	  for (int iEta = 0; iEta < rings; ++iEta)  //actually number of rings for PIX = 1
	    {
	      if (maxPhiModulesPerRing < m_PIX_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta)) //maybe a !=
		maxPhiModulesPerRing = m_PIX_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta);
	    }
	  if (iSide>0)
	    {

	      m_hits_vs_Phi_pix_eca.push_back( new TH1F(("hits_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),("possible hits vs. Phi-ID in Pixel eca Disk "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_hits_vs_Phi_pix_eca[iWheel]) ;

	      //vs LB

	      m_hits_vs_LB_pix_eca.push_back ( new TH1F(("hits_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("possible hits vs. LB-ID in Pixel ECA Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_hits_vs_LB_pix_eca[iWheel]);

	      m_measurements_vs_LB_pix_eca.push_back( new TH1F(("measurements_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("possible measurements vs. LB-ID in Pixel ECA Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_measurements_vs_LB_pix_eca[iWheel]);

	      m_measurements_eff_vs_LB_pix_eca.push_back(new TProfile(("measurements_eff_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("measurements eff per possible hits vs. LB-ID in PIX Eca Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_measurements_eff_vs_LB_pix_eca[iWheel]);

	      //outliers

	      m_outliers_vs_LB_pix_eca.push_back( new TH1F(("outliers_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("outliers vs. LB-ID in Pixel ECA Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_outliers_vs_LB_pix_eca[iWheel]);

	      m_outliers_eff_vs_LB_pix_eca.push_back(new TProfile(("outliers_eff_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("outliers eff per possible hits vs. LB-ID in PIX Eca Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_outliers_eff_vs_LB_pix_eca[iWheel]);

	      //holes

	      m_holes_vs_LB_pix_eca.push_back( new TH1F(("holes_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("holes vs. LB-ID in Pixel ECA Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_holes_vs_LB_pix_eca[iWheel]);

	      m_holes_eff_vs_LB_pix_eca.push_back(new TProfile(("holes_eff_vs_LB_pix_eca"+intToString(iWheel)).c_str(),("holes eff per possible hits vs. LB-ID in PIX Eca Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_holes_eff_vs_LB_pix_eca[iWheel]);



	      if (m_extendedPlots)
		{
		  //overlaps by layer
		  m_overlapY_vs_Phi_pix_eca.push_back(new  TH1F(("overlapY_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),(" Y Overlap Eff. vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
		  RegisterHisto(al_mon,m_overlapY_vs_Phi_pix_eca[iWheel]) ;
		  m_overlapX_vs_Phi_pix_eca.push_back(new  TH1F(("overlapX_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),(" X Overlap Eff. vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
		  RegisterHisto(al_mon,m_overlapX_vs_Phi_pix_eca[iWheel]) ;

		  m_overlapX_eff_vs_Phi_pix_eca.push_back(new  TProfile(("overlapX_eff_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),("overlapX per possible hits vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
		  RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_pix_eca[iWheel]) ;
		  m_overlapY_eff_vs_Phi_pix_eca.push_back(new  TProfile(("overlapY_eff_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),("overlapY per possible hits vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
		  RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_pix_eca[iWheel]) ;
		}
	      //hits on track for endcaps
	      m_measurements_vs_Phi_pix_eca.push_back(new  TH1F(("measurements_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),("possible measurements vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_measurements_vs_Phi_pix_eca[iWheel]) ;
	      //efficiencies for endcaps
	      m_measurements_eff_vs_Phi_pix_eca.push_back(new  TProfile(("measurements_eff_vs_Phi_pix_eca"+intToString(iWheel)).c_str(),("measurements per possible hits vs. Phi-ID in Pixel eca layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
	      RegisterHisto(al_mon,m_measurements_eff_vs_Phi_pix_eca[iWheel]) ;





	    }
	  if (iSide<0)
	    {
	      //hits for endcaps
	      m_hits_vs_Phi_pix_ecc.push_back( new TH1F(("hits_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),("possible hits vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_hits_vs_Phi_pix_ecc[iWheel]) ;

	      //vs LB

	      m_hits_vs_LB_pix_ecc.push_back( new TH1F(("hits_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("possible hits vs. LB-ID in Pixel ECC  Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_hits_vs_LB_pix_ecc[iWheel]);

	      m_measurements_vs_LB_pix_ecc.push_back( new TH1F(("measurements_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("possible measurements vs. LB-ID in Pixel ECC Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_measurements_vs_LB_pix_ecc[iWheel]);

	      m_measurements_eff_vs_LB_pix_ecc.push_back(new TProfile(("measurements_eff_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("measurements eff per possible hits vs. LB-ID in PIX Ecc Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_measurements_eff_vs_LB_pix_ecc[iWheel]);

	      //outliers

	      m_outliers_vs_LB_pix_ecc.push_back( new TH1F(("outliers_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("outliers vs. LB-ID in Pixel ECC Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_outliers_vs_LB_pix_ecc[iWheel]);

	      m_outliers_eff_vs_LB_pix_ecc.push_back(new TProfile(("outliers_eff_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("outliers eff per possible hits vs. LB-ID in PIX Ecc Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_outliers_eff_vs_LB_pix_ecc[iWheel]);

	      //holes

	      m_holes_vs_LB_pix_ecc.push_back( new TH1F(("holes_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("holes vs. LB-ID in Pixel ECC Disk " +intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB));
	      RegisterHisto(al_mon,m_holes_vs_LB_pix_ecc[iWheel]);

	      m_holes_eff_vs_LB_pix_ecc.push_back(new TProfile(("holes_eff_vs_LB_pix_ecc"+intToString(iWheel)).c_str(),("holes eff per possible hits vs. LB-ID in PIX Ecc Disk "+intToString(iWheel)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
	      RegisterHisto(al_mon,m_holes_eff_vs_LB_pix_ecc[iWheel]);


	      if (m_extendedPlots)
		{
		  //overlaps by layer
		  m_overlapY_vs_Phi_pix_ecc.push_back(new  TH1F(("overlapY_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),(" Y Overlap Eff. vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
		  RegisterHisto(al_mon,m_overlapY_vs_Phi_pix_ecc[iWheel]) ;
		  m_overlapX_vs_Phi_pix_ecc.push_back(new  TH1F(("overlapX_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),(" X Overlap Eff. vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
		  RegisterHisto(al_mon,m_overlapX_vs_Phi_pix_ecc[iWheel]) ;
		  m_overlapX_eff_vs_Phi_pix_ecc.push_back(new  TProfile(("overlapX_eff_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),("overlapX per possible hits vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
		  RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_pix_ecc[iWheel]) ;
		  m_overlapY_eff_vs_Phi_pix_ecc.push_back(new  TProfile(("overlapY_eff_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),("overlapY per possible hits vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
		  RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_pix_ecc[iWheel]) ;
		}



	      //hits on track for endcaps
	      m_measurements_vs_Phi_pix_ecc.push_back(new  TH1F(("measurements_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),("possible measurements vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5));
	      RegisterHisto(al_mon,m_measurements_vs_Phi_pix_ecc[iWheel]) ;
	      //efficiencies for endcaps
	      m_measurements_eff_vs_Phi_pix_ecc.push_back(new  TProfile(("measurements_eff_vs_Phi_pix_ecc"+intToString(iWheel)).c_str(),("measurements per possible hits vs. Phi-ID in Pixel ecc layer "+intToString(iWheel)).c_str(),maxPhiModulesPerRing,-0.5,maxPhiModulesPerRing-0.5, 0., 1.));
	      RegisterHisto(al_mon,m_measurements_eff_vs_Phi_pix_ecc[iWheel]) ;

	    }
	}
    }

}

void IDAlignMonEfficiencies::makeSCTBarrelHistograms(MonGroup &al_mon){

  for (int iLayer=0; iLayer < m_SCT_Mgr->numerology().numLayers();++iLayer)
    {
      //ATH_MSG_INFO("iLayer= " << iLayer);
      if (!m_SCT_Mgr->numerology().useLayer(iLayer)){
	msg(MSG::WARNING) << "Layer "<<iLayer<<" Not Present"<<endmsg;
	continue;}
      float maxPhiModulesPerLayer = m_SCT_Mgr->numerology().numPhiModulesForLayer(iLayer);
      //just for checking purposes. Not useful. I will cancel it soon. Peo
      for (int iEta = m_SCT_Mgr->numerology().beginEtaModuleForLayer(iLayer); iEta < m_SCT_Mgr->numerology().endEtaModuleForLayer(iLayer);++iEta)
	{
	  if (!iEta && m_SCT_Mgr->numerology().skipEtaZeroForLayer(iLayer))
	    //iEta=0 is not defined for the SCT
	    continue;
	  //ATH_MSG_INFO("iEta= " << iEta);
	}
      float EtaModules= m_SCT_Mgr->numerology().endEtaModuleForLayer(iLayer) - m_SCT_Mgr->numerology().beginEtaModuleForLayer(iLayer); //(i put float in order to divide by 2. I am not skipping the empty layer. It might be nice to see the separation)
      //m_measurements_eff_vs_Eta_Phi_sct_b.push_back(new TProfile2D(("measurements_eff_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("hit eff. vs. Eta-Phi-ID in Sct barrel layer"+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5,0.,1.));
      //RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_sct_b[iLayer]);
      m_measurements_eff_vs_Eta_Phi_sct_b.push_back(new TProfile2D(("measurements_eff_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("hit eff. vs. Eta-Phi-ID in Sct barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer,0.,1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Eta_Phi_sct_b[iLayer]);
      //all hits
      m_hits_vs_Eta_Phi_sct_b.push_back(new TH2F(("hits_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_hits_vs_Eta_Phi_sct_b[iLayer]);
      //hits on track
      m_measurements_vs_Eta_Phi_sct_b.push_back(new TH2F(("measurements_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_measurements_vs_Eta_Phi_sct_b[iLayer]);


      m_hits_vs_LB_sct_b.push_back(new TH1F(("hits_vs_LB_sct_b"+intToString(iLayer)).c_str(),("hits per possible hits vs. LB-ID in SCT barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_hits_vs_LB_sct_b[iLayer]);

      m_measurements_vs_LB_sct_b.push_back(new TH1F(("measurements_vs_LB_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. LB-ID in SCT barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB));
      RegisterHisto(al_mon,m_measurements_vs_LB_sct_b[iLayer]);

      m_measurements_eff_vs_LB_sct_b.push_back(new TProfile(("measurements_eff_vs_LB_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. LB-ID in SCT barrel layer "+intToString(iLayer)).c_str(),m_nLB,m_minLB,m_maxLB, 0.,1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_LB_sct_b[iLayer]);





      for (int side=0; side < 2; side++) {
	// std::cout << " -- Salva -- hit map of SCT BAR layer " <<  iLayer << " side: " << side << "  Name: " << ("measurements_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str() << std::endl;
	if (side == 0) {
	  // posible hits
	  m_hits_vs_Eta_Phi_sct_s0_b.push_back(new TH2F(("expectedHits_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_hits_vs_Eta_Phi_sct_s0_b[iLayer]);
	  // measurements = seen hits
	  m_measurements_vs_Eta_Phi_sct_s0_b.push_back(new TH2F(("measurements_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_s0_b[iLayer]);
	  // efficiency
	  m_measurements_eff_vs_Eta_Phi_sct_s0_b.push_back(new TProfile2D(("measurements_eff_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_measurements_eff_vs_Eta_Phi_sct_s0_b[iLayer]);
	}
	if (side == 1) {
	  // posible hits
	  m_hits_vs_Eta_Phi_sct_s1_b.push_back(new TH2F(("expectedHits_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_hits_vs_Eta_Phi_sct_s1_b[iLayer]);
	  // measurements = seen hits
	  m_measurements_vs_Eta_Phi_sct_s1_b.push_back(new TH2F(("measurements_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_measurements_vs_Eta_Phi_sct_s1_b[iLayer]);
	  // efficiency
	  m_measurements_eff_vs_Eta_Phi_sct_s1_b.push_back(new TProfile2D(("measurements_eff_vs_Eta_Phi_sct_b"+intToString(iLayer)+"_s"+intToString(side)).c_str(),("measurements per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)+" side "+intToString(side)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon, m_measurements_eff_vs_Eta_Phi_sct_s1_b[iLayer]);
	}
      }

      //outliers
      m_outliers_vs_Eta_Phi_sct_b.push_back(new TH2F(("outliers_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_outliers_vs_Eta_Phi_sct_b[iLayer]);
      //holes
      m_holes_vs_Eta_Phi_sct_b.push_back(new TH2F(("holes_vs_Eta_Phi_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-Phi-ID in SCT barrel layer"+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.,maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_holes_vs_Eta_Phi_sct_b[iLayer]);
      //hits in barrel by layer
      m_hits_vs_Phi_sct_b.push_back(new TH1F(("hits_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("possible hits vs. Phi-ID in SCT barrel layer"+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_hits_vs_Phi_sct_b[iLayer]);
      //hits on track by layer
      m_measurements_vs_Phi_sct_b.push_back(new TH1F(("measurements_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_measurements_vs_Phi_sct_b[iLayer]);
      if (m_extendedPlots)
	{
	  //hits on track by layer Overlap
	  m_overlapX_vs_Phi_sct_b.push_back(new TH1F(("overlapX_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon,m_overlapX_vs_Phi_sct_b[iLayer]);
	  m_overlapY_vs_Phi_sct_b.push_back(new TH1F(("overlapY_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
	  RegisterHisto(al_mon,m_overlapY_vs_Phi_sct_b[iLayer]);
	  //overlap efficiency by layer
	  m_overlapX_eff_vs_Phi_sct_b.push_back(new TProfile(("overlapX_eff_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
	  RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_sct_b[iLayer]);
	  m_overlapY_eff_vs_Phi_sct_b.push_back(new TProfile(("overlapY_eff_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
	  RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_sct_b[iLayer]);
	}
      //outliers by layer
      m_outliers_vs_Phi_sct_b.push_back(new TH1F(("outliers_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_outliers_vs_Phi_sct_b[iLayer]);
      //holes by layer
      m_holes_vs_Phi_sct_b.push_back(new TH1F(("holes_vs_Phi_sct_b3"+intToString(iLayer)).c_str(),("holes per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5));
      RegisterHisto(al_mon,m_holes_vs_Phi_sct_b[iLayer]);
      //hit efficiency by layer
      m_measurements_eff_vs_Phi_sct_b.push_back(new TProfile(("measurements_eff_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Phi_sct_b[iLayer]);
      //outliers eff by layer
      m_outliers_eff_vs_Phi_sct_b.push_back(new TProfile(("outliers_eff_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_Phi_sct_b[iLayer]);
      //hole frac by layer
      m_holes_eff_vs_Phi_sct_b.push_back(new TProfile(("holes_eff_vs_Phi_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Phi-ID in SCT barrel layer "+intToString(iLayer)).c_str(),maxPhiModulesPerLayer,-0.5,maxPhiModulesPerLayer-0.5, 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_Phi_sct_b[iLayer]);
      //hits vs Eta Layer
      m_hits_vs_Eta_sct_b.push_back( new TH1F(("hits_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
      RegisterHisto(al_mon,m_hits_vs_Eta_sct_b[iLayer]);
      //measured hits vs eta
      m_measurements_vs_Eta_sct_b.push_back(new TH1F(("measurements_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
      RegisterHisto(al_mon,m_measurements_vs_Eta_sct_b[iLayer]) ;
      if (m_extendedPlots)
	{
	  //hits overlap Eta
	  m_overlapX_vs_Eta_sct_b.push_back(new TH1F(("overlapX_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
	  RegisterHisto(al_mon,m_overlapX_vs_Eta_sct_b[iLayer]) ;
	  m_overlapY_vs_Eta_sct_b.push_back(new TH1F(("overlapY_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
	  RegisterHisto(al_mon,m_overlapY_vs_Eta_sct_b[iLayer]) ;
	  //OverlapX eff vs Eta
	  m_overlapX_eff_vs_Eta_sct_b.push_back(new TProfile(("overlapX_eff_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("overlapX per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules,EtaModules, 0., 1.));
	  RegisterHisto(al_mon,m_overlapX_eff_vs_Eta_sct_b[iLayer]) ;
	  m_overlapY_eff_vs_Eta_sct_b.push_back(new TProfile(("overlapY_eff_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("overlapY per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules,EtaModules, 0., 1.));
	  RegisterHisto(al_mon,m_overlapY_eff_vs_Eta_sct_b[iLayer]) ;
	}
      //outliers vs eta
      m_outliers_vs_Eta_sct_b.push_back(new TH1F(("outliers_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
      RegisterHisto(al_mon,m_outliers_vs_Eta_sct_b[iLayer]);
      //holes vs eta
      m_holes_vs_Eta_sct_b.push_back(new TH1F(("holes_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-ID in SCT barrel layer"+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2.));
      RegisterHisto(al_mon,m_holes_vs_Eta_sct_b[iLayer]) ;
      //measurements eff vs Eta
      m_measurements_eff_vs_Eta_sct_b.push_back( new TProfile(("measurements_eff_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2, 0., 1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_Eta_sct_b[iLayer]) ;

      //Outliers eff vs Eta
      m_outliers_eff_vs_Eta_sct_b.push_back(new TProfile(("outliers_eff_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2., 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_Eta_sct_b[iLayer]) ;
      //Holes vs Eta
      m_holes_eff_vs_Eta_sct_b.push_back( new TProfile(("holes_eff_vs_Eta_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs. Eta-ID in SCT barrel layer "+intToString(iLayer)).c_str(),EtaModules,-EtaModules/2.,EtaModules/2., 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_Eta_sct_b[iLayer]) ;
      //hits vs PT
      m_hits_vs_pT_sct_b.push_back( new TH1F(("hits_vs_pT_sct_b"+intToString(iLayer)).c_str(),("possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_hits_vs_pT_sct_b[iLayer]);
      //measurements vs Pt
      m_measurements_vs_pT_sct_b.push_back(new TH1F(("measurements_vs_pT_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_measurements_vs_pT_sct_b[iLayer]) ;
      //outliers vs Pt
      m_outliers_vs_pT_sct_b.push_back(new TH1F(("outliers_vs_pT_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_outliers_vs_pT_sct_b[iLayer]) ;
      //holes vs Pt
      m_holes_vs_pT_sct_b.push_back(new TH1F(("holes_vs_pT_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100));
      RegisterHisto(al_mon,m_holes_vs_pT_sct_b[iLayer]) ;
      //measurement eff vs pT
      m_measurements_eff_vs_pT_sct_b.push_back( new TProfile(("measurements_eff_vs_pT_sct_b"+intToString(iLayer)).c_str(),("measurements per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_measurements_eff_vs_pT_sct_b[iLayer]) ;
      //outliers eff vs pT
      m_outliers_eff_vs_pT_sct_b.push_back(new TProfile(("outliers_eff_vs_pT_sct_b"+intToString(iLayer)).c_str(),("outliers per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_outliers_eff_vs_pT_sct_b[iLayer]) ;
      //holes eff vs pT
      m_holes_eff_vs_pT_sct_b.push_back(new TProfile(("holes_eff_vs_pT_sct_b"+intToString(iLayer)).c_str(),("holes per possible hits vs.pT in SCT barrel layer "+intToString(iLayer)).c_str(),100,-100,100, 0., 1.));
      RegisterHisto(al_mon,m_holes_eff_vs_pT_sct_b[iLayer]) ;
    }
}

void IDAlignMonEfficiencies::makeSCTEndCapsHistograms(MonGroup& al_mon){
  for (int iECIndex = 0; iECIndex < m_SCT_Mgr->numerology().numEndcaps(); ++iECIndex)
    {
      int iSide = m_SCT_Mgr->numerology().endcapId(iECIndex);
      for (int iWheel=0; iWheel < m_SCT_Mgr->numerology().numDisks();++iWheel)
	{
	  int rings=0;
	  if (!m_SCT_Mgr->numerology().useDisk(iWheel)){//To check if the Wheel is in use.
	    msg(MSG::WARNING) << "Wheel "<<iWheel<<" Not in Use"<<endmsg;
	    continue;}
	  else
	    {
	      rings=m_SCT_Mgr->numerology().numRingsForDisk(iWheel);
	    }
	  int maxModulesPerRing(0);
	  for (int iEta = 0; iEta < rings; ++iEta)   //iEta<m_SCT_Mgr->numerology().numRingsForDisk(iWheel);
	    {

	      if (maxModulesPerRing < m_SCT_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta)) //maybe a !=
		maxModulesPerRing = m_SCT_Mgr->numerology().numPhiModulesForDiskRing(iWheel,iEta);
	    }
	  if (iSide>0){ //ECA

	    m_hits_vs_Phi_sct_eca.push_back(new TH1F(("hits_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),("possible hits vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
	    RegisterHisto(al_mon,m_hits_vs_Phi_sct_eca[iWheel]) ;
	    if (m_extendedPlots)
	      {
		m_overlapX_vs_Phi_sct_eca.push_back(new TH1F(("overlapX_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),(" X Overlap Eff. vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
		RegisterHisto(al_mon,m_overlapX_vs_Phi_sct_eca[iWheel]) ;
		m_overlapY_vs_Phi_sct_eca.push_back(new TH1F(("overlapY_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),(" Y Overlap Eff. vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
		RegisterHisto(al_mon,m_overlapY_vs_Phi_sct_eca[iWheel]) ;
		m_overlapX_eff_vs_Phi_sct_eca.push_back(new TProfile(("overlapX_eff_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),("overlapX per possible hits vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
		RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_sct_eca[iWheel]) ;
		m_overlapY_eff_vs_Phi_sct_eca.push_back(new TProfile(("overlapY_eff_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),("overlapY per possible hits vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
		RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_sct_eca[iWheel]) ;
	      }

	    m_measurements_vs_Phi_sct_eca.push_back(new TH1F(("measurements_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),("possible measurements vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
	    RegisterHisto(al_mon,m_measurements_vs_Phi_sct_eca[iWheel]) ;
	    m_measurements_eff_vs_Phi_sct_eca.push_back(new TProfile(("measurements_eff_vs_Phi_sct_eca"+intToString(iWheel)).c_str(),("measurements per possible hits vs. Phi-ID in SCT eca layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
	    RegisterHisto(al_mon,m_measurements_eff_vs_Phi_sct_eca[iWheel]) ;




	  }
	  else if (iSide<0){ //ECC
	    m_hits_vs_Phi_sct_ecc.push_back(new TH1F(("hits_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),("possible hits vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
	    RegisterHisto(al_mon,m_hits_vs_Phi_sct_ecc[iWheel]) ;
	    if (m_extendedPlots)
	      {
		m_overlapX_vs_Phi_sct_ecc.push_back(new TH1F(("overlapX_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),(" X Overlap Eff. vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
		RegisterHisto(al_mon,m_overlapX_vs_Phi_sct_ecc[iWheel]) ;
		m_overlapY_vs_Phi_sct_ecc.push_back(new TH1F(("overlapY_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),(" Y Overlap Eff. vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
		RegisterHisto(al_mon,m_overlapY_vs_Phi_sct_ecc[iWheel]) ;
		m_overlapX_eff_vs_Phi_sct_ecc.push_back(new TProfile(("overlapX_eff_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),("overlapX per possible hits vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
		RegisterHisto(al_mon,m_overlapX_eff_vs_Phi_sct_ecc[iWheel]) ;
		m_overlapY_eff_vs_Phi_sct_ecc.push_back(new TProfile(("overlapY_eff_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),("overlapY per possible hits vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
		RegisterHisto(al_mon,m_overlapY_eff_vs_Phi_sct_ecc[iWheel]) ;
	      }

	    m_measurements_vs_Phi_sct_ecc.push_back(new TH1F(("measurements_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),("possible measurements vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5));
	    RegisterHisto(al_mon,m_measurements_vs_Phi_sct_ecc[iWheel]) ;
	    m_measurements_eff_vs_Phi_sct_ecc.push_back(new TProfile(("measurements_eff_vs_Phi_sct_ecc"+intToString(iWheel)).c_str(),("measurements per possible hits vs. Phi-ID in SCT ecc layer "+intToString(iWheel)).c_str(),maxModulesPerRing,-0.5,maxModulesPerRing-0.5, 0., 1.));
	    RegisterHisto(al_mon,m_measurements_eff_vs_Phi_sct_ecc[iWheel]) ;



	  }
	}
    }

  //vs LB

  m_hits_vs_LB_sct_eca = new TH1F("hits_vs_LB_sct_eca","measurements per possible hits vs. LB-ID in SCT ECA",m_nLB,m_minLB,m_maxLB);
  RegisterHisto(al_mon,m_hits_vs_LB_sct_eca);
  m_measurements_vs_LB_sct_eca = new TH1F("measurements_vs_LB_sct_eca","measurements per possible hits vs. LB-ID in SCT ECA",m_nLB,m_minLB,m_maxLB);
  RegisterHisto(al_mon,m_measurements_vs_LB_sct_eca);
  m_measurements_eff_vs_LB_sct_eca = new TProfile("measurements_eff_vs_LB_sct_eca","measurements per possible hits vs. LB-ID in SCT ECA",m_nLB,m_minLB,m_maxLB, 0.,1.);
  RegisterHisto(al_mon,m_measurements_eff_vs_LB_sct_eca);

  //vs LB


  m_hits_vs_LB_sct_ecc = new TH1F("hits_vs_LB_sct_ecc","hits vs. LB-ID in SCT ECC disk",m_nLB,m_minLB,m_maxLB);
  RegisterHisto(al_mon,m_hits_vs_LB_sct_ecc);
  m_measurements_vs_LB_sct_ecc = new TH1F("measurements_vs_LB_sct_ecc","measurements per possible hits vs. LB-ID in SCT ECC",m_nLB,m_minLB,m_maxLB);
  RegisterHisto(al_mon,m_measurements_vs_LB_sct_ecc);
  m_measurements_eff_vs_LB_sct_ecc =new TProfile("measurements_eff_vs_LB_sct_ecc","measurements per possible hits vs. LB-ID in SCT ECC",m_nLB,m_minLB,m_maxLB, 0.,1.);
  RegisterHisto(al_mon,m_measurements_eff_vs_LB_sct_ecc);
}



void IDAlignMonEfficiencies::makeTRTBarrelHistograms(MonGroup& al_mon, MonGroup& al_mon_ls){

  /** Barrel plots
   //==================== */

  for(int lay=0; lay<3; lay++){

    /** outliers in the barrel modules */
    m_trt_b_hist->outliers_vs_phiSector[lay] = MakeHist("outliers_vs_phiSector_trt_b"+intToString(lay),"Outliers vrs phi sector for Barrel layer "+intToString(lay),32, 0, 32,"Phi Sector","Outliers");
    RegisterHisto(al_mon,m_trt_b_hist->outliers_vs_phiSector[lay]);

    m_trt_b_hist->outliers_eff_vs_phiSector[lay] = MakeProfile("outliers_eff_vs_phiSector_trt_b"+intToString(lay),"Ratio Outliers to total measurements vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32, 0., 1.,"Phi Sector","Ratio of Outliers to Total Measurements");
    RegisterHisto(al_mon,m_trt_b_hist->outliers_eff_vs_phiSector[lay],"Fraction of Outliers") ;

    /** hits in the barrel modules */
    m_trt_b_hist->hits_vs_phiSector[lay] = MakeHist("hits_vs_phiSector_trt_b"+intToString(lay),"Hits vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32,"Phi Sector","Number of Hits");
    RegisterHisto(al_mon,m_trt_b_hist->hits_vs_phiSector[lay]) ;

    m_trt_b_hist->hits_eff_vs_phiSector[lay] = MakeProfile("hits_eff_vs_phiSector_trt_b"+intToString(lay),"Ratio Hits to total measurements vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32, 0., 1.,"Phi Sector","Ratio of Hits to Total Measurements");
    if ( m_useLowStat) RegisterHisto(al_mon_ls,m_trt_b_hist->hits_eff_vs_phiSector[lay],"Fraction of Precision Hits") ;
    if (!m_useLowStat) RegisterHisto(al_mon,   m_trt_b_hist->hits_eff_vs_phiSector[lay],"Fraction of Precision Hits") ;

    /** tube hits in the barrel modules */
    m_trt_b_hist->tubeHits_vs_phiSector[lay] = MakeHist("tubeHits_vs_phiSector_trt_b"+intToString(lay),"Tube Hits vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32, "Phi Sector","Number of Tube Hits");
    RegisterHisto(al_mon,m_trt_b_hist->tubeHits_vs_phiSector[lay]) ;

    m_trt_b_hist->tubeHits_eff_vs_phiSector[lay] = MakeProfile("tubeHits_eff_vs_phiSector_trt_b"+intToString(lay),"Ratio Tube Hits to total measurements vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32, 0., 1.,"Phi Sector","Ratio of Tube Hits to Total Measurements");
    RegisterHisto(al_mon,m_trt_b_hist->tubeHits_eff_vs_phiSector[lay],"Fraction of TubeHits") ;

    /** Total measurements in the barrel modules */
    m_trt_b_hist->totHits_vs_phiSector[lay] = MakeHist("totHist_vs_phiSector_trt_b"+intToString(lay),"Total Hits vrs phi sector for TRT Barrel layer "+intToString(lay),32,0,32, "Phi Sector","Number of Total Hits");
    RegisterHisto(al_mon,m_trt_b_hist->totHits_vs_phiSector[lay]);
  }

  m_trt_b_hist->outliers_vs_StrawLay = MakeHist("outliers_vs_StrawLayer_trt_b","Outliers vrs Straw Layer (Intergrated over Phi) for Barrel layer ",73, 0, 73, "Straw Layer","Outliers");

  RegisterHisto(al_mon,m_trt_b_hist->outliers_vs_StrawLay);

  m_trt_b_hist->outliers_eff_vs_StrawLay = MakeProfile("outliers_eff_vs_StrawLayer_trt_b","Ratio Outliers to total measurements vrs Straw Layer (Intergrated over Phi)  for TRT Barrel layer ",73,0,73, 0., 1.,"Straw Layer","Ratio of Outliers to Total Measurements");
  RegisterHisto(al_mon,m_trt_b_hist->outliers_eff_vs_StrawLay,"Fraction of Outliers") ;

  m_trt_b_hist->hits_vs_StrawLay = MakeHist("hits_vs_StrawLayer_trt_b","Hits vrs Straw Layer (Intergrated over Phi) for Barrel layer ",73, 0, 73,"Straw Layer","Hits");
  RegisterHisto(al_mon,m_trt_b_hist->hits_vs_StrawLay);

  m_trt_b_hist->hits_eff_vs_StrawLay = MakeProfile("hits_eff_vs_StrawLayer_trt_b","Ratio Hits to total measurements vrs Straw Layer (Intergrated over Phi)  for TRT Barrel layer ",73,0,73, 0., 1.,"Straw Layer","Ratio of Hits to Total Measurements");
  RegisterHisto(al_mon,m_trt_b_hist->hits_eff_vs_StrawLay,"Fraction of Precision Hits") ;

  m_trt_b_hist->tubeHits_vs_StrawLay = MakeHist("tubeHits_vs_StrawLayer_trt_b","TubeHits vrs Straw Layer (Intergrated over Phi) for Barrel layer ",73, 0, 73,"Straw Layer","TubeHits");
  RegisterHisto(al_mon,m_trt_b_hist->tubeHits_vs_StrawLay);

  m_trt_b_hist->tubeHits_eff_vs_StrawLay = MakeProfile("tubeHits_eff_vs_StrawLayer_trt_b","Ratio TubeHits to total measurements vrs Straw Layer (Intergrated over Phi)  for TRT Barrel layer ",73,0,73, 0., 1.,"Straw Layer","Ratio of TubeHits to Total Measurements");
  RegisterHisto(al_mon,m_trt_b_hist->tubeHits_eff_vs_StrawLay,"Fraction of Tube Hits") ;


  m_trt_b_hist->totHits_vs_StrawLay = TH1F_LW::create("totHits_vs_StrawLay_trt_b","Number of total measurements vrs straw layer for TRT Barrel layer",73, 0, 73);
  m_trt_b_hist->totHits_vs_StrawLay->SetMinimum(0);
  m_trt_b_hist->totHits_vs_StrawLay->GetXaxis()->SetLabelSize(0.03);
  m_trt_b_hist->totHits_vs_StrawLay->GetYaxis()->SetLabelSize(0.03);
  m_trt_b_hist->totHits_vs_StrawLay->GetXaxis()->SetTitle("Straw Layer");
  m_trt_b_hist->totHits_vs_StrawLay->GetYaxis()->SetTitle("Total Measurements");
  RegisterHisto(al_mon,m_trt_b_hist->totHits_vs_StrawLay);

  return;
}

void IDAlignMonEfficiencies::makeTRTEndcapHistograms(MonGroup& al_mon, MonGroup& al_mon_ls){

  /** EndCap */
  std::string endcapName[2] = {"Endcap_A","Endcap_C"};
  for(unsigned int endcap=0; endcap<2; ++endcap){

    /** Total measurements vs ring */
    m_trt_ec_hist->totHits_vs_ring[endcap] = MakeHist("totHits_vs_ring_trt_ec_"+endcapName[endcap],"Number of Total measurements vs Ring for "+endcapName[endcap],40,0,40,"Endcap Ring","Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->totHits_vs_ring[endcap]);

    /** outliers vs ring */
    m_trt_ec_hist->outliers_vs_ring[endcap] = MakeHist("outliers_vs_ring_trt_ec_"+endcapName[endcap],"Ratio of outliers to Total measurements vrs vs Ring for "+endcapName[endcap],40,0,40,"Endcap Ring","Outliers");
    RegisterHisto(al_mon,m_trt_ec_hist->outliers_vs_ring[endcap]);

    m_trt_ec_hist->outliers_eff_vs_ring[endcap] = MakeProfile("outliers_eff_vs_ring_trt_ec_"+endcapName[endcap],"Ratio of outliers to total measurements vs Ring for "+endcapName[endcap] ,40,0,40, 0., 1., "Endcap Ring","Ratio of Outliers to Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->outliers_eff_vs_ring[endcap]);

    /** hits vs ring */
    m_trt_ec_hist->hits_vs_ring[endcap] = MakeHist("hits_vs_ring_trt_ec_"+endcapName[endcap],"total hits(non-outliers) vs Ring for "+endcapName[endcap],40,0,40, "Endcap Ring","Hits");
    RegisterHisto(al_mon,m_trt_ec_hist->hits_vs_ring[endcap]);

    m_trt_ec_hist->hits_eff_vs_ring[endcap] = MakeProfile("hits_eff_vs_ring_trt_ec_"+endcapName[endcap],"Ratio of hits to total measurements vs Ring for "+endcapName[endcap] ,40,0,40, 0., 1.,"Endcap Ring","Ratio of Hits to Total Measurements");
    if ( m_useLowStat) RegisterHisto(al_mon_ls,m_trt_ec_hist->hits_eff_vs_ring[endcap],"Fraction of Precision Hits");
    if (!m_useLowStat) RegisterHisto(al_mon,   m_trt_ec_hist->hits_eff_vs_ring[endcap],"Fraction of Precision Hits");

    /** tube hits vs ring */
    m_trt_ec_hist->tubeHits_vs_ring[endcap] = MakeHist("tubeHits_vs_ring_trt_ec_"+endcapName[endcap],"tube Hits vs Ring for "+endcapName[endcap],40,0,40,"Endcap Ring","Tube Hits");
    RegisterHisto(al_mon,m_trt_ec_hist->tubeHits_vs_ring[endcap]);

    m_trt_ec_hist->tubeHits_eff_vs_ring[endcap] = MakeProfile("tubeHits_eff_vs_ring_trt_ec_"+endcapName[endcap],"Ratio of tubeHits to total measurements vs Ring for "+endcapName[endcap] ,40,0,40, 0., 1.,"Endcap Ring","Ratio of tubeHits to Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->tubeHits_eff_vs_ring[endcap],"Fraction of Tube Hits");

    /** Total measurements vs phiSector */
    m_trt_ec_hist->totHits_vs_phiSector[endcap] = MakeHist("totHits_vs_phiSector_trt_ec_"+endcapName[endcap],"Number of Total measurements vs PhiSector for "+endcapName[endcap],32,0,32,"Endcap PhiSector","Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->totHits_vs_phiSector[endcap]);

    /** outliers vs phiSector */
    m_trt_ec_hist->outliers_vs_phiSector[endcap] = MakeHist("outliers_vs_phiSector_trt_ec_"+endcapName[endcap],"Ratio of outliers to Total measurements vrs vs PhiSector for "+endcapName[endcap],32,0,32,"Endcap PhiSector","Outliers");
    RegisterHisto(al_mon,m_trt_ec_hist->outliers_vs_phiSector[endcap]);

    m_trt_ec_hist->outliers_eff_vs_phiSector[endcap] = MakeProfile("outliers_eff_vs_phiSector_trt_ec_"+endcapName[endcap],"Ratio of outliers to total measurements vs PhiSector for "+endcapName[endcap] ,32,0,32, 0., 1.,"Endcap PhiSector","Ratio of Outliers to Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->outliers_eff_vs_phiSector[endcap],"Fraction of Outliers");

    /** hits vs phiSector */
    m_trt_ec_hist->hits_vs_phiSector[endcap] = MakeHist("hits_vs_phiSector_trt_ec_"+endcapName[endcap],"total hits(non-outliers) vs PhiSector for "+endcapName[endcap],32,0,32,"Endcap PhiSector","Hits");
    RegisterHisto(al_mon,m_trt_ec_hist->hits_vs_phiSector[endcap]);

    m_trt_ec_hist->hits_eff_vs_phiSector[endcap] = MakeProfile("hits_eff_vs_phiSector_trt_ec_"+endcapName[endcap],"Ratio of hits to total measurements vs PhiSector for "+endcapName[endcap] ,32,0,32, 0., 1.,"Endcap PhiSector","Ratio of Hits to Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->hits_eff_vs_phiSector[endcap],"Fraction of Precision Hits");

    /** tube hits vs phiSector */
    m_trt_ec_hist->tubeHits_vs_phiSector[endcap] = MakeHist("tubeHits_vs_phiSector_trt_ec_"+endcapName[endcap],"tube Hits vs PhiSector for "+endcapName[endcap],32,0,32,"Endcap PhiSector","Tube Hits");
    RegisterHisto(al_mon,m_trt_ec_hist->tubeHits_vs_phiSector[endcap]);

    m_trt_ec_hist->tubeHits_eff_vs_phiSector[endcap] = MakeProfile("tubeHits_eff_vs_phiSector_trt_ec_"+endcapName[endcap],"Ratio of tubeHits to total measurements vs PhiSector for "+endcapName[endcap] ,32,0,32, 0., 1.,"Endcap PhiSector","Ratio of tubeHits to Total Measurements");
    RegisterHisto(al_mon,m_trt_ec_hist->tubeHits_eff_vs_phiSector[endcap],"Fraction of Tube");
  }

  return;
}

void IDAlignMonEfficiencies::fillTRTTotalMeasurements(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer){

  //Barrel
  if ( m_barrel_ec == 1 || m_barrel_ec == -1) {
    fillTRTBarrelTotalMeasurements(m_layer_or_wheel,m_phi_module,m_straw_layer);
  }

  //Endcap
  if ( m_barrel_ec == 2 || m_barrel_ec == -2) {
    fillTRTEndcapTotalMeasurements(m_barrel_ec,m_layer_or_wheel,m_phi_module,m_straw_layer);
  }

  return;
}

void IDAlignMonEfficiencies::fillTRTHits(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit){

  //Barrel
  if ( m_barrel_ec == 1 || m_barrel_ec == -1) {
    fillTRTBarrelHits(m_layer_or_wheel,m_phi_module,m_straw_layer,isTubeHit);
  }

  //Endcap
  if ( m_barrel_ec == 2 || m_barrel_ec == -2) {
    fillTRTEndcapHits(m_barrel_ec,m_layer_or_wheel,m_phi_module,m_straw_layer,isTubeHit);
  }

  return;
}

void IDAlignMonEfficiencies::fillTRTOutliers(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer){

  //Barrel
  if ( m_barrel_ec == 1 || m_barrel_ec == -1) {
    fillTRTBarrelOutliers(m_layer_or_wheel,m_phi_module,m_straw_layer);
  }

  //Endcap
  if ( m_barrel_ec == 2 || m_barrel_ec == -2) {
    fillTRTEndcapOutliers(m_barrel_ec,m_layer_or_wheel,m_straw_layer);
  }

  return;
}


void IDAlignMonEfficiencies::fillTRTBarrelTotalMeasurements(int m_layer_or_wheel,int m_phi_module,int m_straw_layer){

  //There are different number of straw layers in the differnt types of module layers
  // and the TRT_Id helper returns the layer with the current module (not global the layer)
  if(m_layer_or_wheel == 0)
    m_trt_b_hist->totHits_vs_StrawLay->Fill(m_straw_layer);
  if(m_layer_or_wheel == 1)
    m_trt_b_hist->totHits_vs_StrawLay->Fill(19+m_straw_layer);
  if(m_layer_or_wheel == 2)
    m_trt_b_hist->totHits_vs_StrawLay->Fill(19+24+m_straw_layer);

  for(int i=0; i<3; i++)
    if(m_layer_or_wheel == i)//Filling phi sectors of layer i
      m_trt_b_hist->totHits_vs_phiSector[i]->Fill(m_phi_module);

  return;
}

void IDAlignMonEfficiencies::fillTRTBarrelHits(int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit){

  //There are different number of straw layers in the differnt types of module layers
  // and the TRT_Id helper returns the layer with the current module (not global the layer)
  if(!isTubeHit){
    if(m_layer_or_wheel == 0)
      m_trt_b_hist->hits_vs_StrawLay->Fill(m_straw_layer);
    if(m_layer_or_wheel == 1)
      m_trt_b_hist->hits_vs_StrawLay->Fill(19+m_straw_layer);
    if(m_layer_or_wheel == 2)
      m_trt_b_hist->hits_vs_StrawLay->Fill(19+24+m_straw_layer);
  }

  if(isTubeHit){
    if(m_layer_or_wheel == 0)
      m_trt_b_hist->tubeHits_vs_StrawLay->Fill(m_straw_layer);
    if(m_layer_or_wheel == 1)
      m_trt_b_hist->tubeHits_vs_StrawLay->Fill(19+m_straw_layer);
    if(m_layer_or_wheel == 2)
      m_trt_b_hist->tubeHits_vs_StrawLay->Fill(19+24+m_straw_layer);
  }

  for(int i=0; i<3; i++){
    if(m_layer_or_wheel == i){//Filling phi sectors of layer i
      if(!isTubeHit)
	m_trt_b_hist->hits_vs_phiSector[i]->Fill(m_phi_module);
      if(isTubeHit)
	m_trt_b_hist->tubeHits_vs_phiSector[i]->Fill(m_phi_module);
    }
  }

  return;
}

void IDAlignMonEfficiencies::fillTRTBarrelOutliers(int m_layer_or_wheel,int m_phi_module,int m_straw_layer){

  //There are different number of straw layers in the differnt types of module layers
  // and the TRT_Id helper returns the layer with the current module (not global the layer)
  if(m_layer_or_wheel == 0)
    m_trt_b_hist->outliers_vs_StrawLay->Fill(m_straw_layer);
  if(m_layer_or_wheel == 1)
    m_trt_b_hist->outliers_vs_StrawLay->Fill(19+m_straw_layer);
  if(m_layer_or_wheel == 2)
    m_trt_b_hist->outliers_vs_StrawLay->Fill(19+24+m_straw_layer);

  for(int i=0; i<3; i++){
    if(m_layer_or_wheel == i)//Filling phi sectors of layer i
      m_trt_b_hist->outliers_vs_phiSector[i]->Fill(m_phi_module);
  }

  return;
}

void IDAlignMonEfficiencies::fillTRTEndcapTotalMeasurements(int m_barrel_ec, int m_layer_or_wheel,int m_phi_module,int m_straw_layer){

  for(unsigned int endcap=0; endcap<2; ++endcap){
    bool doFill = false;
    if(!endcap && m_barrel_ec == 2)
      doFill = true;
    else if(endcap && m_barrel_ec == -2)
      doFill = true;

    if(!doFill)
      continue;

    unsigned int ring = getRing(m_layer_or_wheel,m_straw_layer);
    m_trt_ec_hist->totHits_vs_ring[endcap]->Fill(ring);
    m_trt_ec_hist->totHits_vs_phiSector[endcap]->Fill(m_phi_module);
  }

}


void IDAlignMonEfficiencies::fillTRTEndcapHits(int m_barrel_ec, int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit){

  for(unsigned int endcap=0; endcap<2; ++endcap){
    bool doFill = false;
    if(!endcap && m_barrel_ec == 2)
      doFill = true;
    else if(endcap && m_barrel_ec == -2)
      doFill = true;

    if(!doFill)
      continue;

    unsigned int ring = getRing(m_layer_or_wheel,m_straw_layer);
    if(!isTubeHit){
      m_trt_ec_hist->hits_vs_ring[endcap]->Fill(ring);
      m_trt_ec_hist->hits_vs_phiSector[endcap]->Fill(m_phi_module);
    }
    if(isTubeHit){
      m_trt_ec_hist->tubeHits_vs_ring[endcap]->Fill(ring);
      m_trt_ec_hist->tubeHits_vs_phiSector[endcap]->Fill(m_phi_module);
    }
  }

  return;
}


void IDAlignMonEfficiencies::fillTRTEndcapOutliers(int m_barrel_ec, int m_layer_or_wheel,int m_straw_layer){

  for(unsigned int endcap=0; endcap<2; ++endcap){
    bool doFill = false;
    if(!endcap && m_barrel_ec == 2)
      doFill = true;
    else if(endcap && m_barrel_ec == -2)
      doFill = true;

    if(!doFill)
      continue;

    unsigned int ring = getRing(m_layer_or_wheel,m_straw_layer);
    m_trt_ec_hist->outliers_vs_ring[endcap]->Fill(ring);
    m_trt_ec_hist->outliers_vs_phiSector[endcap]->Fill(ring);
  }
  return;
}

unsigned int IDAlignMonEfficiencies::getRing(unsigned int wheel,unsigned int strawlayer){
  if (wheel < 6)
    return (16*wheel + strawlayer) / 4;
  else
    return (16*6+8*(wheel-6) + strawlayer) /4;
}
