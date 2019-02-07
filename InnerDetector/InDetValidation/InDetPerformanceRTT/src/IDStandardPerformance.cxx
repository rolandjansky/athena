/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDStandardPerformance.cxx
// AUTHORS: Beate Heinemann, Jean-Francois Arguin
// **********************************************************************

#include <sstream>
#include <string>
#include <cmath>
#include "TH1.h"
#include "TF1.h"
#include "TProfile.h"
#include "TMath.h"
#include "TFitResult.h"
#include "TROOT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"

#include "HepPDT/ParticleData.hh"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "HepPDT/ParticleDataTable.hh"

#include "IdDictDetDescr/IdDictManager.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TruthTrajectory.h"

#include "TrkToolInterfaces/IUpdator.h"

#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"

#include "GeneratorObjects/McEventCollection.h"
#include "JetEvent/JetCollection.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetPerformanceRTT/IDStandardPerformance.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

using std::auto_ptr;
using std::string;

#define STR(s) STR_EXP(s)
#define STR_EXP(s) #s

//Should be 100000
#define MAXBARCODE 100000

// *********************************************************************
// Public Methods
// *********************************************************************
IDStandardPerformance::IDStandardPerformance( const std::string & type, const std::string & name, const IInterface* parent )
             :ManagedMonitorToolBase( type, name, parent ),
	      m_Pi(3.1415926),
	      m_truthToTrack("Trk::TruthToTrack"),
	      m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),
	      m_tracksName("ExtendedTracks"),
	      m_tracksTruthName("ExtendedTracksTruthCollection"),
	      m_tracksDetailedTruthName("CombinedInDetTracksDetailedTruth"),
	      m_residualPullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
	      m_updatorHandle("Trk::KalmanUpdator/TrkKalmanUpdator"),
	      m_holeSearchTool("InDetHoleSearchTool"),
	      m_trackSelectorTool("InDet::InDetDetailedTrackSelectorTool"),
	      m_useTrackSelection(false),
	      m_histDirectoryName("AllTracks"),
	      m_trackPtBins(10),
	      m_minTrackPtEff(0.95),
	      m_maxTrackPtEff(100000.0),
	      m_minTrackPtRes(0.95),
              m_trackEtaBins(20),
	      m_maxTrackEta(2.5),
	      m_minTrackEta(0.0),
	      m_maxTrackPhi(999),
	      m_minTrackPhi(-999),
	      m_minProbEff(0.8),
	      m_minProbEffLow(0.5),
              m_truthParticleName("TruthEvent"),
	      m_truthJetCollName("Cone4TruthJets"),
	      m_doTrackInJet(true),
	      m_doUpgrade(false),
	      m_plotsVsAbsEta(false),
	      m_selHardScatter(false),
	      m_z0cutwrtPrimary(999.),
	      m_makeHitPlots(true),
              m_doTruth(true), //CB
              m_doHitBasedMatching(true),
	      m_PixeltracksName("ResolvedPixelTracks"),
	      m_SCTtracksName(std::string("ResolvedSCTTracks"))
{
  declareProperty("TruthToTrackTool",         m_truthToTrack);
  declareProperty("SummaryTool",              m_trkSummaryTool);
  declareProperty("tracksName",               m_tracksName);
  declareProperty("tracksTruthName",          m_tracksTruthName);
  declareProperty("tracksDetailedTruthName",  m_tracksDetailedTruthName);
  declareProperty("ResidualPullCalculatorTool", m_residualPullCalculator,
		  "Tool to calculate residuals and pulls");
  declareProperty("UpdatorTool",              m_updatorHandle,
		  "Measurement updator to calculate unbiased track states");
  declareProperty("HoleSearch",               m_holeSearchTool);
  declareProperty("useTrackSelection",        m_useTrackSelection);
  declareProperty("TrackSelectorTool",        m_trackSelectorTool);
  declareProperty("HistDirectoryName",        m_histDirectoryName,
		  "Directory where histograms are stored in root file");
  declareProperty("MinTrackPtEff",            m_minTrackPtEff,
		  "Minimum pT cut for efficiency plots vs eta");
  declareProperty("MaxTrackPtEff",            m_maxTrackPtEff,
                  "Maximum pT cut for efficiency plots vs eta");
  declareProperty("PtBins",                   m_trackPtBins,
                  "Number of Bins in Pt for the resiudual/pull plots"); 
  declareProperty("EtaBins",                  m_trackEtaBins,
                  "Number of Bins in Eta for the residual/pull plots");  
  declareProperty("MinTrackPtRes",            m_minTrackPtRes,
		  "Minimum pT cut for residual plots");
  declareProperty("MaxTrackEta",              m_maxTrackEta,
                  "Maximum Track Eta");
  declareProperty("MinTrackEta",              m_minTrackEta,
                  "Minimum Track Eta");
  declareProperty("MaxTrackPhi",              m_maxTrackPhi,
                  "Maximum Track Phi");
  declareProperty("MinTrackPhi",              m_minTrackPhi,
                  "Minimum Track Phi");
  declareProperty("minProbEff",               m_minProbEff,
		  "Probability cut for track efficiency and fake rate");
  declareProperty("minProbEffLow",            m_minProbEffLow,
                  "Probability lower cut for track efficiency and fake rate");
  declareProperty("TruthParticleContainerName", m_truthParticleName="TruthEvent"); 
  declareProperty("TruthJetCollName",         m_truthJetCollName,
                  "Truth Jet Collection");
  declareProperty("doTrackInJet",             m_doTrackInJet);
  declareProperty("doUpgrade",                m_doUpgrade);
  declareProperty("plotsVsAbsEta",            m_plotsVsAbsEta);
  declareProperty("selectHardScatter",        m_selHardScatter);
  declareProperty("z0CutwrtPrimary",          m_z0cutwrtPrimary);
  declareProperty("makeHitPlots",             m_makeHitPlots);
  declareProperty("DoTruth",                  m_doTruth); //CB
  declareProperty("doHitBasedMatching",       m_doHitBasedMatching);
  declareProperty("PixeltracksName",          m_PixeltracksName);
  declareProperty("SCTtracksName",            m_SCTtracksName);
  m_UpdatorWarning = false;
  m_isUnbiased = 0;
}


IDStandardPerformance::~IDStandardPerformance() { }

StatusCode IDStandardPerformance::initialize()
{
  //MsgStream log( msgSvc(), name() );
  StatusCode sc = StatusCode :: SUCCESS;

  // init truthToTrack
  if (m_doTruth) { //CB
    if (m_truthToTrack.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthToTrack << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_truthToTrack << endmsg;
    }
  }

  // hole search tool
  if ( m_holeSearchTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endmsg;
  }

  //DetectorStore Service
  //sc=service("DetectorStore",m_detStore);
  //if (sc.isFailure()) {
  //  msg(MSG::ERROR) <<"Could not find DetectorStore"<<endmsg;
  //  return sc;
  //}

  //if (!m_detStore) msg(MSG::ERROR)<<"DetectorStore pointer is zero!"<<endmsg;

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  sc =  service("PartPropSvc", partPropSvc, true);

  if (sc.isFailure()) {
    msg(MSG::FATAL) << " Could not initialize Particle Properties Service"
    << endmsg;
    return StatusCode::FAILURE;
  }

  m_particleDataTable = partPropSvc->PDT();

  //ID Helper
  m_idHelper = new AtlasDetectorID;

  // Get the dictionary manager from the detector store
  sc = detStore()->retrieve(m_idHelper, "AtlasID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get ID helper !" << endmsg;
    return sc;
  }

  //pixel and SCT ID helper
  sc = detStore()->retrieve(m_PIX_Mgr, "Pixel");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel Detector ID manager!" << endmsg;
    return StatusCode::FAILURE;
  }
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get Pixel Detector ID helper!" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_SCT_Mgr, "SCT");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT Detector ID manager!" << endmsg;
    return StatusCode::FAILURE;
  }
  sc = detStore()->retrieve(m_sctID, "SCT_ID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get SCT Detector ID helper!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_doUpgrade) {
    sc = detStore()->retrieve(m_trtID, "TRT_ID");
    if(sc.isFailure()) {
      msg(MSG::FATAL) << "Could not get TRT Detector ID helper!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // ----------------------------------
  // use updator to get unbiased states
  if ( ! m_updatorHandle.empty() ) {
    if (m_updatorHandle.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not retrieve measurement updator tool: "
      << m_updatorHandle << endmsg;
      return StatusCode::FAILURE;
    }
    //m_updator = &(*m_updatorHandle);
  } else {
    if (msgLvl(MSG::DEBUG)) msg() <<
      "No Updator for unbiased track states given, use normal states!"<<endmsg;
    //m_updator = 0;
  }

  // ----------------------------------
  //get residual and pull calculator
  if (m_residualPullCalculator.empty()) {
    msg(MSG::INFO) <<
      "No residual/pull calculator for general hit residuals configured."
    << endmsg;
    msg(MSG::INFO) <<
      "It is recommended to give R/P calculators to the det-specific tool"
    << " handle lists then." << endmsg;
  } else if (m_residualPullCalculator.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not retrieve "<< m_residualPullCalculator
    <<" (to calculate residuals and pulls) "<< endmsg;

  } else {
    msg(MSG::INFO)
    << "Generic hit residuals&pulls will be calculated in one or both "
    << "available local coordinates" << endmsg;
  }

  if (m_trkSummaryTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool "
      << m_trkSummaryTool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_trkSummaryTool << endmsg;
  }

  if (m_useTrackSelection){
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSelectorTool << endmsg;
      return StatusCode::FAILURE;
    } else {
      if (msgLvl(MSG::DEBUG)) msg() << "Retrieved tool " << m_trackSelectorTool << endmsg;
    }
  }

  // Make sure is a multiple of 2 in case of abs plots : m_maxTrackEta
  if (m_plotsVsAbsEta && m_trackEtaBins%2){
    msg(MSG::WARNING) << "Plots chosen to be vs. abs(eta), but odd number of bins given, resseting to " << ++m_trackEtaBins << endmsg;
  }

  // Read Handle Key
  ATH_CHECK(m_SCTtracksName.initialize(m_doHitBasedMatching));
  ATH_CHECK( m_TRTtracksKey.initialize(m_doHitBasedMatching) );
  ATH_CHECK( m_evt.initialize() );

  sc = ManagedMonitorToolBase::initialize();
  return sc;
}

TH1F*
IDStandardPerformance::create_registeredTH1F(MonGroup &mon
                                             , const char* name
                                             , const char* title
                                             , int nBins
                                             , double min
                                             , double max)
{
  TH1F* hist = new TH1F(name, title, nBins, min, max);
  hist->Sumw2();
  StatusCode sc = mon.regHist(hist);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram: " << name << endmsg;
  }
  return hist;
}

TH2F*
IDStandardPerformance::create_registeredTH2F(MonGroup &mon
                                             , const char* name
                                             , const char* title
                                             , int nBinsX
                                             , double minX
                                             , double maxX
                                             , int nBinsY
                                             , double minY
                                             , double maxY)
{
  TH2F* hist = new TH2F(name, title, nBinsX, minX, maxX, nBinsY, minY, maxY);
  hist->Sumw2();
  StatusCode sc = mon.regHist(hist);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram: " << name << endmsg;
  }
  return hist;
}


TProfile*
IDStandardPerformance::create_registeredTProfile(MonGroup &mon
                                                 , const char* name
                                                 , const char* title
                                                 , int nBins
                                                 , double min
                                                 , double max
                                                 , double ylow
                                                 , double yup)
{
  TProfile* hist = new TProfile(name, title, nBins, min, max, ylow, yup);
  StatusCode sc = mon.regHist(hist);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram: " << name << endmsg;
  }
  return hist;
}

TProfile*
IDStandardPerformance::create_registeredTProfile(MonGroup &mon
                                                 , const char* name
                                                 , const char* title
                                                 , int nBins
                                                 , double min
                                                 , double max)
{
  TProfile* hist = new TProfile(name, title, nBins, min, max);
  StatusCode sc = mon.regHist(hist);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram: " << name << endmsg;
  }
  return hist;
}


StatusCode IDStandardPerformance::bookHistograms()
{
  //MsgStream log( msgSvc(), name() );

  //if (msgLvl(MSG::VERBOSE)) msg() << "isNewEventsBlock = " << isNewEventsBlock << ", isNewLumiBlock = " << isNewLumiBlock << ", isNewRun = " << isNewRun << endmsg;

  std::string outputDirName = "IDPerformanceMon/" + m_tracksName + "/" + m_histDirectoryName;
  //  std::string outputDirName =  "/" + m_tracksName + "/" + m_histDirectoryName;
  msg(MSG::INFO) << "Histograms are stored in directory " << outputDirName<<endmsg;
  MonGroup al_expert(this, outputDirName, run);
  MonGroup al_shift(this, outputDirName, run);
  MonGroup pull_parent(this, outputDirName+"/residualPulls", run);
  MonGroup pull_shift(this, outputDirName+"/residualPulls/raw", run);
  MonGroup al_debug(this, outputDirName, run);

  if ( newRunFlag() ) {
    m_ngenevent = create_registeredTH1F(al_expert,"ngenevent","Number of generated MC events; nevent",1000,0,1000);
    m_ntrack = create_registeredTH1F(al_expert, "ntrack","Number of Tracks;Num. tracks",200,0,200);
    m_mu = create_registeredTH1F(al_expert, "mu","mu; mu",50,0,50);
    m_nparticle = create_registeredTH1F(al_expert, "nparticle","Number of Truth Particles;Num. truth particles",200,0,2000);
    m_ntracksel = create_registeredTH1F(al_expert, "ntracksel","Number of Selected Tracks;Num. tracks",200,0,200);
    m_nparticlesel = create_registeredTH1F(al_expert, "nparticlesel","Number of Selected Truth Particles;Num. truth particles",200,0,200);
    m_nparticleprimary = create_registeredTH1F(al_expert, "nparticleprimary","Number of Selected Truth Particles (prim);Num. prim truth particles",200,0,200);
    m_nbarcode0 = create_registeredTH1F(al_expert,"nbcode0","Number of matched tracks with barcode 0; nbarcode0",1000,0,5000);
    m_nbarcode0match = create_registeredTH1F(al_expert,"nbcode0match","Number of matched tracks with barcode 0 with >50% prob; nbarc0m",1000,0,5000);
    m_nbarcode100k = create_registeredTH1F(al_expert,"nbcode100k","Number of matched tracks with barcode>" STR(MAXBARCODE) "; nbc100k",1000,0,5000);
    m_nbarcodegood = create_registeredTH1F(al_expert,"nbcodegood","Number of matched tracks with 0<barcode<" STR(MAXBARCODE) "; nbcnot100k",1000,0,5000);
    m_nbadmatch = create_registeredTH1F(al_expert,"nbadmatch","Number of bad match or barcode=0 tracks; nbadmatch",1000,0,5000);
    m_nlowprob = create_registeredTH1F(al_expert,"nlowprob","Number of bad match for barcode!=0 tracks; nbcnot0antlowprob",1000,0,5000);

    // truth track parameters
    m_truthpT = create_registeredTH1F(al_expert, "truthpT","p_{T} of selected truth tracks (in GeV);p_{T}(GeV/c)",200,0,200);
    m_truthpTlow = create_registeredTH1F(al_expert, "truthpTlow","p_{T} of selected truth tracks (in GeV);p_{T}(GeV/c)",200,0,20);
    m_truthphi = create_registeredTH1F(al_expert, "truthphi","#phi of selected truth tracks;#phi",100,0,2*m_Pi);
    m_trutheta = create_registeredTH1F(al_expert, "trutheta","#eta of selected truth tracks;#eta",100,-5,5);
    m_truthd0 = create_registeredTH1F(al_expert, "truthd0","d_{0} of selected truth tracks (in mm);d_{0}(mm)",200,-5,5);
    m_truthz0 = create_registeredTH1F(al_expert, "truthz0","z_{0} of selected truth tracks (in mm);z_{0}(mm)",120,-300,300);
    m_truthd0c = create_registeredTH1F(al_expert, "truthd0s","Corrected d_{0} of selected truth tracks (in mm);d_{0}(mm)",200,-5,5);
    m_truthcharge = create_registeredTH1F(al_expert, "truthcharge","charge of selected truth tracks (in mm);charge",5,-2.5,2.5);

    m_truthvtx_R = create_registeredTH1F(al_expert, "truthvtx_R","Radius of interaction point (mm);R^{int}_{xy}(mm)",100,-5,5);
    m_truthvtx_z = create_registeredTH1F(al_expert, "truthvtx_z","Z of interaction point (mm);R^{int}_{z}(mm)",120,-300,300);

    // Nuclear int
    m_nuclearint_in_E = create_registeredTH1F(al_expert, "nuclearint_in_E","Nucl. Int.: E incoming",100,0,100.0);
    m_nuclearint_in_pt = create_registeredTH1F(al_expert, "nuclearint_in_pt","Nucl. Int.: p_{T} incoming",100,0,100.0);
    m_nuclearint_in_phi = create_registeredTH1F(al_expert, "nuclearint_in_phi","Nucl. Int.: #phi incoming",100,0,2*m_Pi);
    m_nuclearint_in_eta = create_registeredTH1F(al_expert, "nuclearint_in_eta","Nucl. Int.: #eta incoming",100,-5,5);
    m_nuclearint_in_pdgcode = create_registeredTH1F(al_expert, "nuclearint_in_pdgcode","Nucl. Int.: PDG code incoming",10000,-5000,5000);
    m_nuclearint_in_endvtx_r = create_registeredTH1F(al_expert, "nuclearint_in_endvtx_r","Nucl. Int.: Radius of interaction point (mm)",100,0,1000);
    m_nuclearint_in_endvtx_z = create_registeredTH1F(al_expert, "nuclearint_in_endvtx_z","Nucl. Int.: Z of interaction point (mm)",125,-2500,2500);
    m_nuclearint_n_out = create_registeredTH1F(al_expert, "nuclearint_n_out","Nucl. Int.: Num. of Outgoing Part.",30,0,30.0);
    m_nuclearint_totalEout = create_registeredTH1F(al_expert, "nuclearint_totalEout","Nucl. Int.: Sum E Outgoing Part.",100,0,100.0);
    m_nuclearint_out_E = create_registeredTH1F(al_expert, "nuclearint_out_E","Nucl. Int.: E outgoing",100,0,100.0);
    m_nuclearint_out_pt = create_registeredTH1F(al_expert, "nuclearint_out_pt","Nucl. Int.: p_{T} outgoing",100,0,100.0);
    m_nuclearint_out_dphi = create_registeredTH1F(al_expert, "nuclearint_out_dphi","Nucl. Int.: #Delta#phi outgoing",100,0,2*m_Pi);
    m_nuclearint_out_deta = create_registeredTH1F(al_expert, "nuclearint_out_deta","Nucl. Int.: #Delta#eta outgoing",100,-3,3);
    m_nuclearint_out_pdgcode = create_registeredTH1F(al_expert, "nuclearint_out_pdgcode","Nucl. Int.: PDG code outgoing",10000,-5000,5000);

    // reconstructed track parameters
    m_recpT = create_registeredTH1F(al_expert, "recpT","p_{T} of selected rec tracks (in GeV);p_{T}(GeV/c)",200,0,200);
    m_recpTlow = create_registeredTH1F(al_expert, "recpTlow","p_{T} of selected rec tracks (in GeV);p_{T}(GeV/c)",200,0,20);
    m_recphi = create_registeredTH1F(al_expert, "recphi","#phi of selected rec tracks;#phi",100,0,2*m_Pi);
    m_receta = create_registeredTH1F(al_expert, "receta","#eta of selected rec tracks;#eta",100,-5,5);
    m_recd0 = create_registeredTH1F(al_expert, "recd0","d_{0} of selected rec tracks (in mm);d_{0}(mm)",200,-5,5);
    m_recz0 = create_registeredTH1F(al_expert, "recz0","z_{0} of selected rec tracks (in mm);z_{0}(mm)",120,-300,300);
    m_recd0c = create_registeredTH1F(al_expert, "recd0c","Corrected d_{0} of selected rec tracks (in mm);d_{0}(mm)",200,-5,5);
    m_recchi2 = create_registeredTH1F(al_expert, "recchi2","Chi2 of selected rec tracks (in mm);chi2",200,0,10);
    m_recdeltaz0 = create_registeredTH1F(al_expert, "recdeltaz0","Delta z_{0}sin(#theta) of selected rec tracks (in mm);(z_{0}-z_{vtx})sin(#theta)(mm)",200,-10,10);
    m_reccharge = create_registeredTH1F(al_expert, "reccharge","Delta z_{0} of selected rec tracks (in mm);charge",5,-2.5,2.5);

    // parameters for fake tracks
    m_fakepT = create_registeredTH1F(al_expert, "fakepT","p_{T} of selected fake tracks (in GeV);p_{T}(GeV/c)",200,0,200);
    m_fakepTlow = create_registeredTH1F(al_expert, "fakepTlow","p_{T} of selected fake tracks (in GeV);p_{T}(GeV/c)",200,0,20);
    m_fakephi = create_registeredTH1F(al_expert, "fakephi","#phi of selected fake tracks;#phi",100,0,2*m_Pi);
    m_fakeeta = create_registeredTH1F(al_expert, "fakeeta","#eta of selected fake tracks;#eta",100,-5,5);
    m_faked0 = create_registeredTH1F(al_expert, "faked0","d_{0} of selected fake tracks (in mm);d_{0}(mm)",200,-5,5);
    m_fakez0 = create_registeredTH1F(al_expert, "fakez0","z_{0} of selected fake tracks (in mm);z_{0}(mm)",120,-300,300);
    m_faked0c = create_registeredTH1F(al_expert, "faked0c","Corfaketed d_{0} of selected fake tracks (in mm);d_{0}(mm)",200,-5,5);
    m_fakedeltaz0 = create_registeredTH1F(al_expert, "fakedeltaz0","Delta z_{0}sin(#theta) of selected fake tracks (in mm);(z_{0}-z_{vtx})sin(#theta)(mm)",200,-10,10);

    

    // reserve the eta bins 
    m_hd0.reserve(m_trackEtaBins);
    m_hz0.reserve(m_trackEtaBins);
    m_hphi.reserve(m_trackEtaBins);
    m_htheta.reserve(m_trackEtaBins);
    m_hz0st.reserve(m_trackEtaBins);
    m_hqopt.reserve(m_trackEtaBins);
    m_pulld0.reserve(m_trackEtaBins);
    m_pullz0.reserve(m_trackEtaBins);
    m_pullphi.reserve(m_trackEtaBins);
    m_pulltheta.reserve(m_trackEtaBins);
    m_pullz0st.reserve(m_trackEtaBins);
    m_pullqopt.reserve(m_trackEtaBins);
    constexpr unsigned int bufferLength{200};
    char name[bufferLength];
    // base histograms for resolution plots versus eta and pt
    for (int i=0;i<m_trackEtaBins;i++) {
      // residuals 
      
      snprintf(name, bufferLength,"d0res_bin%i",i);
      m_hd0.push_back(create_registeredTH1F(al_debug, name,name,2000,-1.0,1.0));

      snprintf(name, bufferLength,"z0res_bin%i",i);
      m_hz0.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      snprintf(name, bufferLength,"phires_bin%i",i);
      m_hphi.push_back(create_registeredTH1F(al_debug, name,name,3000,-0.03,0.03));

      snprintf(name, bufferLength,"thetares_bin%i",i);
      m_htheta.push_back(create_registeredTH1F(al_debug, name,name,2000,-0.1,0.1));

      snprintf(name, bufferLength,"z0stres_bin%i",i);
      m_hz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      snprintf(name, bufferLength,"qoptres_bin%i",i);
      m_hqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-0.4,0.4));

      // pull distributions
      snprintf(name, bufferLength,"d0pullres_bin%i",i);
      m_pulld0.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      
      snprintf(name, bufferLength,"z0pullres_bin%i",i);
      m_pullz0.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      snprintf(name, bufferLength,"phipullres_bin%i",i);
      m_pullphi.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      snprintf(name, bufferLength,"thetapullres_bin%i",i);
      m_pulltheta.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      snprintf(name, bufferLength,"z0stpullres_bin%i",i);
      m_pullz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
      snprintf(name, bufferLength,"qoptpullres_bin%i",i);
      m_pullqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-5.,5.));
   }



    // reserve the pt bins
    m_hptd0.reserve(m_trackPtBins);
    m_hptz0.reserve(m_trackPtBins);
    m_hptphi.reserve(m_trackPtBins);
    m_hpttheta.reserve(m_trackPtBins);
    m_hptz0st.reserve(m_trackPtBins);
    m_hptqopt.reserve(m_trackPtBins);
    // resolutions versus pt
    for (int i=0;i<m_trackPtBins;i++) {
      
      snprintf(name, bufferLength,"d0res_ptbin%i",i);
      m_hptd0.push_back(create_registeredTH1F(al_debug, name,name,2000,-1.0,1.0));

      snprintf(name, bufferLength,"z0res_ptbin%i",i);
      m_hptz0.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      snprintf(name, bufferLength,"phires_ptbin%i",i);
      m_hptphi.push_back(create_registeredTH1F(al_debug, name,name,3000,-0.03,0.03));

      snprintf(name, bufferLength,"thetares_ptbin%i",i);
      m_hpttheta.push_back(create_registeredTH1F(al_debug, name,name,2000,-0.1,0.1));

      snprintf(name, bufferLength,"z0stres_ptbin%i",i);
      m_hptz0st.push_back(create_registeredTH1F(al_debug, name,name,200,-1.5,1.5));

      snprintf(name, bufferLength,"qoptres_ptbin%i",i);
      m_hptqopt.push_back(create_registeredTH1F(al_debug, name,name,200,-0.4,0.4));
    }

    // 1D pull plots
    m_pulld0all=new TH1F("pulld0all","Pull Distribution of d0;(d^{rec}_{0}-d^{tru}_{0})/#sigma_{d_{0}}",200,-5,5);
    m_pullz0all=new TH1F("pullz0all","Pull Distribution of z0;(z^{rec}_{0}-z^{tru}_{0})/#sigma_{z_{0}}",200,-5,5);
    m_pullphiall=new TH1F("pullphiall","Pull Distribution of #phi;(#phi^{rec}-#phi^{tru})/#sigma_{#phi}",200,-5,5);
    m_pullthetaall=new TH1F("pullthetaall","Pull Distribution of #theta;(#theta^{rec}-#theta^{tru})/#sigma_{#theta}",200,-5,5);
    m_pullqoptall=new TH1F("pullqoptall","Pull Distribution of q/pt;(q/p_{T}^{rec}-q/p_{T}^{tru})/#sigma_{q/p_{T}}",200,-5,5);
    RegisterHisto(al_shift,m_pulld0all);
    RegisterHisto(al_shift,m_pullz0all);
    RegisterHisto(al_shift,m_pullphiall);
    RegisterHisto(al_shift,m_pullthetaall);
    RegisterHisto(al_shift,m_pullqoptall);
    m_pulld0all_widerange=new TH1F("pulld0all_widerange","Pull Distribution of d0;(d^{rec}_{0}-d^{tru}_{0})/#sigma_{d_{0}}",200,-20,20);
    m_pullz0all_widerange=new TH1F("pullz0all_widerange","Pull Distribution of z0;(z^{rec}_{0}-z^{tru}_{0})/#sigma_{z_{0}}",200,-20,20);
    m_pullphiall_widerange=new TH1F("pullphiall_widerange","Pull Distribution of #phi;(#phi^{rec}-#phi^{tru})/#sigma_{#phi}",200,-20,20);
    m_pullthetaall_widerange=new TH1F("pullthetaall_widerange","Pull Distribution of #theta;(#theta^{rec}-#theta^{tru})/#sigma_{#theta}",200,-20,20);
    m_pullqoptall_widerange=new TH1F("pullqoptall_widerange","Pull Distribution of q/pt;(q/p_{T}^{rec}-q/p_{T}^{tru})/#sigma_{q/p_{T}}",200,-20,20);
    RegisterHisto(al_shift,m_pulld0all_widerange);
    RegisterHisto(al_shift,m_pullz0all_widerange);
    RegisterHisto(al_shift,m_pullphiall_widerange);
    RegisterHisto(al_shift,m_pullthetaall_widerange);
    RegisterHisto(al_shift,m_pullqoptall_widerange);

    int nbins_eta = m_trackEtaBins;
    float min_eta = -m_maxTrackEta;
    float max_eta = +m_maxTrackEta;
    if (m_plotsVsAbsEta){
      nbins_eta = m_trackEtaBins/2;
      min_eta = 0;
    }

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions amnd means versus eta
    // ------------------------------------------------------------------
    m_d0_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_d0","Track Resolution: d_{0} versus #eta;#eta;#sigma(d^{rec}_{0}-d^{tru}_{0})",nbins_eta,min_eta,max_eta);
    m_phi_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_phi","Track Resolution: #phi versus #eta;#eta;#sigma(#phi^{rec}-#phi^{tru})",nbins_eta,min_eta,max_eta);
    m_qopt_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_qopt","Track Resolution: 1/p_{T} versus #eta;#eta;#sigma(q/p_{T}^{rec}-q/p_{T}^{tru})",nbins_eta,min_eta,max_eta);
    m_z0_res_vs_eta= create_registeredTH1F(al_shift, "res_vs_eta_z0","Track Resolution: z_{0} versus #eta;#eta;#sigma(z^{rec}_{0}-z^{tru}_{0})",nbins_eta,min_eta,max_eta);
    m_theta_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_theta","Track Resolution: #theta versus #eta;#eta;#sigma(#theta^{rec}-#theta^{tru})",nbins_eta,min_eta,max_eta);
    m_z0st_res_vs_eta = create_registeredTH1F(al_shift, "res_vs_eta_z0st","Track Resolution: z_{0}*sin(#theta) versus #eta;#eta;#sigma(z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru})",nbins_eta,min_eta,max_eta);

    m_d0_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_d0","Track Measurement Bias: d_{0} versus #eta;#eta;d^{rec}_{0}-d^{tru}_{0}",nbins_eta,min_eta,max_eta);
    m_phi_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_phi","Track Measurement Bias: #phi versus #eta;#eta;#phi^{rec}-#phi^{tru}",nbins_eta,min_eta,max_eta);
    m_qopt_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_qopt","Track Measurement Bias: 1/p_{T} versus #eta;#eta;q/p_{T}^{rec}-q/p_{T}^{tru}",nbins_eta,min_eta,max_eta);
    m_z0_mean_vs_eta= create_registeredTH1F(al_shift, "mean_vs_eta_z0","Track Measurement Bias: z_{0} versus #eta;#eta;z^{rec}_{0}-z^{tru}_{0}",nbins_eta,min_eta,max_eta);
    m_theta_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_theta","Track Measurement Bias: #theta versus #eta;#eta;#theta^{rec}-#theta^{tru}",nbins_eta,min_eta,max_eta);
    m_z0st_mean_vs_eta = create_registeredTH1F(al_shift, "mean_vs_eta_z0st","Track Measurement Bias: z_{0}*sin(#theta) versus #eta;#eta;z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru}",nbins_eta,min_eta,max_eta);

    m_d0_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_d0","Track Pull Width: d_{0} versus #eta;#eta;#sigma(pull(d_{0}))",nbins_eta,min_eta,max_eta);
    m_phi_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_phi","Track Pull Width: #phi versus #eta;#eta;#sigma(pull(#phi))",nbins_eta,min_eta,max_eta);
    m_qopt_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_qopt","Track Pull Width: 1/p_{T} versus #eta;#eta;#sigma(pull(q/p_{T}))",nbins_eta,min_eta,max_eta);
    m_z0_pullres_vs_eta= create_registeredTH1F(al_shift, "pullres_vs_eta_z0","Track Pull Width: z_{0} versus #eta;#eta;#sigma(pull(z_{0}))",nbins_eta,min_eta,max_eta);
    m_theta_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_theta","Track Pull Width: #theta versus #eta;#eta;#sigma(pull(#theta))",nbins_eta,min_eta,max_eta);
    m_z0st_pullres_vs_eta = create_registeredTH1F(al_shift, "pullres_vs_eta_z0st","Track Pull Width: z_{0}*sin(#theta) versus #eta;#eta;#sigma(pull(z_{0}sin(#theta)))",nbins_eta,min_eta,max_eta);

    m_d0_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_d0","Track Pull Mean: d_{0} versus #eta;#eta;pull(d_{0})",nbins_eta,min_eta,max_eta);
    m_phi_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_phi","Track Pull Mean: #phi versus #eta;#eta;pull(#phi)",nbins_eta,min_eta,max_eta);
    m_qopt_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_qopt","Track Pull Mean: 1/p_{T} versus #eta;#eta;pull(q/p_{T})",nbins_eta,min_eta,max_eta);
    m_z0_pullmean_vs_eta= create_registeredTH1F(al_shift, "pullmean_vs_eta_z0","Track Pull Mean: z_{0} versus #eta;#eta;pull(z_{0})",nbins_eta,min_eta,max_eta);
    m_theta_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_theta","Track Pull Mean: #theta versus #eta;#eta;pull(#theta)",nbins_eta,min_eta,max_eta);
    m_z0st_pullmean_vs_eta = create_registeredTH1F(al_shift, "pullmean_vs_eta_z0st","Track Pull Mean: z_{0}*sin(#theta) versus #eta;#eta;pull(z_{0}sin(#theta))",nbins_eta,min_eta,max_eta);

    // ------------------------------------------------------------------
    // summary plots of track parameter resolutions versus pt
    // ------------------------------------------------------------------
    m_d0_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_d0","Track Resolution: d_{0} versus log10(p_{T});p_{T}(GeV/c);#sigma(d^{rec}_{0}-d^{tru}_{0})",m_trackPtBins,-0.5,2);
    m_phi_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_phi","Track Resolution: #phi versus log10(p_{T});p_{T}(GeV/c);#sigma(#phi^{rec}-#phi^{tru})",m_trackPtBins,-0.5,2);
    m_qopt_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_qopt","Track Resolution: p_{T} versus log10(p_{T});p_{T}(GeV/c);#sigma(q/p_{T}^{rec}-q/p_{T}^{tru})",m_trackPtBins,-0.5,2);
    m_z0_res_vs_pt= create_registeredTH1F(al_shift, "res_vs_pt_z0","Track Resolution: z_{0} versus log10(p_{T});p_{T}(GeV/c);#sigma(z^{rec}_{0}-z^{tru}_{0})",m_trackPtBins,-0.5,2);
    m_theta_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_theta","Track Resolution: #theta versus log10(p_{T});p_{T}(GeV/c);#sigma(#theta^{rec}-#theta^{tru})",m_trackPtBins,-0.5,2);
    m_z0st_res_vs_pt = create_registeredTH1F(al_shift, "res_vs_pt_z0st","Track Resolution: z_{0}*sin(#theta) versus log10(p_{T});p_{T}(GeV/c);#sigma(z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru})",m_trackPtBins,-0.5,2);

    // --------------------------------------------
    // tracking efficiency vs eta for all tracks
    //---------------------------------------------
    m_trackeff_vs_eta = create_registeredTProfile(al_shift, "trackeff_vs_eta","track efficiency tracks vs #eta (Det. Paper def.);#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_vs_phi = create_registeredTProfile(al_shift, "trackeff_vs_phi","track efficiency tracks vs #phi (Det. Paper def.);#phi;Efficiency",24,-180,180);

    // tracking efficiency versus pt for three eta regions
    m_trackeff_vs_pt = new TProfile("trackeff_vs_pt","track efficiency vs p_{T} for |#eta|<2.5 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_vs_pt);
    m_trackeff_vs_pt_lowpt = new TProfile("trackeff_vs_pt_lowpt","track efficiency vs p_{T} for |#eta|<2.5 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,5.0);
    RegisterHisto(al_shift,m_trackeff_vs_pt_lowpt);
    m_trackeff_vs_pt_loweta = new TProfile("trackeff_vs_pt_loweta","track efficiency vs p_{T} for |#eta|<0.8 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_vs_pt_loweta);
    m_trackeff_vs_pt_medeta = new TProfile("trackeff_vs_pt_medeta","track efficiency vs p_{T} for 0.8<|#eta|<1.5 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_vs_pt_medeta);
    m_trackeff_vs_pt_higheta = new TProfile("trackeff_vs_pt_higheta","track efficiency vs p_{T} for 1.5<|#eta|<2.5 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_vs_pt_higheta);

    // track efficiency vs log pt
    m_trackeff_vs_logpt = new TProfile("trackeff_vs_logpt","track efficiency vs p_{T} for |#eta|<2.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_vs_logpt);
    m_trackeff_vs_logpt_loweta = new TProfile("trackeff_vs_logpt_loweta","track efficiency vs p_{T} for |#eta|<0.8 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_vs_logpt_loweta);
    m_trackeff_vs_logpt_medeta = new TProfile("trackeff_vs_logpt_medeta","track efficiency vs p_{T} for 0.8<|#eta|<1.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_vs_logpt_medeta);
    m_trackeff_vs_logpt_higheta = new TProfile("trackeff_vs_logpt_higheta","track efficiency vs p_{T} for 1.5<|#eta|<2.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_vs_logpt_higheta);

    // Tracking efficiency in jets
    m_trackinjeteff_vs_eta = create_registeredTProfile(al_shift, "trackinjeteff_vs_eta","track in jets efficiency tracks vs #eta (Det. Paper def.);#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackinjeteff_vs_phi = create_registeredTProfile(al_shift, "trackinjeteff_vs_phi","track in jets efficiency vs #phi (Det. Paper def.);#phi;Efficiency",24,-180,180);
    m_trackinjeteff_vs_pt = create_registeredTProfile(al_shift, "trackinjeteff_vs_pt","track in jets efficiency vs p_{T} for |#eta|<2.5 (Det. Paper def.);p_{T}(GeV/c);Efficiency",25,0,50.0);
    m_trackinjeteff_vs_dr = create_registeredTProfile(al_shift, "trackinjeteff_vs_dr","track in jets efficiency vs #DeltaR for |#eta|<2.5 (Det. Paper def.);#DeltaR;Efficiency",10,0,0.4);
    m_trackinjeteff_vs_dr_lt_j50 = create_registeredTProfile(al_shift, "trackinjeteff_vs_dr_lt_j50","track in jets efficiency vs #DeltaR for |#eta|<2.5 (E_{T}(jet) < 50 GeV);#DeltaR;Efficiency",10,0,0.4);
    m_trackinjeteff_vs_dr_gr_j100 = create_registeredTProfile(al_shift, "trackinjeteff_vs_dr_gr_j100","track in jets efficiency vs #DeltaR for |#eta|<2.5 (E_{T}(jet) > 100 GeV);#DeltaR;Efficiency",10,0,0.4);
    m_trackinjeteff_vs_jetet = create_registeredTProfile(al_shift, "trackinjeteff_vs_jetet","track in jets efficiency vs jet E_{T} for |#eta|<2.5 (Det. Paper def.);E_{jet}(GeV);Efficiency",10,0,250.0);


    m_trackeff_vs_eta_vlowpt = create_registeredTProfile(al_shift, "trackeff_vs_eta_vlowpt","track efficiency vs #eta for 0.1<p_{T}<0.5 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_vs_eta_lowpt = create_registeredTProfile(al_shift, "trackeff_vs_eta_lowpt","track efficiency vs #eta for 0.5<p_{T}<1.0 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    // ------------------------------------------------
    // tracking efficiency vs eta for primary tracks
    // ------------------------------------------------
    m_trackeff_primary_vs_eta = create_registeredTProfile(al_shift, "trackeff_primary_vs_eta","track efficiency for primary tracks vs #eta;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_primary_vs_phi = create_registeredTProfile(al_shift, "trackeff_primary_vs_phi","track efficiency for primary tracks vs #phi;#phi;Efficiency",24,-180,180);
    // tracking efficiency versus pt for three eta regions
    m_trackeff_primary_vs_pt = new TProfile("trackeff_primary_vs_pt","track efficiency for primary tracks vs p_{T} for |#eta|<2.5;p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_primary_vs_pt);
    m_trackeff_primary_vs_pt_lowpt = new TProfile("trackeff_primary_vs_pt_lowpt","track efficiency for primary tracks vs p_{T} for |#eta|<2.5;p_{T}(GeV/c);Efficiency",25,0,5.0);
    RegisterHisto(al_shift,m_trackeff_primary_vs_pt_lowpt);
    m_trackeff_primary_vs_pt_loweta = new TProfile("trackeff_primary_vs_pt_loweta","track efficiency for primary tracks vs p_{T} for |#eta|<0.8;p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_primary_vs_pt_loweta);
    m_trackeff_primary_vs_pt_medeta = new TProfile("trackeff_primary_vs_pt_medeta","track efficiency for primary tracks vs p_{T} for 0.8<|#eta|<1.5;p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_primary_vs_pt_medeta);
    m_trackeff_primary_vs_pt_higheta = new TProfile("trackeff_primary_vs_pt_higheta","track efficiency for primary tracks vs p_{T} for 1.5<|#eta|<2.5;p_{T}(GeV/c);Efficiency",25,0,50.0);
    RegisterHisto(al_shift,m_trackeff_primary_vs_pt_higheta);

    // track efficiency vs log pt
    m_trackeff_primary_vs_logpt = new TProfile("trackeff_primary_vs_logpt","track efficiency for primary tracks vs p_{T} for |#eta|<2.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_primary_vs_logpt);
    m_trackeff_primary_vs_logpt_loweta = new TProfile("trackeff_primary_vs_logpt_loweta","track efficiency for primary tracks vs p_{T} for |#eta|<0.8 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_primary_vs_logpt_loweta);
    m_trackeff_primary_vs_logpt_medeta = new TProfile("trackeff_primary_vs_logpt_medeta","track efficiency for primary tracks vs p_{T} for 0.8<|#eta|<1.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_primary_vs_logpt_medeta);
    m_trackeff_primary_vs_logpt_higheta = new TProfile("trackeff_primary_vs_logpt_higheta","track efficiency for primary tracks vs p_{T} for 1.5<|#eta|<2.5 (Det. Paper def.);Log(p_{T}(GeV/c));Efficiency",30,-0.5,2.5);
    RegisterHisto(al_shift,m_trackeff_primary_vs_logpt_higheta);

    m_trackeff_primary_vs_eta_vlowpt = new TProfile("trackeff_primary_vs_eta_vlowpt","track efficiency for primary tracks vs #eta for 0.1<p_{T}<0.5 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    RegisterHisto(al_shift,m_trackeff_primary_vs_eta_vlowpt);
    m_trackeff_primary_vs_eta_lowpt = new TProfile("trackeff_primary_vs_eta_lowpt","track efficiency for primary tracks vs #eta for 0.5<p_{T}<1.0 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    RegisterHisto(al_shift,m_trackeff_primary_vs_eta_lowpt);

    // ------------------------------------------------
    // tracking efficiency vs eta for secondary tracks
    // ------------------------------------------------
    m_trackeff_secondary_vs_eta = create_registeredTProfile(al_shift, "trackeff_secondary_vs_eta","track efficiency for secondary tracks vs #eta;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_secondary_vs_phi = create_registeredTProfile(al_shift, "trackeff_secondary_vs_phi","track efficiency for secondary tracks vs #phi;#phi;Efficiency",24,-180,180);
    // tracking efficiency versus pt for three eta regions
    m_trackeff_secondary_vs_pt = create_registeredTProfile(al_shift, "trackeff_secondary_vs_pt","track efficiency for secondary tracks vs p_{T} for |#eta|<2.5;p_{T}(GeV/c);Efficiency",25,0,50.0);
    m_trackeff_secondary_vs_eta_vlowpt = create_registeredTProfile(al_shift, "trackeff_secondary_vs_eta_vlowpt","track efficiency for secondary tracks vs #eta for 0.1<p_{T}<0.5 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_secondary_vs_eta_lowpt = create_registeredTProfile(al_shift, "trackeff_secondary_vs_eta_lowpt","track efficiency for secondary tracks vs #eta for 0.5<p_{T}<1.0 GeV;#eta;Efficiency",nbins_eta,min_eta,max_eta);
    m_trackeff_secondary_vs_radius = create_registeredTProfile(al_shift, "trackeff_secondary_vs_radius","track efficiency for secondary tracks vs decay vertex;R_{xy}(mm);Efficiency",36,0,360);

    // hit efficiencies and fractions of holes and outliers
    m_eff_hit_vs_eta_ibl_barrel = create_registeredTProfile(al_shift, "eff_hit_vs_eta_ibl_barrel","Cluster Efficiency: Pixel Barrel IBL;#eta;Clus. Efficiency",10,0,2.5);
    m_eff_hit_vs_eta_blay_barrel = create_registeredTProfile(al_shift, "eff_hit_vs_eta_blay_barrel","Cluster Efficiency: Pixel Barrel B-Layer;#eta;Clus. Efficiency",10,0,2.5);
    m_eff_hit_vs_eta_pix_barrel = create_registeredTProfile(al_shift, "eff_hit_vs_eta_pix_barrel","Cluster Efficiency: Pixel Barrel;#eta;Clus. Efficiency",10,0,2.5);
    m_eff_hit_vs_eta_pix_endcap = create_registeredTProfile(al_shift, "eff_hit_vs_eta_pix_endcap","Cluster Efficiency: Pixel Endcaps;#eta;Clus. Efficiency",4,1.5,2.5);
    m_eff_hit_vs_eta_sct_barrel = create_registeredTProfile(al_shift, "eff_hit_vs_eta_sct_barrel","Cluster Efficiency: SCT Barrel;#eta;Clus. Efficiency",7,0,1.75);
    m_eff_hit_vs_eta_sct_endcap = create_registeredTProfile(al_shift, "eff_hit_vs_eta_sct_endcap","Cluster Efficiency: SCT Endcaps;#eta;Clus. Efficiency",6,1,2.5);
    m_eff_hit_vs_eta_trt_barrel = create_registeredTProfile(al_shift, "eff_hit_vs_eta_trt_barrel","Cluster Efficiency: TRT Barrel;#eta;Hit Efficiency",5,0,1.25);
    m_eff_hit_vs_eta_trt_endcap = create_registeredTProfile(al_shift, "eff_hit_vs_eta_trt_endcap","Cluster Efficiency: TRT Endcap;#eta;Hit Efficiency",6,0.75,2.25);

    m_frac_hole_vs_eta_ibl_barrel = create_registeredTProfile(al_expert, "frac_hole_vs_eta_ibl_barrel","Hole Fraction: Pixel Barrel IBL;#eta;Hole Frac.",10,0,2.5);
    m_frac_hole_vs_eta_blay_barrel = create_registeredTProfile(al_expert, "frac_hole_vs_eta_blay_barrel","Hole Fraction: Pixel Barrel B-Layer;#eta;Hole Frac.",10,0,2.5);
    m_frac_hole_vs_eta_pix_barrel = create_registeredTProfile(al_expert, "frac_hole_vs_eta_pix_barrel","Hole Fraction: Pixel Barrel;#eta;Hole Frac.",10,0,2.5);
    m_frac_hole_vs_eta_pix_endcap = create_registeredTProfile(al_expert, "frac_hole_vs_eta_pix_endcap","Hole Fraction: Pixel Endcaps;#eta;Hole Frac.",4,1.5,2.5);
    m_frac_hole_vs_eta_sct_barrel = create_registeredTProfile(al_expert, "frac_hole_vs_eta_sct_barrel","Hole Fraction: SCT Barrel;#eta;Hole Frac.",7,0,1.75);
    m_frac_hole_vs_eta_sct_endcap = create_registeredTProfile(al_expert, "frac_hole_vs_eta_sct_endcap","Hole Fraction: SCT Endcaps;#eta;Hole Frac.",6,1,2.5);
    m_frac_hole_vs_eta_trt_barrel = create_registeredTProfile(al_expert, "frac_hole_vs_eta_trt_barrel","Hole Fraction: TRT Barrel;#eta;Hole Frac.",5,0,1.25);
    m_frac_hole_vs_eta_trt_endcap = create_registeredTProfile(al_expert, "frac_hole_vs_eta_trt_endcap","Hole Fraction: TRT Endcap;#eta;Hole Frac.",6,0.75,2.25);

    m_frac_outlier_vs_eta_ibl_barrel = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_ibl_barrel","Outlier Fraction: Pixel Barrel IBL;#eta;Outlier Frac.",10,0,2.5);
    m_frac_outlier_vs_eta_blay_barrel = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_blay_barrel","Outlier Fraction: Pixel Barrel B-Layer;#eta;Outlier Frac.",10,0,2.5);
    m_frac_outlier_vs_eta_pix_barrel = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_pix_barrel","Outlier Fraction: Pixel Barrel;#eta;Outlier Frac.",10,0,2.5);
    m_frac_outlier_vs_eta_pix_endcap = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_pix_endcap","Outlier Fraction: Pixel Endcaps;#eta;Outlier Frac.",4,1.5,2.5);
    m_frac_outlier_vs_eta_sct_barrel = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_sct_barrel","Outlier Fraction: SCT Barrel;#eta;Outlier Frac.",7,0,1.75);
    m_frac_outlier_vs_eta_sct_endcap = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_sct_endcap","Outlier Fraction: SCT Endcaps;#eta;Outlier Frac.",6,1,2.5);
    m_frac_outlier_vs_eta_trt_barrel = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_trt_barrel","Outlier Fraction: TRT Barrel;#eta;Outlier Frac.",5,0,1.25);
    m_frac_outlier_vs_eta_trt_endcap = create_registeredTProfile(al_expert, "frac_outlier_vs_eta_trt_endcap","Outlier Fraction: TRT Endcap;#eta;Outlier Frac.",6,0.75,2.25);

    // charge mididentification rate
    // -1 means wrong charge, +1 means correct charge
    m_chargemisid = create_registeredTProfile(al_shift, "chargemisid","Charge Misidentification Rate versus #eta;#eta;Charge Misid.",nbins_eta,min_eta,max_eta,0,1);

    // ---------------------------------------------------------
    // track fake rates:
    //
    // - a "fake" is defined as a track with <20% match probability
    // - a "bad match" is defined as a track with <80% match probability
    // ---------------------------------------------------------
    m_track_notruthrate_vs_eta= create_registeredTProfile(al_shift, "track_notruthrate_vs_eta","Fraction of tracks with no truth match;#eta;No Truth Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_notruthrate_vs_pt= create_registeredTProfile(al_shift, "track_notruthrate_vs_pt","Fraction of tracks with no truth match;p_{T}(GeV/c);No Truth Rate",25,0,50,0,1);
    m_track_notruthrate_vs_phi= create_registeredTProfile(al_shift, "track_notruthrate_vs_phi","Fraction of tracks with no truth match;#phi;No Truth Rate",24,-180,180);
    m_track_notruthrate_vs_d0= create_registeredTProfile(al_shift, "track_notruthrate_vs_d0","Fraction of tracks with no truth match;d_{0}(mm);No Truth Rate",20,-6,6);
    m_track_notruthrate_vs_z0= create_registeredTProfile(al_shift, "track_notruthrate_vs_z0","Fraction of tracks with no truth match;z_{0}(mm);No Truth Rate",20,-300,300);
    m_track_fakerate_vs_eta= create_registeredTProfile(al_shift, "track_fakerate_vs_eta","Fraction of tracks with <20% truth match probability ;#eta;Fake Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_fakerate_vs_pt= create_registeredTProfile(al_shift, "track_fakerate_vs_pt","Fraction of tracks with <20% truth match probability ;p_{T}(GeV/c);Fake Rate",25,0,50,0,1);
    m_track_fakerate_vs_phi= create_registeredTProfile(al_shift, "track_fakerate_vs_phi","Fraction of tracks with <20% truth match probability ;#phi;Fake Rate",24,-180,180);
    m_track_fakerate_vs_d0= create_registeredTProfile(al_shift, "track_fakerate_vs_d0","Fraction of tracks with <20% truth match probability ;d_{0}(mm);Fake Rate",20,-6,6);
    m_track_fakerate_vs_z0= create_registeredTProfile(al_shift, "track_fakerate_vs_z0","Fraction of tracks with <20% truth match probability ;z_{0}(mm);Fake Rate",20,-300,300);
    m_track_fakerate_vs_deltaz0= create_registeredTProfile(al_shift, "track_fakerate_vs_deltaz0","Fraction of tracks with <20% truth match probability ;#Delta(z_{0})xsin(#theta)(mm);Fake Rate",60,-15,15);

    m_track_badmatchrate_vs_eta= create_registeredTProfile(al_shift, "track_badmatchrate_vs_eta","Fraction of tracks with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_badmatchrate_vs_pt= create_registeredTProfile(al_shift, "track_badmatchrate_vs_pt","Fraction of tracks with <80% truth match probability ;p_{T}(GeV/c);Bad Match Rate",25,0,50,0,1);
    m_track_badmatchrate_vs_phi= create_registeredTProfile(al_shift, "track_badmatchrate_vs_phi","Fraction of tracks with <80% truth match probability ;#phi;Bad Match Rate",24,-180,180);
    m_track_badmatchrate_vs_d0= create_registeredTProfile(al_shift, "track_badmatchrate_vs_d0","Fraction of tracks with <80% truth match probability ;d_{0}(mm);Bad Match Rate",20,-6,6);
    m_track_badmatchrate_vs_z0= create_registeredTProfile(al_shift, "track_badmatchrate_vs_z0","Fraction of tracks with <80% truth match probability ;z_{0}(mm);Bad Match Rate",20,-300,300);
    m_track_badmatchrate_vs_deltaz0= create_registeredTProfile(al_shift, "track_badmatchrate_vs_deltaz0","Fraction of tracks with <80% truth match probability ;#Delta(z_{0})xsin(#theta)(mm);Bad Match Rate",60,-15,15);

    m_track_bcode0rate_vs_eta= create_registeredTProfile(al_shift, "track_bcode0rate_vs_eta","Fraction of tracks with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_bcode0rate_vs_pt= create_registeredTProfile(al_shift, "track_bcode0rate_vs_pt","Fraction of tracks with <80% truth match probability ;p_{T}(GeV/c);Bad Match Rate",25,0,50,0,1);
    m_track_bcode0rate_vs_phi= create_registeredTProfile(al_shift, "track_bcode0rate_vs_phi","Fraction of tracks with <80% truth match probability ;#phi;Bad Match Rate",24,-180,180);
    m_track_bcode0rate_vs_d0= create_registeredTProfile(al_shift, "track_bcode0rate_vs_d0","Fraction of tracks with <80% truth match probability ;d_{0}(mm);Bad Match Rate",20,-6,6);
    m_track_bcode0rate_vs_z0= create_registeredTProfile(al_shift, "track_bcode0rate_vs_z0","Fraction of tracks with <80% truth match probability ;z_{0}(mm);Bad Match Rate",20,-300,300);
    m_track_bcode0rate_vs_deltaz0= create_registeredTProfile(al_shift, "track_bcode0rate_vs_deltaz0","Fraction of tracks with <80% truth match probability ;#Delta(z_{0})xsin(#theta)(mm);Bad Match Rate",60,-15,15);
    m_track_bcode0rate_matchbc0_vs_eta = create_registeredTProfile(al_shift, "track_bcode0rate_matchbc0_vs_eta","Fraction of tracks with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_bcode0rate_matchprim_vs_eta = create_registeredTProfile(al_shift, "track_bcode0rate_matchprim_vs_eta","Fraction of tracks with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);
    m_track_bcode0rate_matchsec_vs_eta = create_registeredTProfile(al_shift, "track_bcode0rate_matchsec_vs_eta","Fraction of tracks with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);

    m_trackinjet_fakerate_vs_eta= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_eta","Fraction of tracks in jets with <20% truth match probability ;#eta;Fake Rate",nbins_eta,min_eta,max_eta,0,1);
    m_trackinjet_fakerate_vs_phi= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_phi","Fraction of tracks in jets with <20% truth match probability ;#phi;Fake Rate",24,-180,180);
    m_trackinjet_fakerate_vs_pt= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_pt","Fraction of tracks in jets with <20% truth match probability ;p_{T}(GeV/c);Fake Rate",25,0,50,0,1);
    m_trackinjet_fakerate_vs_dr= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_dr","Fraction of tracks in jets with <20% truth match probability ;#DeltaR;Fake Rate",10,0,0.4,0,1);
    m_trackinjet_fakerate_vs_dr_lt_j50= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_dr_lt_j50","Fraction of tracks in jets with <20% truth match probability (E_{T}(jet) < 50 GeV);#DeltaR;Fake Rate",10,0,0.4,0,1);
    m_trackinjet_fakerate_vs_dr_gr_j100= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_dr_gr_j100","Fraction of tracks in jets with <20% truth match probability (E_{T}(jet) > 100 GeV);#DeltaR;Fake Rate",10,0,0.4,0,1);
    m_trackinjet_fakerate_vs_jetet= create_registeredTProfile(al_shift, "trackinjet_fakerate_vs_jetet","Fraction of tracks in jets with <20% truth match probability ;E_{jet}(GeV);Fake Rate",10,0,250,0,1);
    m_trackinjet_badmatchrate_vs_eta= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_eta","Fraction of tracks in jets with <80% truth match probability ;#eta;Bad Match Rate",nbins_eta,min_eta,max_eta,0,1);
    m_trackinjet_badmatchrate_vs_phi= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_phi","Fraction of tracks in jets with <80% truth match probability ;#phi;Bad Match Rate",24,-180,180);
    m_trackinjet_badmatchrate_vs_pt= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_pt","Fraction of tracks in jets with <80% truth match probability ;p_{T}(GeV/c);Bad Match Rate",25,0,50,0,1);
    m_trackinjet_badmatchrate_vs_dr= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_dr","Fraction of tracks in jets with <80% truth match probability ;#DeltaR;Bad Match Rate",10,0,0.4,0,1);
    m_trackinjet_badmatchrate_vs_dr_lt_j50= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_dr_lt_j50","Fraction of tracks in jets with <80% truth match probability (E_{T}(jet) < 50 GeV);#DeltaR;Bad Match Rate",10,0,0.4,0,1);
    m_trackinjet_badmatchrate_vs_dr_gr_j100= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_dr_gr_j100","Fraction of tracks in jets with <80% truth match probability (E_{T}(jet) > 100 GeV);#DeltaR;Bad Match Rate",10,0,0.4,0,1);
    m_trackinjet_badmatchrate_vs_jetet= create_registeredTProfile(al_shift, "trackinjet_badmatchrate_vs_jetet","Fraction of tracks in jets with <80% truth match probability ;E_{jet}(GeV);Bad Match Rate",10,0,250,0,1);
    m_trackinjet_bcode0rate_vs_eta= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_eta","Fraction of tracks in jets with <80% truth match probability ;#eta;Bad Match or Bcode0 Rate",nbins_eta,min_eta,max_eta,0,1);
    m_trackinjet_bcode0rate_vs_phi= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_phi","Fraction of tracks in jets with <80% truth match probability ;#phi;Bad Match or Bcode0 Rate",24,-180,180);
    m_trackinjet_bcode0rate_vs_pt= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_pt","Fraction of tracks in jets with <80% truth match probability ;p_{T}(GeV/c);Bad Match or Bcode0 Rate",25,0,50,0,1);
    m_trackinjet_bcode0rate_vs_dr= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_dr","Fraction of tracks in jets with <80% truth match probability ;#DeltaR;Bad Match or Bcode0 Rate",10,0,0.4,0,1);
    m_trackinjet_bcode0rate_vs_dr_lt_j50= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_dr_lt_j50","Fraction of tracks in jets with <80% truth match probability (E_{T}(jet) < 50 GeV);#DeltaR;Bad Match or Bcode0 Rate",10,0,0.4,0,1);
    m_trackinjet_bcode0rate_vs_dr_gr_j100= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_dr_gr_j100","Fraction of tracks in jets with <80% truth match probability (E_{T}(jet) > 100 GeV);#DeltaR;Bad Match or Bcode0 Rate",10,0,0.4,0,1);
    m_trackinjet_bcode0rate_vs_jetet= create_registeredTProfile(al_shift, "trackinjet_bcode0rate_vs_jetet","Fraction of tracks in jets with <80% truth match probability ;E_{jet}(GeV);Bad Match or Bcode0 Rate",10,0,250,0,1);

    m_prob = create_registeredTH1F(al_shift, "prob","Truth match probability;Prob.",57,-0.12,1.02);
    m_prob_injet = create_registeredTH1F(al_shift, "prob_injet","Truth match probability (inside jet);Prob.",57,-0.12,1.02);
    m_prob_barcode0 = create_registeredTH1F(al_shift, "prob_barcode0","Truth match probability; prob",57,-0.12,1.02);
    m_prob_barcodeMAXBARCODE = create_registeredTH1F(al_shift, "prob_barcode" STR(MAXBARCODE),"Truth match probability; prob",57,-0.12,1.02);
    m_prob_barcode_primary = create_registeredTH1F(al_shift, "prob_barcode_primary","Truth match probability; prob",57,-0.12,1.02);
    m_nTrkMatchesSameHmpl = create_registeredTH1F(al_shift, "nTrkMatchesSameHmpl","Number of tracks matching the same truth particle; ntrk",20,0,20);
    m_nTrkMatchesSameHmpl_barcode0 = create_registeredTH1F(al_shift, "nTrkMatchesSameHmpl_barcode0","Number of tracks matching the same truth particle; ntrk",20,0,20);
    m_nTrkMatchesSameHmpl_barcodeMAXBARCODE = create_registeredTH1F(al_shift, "nTrkMatchesSameHmpl_barcode" STR(MAXBARCODE),"Number of tracks matching the same truth particle; ntrk",20,0,20);
    m_nTrkMatchesSameHmpl_barcode_primary = create_registeredTH1F(al_shift, "nTrkMatchesSameHmpl_barcode_primary","Number of tracks matching the same truth particle; ntrk",20,0,20);

    // hit map
    m_nHits_blay_2d = create_registeredTH2F(al_shift, "nHits_blay_2d","B-Layer clus. #eta vs #phi;#eta;#phi", 30,-3,3,32,-M_PI,M_PI);
    m_nExpectedHits_blay_2d = create_registeredTH2F(al_shift, "nExpectedHits_blay_2d","Expected B-Layer hit. #eta vs #phi", 30, -3, 3, 32, -M_PI, M_PI);
    m_nHits_pixel_2d = create_registeredTH2F(al_shift, "nHits_pixel_2d","Pixel clus. #eta vs #phi;#eta;#phi", 30,-3,3,32,-M_PI,M_PI);
    m_nHits_SCT_2d = create_registeredTH2F(al_shift, "nHits_SCT_2d","SCT clus. #eta vs #phi;#eta;#phi", 30,-3,3,32,-M_PI,M_PI);
    m_nHits_TRT_2d = create_registeredTH2F(al_shift, "nHits_TRT_2d","TRT clus. #eta vs #phi;#eta;#phi", 30,-3,3,32,-M_PI,M_PI);
    m_nHits_ALL_RZ = create_registeredTH2F(al_shift, "nHits_ALL_RZ","Cluster map plot RZ;r(mm);z(mm)", 400,-3200,3200,400,0,1150);

    m_prodvtx_secondaries_RZ = create_registeredTH2F(al_shift, "prodvtx_secondaries_RZ","Secondaries Prod. vtx Map Plots; r(mm); z(mm)", 400,-3200,3200,400,0,1150);
    m_prodvtx_primaries_RZ = create_registeredTH2F(al_shift, "prodvtx_primaries_RZ","Primaries Prod. vtx Map Plots; r(mm); z(mm)", 400,-3200,3200,400,0,1150);

    // Multi turth match with different event index
    m_num_eventindex_match = create_registeredTH1F(al_shift, "num_eventindex_match", "num_eventindex_match",10,0,10);
    m_Good_num_eventindex_match = create_registeredTH1F(al_shift, "Good_num_eventindex_match", "Good_num_eventindex_match; evi match",10,0,10);
    m_Fake_num_eventindex_match = create_registeredTH1F(al_shift, "Fake_num_eventindex_match", "Fake_num_eventindex_match; evi match",10,0,10);
    m_bc0_num_eventindex_match = create_registeredTH1F(al_shift, "bc0_num_eventindex_match", "bc0_num_eventindex_match; evi match",10,0,10);
    m_num_truthmatch_match = create_registeredTH1F(al_shift, "num_truthmatch_match", "num_truthmatch_match; evi match",10,0,10);
    m_Good_num_truthmatch_match = create_registeredTH1F(al_shift, "Good_num_truthmatch_match", "Good_num_truthmatch_match; evi match",10,0,10);
    m_Fake_num_truthmatch_match = create_registeredTH1F(al_shift, "Fake_num_truthmatch_match", "Fake_num_truthmatch_match; evi match",10,0,10);
    m_bc0_num_truthmatch_match = create_registeredTH1F(al_shift, "bc0_num_truthmatch_match", "bc0_num_truthmatch_match; evi match",10,0,10);

    // Cluster information
    m_cluster_width_phi_pixel = create_registeredTH1F(al_expert, "cluster_width_phi_pixel", "Cluster width in #phi for pixels", 5, 0.5, 5.5);
    m_cluster_width_eta_pixel = create_registeredTH1F(al_expert, "cluster_width_eta_pixel", "Cluster width in #eta for pixels", 5, 0.5, 5.5);
    m_cluster_width_phi_sct = create_registeredTH1F(al_expert, "cluster_width_phi_sct", "Cluster width in #phi for SCT", 5, 0.5, 5.5);
    m_cluster_z_phi_pixel = create_registeredTH2F(al_expert, "cluster_z_phi_pixel", "Cluster size for pixels;width in #eta;width in #phi", 5, 0.5, 5.5, 5, 0.5, 5.5);
    m_cluster_z_phi_sct = create_registeredTH2F(al_expert, "cluster_z_phi_sct", "Cluster size for SCT;width in #eta;width in #phi", 5, 0.5, 5.5, 5, 0.5, 5.5);
    m_cluster_width_phi_pixel_eta = create_registeredTH2F(al_expert, "cluster_width_phi_pixel_eta", "Cluster width distrubution vs. #eta, pixels", nbins_eta, min_eta, max_eta, 5, 0.5, 5.5);
    m_cluster_width_eta_pixel_eta = create_registeredTH2F(al_expert, "cluster_width_eta_pixel_eta", "Cluster #eta width distrubution vs. #eta, pixels", nbins_eta, min_eta, max_eta, 5, 0.5, 5.5);
    m_cluster_width_phi_sct_eta = create_registeredTH2F(al_expert, "cluster_width_phi_sct_eta", "Cluster width distrubution vs. #eta, SCT", nbins_eta, min_eta, max_eta, 5, 0.5, 5.5);

    // Track incidence angle cluster information
    string detAreaTypesStrings[n_detAreaTypes] = {"pix_b", "pix_e", "sct_b", "sct_e"};
    string num[4] = {"1", "2", "3", "4"};
    for (int i=0; i<n_detAreaTypes; i++)
      {
	for (int j=0; j<4; j++)
	  {
	    m_cluster_angle_phi[i][j] =
	      create_registeredTH1F(al_expert,
				    (string("cluster_angle_phi_")+
                                     detAreaTypesStrings[i]+"_"+num[j]).c_str(),
				    (num[j]+
				     " width (#phi) clusters vs. incidence angle").c_str(),
				    100, 0, 3.1416);
	    m_cluster_angle_eta[i][j] =
	      create_registeredTH1F(al_expert,
				    (string("cluster_angle_eta_")+
				     detAreaTypesStrings[i]+"_"+num[j]).c_str(),
				    (num[j]+
				     " width (#eta) clusters vs. incidence angle").c_str(),
				    100, 0, 3.1416);
	  }
      }

    // residuals
    m_residualx_pixel_barrel = create_registeredTH1F(al_shift, "residualx_pixel_barrel", "Residual: Pixel Barrel X;x residual(#mum)",120,-50,50);
    m_residualy_pixel_barrel = create_registeredTH1F(al_shift, "residualy_pixel_barrel", "Residual: Pixel Barrel Y;y residual(#mum)",120,-300,300);
    m_residualx_pixel_endcap = create_registeredTH1F(al_shift, "residualx_pixel_endcap", "Residual: Pixel Endcap X;x residual(#mum)",120,-50,50);
    m_residualy_pixel_endcap = create_registeredTH1F(al_shift, "residualy_pixel_endcap", "Residual: Pixel Endcap Y;y residual(#mum)",120,-300,300);

    const double min_normal = 0;
    const double max_normal = TMath::PiOver2();
    const double min_phi = 0;
    const double max_phi = TMath::PiOver4();
    const double min_phi_endcap = -0.5*TMath::PiOver4();
    const double max_phi_endcap =  0.5*TMath::PiOver4();
    const double min_eta_endcap = 0;
    const double max_eta_endcap = TMath::PiOver4();
    const double min_normal_endcap = 0.1;
    const double max_normal_endcap = 0.6;// TMath::PiOver4();
    m_pullPhi_vs_incident_pixel = create_registeredTH2F(pull_shift, "pullPhi_vs_incident_pixel", "Pull (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi)", 25, min_normal, max_normal, 100, -5, 5);
    m_residualPhi_vs_incident_pixel = create_registeredTH2F(pull_shift, "residualPhi_vs_incident_pixel", "Residual (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi)", 25, min_normal, max_normal, 200, -0.5, 0.5);
    m_pullMeanPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_incident_pixel", "Pull Mean (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_incident_pixel", "Pull Width (r-phi) vs. incident on the pixel plane;angle w/ normal;pull (r-phi) width", 100, -5, 5);
    m_residualMeanPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "residualMeanPhi_vs_incident_pixel", "Residual Mean (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi) mean", 20, 0, 1);
    m_residualWidthPhi_vs_incident_pixel = create_registeredTH1F(pull_parent, "residualWidthPhi_vs_incident_pixel", "Residual Width (r-phi) vs. incident on the pixel plane;angle w/ normal;residual (r-phi) width", 20, 0, 1);
    m_pullPhi_vs_phi_pixelBarrel = create_registeredTH2F(pull_shift, "pullPhi_vs_phi_pixelBarrel", "Pull (r-phi) vs. phi, Barrel", 25, min_phi, max_phi, 100, -5, 5);
    m_pullPhi_vs_phi_pixelEndcap = create_registeredTH2F(pull_shift, "pullPhi_vs_phi_pixelEndcap", "Pull (r-phi) vs. phi, Endcap", 25, min_phi_endcap, max_phi_endcap, 100, -5, 5);
    m_pullMeanPhi_vs_phi_pixelBarrel = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_phi_pixelBarrel", "Pull Mean (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_phi_pixelBarrel = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_phi_pixelBarrel", "Pull Width (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullMeanPhi_vs_phi_pixelEndcap = create_registeredTH1F(pull_parent, "pullMeanPhi_vs_phi_pixelEndcap", "Pull Mean (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);
    m_pullWidthPhi_vs_phi_pixelEndcap = create_registeredTH1F(pull_parent, "pullWidthPhi_vs_phi_pixelEndcap", "Pull Width (r-phi) vs. local phi, Endcap;local phi;pull (r-phi) mean", 100, -5, 5);

    // the number of Pixel layers and disks
    int pixelBarrelLayers = m_PIX_Mgr->numerology().numLayers();
    int pixelEndcapDisks  = m_PIX_Mgr->numerology().numDisks();

    int nPixelLayers = pixelBarrelLayers > pixelEndcapDisks ? pixelBarrelLayers : pixelEndcapDisks;

    m_residualx_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualy_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualpullx_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualpully_pixel_barrel_l.reserve(pixelBarrelLayers);

    m_pullZ_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_incident_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullZ_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_eta_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullZ_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualZ_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullPhi_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_residualPhi_vs_phi_pixel_barrel_l.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthPhi_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_incident_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_eta_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullMeanZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_pullWidthZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualMeanZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);
    m_residualWidthZ_vs_phi_pixel_barrel_layer.reserve(pixelBarrelLayers);

    // initialize the eta binned histograms
    for (int ieta=0; ieta<5; ieta++){
        m_residualPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH2F*>(pixelBarrelLayers));
        m_pullPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH2F*>(pixelBarrelLayers));
        m_residualMeanPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_residualWidthPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_pullMeanPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
        m_pullWidthPhi_vs_phi_pixel_barrel_etaBins.push_back(std::vector<TH1F*>(pixelBarrelLayers));
    }

    m_residualx_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualy_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualpullx_pixel_endcap_d.reserve(pixelBarrelLayers);
    m_residualpully_pixel_endcap_d.reserve(pixelBarrelLayers);

    m_pullZ_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_incident_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullZ_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_eta_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullZ_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualZ_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullPhi_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_residualPhi_vs_phi_pixel_endcap_d.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthPhi_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_incident_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_eta_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullMeanZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_pullWidthZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualMeanZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    m_residualWidthZ_vs_phi_pixel_endcap_disk.reserve(pixelEndcapDisks);
    char title[bufferLength];
    for (int ilayer=0; ilayer<nPixelLayers; ilayer++) {
      // IBL 
      if (ilayer == 0){
        m_residualx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualx_pixel_barrel_ibl", "Residual: Pixel Barrel X IBL ;x residual(#mum)", 120, -50, 50));
        m_residualy_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualy_pixel_barrel_ibl", "Residual: Pixel Barrel Y IBL ;y residual(#mum)", 120, -300, 300));
        m_residualpullx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualpullx_pixel_barrel_ibl", "Residualpull: Pixel Barrel X IBL ;x pull residual", 100, -5, 5));
        m_residualpully_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualpully_pixel_barrel_ibl", "Residualpull: Pixel Barrel Y IBL ;y pull residual", 100, -5, 5)); 
      }
      // B-layer
      if (ilayer == 1){
        m_residualx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualx_pixel_barrel_blayer", "Residual: Pixel Barrel X B-layer :x residual(#mum)", 120, -50, 50));
        m_residualy_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualy_pixel_barrel_blayer", "Residual: Pixel Barrel Y B-layer ;y residual(#mum)", 120, -300, 300));
        m_residualpullx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualpullx_pixel_barrel_blayer", "Residualpull: Pixel Barrel X B-layer ;x pull residual", 100, -5, 5));
        m_residualpully_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, "residualpully_pixel_barrel_blayer", "Residualpull: Pixel Barrel Y B-layer ;y pull residual", 100, -5, 5)); 
      }
      // pixel barrel section
      if (ilayer < pixelBarrelLayers && ilayer>1){
        snprintf(name, bufferLength, "residualx_pixel_barrel_l%i", ilayer-1);
        snprintf(title, bufferLength, "Residual: Pixel Barrel X Layer %i;x residual(#mum)" ,ilayer-1);
        m_residualx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, name, title, 120, -50, 50));

        snprintf(name, bufferLength, "residualy_pixel_barrel_l%i", ilayer-1);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Y Layer %i;y residual(#mum)" ,ilayer-1);
        m_residualy_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, name, title, 120, -300, 300));

        snprintf(name, bufferLength, "residualpullx_pixel_barrel_l%i", ilayer-1);
        snprintf(title, bufferLength, "Residualpull: Pixel Barrel X Layer %i;x pull residual" ,ilayer-1);
        m_residualpullx_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, name, title, 100,-5,5));

        snprintf(name, bufferLength, "residualpully_pixel_barrel_l%i", ilayer-1);
        snprintf(title, bufferLength, "Residualpull: Pixel Barrel Y Layer %i;y pull residual" ,ilayer-1);
        m_residualpully_pixel_barrel_l.push_back(create_registeredTH1F(al_expert, name, title, 100,-5,5));
      }
      // pixel endcap section
      if (ilayer < pixelEndcapDisks){
        snprintf(name, bufferLength, "residualx_pixel_endcap_d%i", ilayer+1);
        snprintf(title, bufferLength, "Residual: Pixel Endcap X Disk %i;x residual(#mum)" ,ilayer+1);
        m_residualx_pixel_endcap_d.push_back(create_registeredTH1F(al_expert, name, title, 120, -50, 50));

        snprintf(name, bufferLength, "residualy_pixel_endcap_d%i", ilayer+1);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Y Disk %i;y residual(#mum)" ,ilayer+1);
        m_residualy_pixel_endcap_d.push_back(create_registeredTH1F(al_expert, name, title, 120, -300, 300));

        snprintf(name, bufferLength, "residualpullx_pixel_endcap_d%i", ilayer+1);
        snprintf(title, bufferLength, "Residualpull: Pixel Endcap X Disk %i;x pull residual" ,ilayer+1);
        m_residualpullx_pixel_endcap_d.push_back(create_registeredTH1F(al_expert, name, title, 100,-5,5));

        snprintf(name, bufferLength, "residualpully_pixel_endcap_d%i", ilayer+1);
        snprintf(title, bufferLength, "Residualpull: Pixel Endcap Y Disk %i;y pull residual" ,ilayer+1);
        m_residualpully_pixel_endcap_d.push_back(create_registeredTH1F(al_expert, name, title, 100,-5,5));
      }

// --
      // pixel barrel section
      if (ilayer < pixelBarrelLayers){
        snprintf(name, bufferLength, "pullPhi_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Phi vs incident normal Layer %i;angle w/ normal;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 100, -5, 5));
        snprintf(name, bufferLength, "residualPhi_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Phi vs incident normal Layer %i;angle w/ normal;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 200, -0.5, 0.5));

        snprintf(name, bufferLength, "pullPhi_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Phi vs incident eta Layer %i;pixel eta;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 100, -5, 5));
        snprintf(name, bufferLength, "residualPhi_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Phi vs incident eta Layer %i;pixel eta;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 200, -0.5, 0.5));

        snprintf(name, bufferLength, "pullPhi_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Phi vs incident phi Layer %i;pixel phi;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 100, -5, 5));
        snprintf(name, bufferLength, "residualPhi_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Phi vs incident phi Layer %i;pixel phi;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 200, -0.5, 0.5));
// --
        snprintf(name, bufferLength, "pullZ_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Z vs incident normal Layer %i;angle w/ normal;pull (r-z)" ,ilayer);
        m_pullZ_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 100, -5, 5));
        snprintf(name, bufferLength, "residualZ_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Z vs incident normal Layer %i;angle w/ normal;residual (r-z)" ,ilayer);
        m_residualZ_vs_incident_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal, max_normal, 200, -1, 1));

        snprintf(name, bufferLength, "pullZ_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Z vs incident eta Layer %i;pixel eta;pull (r-z)" ,ilayer);
        m_pullZ_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 100, -5, 5));
        snprintf(name, bufferLength, "residualZ_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Z vs incident eta Layer %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_eta_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta, max_eta, 200, -1, 1));

        snprintf(name, bufferLength, "pullZ_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Barrel Z vs incident phi Layer %i;pixel phi;pull (r-z)" ,ilayer);
        m_pullZ_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 100, -5, 5));
        snprintf(name, bufferLength, "residualZ_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Barrel Z vs incident phi Layer %i;pixel phi;residual (r-z)" ,ilayer);
        m_residualZ_vs_phi_pixel_barrel_l.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_phi, max_phi, 200, -1, 1));

        // One profile each for the value and the width
        // The axis is meaningless, as it's projected from the 2d hist
        snprintf(name, bufferLength, "pullMeanPhi_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthPhi_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanPhi_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        snprintf(name, bufferLength, "pullWidthPhi_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanPhi_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthPhi_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;residual (r-phi) mean" ,ilayer);

        m_residualMeanPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        snprintf(name, bufferLength, "residualWidthPhi_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident normal;angle w/ normal;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthPhi_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident eta;pixel eta;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        snprintf(name, bufferLength, "residualWidthPhi_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident phi;pixel phi;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        snprintf(name, bufferLength, "pullWidthZ_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthZ_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthZ_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel Z Layer %i vs. incident normal;angle w/ normal;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthZ_vs_incident_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel Z Layer %i vs. incident normal;angle w/normal;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_incident_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthZ_vs_eta_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel Z Layer %i vs. incident eta;pixel eta;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_eta_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthZ_vs_phi_pixel_barrel_l%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Barrel Z Layer %i vs. incident phi;pixel phi;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_phi_pixel_barrel_layer.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

     }

// --
      // pixel endcap section
      if (ilayer < pixelEndcapDisks){
        snprintf(name, bufferLength, "pullPhi_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Phi vs incident normal Disk %i;angle w/ normal;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal_endcap, max_normal_endcap, 100, -5, 5));
        snprintf(name, bufferLength, "residualPhi_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Phi vs incident normal Disk %i;angle w/ normal;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_normal_endcap, max_normal_endcap, 200, -0.5, 0.5));

        snprintf(name, bufferLength, "pullPhi_pixel_eta_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Phi vs incident eta Disk %i;pixel eta;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta_endcap, max_eta_endcap, 100, -5, 5));
        snprintf(name, bufferLength, "residualPhi_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Phi vs incident eta Disk %i;pixel eta;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, 25, min_eta_endcap, max_eta_endcap, 200, -0.5, 0.5));

        snprintf(name, bufferLength, "pullPhi_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Phi vs incident phi Disk %i;pixel phi;pull (r-phi)" ,ilayer);
        m_pullPhi_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 100, -5, 5));

        snprintf(name, bufferLength, "residualPhi_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Phi vs incident phi Disk %i;pixel phi;residual (r-phi)" ,ilayer);
        m_residualPhi_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 200, -0.5, 0.5));
// --
        snprintf(name, bufferLength, "pullZ_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Z vs incident normal Disk %i;angle w/ normal;pull (r-z)" ,ilayer);
        m_pullZ_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_normal_endcap, max_normal_endcap, 100, -5, 5));

        snprintf(name, bufferLength, "residualZ_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Z vs incident normal Disk %i;angle w/ normal;residual (r-z)" ,ilayer);
        m_residualZ_vs_incident_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_normal_endcap, max_normal_endcap, 200, -1, 1));

        snprintf(name, bufferLength, "pullZ_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Z vs incident eta Disk %i;pixel eta;pull (r-z)" ,ilayer);
        m_pullZ_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_eta_endcap, max_eta_endcap, 100, -5, 5));

        snprintf(name, bufferLength, "residualZ_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Z vs incident eta Disk %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_eta_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_eta_endcap, max_eta_endcap, 200, -1, 1));

        snprintf(name, bufferLength, "pullZ_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull: Pixel Endcap Z vs incident phi Disk %i;pixel phi;pull (r-z)" ,ilayer);
        m_pullZ_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 100, -5, 5));

        snprintf(name, bufferLength, "residualZ_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual: Pixel Endcap Z vs incident phi Disk %i;pixel eta;residual (r-z)" ,ilayer);
        m_residualZ_vs_phi_pixel_endcap_d.push_back(create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi_endcap, max_phi_endcap, 200, -1, 1));

        snprintf(name, bufferLength, "pullMeanPhi_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthPhi_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanPhi_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthPhi_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanPhi_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull mean: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;pull (r-phi) mean" ,ilayer);
        m_pullMeanPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullWidthPhi_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;pull (r-phi) width" ,ilayer);
        m_pullWidthPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthPhi_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident normal;angle w/ normal;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        snprintf(name, bufferLength, "residualWidthPhi_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident eta;pixel eta;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanPhi_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;residual (r-phi) mean" ,ilayer);
        m_residualMeanPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthPhi_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap R-Phi Disk %i vs. incident phi;pixel phi;residual (r-phi) width" ,ilayer);
        m_residualWidthPhi_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        snprintf(name, bufferLength, "pullWidthZ_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        snprintf(name, bufferLength, "pullWidthZ_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "pullMeanZ_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;pull (r-z) mean" ,ilayer);
        m_pullMeanZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
        
        snprintf(name, bufferLength, "pullWidthZ_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Pull width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;pull (r-z) width" ,ilayer);
        m_pullWidthZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthZ_vs_incident_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap Z Disk %i vs. incident normal;angle w/ normal;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_incident_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualWidthZ_vs_eta_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap Z Disk %i vs. incident eta;pixel eta;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_eta_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

        snprintf(name, bufferLength, "residualMeanZ_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual mean: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;residual (r-z) mean" ,ilayer);
        m_residualMeanZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));
      
        snprintf(name, bufferLength, "residualWidthZ_vs_phi_pixel_endcap_d%i", ilayer);
        snprintf(title, bufferLength, "Residual width: Pixel Endcap Z Disk %i vs. incident phi;pixel phi;residual (r-z) width" ,ilayer);
        m_residualWidthZ_vs_phi_pixel_endcap_disk.push_back(create_registeredTH1F(pull_parent, name, title, 100, -5, 5));

      }

      // eta binned regions
      string etaBinLabels[5];
      etaBinLabels[0] = "0 < eta <= 0.5";
      etaBinLabels[1] = "0.5 < eta <= 1.0";
      etaBinLabels[2] = "1.0 < eta <= 1.5";
      etaBinLabels[3] = "1.5 < eta <= 2.0";
      etaBinLabels[4] = "2.0 < eta <= 2.5";
      for (int iEtaBin=0; iEtaBin<5; iEtaBin++) {
          if (ilayer < pixelBarrelLayers){
            snprintf(name, bufferLength, "pullPhi_phi_pixel_endcap_d%i_etaBin%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Pull: Pixel Barrel Phi vs incident phi Layer %i, %s;pixel phi;pull (r-phi)" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi, max_phi, 200, -5, 5);
            snprintf(name, bufferLength, "residualPhi_phi_pixel_endcap_d%i_etaBin%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Residual: Pixel Barrel Phi vs incident phi Layer %i, %s;pixel phi;residual (r-phi)" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH2F(pull_shift, name, title, nbins_eta, min_phi, max_phi, 200, -1, 1);
            // One profile each for the value and the width
            // The axis is meaningless, as it's projected from the 2d hist
            snprintf(name, bufferLength, "pullMeanPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Pull mean: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;pull (r-phi) mean" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            snprintf(name, bufferLength, "pullWidthPhi_vs_incident_pixel_barrel_l%i_etaBin_%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Pull width: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;pull (r-phi) width" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            snprintf(name, bufferLength, "residualMeanPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Residual mean: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;residual (r-phi) mean" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
            snprintf(name, bufferLength, "residualWidthPhi_vs_incident_pixel_barrel_l%i_etaBin%i", ilayer, iEtaBin);
            snprintf(title, bufferLength, "Residual width: Pixel Barrel R-Phi Layer %i vs. incident phi %s;pixel phi;residual(r-phi) width" ,ilayer, etaBinLabels[iEtaBin].c_str());
            m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer] = create_registeredTH1F(pull_parent, name, title, 100, -5, 5);
         }
      }
// --
    } // Pixel Layer section done 

    m_residualx_sct_barrel = create_registeredTH1F(al_shift, "residualx_sct_barrel", "Residual: SCT Barrel X;x residual(#mum)",140,-70,70);
    m_residualx_sct_endcap = create_registeredTH1F(al_shift, "residualx_sct_endcap", "Residual: SCT Endcap X;x residual(#mum)",140,-70,70);

    // residuals by cluster
    m_residualx_pixel_barrel_1hit = create_registeredTH1F(al_expert, "residualx_pixel_barrel_1hit", "Residual: Pixel Barrel X, 1 hit;x residual(#mum)",120,-50,50);
    m_residualy_pixel_barrel_1hit = create_registeredTH1F(al_expert, "residualy_pixel_barrel_1hit", "Residual: Pixel Barrel Y, 1 hit;y residual(#mum)",120,-300,300);
    m_residualx_pixel_endcap_1hit = create_registeredTH1F(al_expert, "residualx_pixel_endcap_1hit", "Residual: Pixel Endcap X, 1 hit;x residual(#mum)",120,-50,50);
    m_residualy_pixel_endcap_1hit = create_registeredTH1F(al_expert, "residualy_pixel_endcap_1hit", "Residual: Pixel Endcap Y, 1 hit;y residual(#mum)",120,-300,300);
    m_residualx_sct_barrel_1hit = create_registeredTH1F(al_expert, "residualx_sct_barrel_1hit", "Residual: SCT Barrel X, 1 hit;x residual(#mum)",140,-70,70);
    m_residualx_sct_endcap_1hit = create_registeredTH1F(al_expert, "residualx_sct_endcap_1hit", "Residual: SCT Endcap X, 1 hit;x residual(#mum)",140,-70,70);
    //
    m_residualx_pixel_barrel_2ormorehits = create_registeredTH1F(al_expert, "residualx_pixel_barrel_2ormorehits", "Residual: Pixel Barrel X, 2+ hits;x residual(#mum)",120,-50,50);
    m_residualy_pixel_barrel_2ormorehits = create_registeredTH1F(al_expert, "residualy_pixel_barrel_2ormorehits", "Residual: Pixel Barrel Y, 2+ hits;y residual(#mum)",120,-300,300);
    m_residualx_pixel_endcap_2ormorehits = create_registeredTH1F(al_expert, "residualx_pixel_endcap_2ormorehits", "Residual: Pixel Endcap X, 2+ hits;x residual(#mum)",120,-50,50);
    m_residualy_pixel_endcap_2ormorehits = create_registeredTH1F(al_expert, "residualy_pixel_endcap_2ormorehits", "Residual: Pixel Endcap Y, 2+ hits;y residual(#mum)",120,-300,300);
    m_residualx_sct_barrel_2ormorehits = create_registeredTH1F(al_expert, "residualx_sct_barrel_2ormorehits", "Residual: SCT Barrel X, 2+ hits;x residual(#mum)",140,-70,70);
    m_residualx_sct_endcap_2ormorehits = create_registeredTH1F(al_expert, "residualx_sct_endcap_2ormorehits", "Residual: SCT Endcap X, 2+ hits;x residual(#mum)",140,-70,70);

    // the number of SCT layers and disks
    int sctBarrelLayers = m_SCT_Mgr->numerology().numLayers();

    for (int ilayer=0; ilayer<sctBarrelLayers; ilayer++) {
      char name[200];
      char title[200];
      snprintf(name, bufferLength, "residualx_sct_barrel_l%i", ilayer);
      snprintf(title, bufferLength, "Residual: SCT Barrel X L%i;x residual(#mum)" ,ilayer);
      m_residualx_sct_barrel_l.push_back(create_registeredTH1F(al_expert, name, title,140,-70,70));

      // residual pulls
      snprintf(name, bufferLength, "residualpullx_sct_barrel_l%i", ilayer);
      snprintf(title, bufferLength, "Residualpull: SCT Barrel X L%i;x pull residual" ,ilayer);
      m_residualpullx_sct_barrel_l.push_back(create_registeredTH1F(al_expert, name, title, 100, -5, 5));
    }

    int sctEndcapDisks  = m_SCT_Mgr->numerology().numDisks();

    for (int ilayer=0; ilayer<sctEndcapDisks; ilayer++) {
      char name[200];
      char title[200];
      snprintf(name, bufferLength, "residualx_sct_endcap_d%i", ilayer);
      snprintf(title, bufferLength, "Residual: SCT Endcap X L%i;x residual(#mum)" ,ilayer);
      m_residualx_sct_endcap_d.push_back(create_registeredTH1F(al_expert, name, title,140,-70,70));

      // residual pulls
      snprintf(name, bufferLength, "residualpullx_sct_endcap_d%i", ilayer);
      snprintf(title, bufferLength, "Residualpull: SCT Endcap X L%i;x pull residual" ,ilayer);
      m_residualpullx_sct_endcap_d.push_back(create_registeredTH1F(al_expert, name, title, 100,-5,5));
    }

    m_residualx_trt_barrel = create_registeredTH1F(al_shift, "residualx_trt_barrel", "Residual: TRT Barrel X;x residual(#mum)",200,-500,500);
    m_residualx_trt_endcap = create_registeredTH1F(al_shift, "residualx_trt_endcap", "Residual: TRT Endcap X;x residual(#mum)",200,-500,500);

    // residual pulls
    m_residualpullx_pixel_barrel = create_registeredTH1F(al_shift, "residualpullx_pixel_barrel", "Residualpull: Pixel Barrel X;x pull residual",100,-5,5);
    m_residualpully_pixel_barrel = create_registeredTH1F(al_shift, "residualpully_pixel_barrel", "Residualpull: Pixel Barrel Y;y pull residual",100,-5,5);
    m_residualpullx_pixel_endcap = create_registeredTH1F(al_shift, "residualpullx_pixel_endcap", "Residualpull: Pixel Endcap X;x pull residual",100,-5,5);
    m_residualpully_pixel_endcap = create_registeredTH1F(al_shift, "residualpully_pixel_endcap", "Residualpull: Pixel Endcap Y;y pull residual",100,-5,5);

    m_residualpullx_sct_barrel = create_registeredTH1F(al_shift, "residualpullx_sct_barrel", "Residualpull: SCT Barrel X;x pull residual",100,-5,5);
    m_residualpullx_sct_endcap = create_registeredTH1F(al_shift, "residualpullx_sct_endcap", "Residualpull: SCT Endcap X;x pull residual",100,-5,5);


    m_residualpullx_trt_barrel = create_registeredTH1F(al_shift, "residualpullx_trt_barrel", "Residualpull: TRT Barrel X;x pull residual",100,-5,5);
    m_residualpullx_trt_endcap = create_registeredTH1F(al_shift, "residualpullx_trt_endcap", "Residualpull: TRT Endcap X;x pull residual",100,-5,5);

    // hit information from TrackSummary object
    // IBL 
    m_HitContent_NiblHits = create_registeredTH1F(al_expert,"HitContent_NIBLHits","Number of IBL clus.;Num. Clus.",5,-0.5,4.5);
    m_HitContent_NiblOutliers = create_registeredTH1F(al_expert,"HitContent_NIBLOutliers","Number of IBL outliers;Num. Outliers",5,-0.5,4.5);
    m_HitContent_NiblSharedHits = create_registeredTH1F(al_expert,"HitContent_NIBLSharedHits","Number of shared IBL clus.;Num. Shared. Clus.",5,-0.5,4.5);     
    // B-layer
    m_HitContent_NBlayerHits = create_registeredTH1F(al_expert,"HitContent_NBlayerHits","Number of B-layer clus.;Num. Clus.",5,-0.5,4.5);
    m_HitContent_NBlayerOutliers = create_registeredTH1F(al_expert,"HitContent_NBlayerOutliers","Number of B-layer outliers;Num. Outliers",5,-0.5,4.5);
    m_HitContent_NBlayerSharedHits = create_registeredTH1F(al_expert,"HitContent_NBlayerSharedHits","Number of shared B-layer clus.;Num. Shared. Clus.",5,-0.5,4.5);
    // pixels
    m_HitContent_NPixelHits = create_registeredTH1F(al_expert,"HitContent_NPixelHits","Number of Pixel clus.;Num. Clus.",10,-0.5,9.5);
    m_HitContent_NPixelHoles = create_registeredTH1F(al_expert,"HitContent_NPixelHoles","Number of Pixel holes;Num. Holes",10,-0.5,9.5);
    m_HitContent_NPixelOutliers = create_registeredTH1F(al_expert,"HitContent_NPixelOutliers","Number of Pixel outliers;Num. Outliers",10,-0.5,9.5);
    m_HitContent_NPixelContribLayers = create_registeredTH1F(al_expert,"HitContent_NPixelContribLayers","Number of contributed Pixel layers;Num. Layers",5,-0.5,4.5);
    m_HitContent_NPixelSharedHits = create_registeredTH1F(al_expert,"HitContent_NPixelSharedHits","Number of shared Pixel clus.;Num. Shared. Clus.",10,-0.5,9.5);
    m_HitContent_NPixelGangedHits = create_registeredTH1F(al_expert,"HitContent_NPixelGangedHits","Number of ganged Pixel clus.;Num. Clus.",10,-0.5,9.5);
    m_HitContent_NPixelGangedHitsFlaggedFakes = create_registeredTH1F(al_expert,"HitContent_NPixelGangedHitsFlaggedFakes","Number of ganged flagged fake Pixel clus.;Num. Clus.",10,-0.5,9.5);
    // SCT
    m_HitContent_NSCTHits = create_registeredTH1F(al_expert,"HitContent_NSCTHits","Number of SCT clus.;Num. Clus.",21,-0.5,20.5);
    m_HitContent_NSCTHoles = create_registeredTH1F(al_expert,"HitContent_NSCTHoles","Number of SCT holes;Num. Holes",21,-0.5,20.5);
    m_HitContent_NSCTOutliers = create_registeredTH1F(al_expert,"HitContent_NSCTOutliers","Number of SCT Outliers;Num. Outliers",21,-0.5,20.5);
    m_HitContent_NSCTDoubleHoles = create_registeredTH1F(al_expert,"HitContent_NSCTDoubleHoles","Number of SCT double holes;Num. Double Holes",21,-0.5,20.5);
    m_HitContent_NSCTSharedHits = create_registeredTH1F(al_expert,"HitContent_NSCTSharedHits","Number of shared SCT clus.;Num. Shared. Clus.",21,-0.5,20.5);
    // TRT
    m_HitContent_NTRTHits = create_registeredTH1F(al_expert,"HitContent_NTRTHits","Number of TRT clus.;Num. Hits",51,-0.5,50.5);
    m_HitContent_NTRTOutliers = create_registeredTH1F(al_expert,"HitContent_NTRTOutliers","Number of TRT outliers;Num. Outliers",51,-0.5,50.5);
    m_HitContent_NTRTHighThresholdHits = create_registeredTH1F(al_expert,"HitContent_NTRTHighThresholdHits","Number of TRT high threshold clus.;Num. Hits",51,-0.5,50.5);
    m_HitContent_NTRTHighThresholdOutliers = create_registeredTH1F(al_expert,"HitContent_NTRTHighThresholdOutliers","Number of TRT High Threshold outliers;Num. Outliers",51,-0.5,50.5);

    // fake tracks
    // IBL 
    m_Fake_HitContent_NiblHits = create_registeredTH1F(al_expert,"Fake_HitContent_NIBLHits","Number of IBL clus. for fake tracks;Num. Clus.",5,-0.5,4.5);
    m_Fake_HitContent_NiblOutliers = create_registeredTH1F(al_expert,"Fake_HitContent_NIBLOutliers","Number of IBL outliers for fake tracks;Num. Outliers",5,-0.5,4.5);
    m_Fake_HitContent_NiblSharedHits = create_registeredTH1F(al_expert,"Fake_HitContent_NIBLSharedHits","Number of shared IBL clus. for fake tracks;Num. Shared. Clus.",5,-0.5,4.5);
    // B-layer
    m_Fake_HitContent_NBlayerHits = create_registeredTH1F(al_expert,"Fake_HitContent_NBlayerHits","Number of B-layer clus. for fake tracks;Num. Clus.",5,-0.5,4.5);
    m_Fake_HitContent_NBlayerOutliers = create_registeredTH1F(al_expert,"Fake_HitContent_NBlayerOutliers","Number of B-layer outliers for fake tracks;Num. Outliers",5,-0.5,4.5);
    m_Fake_HitContent_NBlayerSharedHits = create_registeredTH1F(al_expert,"Fake_HitContent_NBlayerSharedHits","Number of shared B-layer clus. for fake tracks;Num. Shared. Clus.",5,-0.5,4.5);
    // pixels
    m_Fake_HitContent_NPixelHits = create_registeredTH1F(al_expert,"Fake_HitContent_NPixelHits","Number of Pixel clus. for fake tracks;Num. Clus.",10,-0.5,9.5);
    m_Fake_HitContent_NPixelHoles = create_registeredTH1F(al_expert,"Fake_HitContent_NPixelHoles","Number of Pixel holes for fake tracks;Num. Holes",10,-0.5,9.5);
    m_Fake_HitContent_NPixelOutliers = create_registeredTH1F(al_expert,"Fake_HitContent_NPixelOutliers","Number of Pixel outliers for fake tracks;Num. Outliers",10,-0.5,9.5);
    m_Fake_HitContent_NPixelContribLayers = create_registeredTH1F(al_expert,"Fake_HitContent_NPixelContribLayers","Number of contributed Pixel layers for fake tracks;Num. Layers",5,-0.5,4.5);
    m_Fake_HitContent_NPixelSharedHits = create_registeredTH1F(al_expert,"Fake_HitContent_NPixelSharedHits","Number of shared Pixel clus. for fake tracks;Num. Shared. Clus.",10,-0.5,9.5);
    // SCT
    m_Fake_HitContent_NSCTHits = create_registeredTH1F(al_expert,"Fake_HitContent_NSCTHits","Number of SCT clus. for fake tracks;Num. Clus.",21,-0.5,20.5);
    m_Fake_HitContent_NSCTHoles = create_registeredTH1F(al_expert,"Fake_HitContent_NSCTHoles","Number of SCT holes for fake tracks;Num. Holes",21,-0.5,20.5);
    m_Fake_HitContent_NSCTOutliers = create_registeredTH1F(al_expert,"Fake_HitContent_NSCTOutliers","Number of SCT Outliers for fake tracks;Num. Outliers",21,-0.5,20.5);
    m_Fake_HitContent_NSCTDoubleHoles = create_registeredTH1F(al_expert,"Fake_HitContent_NSCTDoubleHoles","Number of SCT double holes for fake tracks;Num. Double Holes",21,-0.5,20.5);
    m_Fake_HitContent_NSCTSharedHits = create_registeredTH1F(al_expert,"Fake_HitContent_NSCTSharedHits","Number of shared SCT clus. for fake tracks;Num. Shared. Clus.",21,-0.5,20.5);
    m_Fake_NSiHit = create_registeredTH1F(al_expert,"Fake_NSiHit","Number of shared Si clus. for fake tracks;Num. Si Clus.",21,-0.5,20.5);
    m_Fake_Chi2 = create_registeredTH1F(al_expert,"Fake_Chi2","chi2 for fake tracks;chi2",200,0,10);
    m_Fake_NPixvNSct = create_registeredTH2F(al_expert,"Fake_NPixvNSct","pixel vs sct hits for fake tracks;nsct;npix",21,-0.5,20.5,21,-0.5,20.5);

    // good tracks
    // IBL 
    m_Good_HitContent_NiblHits = create_registeredTH1F(al_expert,"Good_HitContent_NIBLHits","Number of IBL clus. for good tracks;Num. Clus.",5,-0.5,4.5);
    m_Good_HitContent_NiblOutliers = create_registeredTH1F(al_expert,"Good_HitContent_NIBLOutliers","Number of IBL outliers for good tracks;Num. Outliers",5,-0.5,4.5);
    m_Good_HitContent_NiblSharedHits = create_registeredTH1F(al_expert,"Good_HitContent_NIBLSharedHits","Number of shared IBL clus. for good tracks;Num. Shared. Clus.",5,-0.5,4.5);
    // B-layer
    m_Good_HitContent_NBlayerHits = create_registeredTH1F(al_expert,"Good_HitContent_NBlayerHits","Number of B-layer clus. for good tracks;Num. Clus.",5,-0.5,4.5);
    m_Good_HitContent_NBlayerOutliers = create_registeredTH1F(al_expert,"Good_HitContent_NBlayerOutliers","Number of B-layer outliers for good tracks;Num. Outliers",5,-0.5,4.5);
    m_Good_HitContent_NBlayerSharedHits = create_registeredTH1F(al_expert,"Good_HitContent_NBlayerSharedHits","Number of shared B-layer clus. for good tracks;Num. Shared. Clus.",5,-0.5,4.5);
    // pixels
    m_Good_HitContent_NPixelHits = create_registeredTH1F(al_expert,"Good_HitContent_NPixelHits","Number of Pixel clus. for good tracks;Num. Clus.",10,-0.5,9.5);
    m_Good_HitContent_NPixelHoles = create_registeredTH1F(al_expert,"Good_HitContent_NPixelHoles","Number of Pixel holes for good tracks;Num. Holes",10,-0.5,9.5);
    m_Good_HitContent_NPixelOutliers = create_registeredTH1F(al_expert,"Good_HitContent_NPixelOutliers","Number of Pixel outliers for good tracks;Num. Outliers",10,-0.5,9.5);
    m_Good_HitContent_NPixelContribLayers = create_registeredTH1F(al_expert,"Good_HitContent_NPixelContribLayers","Number of contributed Pixel layers for good tracks;Num. Layers",5,-0.5,4.5);
    m_Good_HitContent_NPixelSharedHits = create_registeredTH1F(al_expert,"Good_HitContent_NPixelSharedHits","Number of shared Pixel clus. for good tracks;Num. Shared. Clus.",10,-0.5,9.5);
    // SCT
    m_Good_HitContent_NSCTHits = create_registeredTH1F(al_expert,"Good_HitContent_NSCTHits","Number of SCT clus. for good tracks;Num. Clus.",21,-0.5,20.5);
    m_Good_HitContent_NSCTHoles = create_registeredTH1F(al_expert,"Good_HitContent_NSCTHoles","Number of SCT holes for good tracks;Num. Holes",21,-0.5,20.5);
    m_Good_HitContent_NSCTOutliers = create_registeredTH1F(al_expert,"Good_HitContent_NSCTOutliers","Number of SCT Outliers for good tracks;Num. Outliers",21,-0.5,20.5);
    m_Good_HitContent_NSCTDoubleHoles = create_registeredTH1F(al_expert,"Good_HitContent_NSCTDoubleHoles","Number of SCT double holes for good tracks;Num. Double Holes",21,-0.5,20.5);
    m_Good_HitContent_NSCTSharedHits = create_registeredTH1F(al_expert,"Good_HitContent_NSCTSharedHits","Number of shared SCT clus. for good tracks;Num. Shared. Clus.",21,-0.5,20.5);
    m_Good_NSiHit = create_registeredTH1F(al_expert,"Good_NSiHit","Number of shared Si clus. for good tracks;Num. Si Clus.",21,-0.5,20.5);
    m_Good_Chi2 = create_registeredTH1F(al_expert,"Good_Chi2","chi2 for good tracks;chi2",200,0,10);
    m_Good_NPixvNSct = create_registeredTH2F(al_expert,"Good_NPixvNSct","pixel vs sct hits for good tracks;nsct;npix",21,-0.5,20.5,21,-0.5,20.5);

    // eta dependence of hit content
    // IBL 
    m_HitContent_vs_eta_NiblHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NIBLHits","Number of IBL hits vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NiblOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NIBLOutliers","Number of IBL outliers  vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NiblSharedHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NIBLSharedHits","Number of shared IBL hits  vs eta;#eta;Num. Shared. Clus.",nbins_eta,min_eta,max_eta,0,100); 
    // B-layer
    m_HitContent_vs_eta_NBlayerHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NBlayerHits","Number of B-layer hits vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NBlayerOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NBlayerOutliers","Number of B-layer outliers  vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NBlayerSharedHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NBlayerSharedHits","Number of shared B-layer hits  vs eta;#eta;Num. Shared. Clus.",nbins_eta,min_eta,max_eta,0,100);
    // pixels
    m_HitContent_vs_eta_NPixelHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelHits","Number of Pixel hits  vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NPixelHoles = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelHoles","Number of Pixel holes  vs eta;#eta;Num. Holes",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_phi_NPixelHoles = create_registeredTProfile(al_expert, "HitContent_vs_phi_NPixelHoles","Number of Pixel holes vs phi;#phi;Num. Holes",50,0.0,6.3,0,100);
    m_HitContent_vs_eta_NPixelOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelOutliers","Number of Pixel Outliers  vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);

    m_HitContent_vs_eta_NPixelContribLayers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelContribLayers","Number of contributed Pixel layers  vs eta;#eta;Num. Layers",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NPixelSharedHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelSharedHits","Number of shared Pixel hits vs eta;#eta;Num. Shared. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NPixelGangedHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelGangedHits","Number of ganged Pixel hits vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NPixelGangedHitsFlaggedFakes = create_registeredTProfile(al_expert, "HitContent_vs_eta_NPixelGangedHitsFlaggedFakes","Number of ganged flagged fake Pixel hits  vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    // SCT
    m_HitContent_vs_eta_NSCTHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NSCTHits","Number of SCT hits vs eta;#eta;Num. Clus.",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NSCTHoles = create_registeredTProfile(al_expert, "HitContent_vs_eta_NSCTHoles","Number of SCT holes vs eta;#eta;Num. Holes",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NSCTOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NSCTOutliers","Number of SCT Outliers vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NSCTDoubleHoles = create_registeredTProfile(al_expert, "HitContent_vs_eta_NSCTDoubleHoles","Number of SCT double holes vs eta;#eta;Num. Double Holes",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NSCTSharedHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NSCTSharedHits","Number of shared SCT hits vs eta;#eta;Num. Shared. Clus.",nbins_eta,min_eta,max_eta,0,100);
    // TRT
    m_HitContent_vs_eta_NTRTHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NTRTHits","Number of TRT hits vs eta;#eta;Num. Hits",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NTRTOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NTRTOutliers","Number of TRT outliers vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NTRTHighThresholdHits = create_registeredTProfile(al_expert, "HitContent_vs_eta_NTRTHighThresholdHits","Number of TRT high threshold hits vs eta;#eta;Num. Hits",nbins_eta,min_eta,max_eta,0,100);
    m_HitContent_vs_eta_NTRTHighThresholdOutliers = create_registeredTProfile(al_expert, "HitContent_vs_eta_NTRTHighThresholdOutliers","Number of TRT High Threshold outliers vs eta;#eta;Num. Outliers",nbins_eta,min_eta,max_eta,0,100);

    // Look for outliers that might fake vertices for b-tagging
    m_nTrackpT1_d0siggt3 = create_registeredTH1F(al_expert,"nTrackpT1_d0siggt3","Number of tracks",1000,0,1000);
    m_nTrackpT2_d0siggt3 = create_registeredTH1F(al_expert,"nTrackpT2_d0siggt3","Number of tracks",1000,0,1000);
    m_nTrackpT1_d0siggt5 = create_registeredTH1F(al_expert,"nTrackpT1_d0siggt5","Number of tracks",1000,0,1000);
    m_nTrackpT2_d0siggt5 = create_registeredTH1F(al_expert,"nTrackpT2_d0siggt5","Number of tracks",1000,0,1000);

    m_d0sig_trackpT1 = create_registeredTH1F(al_expert,"d0sig_trackpT1","d0 significance (pt>1); d0 sig",100,-25,25);
    m_d0sig_trackpT2 = create_registeredTH1F(al_expert,"d0sig_trackpT2","d0 significance (pt>2); d0 sig",100,-25,25);
    m_d0sig_trackpT5 = create_registeredTH1F(al_expert,"d0sig_trackpT5","d0 significance (pt>5); d0 sig",100,-25,25);
    m_d0sig_trackpT10 = create_registeredTH1F(al_expert,"d0sig_trackpT10","d0 significance (pt>10); d0 sig",100,-25,25);
    m_d0sig_pt1_sharedBL = create_registeredTH1F(al_expert,"d0sig_pt1_sharedBL","d0 significance (pt>1); d0 sig",100,-25,25);
    m_d0sig_pt1_sharedPIX = create_registeredTH1F(al_expert,"d0sig_pt1_sharedPIX","d0 significance (pt>1); d0 sig",100,-25,25);
    m_d0sig_pt1_sharedSCT = create_registeredTH1F(al_expert,"d0sig_pt1_sharedSCT","d0 significance (pt>1); d0 sig",100,-25,25);

    m_nprimperevent = create_registeredTH1F(al_expert,"nprimpev","Number of primaries per event with |eta<3|; nprim",101,-0.5,100.5);
    m_nsecperevent = create_registeredTH1F(al_expert,"nsevpev","Number of primaries per event with |eta<3|; nsec",101,-0.5,100.5);
    m_nprimperevent05 = create_registeredTH1F(al_expert,"nprimpev05","Number of primaries per event with |eta<3| and pt>0.5 GeV; nprim>0.5 GeV",101,-0.5,100.5);
    m_nsecperevent05 = create_registeredTH1F(al_expert,"nsecpev05","Number of primaries per event with |eta<3| ant pt>0.5 GeV; nsec>0.5 GeV",101,-0.5,100.5);

    if (m_doHitBasedMatching) {
      //HitBasedMatching plots CB
      m_patternRecoInfo = create_registeredTH1F(al_expert,"patternRecoInfo","patternRecoInfo",40,0,40);
      m_patternRecoInfo_npat = create_registeredTH1F(al_expert,"patternRecoInfo_npat","patternRecoInfo_npat",40,0,40);
      m_patternRecoInfo_p1p2 = create_registeredTH2F(al_expert,"patternRecoInfo_p1p2","patternRecoInfo_p1p2",40,0,40,40,0,40);
      m_nTrks = create_registeredTH1F(al_expert,"nTrks","nTrks",100,0,100);
      m_nPixelTrks = create_registeredTH1F(al_expert,"nPixelTrks","nPixelTrks",100,0,100);
      m_nSCTTrks = create_registeredTH1F(al_expert,"nSCTTrks","nSCTTrks",100,0,100);
      m_nTRTTrks = create_registeredTH1F(al_expert,"nTRTTrks","nTRTTrks",100,0,100);

      m_Pix_Track_Etaeff = create_registeredTProfile(al_expert, "pix_track_hit_match_eta","Efficiency of hit Based matching Track hits to Pixel hits as a function of #eta",m_trackEtaBins,-2.5,2.5,0,1);
      m_Pix_Track_Phieff = create_registeredTProfile(al_expert, "pix_track_hit_match_phi","Efficiency of hit Based matching Track hits to Pixel hits as a function of #phi",m_trackEtaBins,-2.5,2.5,0,1);
      m_Pix_Track_Pteff = create_registeredTProfile(al_expert, "pix_track_hit_match_pt","Efficiency of hit Based matching Track hits to Pixel hits as a function of p_{T}",m_trackEtaBins,0,100, 0,1);
      m_Pix_Track_Ptlow_eff = create_registeredTProfile(al_expert, "pix_track_hit_match_pt_low","Efficiency of hit Based matching Track hits to Pixel hits as a function of p_{T}",m_trackEtaBins,0,10, 0,1);
      m_SCT_Track_Etaeff = create_registeredTProfile(al_expert, "sct_track_hit_match_eta","Efficiency of hit Based matching Track hits to SCT hits as a function of #eta",m_trackEtaBins,-2.5,2.5,0,1);
      m_SCT_Track_Phieff = create_registeredTProfile(al_expert, "sct_track_hit_match_phi","Efficiency of hit Based matching Track hits to SCT hits as a function of #phi",m_trackEtaBins,-m_Pi,m_Pi, 0,1);
      m_SCT_Track_Pteff = create_registeredTProfile(al_expert, "sct_track_hit_match_pt","Efficiency of hit Based matching Track hits to SCT hits as a function of p_{T}",m_trackEtaBins,0,100, 0,1);
      m_SCT_Track_Ptlow_eff = create_registeredTProfile(al_expert, "sct_track_hit_match_pt_low","Efficiency of hit Based matching Track hits to SCT hits as a function of p_{T}",m_trackEtaBins,0,10, 0,1);

      m_TRT_Track_Etaeff = create_registeredTProfile(al_expert, "trt_track_hit_match_eta","Efficiency of hit Based matching Track hits to TRT hits as a function of #eta",m_trackEtaBins,-2.5,2.5,0,1);
      m_TRT_Track_Phieff = create_registeredTProfile(al_expert, "trt_track_hit_match_phi","Efficiency of hit Based matching Track hits to TRT hits as a function of #phi",m_trackEtaBins,-m_Pi,m_Pi, 0,1);
      m_TRT_Track_Pteff = create_registeredTProfile(al_expert, "trt_track_hit_match_pt","Efficiency of hit Based matching Track hits to TRT hits as a function of p_{T}",m_trackEtaBins,0,100, 0,1);
      m_TRT_Track_Ptlow_eff = create_registeredTProfile(al_expert, "trt_track_hit_match_pt_low","Efficiency of hit Based matching Track hits to TRT hits as a function of p_{T}",m_trackEtaBins,0,10, 0,1);
    }
  }
  return StatusCode::SUCCESS;
}

void IDStandardPerformance::RegisterHisto(MonGroup &mon, TH1* histo) {

  //MsgStream log( msgSvc(), name() );
  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH1 Histogram:" << endmsg;
  }
}

void IDStandardPerformance::RegisterHisto(MonGroup &mon, TProfile* histo) {

  //MsgStream log( msgSvc(), name() );
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TProfile Histogram:" << endmsg;
  }
}

void IDStandardPerformance::RegisterHisto(MonGroup &mon, TH2* histo) {

  //MsgStream log( msgSvc(), name() );
  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::ERROR) << "Cannot book TH2 Histogram:" << endmsg;
  }
}


StatusCode IDStandardPerformance::fillHistograms()
{
  //MsgStream log( msgSvc(), name() );
  // get TrackCollection
  const DataVector<Trk::Track>* trks;
  if (StatusCode::SUCCESS!=evtStore()->retrieve(trks,m_tracksName)) {
    msg(MSG::WARNING) << "Cannot find " << m_tracksName  << endmsg;
  } else {
    if (msgLvl(MSG::VERBOSE)) msg() << "Track Collection with name " << m_tracksName << " with size " << trks->size() <<" found in StoreGate" << endmsg;
  }

  // Although we select tracks using the TrackSelectionTool, we still need to get a complete TrackCollection
  // from StoreGate for use in the track-truth map, otherwise the track-truth matching is screwed up
  const TrackCollection       * RecCollection = NULL;
  StatusCode sc = StatusCode :: SUCCESS;
  sc = evtStore()->retrieve(RecCollection, m_tracksName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Track collection \"" << m_tracksName << "\" not found." << endmsg;
    return StatusCode::RECOVERABLE;
  }
  if (RecCollection)  if (msgLvl(MSG::DEBUG)) msg() << "Retrieved " << m_tracksName << " with size " << RecCollection->size() << " reconstructed tracks from storegate" << endmsg;


  bool haveTruth = m_doTruth;
  const TrackTruthCollection* TruthMap  = NULL;
  // get TrackTruthCollection
  if (haveTruth) {

    if (StatusCode::SUCCESS!=evtStore()->retrieve(TruthMap,m_tracksTruthName)) {
      msg(MSG::WARNING) << "Cannot find " << m_tracksTruthName  << endmsg;
      haveTruth = false;
    } else {
      if (msgLvl(MSG::VERBOSE)) msg() << "Track Truth Collection with name " << m_tracksTruthName << " with size " << TruthMap->size() <<" found in StoreGate" << endmsg;
    }
  }

  // get MC event collection
  const McEventCollection* SimTracks = NULL;
  if (haveTruth) {
    if (evtStore()->retrieve(SimTracks,m_truthParticleName).isFailure()) {
      std::string key = "G4Truth";
      if (evtStore()->retrieve(SimTracks,key).isFailure()) {
          key = "";
          if (evtStore()->retrieve(SimTracks,key).isFailure()) {
            msg(MSG::FATAL) << "Could not find the McEventCollection" << endmsg;
            haveTruth = false;
            return StatusCode::FAILURE;
          }
      }
      if (msgLvl(MSG::VERBOSE)) msg() << "Retrieved Truth Collection with name " << key.c_str() << endmsg;
    }
    else
      if (msgLvl(MSG::VERBOSE)) msg() << "Retrieved Truth Collection with name " << m_truthParticleName.c_str() << endmsg;
  }
    
  // get Pixel tracklet collection
  const DataVector<Trk::Track>* pixel_trks = 0;
  const DataVector<Trk::Track>* sct_trks = 0;
  const DataVector<Trk::Track>* trt_trks = 0;
  if (m_doHitBasedMatching) {
    if (evtStore()->contains< DataVector<Trk::Track> >(m_PixeltracksName) &&
        StatusCode::SUCCESS==evtStore()->retrieve(pixel_trks,m_PixeltracksName)) {
      if (msgLvl(MSG::VERBOSE)) msg() << "Track Collection with name " << m_PixeltracksName << " with size " << pixel_trks->size() <<" found in StoreGate" << endmsg;
    }

    // get SCT tracklet collection
    if (evtStore()->contains<TrackCollection>(m_SCTtracksName.key())) {
      SG::ReadHandle<TrackCollection> h_sct_trks(m_SCTtracksName);
      if(h_sct_trks.isValid()) {
	sct_trks = &*h_sct_trks;
	if (msgLvl(MSG::VERBOSE)) msg() << "Track Collection with name " << m_SCTtracksName.key() << " with size " << sct_trks->size() <<" found in StoreGate" << endmsg;
      }
    }

    // get TRT tracklet collection
    SG::ReadHandle<TrackCollection> h_trt_trks( m_TRTtracksKey );
    if ( h_trt_trks.isValid() ) {
      trt_trks = h_trt_trks.cptr();
      if (msgLvl(MSG::VERBOSE)) msg() << "Track Collection with name " << m_TRTtracksKey.key() << " with size " << trt_trks->size() <<" found in StoreGate" << endmsg;
    }
  }

  // get Truth jet collection
  const JetCollection* jetColl = NULL;
  if (m_doTrackInJet && m_doTruth) {
    if (evtStore()->contains<JetCollection>(m_truthJetCollName)){
      if (evtStore()->retrieve(jetColl,m_truthJetCollName).isFailure()) {
        msg(MSG::WARNING)
          << "No Jet Collection " << m_truthJetCollName << " found in StoreGate "
          << endmsg;
        jetColl = NULL;
      }
      else if (msgLvl(MSG::VERBOSE)) msg()<<"Retrieved Jet Collection"<<endmsg;
    }
    else {
      if (msgLvl(MSG::DEBUG)) msg() << "storeGate does not contain JetCollection with name " << m_truthJetCollName << endmsg;
    }
  }
  // WJM start: Get mu
  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  double mu_val = 0;
  if(evt.isValid()) {
    mu_val = evt->averageInteractionsPerCrossing();
  }
  m_mu->Fill(mu_val);
  // WJM end
  if (haveTruth)
    MakeTrackPlots(trks,RecCollection,TruthMap,SimTracks,jetColl);
  else
    MakeDataPlots(trks); //CB - subset of MakeTrackPlots that can be made from data
  if (m_makeHitPlots) MakeHitPlots(trks);
  if (m_doHitBasedMatching && pixel_trks && sct_trks && trt_trks) HitBasedMatching (trks,pixel_trks, sct_trks, trt_trks);

  return StatusCode::SUCCESS;
}


// Utility routine used by projectStandardProfileY: fill one bin from a 1D slice.
void SetProfileBin (Int_t i, TH1* h1, TH1* pm, TH1* pw, const char* fitfun=0, Double_t cut_tail=3.0)
{
  if (h1->GetXaxis()->TestBit(TAxis::kAxisRange)) {
    // remove range if set previously
    h1->GetXaxis()->SetRange();
    h1->ResetStats();
  }
  Double_t x=0.0, xe=0.0, e=0.0, ee=0.0;
  if (h1->GetEntries() != 0.0) {
    x=  h1->GetMean();      e=  h1->GetRMS();
    xe= h1->GetMeanError(); ee= h1->GetRMSError();
    if (fitfun) {
      TFitResultPtr r= h1->Fit (fitfun, "QS0");
      if (r.Get() && r->Status()%1000==0) {
        x=  r->Parameter(1); e=  r->Parameter(2);
        xe= r->ParError (1); ee= r->ParError (2);
      }
    } else if (cut_tail!=0.0) {
      Double_t rmin, rmax;
      if (cut_tail<0.0) { rmin=   cut_tail*e; rmax=  -cut_tail*e; }
      else              { rmin= x-cut_tail*e; rmax= x+cut_tail*e; }
      // Work round ROOT 5.34.04 change that would include the under/overflow bins
      // if the new range extends beyond the histogram edges.
      if (rmin < h1->GetBinLowEdge(1))              rmin= h1->GetBinLowEdge(1);
      if (rmax > h1->GetBinCenter(h1->GetNbinsX())) rmax= h1->GetBinCenter(h1->GetNbinsX());
      h1->SetAxisRange (rmin, rmax);
      x=  h1->GetMean();      e=  h1->GetRMS();
      xe= h1->GetMeanError(); ee= h1->GetRMSError();
    }
  }
  if (pm) {
    pm->SetBinContent (i, x);
    pm->SetBinError   (i, xe);
  }
  if (pw) {
    pw->SetBinContent (i, e);
    pw->SetBinError   (i, ee);
  }
}


// projects a TH2 into a profile, expressed as a TH1F.
// This is similar to the TH2::ProfileX method with the difference
// that only the error within 3RMS around the mean is considered in the profile.
// Specify a different range with cut_tail (>0:mean+/-RMS, <0:0+/-RMS).
// Alternatively, fit a Gaussian by specifying fitfun="gaus".
void projectStandardProfileY (const TH2* h, TH1* profMean, TH1* profWidth=0,
                              const char* fitfun=0, Double_t cut_tail=3.0)
{
  if (!h)                      return;
  if (!profMean && !profWidth) return;
  Int_t nb= h->GetNbinsX();
  Double_t xlo= h->GetXaxis()->GetXmin(), xhi= h->GetXaxis()->GetXmax();
  if (profMean)  profMean ->SetBins (nb, xlo, xhi);
  if (profWidth) profWidth->SetBins (nb, xlo, xhi);
  for (Int_t i=0; i<=nb+1; i++) {
    TH1D* h1= h->ProjectionY(Form("%s_projy_bin%d",h->GetName(),i), i, i);
    SetProfileBin (i, h1, profMean, profWidth, fitfun, cut_tail);
    delete h1;
  }
  // restore #entries, which was modified by SetBinContent in SetProfileBin
  if (profMean)  profMean ->SetEntries(h->GetEntries());
  if (profWidth) profWidth->SetEntries(h->GetEntries());
}


void projectStandardProfileY (const std::vector<TH1F*>& hvec, TH1* profMean, TH1* profWidth=0,
                              Int_t fold=0, const char* fitfun=0, Double_t cut_tail=3.0)
{
  TH1 *hx= profMean ? profMean : profWidth;
  if (!hx) return;
  Int_t nvec= hvec.size();
  if (fold>nvec) fold= nvec;
  Int_t nb= fold>0 ? fold : nvec;
  Double_t xlo= hx->GetXaxis()->GetXmin(), xhi= hx->GetXaxis()->GetXmax();
  if (profMean)  profMean ->SetBins (nb, xlo, xhi);
  if (profWidth) profWidth->SetBins (nb, xlo, xhi);
  Double_t nent= 0.0;
  for (Int_t i=0; i<nb; i++) {
    Int_t j1=i, j2=-1;
    if (fold>0) {
      j1= fold+i;
      if (j1<nvec && hvec[j1]) j2= fold-i-1;
      else                     j1= fold-i-1;
    }
    if (!(j1>=0 && hvec[j1])) continue;
    TH1F* h1= dynamic_cast<TH1F*>(hvec[j1]->Clone(Form("%s_mod",hvec[j1]->GetName())));
    if (!h1) continue;
    if (  j2>=0 && hvec[j2]) h1->Add (hvec[j2]);
    SetProfileBin (i+1, h1, profMean, profWidth, fitfun, cut_tail);
    nent += h1->GetEntries();
    delete h1;
  }
  // restore #entries, which was modified by SetBinContent in SetProfileBin
  if (profMean)  profMean ->SetEntries(nent);
  if (profWidth) profWidth->SetEntries(nent);
}


StatusCode IDStandardPerformance::procHistograms()
{

  //MsgStream log( msgSvc(), name() );
  //  if (m_idHelper) delete m_idHelper;

  if ( endOfRunFlag() ) {
 
    int halfEtaBins = m_trackEtaBins/2;
    int fold= m_plotsVsAbsEta ? halfEtaBins : 0;

    projectStandardProfileY (m_hd0,    m_d0_mean_vs_eta,    m_d0_res_vs_eta,    fold, 0, -3.0);
    projectStandardProfileY (m_hz0,    m_z0_mean_vs_eta,    m_z0_res_vs_eta,    fold, 0, -3.0);
    projectStandardProfileY (m_hz0st,  m_z0st_mean_vs_eta,  m_z0st_res_vs_eta,  fold, 0, -3.0);
    projectStandardProfileY (m_hphi,   m_phi_mean_vs_eta,   m_phi_res_vs_eta,   fold, 0, -3.0);
    projectStandardProfileY (m_hqopt,  m_qopt_mean_vs_eta,  m_qopt_res_vs_eta,  fold, 0, -3.0);
    projectStandardProfileY (m_htheta, m_theta_mean_vs_eta, m_theta_res_vs_eta, fold, 0, -3.0);

    projectStandardProfileY (m_pulld0,    m_d0_pullmean_vs_eta,    m_d0_pullres_vs_eta,    fold, "gaus");
    projectStandardProfileY (m_pullz0,    m_z0_pullmean_vs_eta,    m_z0_pullres_vs_eta,    fold, "gaus");
    //projectStandardProfileY (m_pullz0st,  m_z0st_pullmean_vs_eta,  m_z0st_pullres_vs_eta,  fold, "gaus");
    projectStandardProfileY (m_pullphi,   m_phi_pullmean_vs_eta,   m_phi_pullres_vs_eta,   fold, "gaus");
    projectStandardProfileY (m_pullqopt,  m_qopt_pullmean_vs_eta,  m_qopt_pullres_vs_eta,  fold, "gaus");
    projectStandardProfileY (m_pulltheta, m_theta_pullmean_vs_eta, m_theta_pullres_vs_eta, fold, "gaus");

    projectStandardProfileY (m_hptd0,    0, m_d0_res_vs_pt,    -1, 0, -3.0);
    projectStandardProfileY (m_hptz0,    0, m_z0_res_vs_pt,    -1, 0, -3.0);
    projectStandardProfileY (m_hptz0st,  0, m_z0st_res_vs_pt,  -1, 0, -3.0);
    projectStandardProfileY (m_hptphi,   0, m_phi_res_vs_pt,   -1, 0, -3.0);
    projectStandardProfileY (m_hptqopt,  0, m_qopt_res_vs_pt,  -1, 0, -3.0);
    projectStandardProfileY (m_hpttheta, 0, m_theta_res_vs_pt, -1, 0, -3.0);

    SetSafeMinimumMaximum(m_d0_mean_vs_eta, -0.03, 0.03);
    SetSafeMinimumMaximum(m_phi_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_qopt_mean_vs_eta, -0.01, 0.01);
    SetSafeMinimumMaximum(m_theta_mean_vs_eta, -0.001, 0.001);
    SetSafeMinimumMaximum(m_z0st_mean_vs_eta, -0.05, 0.05);

    SetSafeMinimumMaximum(m_trackeff_vs_eta, 0.65, 1.05);
    SetSafeMinimumMaximum(m_trackeff_vs_phi, 0.65, 1.05);
    SetSafeMinimumMaximum(m_trackeff_vs_pt, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_vs_pt_lowpt, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_eta, 0.5, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_phi, 0.65, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_pt, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_dr, 0.5, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_dr_lt_j50, 0.5, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_dr_gr_j100, 0.5, 1.05);
    SetSafeMinimumMaximum(m_trackinjeteff_vs_jetet, 0.65, 1.05);

    SetSafeMinimumMaximum(m_trackeff_vs_pt_loweta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_vs_pt_medeta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_vs_pt_higheta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_eta, 0.70, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_phi, 0.70, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_pt, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_pt_lowpt, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_pt_loweta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_pt_medeta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_primary_vs_pt_higheta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_secondary_vs_eta, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_secondary_vs_phi, 0.0, 1.05);
    SetSafeMinimumMaximum(m_trackeff_secondary_vs_radius, 0.0, 1.05);
    // hit efficiency: set minimum to 0.95 for barrel and 0.85 for endcap
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_ibl_barrel, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_blay_barrel, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_pix_barrel, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_sct_barrel, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_trt_barrel, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_pix_endcap, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_sct_endcap, 0.95, 1.02);
    SetSafeMinimumMaximum(m_eff_hit_vs_eta_trt_endcap, 0.95, 1.02);

    m_frac_outlier_vs_eta_blay_barrel->SetMinimum(0.0);
    m_frac_outlier_vs_eta_ibl_barrel->SetMinimum(0.0);
    m_frac_outlier_vs_eta_pix_barrel->SetMinimum(0.0);
    m_frac_outlier_vs_eta_pix_endcap->SetMinimum(0.0);
    m_frac_outlier_vs_eta_sct_barrel->SetMinimum(0.0);
    m_frac_outlier_vs_eta_sct_endcap->SetMinimum(0.0);
    m_frac_outlier_vs_eta_trt_barrel->SetMinimum(0.0);
    m_frac_outlier_vs_eta_trt_endcap->SetMinimum(0.0);

    SetSafeMinimumMaximum(m_track_badmatchrate_vs_eta, 0.0, 0.05);
    SetSafeMinimumMaximum(m_track_badmatchrate_vs_phi, 0.0, 0.05);
    SetSafeMinimumMaximum(m_track_fakerate_vs_eta, 0.0, 0.05);
    SetSafeMinimumMaximum(m_track_fakerate_vs_phi, 0.0, 0.05);
    SetSafeMinimumMaximum(m_track_notruthrate_vs_eta, 0.0, 0.05);
    SetSafeMinimumMaximum(m_track_notruthrate_vs_phi, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_eta, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_phi, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_eta, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_phi, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_dr, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_dr_lt_j50, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_dr_gr_j100, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_dr, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_dr_lt_j50, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_dr_gr_j100, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_badmatchrate_vs_jetet, 0.0, 0.05);
    SetSafeMinimumMaximum(m_trackinjet_fakerate_vs_jetet, 0.0, 0.05);
  }
  projectStandardProfileY(m_pullPhi_vs_incident_pixel
  , m_pullMeanPhi_vs_incident_pixel, m_pullWidthPhi_vs_incident_pixel);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel, 0.7, 1.3);
  projectStandardProfileY(m_residualPhi_vs_incident_pixel
  , m_residualMeanPhi_vs_incident_pixel, m_residualWidthPhi_vs_incident_pixel);
  SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel, 0, 0.5);

  projectStandardProfileY(m_pullPhi_vs_phi_pixelBarrel
  , m_pullMeanPhi_vs_phi_pixelBarrel, m_pullWidthPhi_vs_phi_pixelBarrel);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixelBarrel, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixelBarrel, 0.7, 1.3);
  projectStandardProfileY(m_pullPhi_vs_phi_pixelEndcap
  , m_pullMeanPhi_vs_phi_pixelEndcap, m_pullWidthPhi_vs_phi_pixelEndcap);
  SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixelEndcap, -0.3, 0.3);
  SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixelEndcap, 0.7, 1.3);
  
  // same procedure as before
  // the number of Pixel layers and disks
  int pixelBarrelLayers = m_PIX_Mgr->numerology().numLayers();
  int pixelEndcapDisks  = m_PIX_Mgr->numerology().numDisks();
  int nPixelLayers = pixelBarrelLayers > pixelEndcapDisks ? pixelBarrelLayers : pixelEndcapDisks;


  for (int ilayer=0; ilayer<nPixelLayers; ilayer++) {
    // ------------- residualPulls in the pixels ---------------------------------------------------
    if (ilayer < pixelBarrelLayers){
    projectStandardProfileY(m_pullPhi_vs_incident_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_incident_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel_barrel_layer[ilayer], 0.7, 1.3);
    projectStandardProfileY(m_pullPhi_vs_eta_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_eta_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_eta_pixel_barrel_layer[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_phi_pixel_barrel_l[ilayer]
      , m_pullMeanPhi_vs_phi_pixel_barrel_layer[ilayer], m_pullWidthPhi_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_barrel_layer[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualPhi_vs_incident_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_incident_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel_barrel_layer[ilayer], -0.05, 0.05);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel_barrel_layer[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_eta_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_eta_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_eta_pixel_barrel_layer[ilayer], -0.2, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_eta_pixel_barrel_layer[ilayer], -0.05, 0.3);
    projectStandardProfileY(m_residualPhi_vs_phi_pixel_barrel_l[ilayer]
      , m_residualMeanPhi_vs_phi_pixel_barrel_layer[ilayer], m_residualWidthPhi_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_barrel_layer[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_barrel_layer[ilayer], -0.05, 0.05);

    projectStandardProfileY(m_pullZ_vs_incident_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_incident_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_incident_pixel_barrel_layer[ilayer], 0.7, 1.3);
    projectStandardProfileY(m_pullZ_vs_eta_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_eta_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_eta_pixel_barrel_layer[ilayer], -1, 1);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullZ_vs_phi_pixel_barrel_l[ilayer]
      , m_pullMeanZ_vs_phi_pixel_barrel_layer[ilayer], m_pullWidthZ_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_phi_pixel_barrel_layer[ilayer], -0.5, 0.5);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualZ_vs_incident_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_incident_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_incident_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_incident_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_incident_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_eta_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_eta_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_eta_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_eta_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_eta_pixel_barrel_layer[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_phi_pixel_barrel_l[ilayer]
      , m_residualMeanZ_vs_phi_pixel_barrel_layer[ilayer], m_residualWidthZ_vs_phi_pixel_barrel_layer[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_phi_pixel_barrel_layer[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_phi_pixel_barrel_layer[ilayer], 0.5, 1.5);
    }

    if (ilayer < pixelEndcapDisks){
    projectStandardProfileY(m_pullPhi_vs_incident_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_incident_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_eta_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_eta_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_eta_pixel_endcap_disk[ilayer], -0.5, 0.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullPhi_vs_phi_pixel_endcap_d[ilayer]
      ,  m_pullMeanPhi_vs_phi_pixel_endcap_disk[ilayer],  m_pullWidthPhi_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_endcap_disk[ilayer], -1.5, 1.5);
    SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_endcap_disk[ilayer], -0.2, 1.5);

    projectStandardProfileY(m_residualPhi_vs_incident_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_incident_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_incident_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_incident_pixel_endcap_disk[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_eta_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_eta_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_eta_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_eta_pixel_endcap_disk[ilayer], -0.05, 0.05);
    projectStandardProfileY(m_residualPhi_vs_phi_pixel_endcap_d[ilayer]
      , m_residualMeanPhi_vs_phi_pixel_endcap_disk[ilayer], m_residualWidthPhi_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_endcap_disk[ilayer], -0.05, 0.2);
    SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_endcap_disk[ilayer], -0.05, 0.05);

    projectStandardProfileY(m_pullZ_vs_incident_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_incident_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
   projectStandardProfileY(m_pullZ_vs_eta_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_eta_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_eta_pixel_endcap_disk[ilayer], -0.03, 0.03);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_pullZ_vs_phi_pixel_endcap_d[ilayer]
      , m_pullMeanZ_vs_phi_pixel_endcap_disk[ilayer], m_pullWidthZ_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_pullMeanZ_vs_phi_pixel_endcap_disk[ilayer], -0.03, 0.03);
    SetSafeMinimumMaximum(m_pullWidthZ_vs_phi_pixel_endcap_disk[ilayer], 0.5, 1.5);

    projectStandardProfileY(m_residualZ_vs_incident_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_incident_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_incident_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_incident_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_incident_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_eta_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_eta_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_eta_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_eta_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_eta_pixel_endcap_disk[ilayer], 0.5, 1.5);
    projectStandardProfileY(m_residualZ_vs_phi_pixel_endcap_d[ilayer]
      , m_residualMeanZ_vs_phi_pixel_endcap_disk[ilayer], m_residualWidthZ_vs_phi_pixel_endcap_disk[ilayer]);
    SetSafeMinimumMaximum(m_residualMeanZ_vs_phi_pixel_endcap_disk[ilayer], -0.3, 0.3);
    SetSafeMinimumMaximum(m_residualWidthZ_vs_phi_pixel_endcap_disk[ilayer], 0.5, 1.5);

    }


    for (int iEtaBin=0; iEtaBin<5; iEtaBin++) {

      if (ilayer < pixelBarrelLayers){
        projectStandardProfileY(m_residualPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]
            , m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]);
        SetSafeMinimumMaximum(m_residualMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], -0.3, 0.3);
        SetSafeMinimumMaximum(m_residualWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], 0., 0.25);
       }
 
       if (ilayer < pixelBarrelLayers){
        projectStandardProfileY(m_pullPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]
            , m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer]);
        SetSafeMinimumMaximum(m_pullMeanPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], -0.5, 0.5);
        SetSafeMinimumMaximum(m_pullWidthPhi_vs_phi_pixel_barrel_etaBins[iEtaBin][ilayer], 0.5, 1.5);
       }
    }
  }
  return StatusCode::SUCCESS;
}

// BH: up to here *****
void
IDStandardPerformance::MakeTrackPlots(const DataVector<Trk::Track>* trks,
                                      const TrackCollection * RecCollection,
                                      const TrackTruthCollection* TruthMap,
                                      const McEventCollection* SimTracks,
                                      const JetCollection* jetColl) {
  // ---------------------------------------------------
  // this function makes track based plots assessing track resolutions and efficiencies w.r.t. the truth and
  // can thus only be run on MC with truth information
  // --------------------------------------------------

  //MsgStream log( msgSvc(), name() );

 //find the hard scatter genevent number
  const HepMC::GenEvent* genEventPrimary = SimTracks->at(0);
  int genEventNumberPrimary = genEventPrimary->event_number();

  float zVertex=0.0;
  int nPrimaries=0;
  // Get the primary z-vertex
  //HepGeom::Point3D<double>  primVtx(0,0,0);
  Amg::Vector3D  primVtx(0,0,0);
  // The event has a signal process vertex unless it was generated as single particles
  for (HepMC::GenEvent::particle_const_iterator it = genEventPrimary->particles_begin();
       it != genEventPrimary->particles_end(); ++it) {
    HepMC::GenParticle * const particle = *it;
    float genPt = particle->momentum().perp()/1000;
    float genEta=particle->momentum().pseudoRapidity();
    int   pdgCode         = particle->pdg_id();
    if (!particle->production_vertex() || particle->barcode()<=0 || particle->barcode()>MAXBARCODE || particle->status()%1000!=1 || fabs(genEta)>m_maxTrackEta || fabs(genEta)<m_minTrackEta || genPt < 0.1) {
      msg(MSG::DEBUG) << "Skipping GenParticle with production_vertex=" << particle->production_vertex()<<" barcode="<<particle->barcode()<< " status="<< particle->status() << " pdgID=" << pdgCode << ", eta=" << genEta << ", pT=" << genPt << endmsg;
      continue;
    }
    const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(particle);
    //    auto_ptr<const Trk::TrackParameters> generatedTrackPerigee(m_truthToTrack->makePerigeeParameters(particle));
    //    if (!generatedTrackPerigee.get())   msg(MSG::DEBUG) <<  "Unable to extrapolate genparticle to perigee!" << endmsg;
    //    else {
    if (generatedTrackPerigee){
      zVertex+=generatedTrackPerigee->parameters()[Trk::z0];
      nPrimaries++;
      delete generatedTrackPerigee;
    }
  }
  if (nPrimaries>0) zVertex/=float(nPrimaries);
  msg(MSG::DEBUG) << "Found " << nPrimaries << " primary GenParticles." << endmsg;


  m_rttMap.clear();
  recoToTruthMap::const_iterator imap;

  int nTracks=0;
  int nTrackSelected=0;
  int nTrackRejected=0;

  int ntrackpt1d0sig3=0;
  int ntrackpt2d0sig3=0;
  int ntrackpt1d0sig5=0;
  int ntrackpt2d0sig5=0;


  int nBarCode0=0;
  int nBarCode0Match=0;
  int nBarCode100k=0;
  int nBarCodeGood=0;
  int nBadMatch=0;
  int nLowProb=0;


  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
  DataVector<Trk::Track>::const_iterator trksItr2  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE2 = trks->end();
  for (; trksItr != trksItrE; ++trksItr) {

    // ---------------------------------------------
    //
    // analyze the hit content of tracks
    //
    // ---------------------------------------------

    nTracks++;

    //int trkAuthor = (*trksItr)->info().author();
    //if (msgLvl(MSG::VERBOSE)) msg() <<(*trksItr)->info().dumpInfo()<< endmsg;

    // ------------------------
    // track selection tool
    // ------------------------
    if (m_useTrackSelection) {
      if (m_trackSelectorTool->decision(**trksItr)) {

        if (msgLvl(MSG::VERBOSE)) {
          msg() << "track selected!"<<endmsg;
        }
      } else {
        nTrackRejected++;
        if (msgLvl(MSG::VERBOSE)) {
          msg() << "track rejected!"<<endmsg;
        }
        continue;
      }
    }
    if (msgLvl(MSG::VERBOSE)) msg() << "Selected the track!" << endmsg;


    if (msgLvl(MSG::VERBOSE)) {
      msg() << "Found a track!" << endmsg;
    }
    float trkd0          = -999;
    float trkz0          = -999;
    float trkphi         = -999;
    float trktheta       = -999;
    float trketa         = -999;
    float trkpt          = -999;
    //float trkcot         = -999;
    float trkqOverPt     = -999;
    float charge         = 0;
    float trkd0c         = -999;
    float trkd0err          = -999;
    float trkz0err          = -999;
    float trkphierr         = -999;
    float trkthetaerr       = -999;
    //float trkcoterr         = -999;
    float trkqopterr     = -999;

    // get fit quality and chi2 probability of track
    //chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();
    const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
    if (startPerigee == NULL) {
      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee => skip track"<<endmsg;
      continue;
    }
    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );

    if (!startPerigee->covariance()) {
      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
    } else {
      AmgVector(5) perigeeParams = startPerigee->parameters();
      trkd0    = perigeeParams[Trk::d0];
      trkz0    = perigeeParams[Trk::z0];
      trkphi   = perigeeParams[Trk::phi0];
      trktheta = perigeeParams[Trk::theta];
      //trkcot   = 1./tan(trktheta);
      trketa   = - log(tan(trktheta / 2.0));
      trkqOverPt  = perigeeParams[Trk::qOverP]*1000./sin(trktheta);
      //trkpt       = fabs(1.0 / trkqOverPt);
      trkpt         = fabs(1.0 / trkqOverPt);
      if (trkqOverPt<0) charge=-1;
      else charge=+1;
      const AmgSymMatrix(5) * ErrorMat = startPerigee->covariance();
      double mtheta = (startPerigee->parameters())[Trk::theta];
      double mqp = (startPerigee->parameters())[Trk::qOverP];
      Trk::JacobianThetaPToCotThetaPt TheJac(mtheta,mqp);
      AmgSymMatrix(5) covVert;
      covVert = ErrorMat->similarity(TheJac);
      //trkd0err    = sqrt(covVert[0][0]);
      //trkz0err    = sqrt(covVert[1][1]);
      //trkphierr   = sqrt(covVert[2][2]);
      //trkcoterr   = sqrt(covVert[3][3]);
      //trkqopterr  = sqrt(covVert[4][4])*1000.;
      //trkthetaerr = ErrorMat->error(Trk::theta);
      trkd0err      = Amg::error(covVert,0);
      trkz0err      = Amg::error(covVert,1);
      trkphierr     = Amg::error(covVert,2);
      //trkcoterr   = Amg::error(covVert,3);
      trkqopterr    = Amg::error(covVert,4)*1000.;
      trkthetaerr   = Amg::error((*ErrorMat),Trk::theta);
      //////////////////////////////////////////////////////
    }
    float signed_trkphi = trkphi;
    if (trkphi<0) trkphi+=2*m_Pi;

    if (fabs(trketa)>m_maxTrackEta) continue;
    if (fabs(trketa)<m_minTrackEta) continue;
    if (trkphi>m_maxTrackPhi) continue;
    if (trkphi<m_minTrackPhi) continue;
    m_recdeltaz0->Fill((trkz0-zVertex)*sin(trktheta));
    if (fabs(trkz0-zVertex)*sin(trktheta)>m_z0cutwrtPrimary) continue;
    nTrackSelected++;

    // ========================================================================================
    // make a map between gnerated and reconstructed tracks (needed for efficiencies and fake rates)
    // =========================================================================================
    if (TruthMap) {
      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trksItr));
      tracklink.setStorableObject(*RecCollection);
      const ElementLink<TrackCollection> tracklink2=tracklink;

      TrackTruthCollection::const_iterator found = TruthMap->find(tracklink2);
      if (found != TruthMap->end()) {
        TrackTruth trtruth=found->second;
        HepMcParticleLink hmpl = trtruth.particleLink();
	msg(MSG::VERBOSE) << "Adding HMPL=" << hmpl << " with probability=" << trtruth.probability() << endmsg;
        m_rttMap.insert(std::pair<HepMcParticleLink,float>(hmpl,trtruth.probability()));
      }
    } else {
      msg(MSG::WARNING) << "No Truth Map found"<<endmsg;
    }


    float chi2oDoF = -1;
    if (fitQual!=0) {
      float chisquared = (fitQual) ? fitQual->chiSquared() : -1.;
      int DoF = (fitQual) ? fitQual->numberDoF() : -1;
      if(DoF>0) chi2oDoF = chisquared/(float)DoF;
    }


    m_recpT->Fill(trkpt);
    m_recpTlow->Fill(trkpt);
    m_recphi->Fill(trkphi);
    m_receta->Fill(trketa);
    m_recd0->Fill(trkd0);
    m_recz0->Fill(trkz0);
    m_recd0c->Fill(trkd0c); // currently just -999
    m_recchi2->Fill(chi2oDoF);
    m_reccharge->Fill(charge);

    if (trkpt>1) m_d0sig_trackpT1->Fill(trkd0/trkd0err);
    if (trkpt>2) m_d0sig_trackpT2->Fill(trkd0/trkd0err);
    if (trkpt>5) m_d0sig_trackpT5->Fill(trkd0/trkd0err);
    if (trkpt>10) m_d0sig_trackpT10->Fill(trkd0/trkd0err);

    if (trkpt>1 && fabs(trkd0/trkd0err)>3) ntrackpt1d0sig3++;
    if (trkpt>2 && fabs(trkd0/trkd0err)>3) ntrackpt2d0sig3++;
    if (trkpt>1 && fabs(trkd0/trkd0err)>5) ntrackpt1d0sig5++;
    if (trkpt>2 && fabs(trkd0/trkd0err)>5) ntrackpt2d0sig5++;

    if (msgLvl(MSG::VERBOSE)) msg() << "track parameters: d0=" <<trkd0 << " +/-"<< trkd0err<<" , z0=" <<trkz0 << " +/-"<< trkz0err
       << " , q/pt=" <<trkqOverPt << " +/-"<< trkqopterr << " , phi=" <<trkphi << " +/-"<< trkphierr<< " , theta=" <<trktheta<< " +/-"<< trkthetaerr<<endmsg;

    // Now check if the track is located inside a jet

    bool matchedJet = false;
    float matchedJetDr = -999.;
    float matchedJetEt = -999.;
    //float matchedJetEta = -999.;
    //float matchedJetPhi = -999.;

    if (jetColl){

      if (msgLvl(MSG::VERBOSE)) msg() << "Starting jet collection iteration" << endmsg;
      JetCollection::const_iterator jetItr  = jetColl->begin();
      JetCollection::const_iterator jetItrE = jetColl->end();
      for (; jetItr != jetItrE; ++jetItr) {
        const Jet * pj = (*jetItr);

        float jetPhi = pj->phi();
        if (jetPhi<0) jetPhi+=2*m_Pi;
        float dphi=acos(cos(trkphi-jetPhi));
        float deta=trketa-pj->eta();
        float dr=sqrt(dphi*dphi+deta*deta);

        // Jet is a 4Momentum, we can ask any kinematics :
        if (msgLvl(MSG::VERBOSE)) msg() << "JFA Jet: e=" << pj->e() << ", eta=" << pj->eta() << ", phi=" << jetPhi << ", DeltaR=" << dr << endmsg;

        if (dr < 0.4) {
          if (msgLvl(MSG::VERBOSE)) msg() << "JFA Matched jet!!!" << endmsg;
          matchedJet = true;
          matchedJetDr = dr;
          matchedJetEt = pj->et()/1000.;
          //matchedJetEta = pj->eta();
          //matchedJetPhi = pj->phi();
        }
      }
    } // If do track in jet

    // now check for duplicates/close-by tracks
    if (msgLvl(MSG::VERBOSE)) {
        for (; trksItr2 != trksItrE2 && trksItr2 != trksItr; ++trksItr2) {
          if (m_useTrackSelection) {
            if (!m_trackSelectorTool->decision(**trksItr2)) {
              continue;
            }
          }
          const Trk::Perigee* startPerigee2 = (*trksItr2)->perigeeParameters();
          //const Trk::MeasuredPerigee* measPer2 = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee2 );
          if (!(startPerigee2->covariance())) {
            msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
            continue;
          }
          AmgVector(5) perigeeParams2 =  startPerigee2->parameters();

          float trketa2   = - log(tan(perigeeParams2[Trk::theta] / 2.0));
          float trkphi2   = perigeeParams2[Trk::phi0];
          if (trkphi2<0) trkphi2+=2*m_Pi;
          float dphi=acos(cos(trkphi-trkphi2));
          float deta=trketa-trketa2;
          float dr=sqrt(dphi*dphi+deta*deta);
          if (msgLvl(MSG::VERBOSE)) msg() << "Distance of this track to next closest track: "<<dr<<endmsg;
        }
    }
    float prob = -0.1;
    float barcode=-999;
    //bool goodMatchToTruth=false; // match probability>80%
    //bool someMatchToTruth=false; // match probability>20%
    //bool isPion = false;

    if (TruthMap) {

      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(*trksItr));
      tracklink.setStorableObject(*RecCollection);
      const ElementLink<TrackCollection> tracklink2=tracklink;

      TrackTruthCollection::const_iterator found = TruthMap->find(tracklink2);
      if (found != TruthMap->end())
	{
	  TrackTruth trtruth = found->second;
	  prob =  trtruth.probability();
	  //if (prob>m_minProbEff) goodMatchToTruth=true;
	  //if (prob>m_minProbEffLow) someMatchToTruth=true;
	  HepMcParticleLink HMPL = trtruth.particleLink();

	  float barcode = HMPL.barcode();

	  const DetailedTrackTruthCollection *dtt(0);
	  if (!m_tracksDetailedTruthName.empty() &&
              evtStore()->contains<DetailedTrackTruthCollection>(m_tracksDetailedTruthName) &&
              evtStore()->retrieve(dtt, m_tracksDetailedTruthName).isSuccess()) {
	    //log << MSG::ERROR << "Got DetailedTrackTruthCollection.  Printing out matches track number "<< tracklink<< endmsg;

	    typedef DetailedTrackTruthCollection::const_iterator DTTIter;
	    std::pair<DTTIter,DTTIter> range = dtt->equal_range(Trk::TrackTruthKey(tracklink));
	    std::vector<int> list_eventIndex;
	    int number_truthmatches = 0;
	    for(DTTIter i = range.first; i  != range.second; ++i) {

	      number_truthmatches++;
	      //log << MSG::ERROR << i->second << endmsg;
	      // Iterate on HMPL of the truth trajectory
	      TruthTrajectory traj = i->second.trajectory();
	      //log<<i->second<<endmsg;
	      for(unsigned int i_hmpl=0; i_hmpl<traj.size(); i_hmpl++){

		int eventIndex = (int)traj[i_hmpl].eventIndex();
		//msg(MSG::ERROR)<<"Evt Index = " << eventIndex << endmsg;
		//msg(MSG::ERROR)<<"barcode = " << (int)traj[i_hmpl].barcode() << endmsg;
		// Iterate on vector on event index to check if this one already exists
		std::vector<int>::iterator itVectorData;
		bool alreadyExist = false;
		for(itVectorData = list_eventIndex.begin(); itVectorData != list_eventIndex.end(); itVectorData++){
		  //msg(MSG::ERROR)<< "*(itVectorData) = " << *(itVectorData) << endmsg;
		  if(eventIndex == *(itVectorData)){
                      //msg(MSG::ERROR)<< "eventIndex already stored!!" << endmsg;
		    alreadyExist = true;
		  }
		}

		if(!alreadyExist)
		  list_eventIndex.push_back(eventIndex);
	      }
	      }
	    m_num_eventindex_match->Fill(list_eventIndex.size());
	    m_num_truthmatch_match->Fill(number_truthmatches);
	    if (prob<0.5){
	      m_Fake_num_eventindex_match->Fill(list_eventIndex.size());
	      m_Fake_num_truthmatch_match->Fill(number_truthmatches);
	    }
	    if (barcode==0){
	      m_bc0_num_eventindex_match->Fill(list_eventIndex.size());
	      m_bc0_num_truthmatch_match->Fill(number_truthmatches);
	    }
	    if (barcode<MAXBARCODE && barcode !=0 && prob>0.5){
	      m_Good_num_eventindex_match->Fill(list_eventIndex.size());
	      m_Good_num_truthmatch_match->Fill(number_truthmatches);
	    }
	  }
	  if(barcode==0) m_prob_barcode0->Fill(prob);
	  if(barcode<MAXBARCODE && barcode !=0) m_prob_barcode_primary->Fill(prob);
	  if(barcode>MAXBARCODE) m_prob_barcodeMAXBARCODE->Fill(prob);


	  // Do not fill track performance plots for Geant particles, but they will be included in fake rate calculation
	  if(barcode<MAXBARCODE && barcode !=0){
	    //if (barcode>MAXBARCODE || barcode == 0) continue;

	    if ( HMPL.isValid())
	      {
		const HepMC::GenParticle *genparptr = HMPL.cptr();
		//float barcode = HMPL.barcode();
		//int pdgCode = genparptr->pdg_id();

		//if(abs(pdgCode) == 211) isPion = true;

		if (genparptr) {
		  if (m_selHardScatter && genparptr->parent_event()->event_number()!=genEventNumberPrimary) {
                    if (msgLvl(MSG::VERBOSE)) msg()<< "Don't use track from pileup event "
                                                   << genparptr->parent_event()->event_number()
                                                   << " (hard scatter event is "<<genEventNumberPrimary<<")"
                                                   <<endmsg;
                    continue;
                  }

		  if (genparptr->production_vertex()) {
		    // check if there is a good match to truth
		    // prob =  trtruth.probability();
		    // if (prob>0.8) goodMatchToTruth=true;
		    // if (prob>0.2) someMatchToTruth=true;

		    // now make some cuts as made by Andi Salzburger for detector paper
		    // JFA: Sept. 29, 2008: bug fix, remove the if on prob<0.8 to get a proper fake rate estimate
		    //if (prob<0.8) continue;
		    //if (barcode>MAXBARCODE) continue;
		    // calculate d0 w.r.t. beam spot

		    if (msgLvl(MSG::VERBOSE)) msg()<< genparptr->status()<<" mass "<< genparptr->momentum().m() <<" eta "
		    <<genparptr->momentum().eta()<<" phi "<<genparptr->momentum().phi()<<" Gen Vertex barcode "
		    <<genparptr->production_vertex()->barcode()<<"Gen Vertex Position x"
		    <<genparptr->production_vertex()->position().x()<<" y "
		    <<genparptr->production_vertex()->position().y()<<" z "
		    <<genparptr->production_vertex()->position().z()<<endmsg;

		    if(genparptr->pdg_id() == 0){
		      msg(MSG::WARNING)<<" Particle with PDG ID = 0! Status "<<endmsg;
		    }else{
		      const Trk::TrackParameters* generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genparptr);
		      if (!generatedTrackPerigee)   msg(MSG::WARNING) <<  "Unable to extrapolate genparticle to perigee!" << endmsg;
		      if ( generatedTrackPerigee) {
			float track_truth_qoverpt = 1000. * generatedTrackPerigee->parameters()[Trk::qOverP]/sin(generatedTrackPerigee->parameters()[Trk::theta]);
			float track_truth_phi     = generatedTrackPerigee->parameters()[Trk::phi0];
			float track_truth_d0      = generatedTrackPerigee->parameters()[Trk::d0];
			float track_truth_z0      = generatedTrackPerigee->parameters()[Trk::z0];
			float track_truth_theta   = generatedTrackPerigee->parameters()[Trk::theta];
			float track_truth_eta     = generatedTrackPerigee->eta();
			//float m_track_truth_cot      = 1/tan(generatedTrackPerigee->parameters()[Trk::theta]);
			//float m_track_truth_pdgid    = genparptr->pdg_id();

			// Delete  generatedTrackPerigee;
			float track_truth_pt      = 1./fabs(track_truth_qoverpt);
			float track_truth_charge  = 1;
			if (track_truth_qoverpt<0)    
                        track_truth_charge = -1;
			if (track_truth_phi<0)
                        track_truth_phi+=2*m_Pi;
			ATH_MSG_VERBOSE("Found matched truth track with phi, PT = " << track_truth_phi << ", " << track_truth_pt);

			// Calculate d0 corrected to vertex position
			//float d0x=track_truth_d0*sin(track_truth_phi)-genparptr->production_vertex()->position().x();
			//float d0y=track_truth_d0*cos(track_truth_phi)-genparptr->production_vertex()->position().y();
			//float d0corr=sqrt(d0x*d0x+d0y*d0y);

			// Fill generic tracks parameters
			m_truthpT->Fill(track_truth_pt);
			m_truthpTlow->Fill(track_truth_pt);
			m_truthcharge->Fill(track_truth_charge);
			m_truthphi->Fill(track_truth_phi);
			m_trutheta->Fill(track_truth_eta);
			m_truthd0->Fill(track_truth_d0);
			m_truthz0->Fill(track_truth_z0);
			//m_truthd0c->Fill(d0corr);

			//int ieta = fabs(int(track_truth_eta/0.25));
			//int ieta = int(track_truth_eta/0.25 + 10.0);
			int ieta = int(0.5*m_trackEtaBins*(track_truth_eta/m_maxTrackEta + 1.0));
			if (ieta < 0 || ieta>=m_trackEtaBins){
			  delete generatedTrackPerigee;
			  continue;
			}
			float log10pt = log10(track_truth_pt);
			//int ipt     = int((log10pt+0.5)/0.25);
			int ipt       = int(m_trackPtBins*(log10pt+0.5)/2.5);
			if (ipt>=m_trackPtBins) ipt=m_trackPtBins-1;
			if (ipt<0) ipt=0;

			// Fill histograms for resolutions
			// versus eta
			m_hd0[ieta]->Fill(trkd0-track_truth_d0);
			m_hz0[ieta]->Fill(trkz0-track_truth_z0);
			m_hphi[ieta]->Fill(trkphi-track_truth_phi);
			m_htheta[ieta]->Fill(trktheta-track_truth_theta);
			// JFA: bug fix qopt Sept. 26, 2008
			//m_hqopt[ieta]->Fill((trkqOverPt-track_truth_qoverpt)*fabs(track_truth_pt));
			m_hqopt[ieta]->Fill((trkqOverPt-track_truth_qoverpt)*fabs(track_truth_pt)*track_truth_charge);
			m_hz0st[ieta]->Fill(trkz0*sin(trktheta)-track_truth_z0*sin(track_truth_theta));
			// versus pt
			m_hptd0[ipt]->Fill(trkd0-track_truth_d0);
			m_hptz0[ipt]->Fill(trkz0-track_truth_z0);
			m_hptphi[ipt]->Fill(trkphi-track_truth_phi);
			m_hpttheta[ipt]->Fill(trktheta-track_truth_theta);
			// JFA: bug fix qopt Sept. 26, 2008
			//m_hptqopt[ipt]->Fill((trkqOverPt-track_truth_qoverpt)*fabs(track_truth_pt));
			m_hptqopt[ipt]->Fill((trkqOverPt-track_truth_qoverpt)*fabs(track_truth_pt)*track_truth_charge);
			m_hptz0st[ipt]->Fill(trkz0*sin(trktheta)-track_truth_z0*sin(track_truth_theta));
			// fill histograms for pulls vs eta
			m_pulld0[ieta]->Fill((trkd0-track_truth_d0)/trkd0err);
			m_pullz0[ieta]->Fill((trkz0-track_truth_z0)/trkz0err);
			m_pullphi[ieta]->Fill((trkphi-track_truth_phi)/trkphierr);
			m_pulltheta[ieta]->Fill((trktheta-track_truth_theta)/trkthetaerr);
			m_pullqopt[ieta]->Fill(((trkqOverPt-track_truth_qoverpt)*track_truth_charge)/trkqopterr);
			// fill histograms for pulls overall
			m_pulld0all->Fill((trkd0-track_truth_d0)/trkd0err);
			m_pullz0all->Fill((trkz0-track_truth_z0)/trkz0err);
			m_pullphiall->Fill((trkphi-track_truth_phi)/trkphierr);
			m_pullthetaall->Fill((trktheta-track_truth_theta)/trkthetaerr);
			m_pullqoptall->Fill(((trkqOverPt-track_truth_qoverpt)*track_truth_charge)/trkqopterr);
			m_pulld0all_widerange->Fill((trkd0-track_truth_d0)/trkd0err);
			m_pullz0all_widerange->Fill((trkz0-track_truth_z0)/trkz0err);
			m_pullphiall_widerange->Fill((trkphi-track_truth_phi)/trkphierr);
			m_pullthetaall_widerange->Fill((trktheta-track_truth_theta)/trkthetaerr);
			m_pullqoptall_widerange->Fill(((trkqOverPt-track_truth_qoverpt)*track_truth_charge)/trkqopterr);

			// charge mididentification rate versus eta
			if (track_truth_charge!=charge) {
			  if(m_plotsVsAbsEta) m_chargemisid->Fill(fabs(track_truth_eta),1);
			  else m_chargemisid->Fill(track_truth_eta,1);
			} else {
			  if(m_plotsVsAbsEta) m_chargemisid->Fill(fabs(track_truth_eta),0);
			  else m_chargemisid->Fill(track_truth_eta,0);
			}
			delete generatedTrackPerigee;
		      }
		    }
		  } else msg(MSG::WARNING) << " no genparptr->production_vertex() " << endmsg;
		} else msg(MSG::WARNING) << " no genparptr found " << endmsg;
	      } else msg(MSG::WARNING) << " HMPL not Valid, prob =  " << prob << endmsg;
	  } // Do not consider Geant particle for track perf plots
	}
    } else msg(MSG::WARNING) << " No TruthMap found " << endmsg;
    if (msgLvl(MSG::VERBOSE)) msg() << " Fill fake rate histograms " << endmsg;

    if(m_plotsVsAbsEta && trketa < 0.0) trketa *= -1.0;

    //if(isPion){
    // Fake rates: Move fake rate cut from 0.2 to 0.5. This is the same as InDetRecStatistics.
    if (prob>m_minProbEffLow){
      //if (prob>0.2) {
      m_track_fakerate_vs_eta->Fill(trketa,0);
      m_track_fakerate_vs_phi->Fill(signed_trkphi*57.296,0);
      m_track_fakerate_vs_pt->Fill(trkpt,0);
      m_track_fakerate_vs_d0->Fill(trkd0,0);
      m_track_fakerate_vs_z0->Fill(trkz0,0);
      m_track_fakerate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),0);
      if (matchedJet){
	m_trackinjet_fakerate_vs_eta->Fill(trketa,0);
	m_trackinjet_fakerate_vs_phi->Fill(signed_trkphi*57.296,0);
	m_trackinjet_fakerate_vs_pt->Fill(trkpt,0);
	m_trackinjet_fakerate_vs_dr->Fill(matchedJetDr,0);
	if (matchedJetEt < 50.0)
	  m_trackinjet_fakerate_vs_dr_lt_j50->Fill(matchedJetDr,0);
	if (matchedJetEt > 100.0)
	  m_trackinjet_fakerate_vs_dr_gr_j100->Fill(matchedJetDr,0);
	m_trackinjet_fakerate_vs_jetet->Fill(matchedJetEt,0);
      }
    } else {
      m_track_fakerate_vs_eta->Fill(trketa,1);
      m_track_fakerate_vs_phi->Fill(signed_trkphi*57.296,1);
      m_track_fakerate_vs_pt->Fill(trkpt,1);
      m_track_fakerate_vs_d0->Fill(trkd0,1);
      m_track_fakerate_vs_z0->Fill(trkz0,1);
      m_track_fakerate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),1);
      if (matchedJet){
	m_trackinjet_fakerate_vs_eta->Fill(trketa,1);
	m_trackinjet_fakerate_vs_phi->Fill(signed_trkphi*57.296,1);
	m_trackinjet_fakerate_vs_pt->Fill(trkpt,1);
	m_trackinjet_fakerate_vs_dr->Fill(matchedJetDr,1);
	if (matchedJetEt < 50.0)
	  m_trackinjet_fakerate_vs_dr_lt_j50->Fill(matchedJetDr,1);
	if (matchedJetEt > 100.0)
	  m_trackinjet_fakerate_vs_dr_gr_j100->Fill(matchedJetDr,1);
	m_trackinjet_fakerate_vs_jetet->Fill(matchedJetEt,1);
      }
    }

    if (msgLvl(MSG::VERBOSE)) msg() << " Fill badmatch rate histograms " << endmsg;

    if (prob>m_minProbEff){
      m_track_badmatchrate_vs_eta->Fill(trketa,0);
      m_track_badmatchrate_vs_phi->Fill(signed_trkphi*57.296,0);
      m_track_badmatchrate_vs_pt->Fill(trkpt,0);
      m_track_badmatchrate_vs_d0->Fill(trkd0,0);
      m_track_badmatchrate_vs_z0->Fill(trkz0,0);
      m_track_badmatchrate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),0);
      if (matchedJet){
	m_trackinjet_badmatchrate_vs_eta->Fill(trketa,0);
	m_trackinjet_badmatchrate_vs_phi->Fill(signed_trkphi*57.296,0);
	m_trackinjet_badmatchrate_vs_pt->Fill(trkpt,0);
	m_trackinjet_badmatchrate_vs_dr->Fill(matchedJetDr,0);
	if (matchedJetEt < 50.0) {
	  m_trackinjet_badmatchrate_vs_dr_lt_j50->Fill(matchedJetDr,0);
	}
	if (matchedJetEt > 100.0) {
	  m_trackinjet_badmatchrate_vs_dr_gr_j100->Fill(matchedJetDr,0);
	}
	m_trackinjet_badmatchrate_vs_jetet->Fill(matchedJetEt,0);
      }
    } else {
      m_track_badmatchrate_vs_eta->Fill(trketa,1);
      m_track_badmatchrate_vs_phi->Fill(signed_trkphi*57.296,1);
      m_track_badmatchrate_vs_pt->Fill(trkpt,1);
      m_track_badmatchrate_vs_d0->Fill(trkd0,1);
      m_track_badmatchrate_vs_z0->Fill(trkz0,1);
      m_track_badmatchrate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),1);
      if (matchedJet){
	m_trackinjet_badmatchrate_vs_eta->Fill(trketa,1);
	m_trackinjet_badmatchrate_vs_phi->Fill(signed_trkphi*57.296,1);
	m_trackinjet_badmatchrate_vs_pt->Fill(trkpt,1);
	m_trackinjet_badmatchrate_vs_dr->Fill(matchedJetDr,1);
	if (matchedJetEt < 50.0)
	  m_trackinjet_badmatchrate_vs_dr_lt_j50->Fill(matchedJetDr,1);
	if (matchedJetEt > 100.0)
	  m_trackinjet_badmatchrate_vs_dr_gr_j100->Fill(matchedJetDr,1);
	m_trackinjet_badmatchrate_vs_jetet->Fill(matchedJetEt,1);
      }
    }

    if (msgLvl(MSG::VERBOSE)) msg() << " Fill no truth histograms " << endmsg;
    // No truth rates
    if (prob>0.0) {
      m_track_notruthrate_vs_eta->Fill(trketa,0);
      m_track_notruthrate_vs_phi->Fill(signed_trkphi*57.296,0);
      m_track_notruthrate_vs_pt->Fill(trkpt,0);
      m_track_notruthrate_vs_d0->Fill(trkd0,0);
      m_track_notruthrate_vs_z0->Fill(trkz0,0);
    } else {
      m_track_notruthrate_vs_eta->Fill(trketa,1);
      m_track_notruthrate_vs_phi->Fill(signed_trkphi*57.296,1);
      m_track_notruthrate_vs_pt->Fill(trkpt,1);
      m_track_notruthrate_vs_d0->Fill(trkd0,1);
      m_track_notruthrate_vs_z0->Fill(trkz0,1);
    }


    if (msgLvl(MSG::VERBOSE)) msg() << " Fill bcode0 rate histograms " << endmsg;

    if (barcode==0 || prob<m_minProbEffLow) {
      m_track_bcode0rate_vs_eta->Fill(trketa,1);
      if(barcode==0)
        m_track_bcode0rate_matchbc0_vs_eta->Fill(trketa,1);
      if(barcode>MAXBARCODE)
        m_track_bcode0rate_matchsec_vs_eta->Fill(trketa,1);
      if(barcode<MAXBARCODE && barcode!=0)
        m_track_bcode0rate_matchprim_vs_eta->Fill(trketa,1);
      m_track_bcode0rate_vs_phi->Fill(signed_trkphi*57.296,1);
      m_track_bcode0rate_vs_pt->Fill(trkpt,1);
      m_track_bcode0rate_vs_d0->Fill(trkd0,1);
      m_track_bcode0rate_vs_z0->Fill(trkz0,1);
      m_track_bcode0rate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),1);
      if(matchedJet){
        m_trackinjet_bcode0rate_vs_eta->Fill(trketa,1);
        m_trackinjet_bcode0rate_vs_phi->Fill(signed_trkphi*57.296,1);
        m_trackinjet_bcode0rate_vs_pt->Fill(trkpt,1);
        m_trackinjet_bcode0rate_vs_dr->Fill(matchedJetDr,1);
        if(matchedJetEt < 50.0)
          m_trackinjet_bcode0rate_vs_dr_lt_j50->Fill(matchedJetDr,1);
        if(matchedJetEt > 100.0)
          m_trackinjet_bcode0rate_vs_dr_gr_j100->Fill(matchedJetDr,1);
        m_trackinjet_bcode0rate_vs_jetet->Fill(matchedJetEt,1);
      }
    } else {
      m_track_bcode0rate_vs_eta->Fill(trketa,0);
      if(barcode==0)
        m_track_bcode0rate_matchbc0_vs_eta->Fill(trketa,0);
      if(barcode>MAXBARCODE)
        m_track_bcode0rate_matchsec_vs_eta->Fill(trketa,0);
      if(barcode<MAXBARCODE && barcode !=0)
        m_track_bcode0rate_matchprim_vs_eta->Fill(trketa,0);
      m_track_bcode0rate_vs_phi->Fill(signed_trkphi*57.296,0);
      m_track_bcode0rate_vs_pt->Fill(trkpt,0);
      m_track_bcode0rate_vs_d0->Fill(trkd0,0);
      m_track_bcode0rate_vs_z0->Fill(trkz0,0);
      m_track_bcode0rate_vs_deltaz0->Fill((trkz0-zVertex)*sin(trktheta),0);
      if(matchedJet){
        m_trackinjet_bcode0rate_vs_eta->Fill(trketa,0);
        m_trackinjet_bcode0rate_vs_phi->Fill(signed_trkphi*57.296,0);
        m_trackinjet_bcode0rate_vs_pt->Fill(trkpt,0);
        m_trackinjet_bcode0rate_vs_dr->Fill(matchedJetDr,0);
        if(matchedJetEt < 50.0)
          m_trackinjet_bcode0rate_vs_dr_lt_j50->Fill(matchedJetDr,0);
        if(matchedJetEt > 100.0)
          m_trackinjet_bcode0rate_vs_dr_gr_j100->Fill(matchedJetDr,0);
        m_trackinjet_bcode0rate_vs_jetet->Fill(matchedJetEt,0);
      }
   }

   if (msgLvl(MSG::VERBOSE)) msg() << " Done filling fake rate histograms " << endmsg;

   // Barcode 0 rate
   if (barcode==0) {
     nBarCode0++;
     if (prob>m_minProbEffLow) nBarCode0Match++;
   } else if (barcode>MAXBARCODE) {
     // barcode >MAXBARCODE rate
     nBarCode100k++;
   } else if (barcode>0 && barcode<MAXBARCODE) {
     nBarCodeGood++;
   }
   if (barcode==0 || prob < m_minProbEff) {
     nBadMatch++;
   }
   if (barcode!=0 && prob < m_minProbEff) {
     nLowProb++;
   }


   m_prob->Fill(prob);
   if (matchedJet)
     m_prob_injet->Fill(prob);
   // } // If: isPion

   if (msgLvl(MSG::VERBOSE)) msg() << " count tracks associated to same mc particle" << endmsg;
   // Count number of tracks associated to the same hmpl
   for(recoToTruthMap::iterator rtt_it = m_rttMap.begin(); rtt_it != m_rttMap.end(); ++rtt_it){

      HepMcParticleLink hmpl = rtt_it->first;
      float barcode = hmpl.barcode();
      float nTrkMatchesSameHmpl = m_rttMap.count(hmpl);

      //log << MSG::ERROR << "HMPL = " << hmpl << ", barcode = " << barcode << ", nmatches = " << nTrkMatchesSameHmpl << endmsg;

      m_nTrkMatchesSameHmpl->Fill(nTrkMatchesSameHmpl);
      if(barcode==0)
        m_nTrkMatchesSameHmpl_barcode0->Fill(nTrkMatchesSameHmpl);
      if(barcode<MAXBARCODE && barcode !=0)
        m_nTrkMatchesSameHmpl_barcode_primary->Fill(nTrkMatchesSameHmpl);
      if(barcode>MAXBARCODE)
        m_nTrkMatchesSameHmpl_barcodeMAXBARCODE->Fill(nTrkMatchesSameHmpl);
    }
        // Now also make plotsof the hit content for fake and good tracks
    const Trk::TrackSummary* summary = m_trkSummaryTool->createSummary(**trksItr);
    if (summary) {
      // Fill impact parameter significance for different numbers of shared hits
      if (summary->get(Trk::numberOfInnermostPixelLayerSharedHits)>0) {
        m_d0sig_pt1_sharedBL->Fill(trkd0/trkd0err);
      }
      if (summary->get(Trk::numberOfPixelSharedHits)>0) {
        m_d0sig_pt1_sharedPIX->Fill(trkd0/trkd0err);
      }
      if (summary->get(Trk::numberOfSCTSharedHits)>0) {
        m_d0sig_pt1_sharedSCT->Fill(trkd0/trkd0err);
      }

      if (prob<m_minProbEffLow) {

        // IBL
        m_Fake_HitContent_NiblHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerHits));
        m_Fake_HitContent_NiblOutliers->Fill(summary->get(Trk::numberOfInnermostPixelLayerOutliers));
        m_Fake_HitContent_NiblSharedHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerSharedHits));
        // B-layer
        m_Fake_HitContent_NBlayerHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerHits));
        m_Fake_HitContent_NBlayerOutliers->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerOutliers));
        m_Fake_HitContent_NBlayerSharedHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerSharedHits));
        // Pixel
        m_Fake_HitContent_NPixelHits->Fill(summary->get(Trk::numberOfPixelHits));
        m_Fake_HitContent_NPixelHoles->Fill(summary->get(Trk::numberOfPixelHoles));
        m_Fake_HitContent_NPixelContribLayers->Fill(summary->get(Trk::numberOfContribPixelLayers));
        m_Fake_HitContent_NPixelSharedHits->Fill(summary->get(Trk::numberOfPixelSharedHits));
        // SCT
        m_Fake_HitContent_NSCTHits->Fill(summary->get(Trk::numberOfSCTHits));
        m_Fake_HitContent_NSCTHoles->Fill(summary->get(Trk::numberOfSCTHoles));
        m_Fake_HitContent_NSCTDoubleHoles->Fill(summary->get(Trk::numberOfSCTDoubleHoles));
        m_Fake_HitContent_NSCTSharedHits->Fill(summary->get(Trk::numberOfSCTSharedHits));

        m_Fake_NSiHit->Fill(summary->get(Trk::numberOfSCTHits)+summary->get(Trk::numberOfPixelHits));
        m_Fake_NPixvNSct->Fill(summary->get(Trk::numberOfPixelHits),summary->get(Trk::numberOfSCTHits));
        m_Fake_Chi2->Fill(chi2oDoF);
        m_fakepT->Fill(trkpt);
        m_fakepTlow->Fill(trkpt);
        m_fakeeta->Fill(trketa);
        m_fakez0->Fill(trkz0);
        m_faked0->Fill(trkd0);
        m_faked0c->Fill(trkd0c);  // Currently just -999
        m_fakephi->Fill(trkphi);
        m_fakedeltaz0->Fill((trkz0-zVertex)*sin(trktheta));
     }  else if (barcode<MAXBARCODE && barcode !=0) {
 
        // IBL 
        m_Good_HitContent_NiblHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerHits));
        m_Good_HitContent_NiblOutliers->Fill(summary->get(Trk::numberOfInnermostPixelLayerOutliers));
        m_Good_HitContent_NiblSharedHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerSharedHits));
        // B-layer
        m_Good_HitContent_NBlayerHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerHits));
        m_Good_HitContent_NBlayerOutliers->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerOutliers));
        m_Good_HitContent_NBlayerSharedHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerSharedHits));
        // Pixel
        m_Good_HitContent_NPixelHits->Fill(summary->get(Trk::numberOfPixelHits));
        m_Good_HitContent_NPixelHoles->Fill(summary->get(Trk::numberOfPixelHoles));
        m_Good_HitContent_NPixelContribLayers->Fill(summary->get(Trk::numberOfContribPixelLayers));
        m_Good_HitContent_NPixelSharedHits->Fill(summary->get(Trk::numberOfPixelSharedHits));
        // SCT
        m_Good_HitContent_NSCTHits->Fill(summary->get(Trk::numberOfSCTHits));
        m_Good_HitContent_NSCTHoles->Fill(summary->get(Trk::numberOfSCTHoles));
        m_Good_HitContent_NSCTDoubleHoles->Fill(summary->get(Trk::numberOfSCTDoubleHoles));
        m_Good_HitContent_NSCTSharedHits->Fill(summary->get(Trk::numberOfSCTSharedHits));

        m_Good_NSiHit->Fill(summary->get(Trk::numberOfSCTHits)+summary->get(Trk::numberOfPixelHits));
        m_Good_NPixvNSct->Fill(summary->get(Trk::numberOfPixelHits),summary->get(Trk::numberOfSCTHits));
        m_Good_Chi2->Fill(chi2oDoF);

      }
    }
    if(summary) {delete summary; summary=0;}

  }

  m_nTrackpT1_d0siggt3->Fill(ntrackpt1d0sig3);
  m_nTrackpT2_d0siggt3->Fill(ntrackpt2d0sig3);
  m_nTrackpT1_d0siggt5->Fill(ntrackpt1d0sig5);
  m_nTrackpT2_d0siggt5->Fill(ntrackpt2d0sig5);

  int nChargedParticle=0;
  int nChargedParticleEta=0;
  int nChargedParticleSelected=0;
  int nChargedParticlePrimary=0;

  if (msgLvl(MSG::VERBOSE)) msg() << " obtain truth information " << endmsg;
  // ------------------------------------------------------------------------------------
  //     now make efficiency plots:
  //     loop through generated tracks and check if there is a reconstructed tracks near it
  // ------------------------------------------------------------------------------------

  unsigned int nb_mc_event = SimTracks->size();
  m_ngenevent->Fill(nb_mc_event);

  //int nparticle=0;
  for (unsigned int ievt=0; ievt<nb_mc_event; ++ievt){
    //const HepMC::GenEvent* genEvent = *(gen->begin());
    const HepMC::GenEvent* genEvent = SimTracks->at(ievt);

    int nprimperevent=0;
    int nsecperevent=0;
    int nprimperevent05=0;
    int nsecperevent05=0;

    // Get the primary vertex
    //HepGeom::Point3D<double>  primaryVtx(0,0,0);
    Amg::Vector3D  primaryVtx(0,0,0); 
    bool singPart = false;
    // The event has a signal process vertex unless it was generated as single particles
    if (genEvent->signal_process_vertex() != 0){
      primaryVtx[0] = genEvent->signal_process_vertex()->point3d().x();
      primaryVtx[1] = genEvent->signal_process_vertex()->point3d().y();
      primaryVtx[2] = genEvent->signal_process_vertex()->point3d().z();
    }
    if (msgLvl(MSG::VERBOSE)) msg() <<"JFA Signal process vertex perp =  "<< primaryVtx.perp() << ", z = " << primaryVtx.z() << endmsg;
        //---


    for (HepMC::GenEvent::particle_const_iterator it = genEvent->particles_begin();
      it != genEvent->particles_end(); ++it) {

      HepMC::GenParticle * const particle = *it;

      if (!particle->production_vertex()) {
	msg(MSG::VERBOSE) << "Skipping GenParticle without production vertex" << endmsg;
	continue;
      }

      // -----------------------------------------------
      // Require the particles to be stable and charged
      // -----------------------------------------------
      // only consider stable particles
      if (particle->status()%1000!=1) continue;
      int   pdgCode         = particle->pdg_id();
      const HepPDT::ParticleData* pd =	m_particleDataTable->particle(abs(pdgCode));
      if (!pd) {
	//MsgStream log(msgSvc(), name());
	if (msgLvl(MSG::DEBUG)) msg() <<"Could not get particle data for particle with "
				      <<"pdgCode="<<pdgCode<< ", status=" << particle->status()
				      << ", barcode=" << particle->barcode() << endmsg;
	if (msgLvl(MSG::DEBUG)) msg() << "GenParticle= " << particle << endmsg;
	continue;
      }
      // run only over stable charged particles

      float charge          = pd->charge();
      if (fabs(charge)<0.5) continue;
      //bool isPion=false;
      //if (abs(pdgCode)==211) isPion=true;
      //if (!isPion) continue;

      // Only consider stable particles
      if (particle->status()%1000!=1) continue;
      nChargedParticle++;
      if (fabs(particle->momentum().pseudoRapidity())<3) nChargedParticleEta++;
      if (particle->barcode()>MAXBARCODE || particle->barcode()==0) continue;

      float genPt=particle->momentum().perp()/1000;
      float genEta=particle->momentum().pseudoRapidity();
      float genPhi=particle->momentum().phi();

      if (fabs(genEta)<3) {
        if (particle->barcode()<MAXBARCODE && particle->barcode()>0) {
          nprimperevent++;
          if (genPt>0.5) nprimperevent05++;
        } else if (particle->barcode()>MAXBARCODE) {
          nsecperevent++;
          if (genPt>0.5) nsecperevent05++;
        }
      }

      if (fabs(genEta)>m_maxTrackEta || fabs(genEta)<m_minTrackEta) {
        continue;
      }
      if (genPhi>m_maxTrackPhi || genPhi<m_minTrackPhi) {
        continue;
      }

      //if (genPt<m_minTrackPtEff)  continue;


      // ------------------------------------------------
      // Determine if particle is prompt or secondary
      // ------------------------------------------------
      bool isPrimary=true;
      bool isSecondary=true;
      bool isDetPaperCut=true;

      // V47: Create generated track perigee to cut on truth tracks
      float track_truth_d0 = 999.;
      float track_truth_phi = 999.;
      auto_ptr<const Trk::TrackParameters> generatedTrackPerigee(m_truthToTrack->makePerigeeParameters(particle));
      if (!generatedTrackPerigee.get())   msg(MSG::DEBUG) <<  "Unable to extrapolate genparticle to perigee!" << endmsg;
      else {
	track_truth_d0 = generatedTrackPerigee->parameters()[Trk::d0];
	track_truth_phi = generatedTrackPerigee->parameters()[Trk::phi0];
      }
      float truth_d0corr=track_truth_d0-(primaryVtx.y()*cos(track_truth_phi)-primaryVtx.x()*sin(track_truth_phi));
      float truthvtx_R=sqrt(primaryVtx.x()*primaryVtx.x()+primaryVtx.y()*primaryVtx.y());
      m_truthd0c->Fill(truth_d0corr);
      m_truthvtx_R->Fill(truthvtx_R);
      m_truthvtx_z->Fill(primaryVtx.z());

      //HepGeom::Point3D<double>  startVertex(particle->production_vertex()->point3d().x(),
      //particle->production_vertex()->point3d().y(),
      //particle->production_vertex()->point3d().z());
      Amg::Vector3D  startVertex(particle->production_vertex()->point3d().x(),particle->production_vertex()->point3d().y(),particle->production_vertex()->point3d().z());
      // Detector paper cuts: impact parameter <2 and barcode reasonable
      // JFA (Sep 18, 2008): replace IP cut by cut on difference between primary and track start vertex
      // If (startVertex.perp()>2) isPrimary=false;
      // JFA (Feb. 2, 2009): Replace cut on difference between IP and startvertex by a cut on truth d0
      //if (genEvent->signal_process_vertex() != 0 && primaryVtx.distance(startVertex) > 2.0) isPrimary=false;
      if (!singPart && fabs(truth_d0corr) > 2.0) isPrimary=false;
      if (particle->barcode()>MAXBARCODE || particle->barcode()==0) isPrimary=false;
      isDetPaperCut=isPrimary;

      if (fabs(startVertex.z())>100) isPrimary=false;

      if (startVertex.perp()>360) isSecondary=false;
      if (fabs(startVertex.z())>2000) isSecondary=false;


      float reDefGenPhi = genPhi;
      if (reDefGenPhi<0) reDefGenPhi+=2*m_Pi;

      if (particle->end_vertex() != 0) {
	//HepGeom::Point3D<double>  endVertex(particle->end_vertex()->point3d().x(),
	//particle->end_vertex()->point3d().y(),
	//particle->end_vertex()->point3d().z());

	Amg::Vector3D  endVertex(particle->end_vertex()->point3d().x(),
			      particle->end_vertex()->point3d().y(),
			      particle->end_vertex()->point3d().z());
	if (endVertex.perp()<400 && fabs(endVertex.z())<2300) isPrimary=false;
	if((startVertex - endVertex).mag() < 500) isSecondary=false;

	//
	// JFA Fatras: iterate of particle end_verex to look for nuclear interactions;
	//
	// Look only for particles interacting inside the ID
	if(endVertex.perp()<1000 && fabs(endVertex.z())<2500)  {
	  //log<< MSG::ERROR << "****** Particle with end vertex inside ID ******" << endmsg;
	  //log << MSG::ERROR<<"PdgCode="<<particle->pdg_id() << ", status=" << particle->status()<< ", barcode=" << particle->barcode()<< ", E = "<< particle->momentum().e()/1000<< ", m = "<< particle->momentum().m()/1000<< ", pT = "<< particle->momentum().perp()/1000<< ", eta = "<<particle->momentum().pseudoRapidity()<< ", phi = "<<particle->momentum().phi()<< ", stavtx = "<<startVertex.perp()<< ", endvtx = "<<endVertex.perp()<< endmsg;

	  // First iteration: Flag if nuclear interaction

	  //log<< MSG::ERROR << "*** Iterate on children ***" << endmsg;
	  float mass_initial = particle->momentum().m()/1000;
	  float mass_final = 0.0;
	  int n_out = 0;

	  for ( HepMC::GenVertex::particle_iterator child=particle->end_vertex()->particles_begin(HepMC::children);
		child != particle->end_vertex()->particles_end(HepMC::children);
		++child ) {
	    if(abs((*child)->pdg_id()) < 1000000000) {
	      if ((*child)->momentum().e() > 200)
		{
		  n_out++;
		}
	      //log << MSG::ERROR <<"PdgCode="<<(*child)->pdg_id() << ", status=" << (*child)->status() << ", barcode=" << (*child)->barcode()	<< ", E = "<< (*child)->momentum().e()/1000<< ", m = "<< (*child)->momentum().m()/1000<< ", pT = "<< (*child)->momentum().perp()/1000<< ", eta = "<< (*child)->momentum().pseudoRapidity() << ", phi = "<<particle->momentum().phi()<< ", stavtx = "<<(*child)->production_vertex()->point3d().perp()<< endmsg;
	      mass_final += (*child)->momentum().m()/1000;
	    }
	  }
	  // Decide if it's a nuclear interaction: no lepton, no photon and mass_final > mass_initial
	  // Also ask now n_out>=3 to remove quasi-elastic interaction that happen in Fullsim (assuming they have a small impact)
	  if(abs(pdgCode) != 22 && abs(pdgCode) != 11 && abs(pdgCode) != 13 && abs(pdgCode) != 15 && mass_final > mass_initial && n_out >=2){
	    //log<< MSG::ERROR << "NUCLEAR INTERACTIONS!!!!!!!!!" << endmsg;

	    m_nuclearint_in_E->Fill(particle->momentum().e()/1000);
	    m_nuclearint_in_pt->Fill(particle->momentum().perp()/1000);
	    m_nuclearint_in_eta->Fill(particle->momentum().pseudoRapidity());
	    m_nuclearint_in_phi->Fill(reDefGenPhi);
	    m_nuclearint_in_pdgcode->Fill(pdgCode);
	    m_nuclearint_in_endvtx_r->Fill(endVertex.perp());
	    m_nuclearint_in_endvtx_z->Fill(endVertex.z());

	    float total_E_out = 0.0;
	    // Iterate once again to fill variables
	    for ( HepMC::GenVertex::particle_iterator child=particle->end_vertex()->particles_begin(HepMC::children);
		  child != particle->end_vertex()->particles_end(HepMC::children);
		  ++child ) {
	      if(abs((*child)->pdg_id()) < 1000000000){
		total_E_out += (*child)->momentum().e()/1000;

		float outPhi = (*child)->momentum().phi();
		if (outPhi<0) outPhi+=2*m_Pi;
		float dphi=acos(cos(reDefGenPhi-outPhi));
		float deta=particle->momentum().pseudoRapidity()-(*child)->momentum().pseudoRapidity();
		m_nuclearint_out_E->Fill((*child)->momentum().e()/1000);
		m_nuclearint_out_pt->Fill((*child)->momentum().perp()/1000);
		m_nuclearint_out_deta->Fill(deta);
		m_nuclearint_out_dphi->Fill(dphi);
		m_nuclearint_out_pdgcode->Fill((*child)->pdg_id());
	      }
	    } // Second iteration
	    m_nuclearint_n_out->Fill(n_out);
	    m_nuclearint_totalEout->Fill(total_E_out);
	  } // If: nuclear interaction
	} // If: Decay inside ID

      }

      if (isPrimary) isSecondary=false;

      // Iteration on jets
      bool  matchedJet   = false;
      float matchedJetDr = -999.;
      float matchedJetEt = -999.;
      //float matchedJetEta = -999.;
      //float matchedJetPhi = -999.;

      if (jetColl){

	if (msgLvl(MSG::VERBOSE)) msg() << "Starting jet collection iteration" << endmsg;
	JetCollection::const_iterator jetItr  = jetColl->begin();
	JetCollection::const_iterator jetItrE = jetColl->end();
	for (; jetItr != jetItrE; ++jetItr) {
	  const Jet * pj = (*jetItr);

	  float jetPhi = pj->phi();
	  if (jetPhi<0) jetPhi+=2*m_Pi;
	  float dphi=acos(cos(reDefGenPhi-jetPhi));
	  float deta=genEta-pj->eta();
	  float dr=sqrt(dphi*dphi+deta*deta);

	  // Jet is a 4Momentum, we can ask any kinematics :
	  if (msgLvl(MSG::VERBOSE)) msg() << "JFA Jet: e=" << pj->e() << ", eta=" << pj->eta() << ", phi=" << jetPhi << ", DeltaR=" << dr << endmsg;

	  if (dr < 0.4){
	    if (msgLvl(MSG::VERBOSE)) msg() << "JFA Matched jet!!!" << endmsg;
	    matchedJet = true;
	    matchedJetDr = dr;
	    matchedJetEt = pj->et()/1000.;
	    //matchedJetEta = pj->eta();
	    //matchedJetPhi = pj->phi();
	  }
	}
      } // If do track in jet

      //bool matched = false;
      bool matchedDetPaper = false;
      bool matchedIDRS = false;
      int nmatched = 0;
      HepMcParticleLink hmpl2(particle, ievt);
      recoToTruthMap::iterator barcode=m_rttMap.find(hmpl2);
      if (msgLvl(MSG::VERBOSE)) msg() << "Looking for HMPL=" << hmpl2 << "... " << endmsg;
      if (barcode != m_rttMap.end()){
	if (msgLvl(MSG::VERBOSE)) msg() << "...found some" << endmsg;
	for(imap = m_rttMap.lower_bound(hmpl2); imap !=m_rttMap.upper_bound(hmpl2); ++imap){
	  if (genPt>30) if (msgLvl(MSG::VERBOSE)) msg() << "track match probability = "<< imap->second<<endmsg;
	  if (imap->second > m_minProbEff){  // 80% match probability
	    matchedDetPaper = true;
	    nmatched++;
	  }
	  if (imap->second > m_minProbEffLow){  // 50% match probability: InDetRecStatistics cut
	    matchedIDRS = true;
	  }
	}
      }

      if(particle->barcode()>MAXBARCODE)
        m_prodvtx_secondaries_RZ->Fill( startVertex.z(),sqrt(pow(startVertex.x(),2)+pow(startVertex.y(),2)) );
      if(particle->barcode()<MAXBARCODE && particle->barcode()!=0)
        m_prodvtx_primaries_RZ->Fill( startVertex.z(),sqrt(pow(startVertex.x(),2)+pow(startVertex.y(),2)) );

      // now printout for debugging gen particles
      if (isDetPaperCut && genPt>30){
	if (msgLvl(MSG::VERBOSE)) msg()
				    <<"PdgCode="<<pdgCode<< ", status=" << particle->status()
				    << ", barcode=" << particle->barcode()
				    << ", pT = "<< genPt
				    << ", eta = "<<genEta
				    << ", stavtx = "<<startVertex.perp()
				    //      << ", endvtx = "<<endVertex.perp()
				    << endmsg;
	if (matchedDetPaper) {
	  if (msgLvl(MSG::VERBOSE)) msg() <<"Particle reconstrcuted and matched! "<<endmsg;
	} else {
	  if (msgLvl(MSG::VERBOSE)) msg() <<"Particle not found! "<<endmsg;
	}
      }

      if (m_plotsVsAbsEta && genEta < 0.0) genEta *= -1.0;

      // Det. paper prob cut of 80%
      // 1 GeV cut for tracking efficiency versus eta
      if (genPt>m_minTrackPtEff){
	if (isPrimary) nChargedParticlePrimary++;
	if (isDetPaperCut) nChargedParticleSelected++;
      }
      // BHcheck
      if (m_selHardScatter && ievt!=0) {
        //delete generatedTrackPerigee;
        continue;
      }
      if (genPt>m_minTrackPtEff && genPt<m_maxTrackPtEff){
       	if (matchedDetPaper){
	  if (isDetPaperCut) {
	    m_trackeff_vs_eta->Fill(genEta,1.0);
	    m_trackeff_vs_phi->Fill(genPhi*57.296,1.0);
	  }
	  if (matchedJet){
	    if (isDetPaperCut){
	      m_trackinjeteff_vs_eta->Fill(genEta,1.0);
	      m_trackinjeteff_vs_phi->Fill(genPhi*57.296,1.0);
	      m_trackinjeteff_vs_jetet->Fill(matchedJetEt,1.0);
	      m_trackinjeteff_vs_dr->Fill(matchedJetDr,1.0);
	      if (matchedJetEt < 50.0)
		m_trackinjeteff_vs_dr_lt_j50->Fill(matchedJetDr,1.0);
	      if (matchedJetEt > 100.0)
		m_trackinjeteff_vs_dr_gr_j100->Fill(matchedJetDr,1.0);
	    }
	  }
	} else {
	  if (isDetPaperCut) {
	    m_trackeff_vs_eta->Fill(genEta,0.0);
	    m_trackeff_vs_phi->Fill(genPhi*57.296,0.0);
	  }
	  if (matchedJet){
	    if (isDetPaperCut){
	      m_trackinjeteff_vs_eta->Fill(genEta,0.0);
	      m_trackinjeteff_vs_phi->Fill(genPhi*57.296,0.0);
	      m_trackinjeteff_vs_jetet->Fill(matchedJetEt,0.0);
	      m_trackinjeteff_vs_dr->Fill(matchedJetDr,0.0);
	      if (matchedJetEt < 50.0)
		m_trackinjeteff_vs_dr_lt_j50->Fill(matchedJetDr,0.0);
	      if (matchedJetEt > 100.0)
		m_trackinjeteff_vs_dr_gr_j100->Fill(matchedJetDr,0.0);
	    }
	  }
	}
      } else if (genPt>0.5){
	if (matchedDetPaper){
	  if (isDetPaperCut) m_trackeff_vs_eta_lowpt->Fill(genEta,1.0);
	} else {
	  if (isDetPaperCut) m_trackeff_vs_eta_lowpt->Fill(genEta,0.0);
	}
      } else if (genPt>0.1){
	if (matchedDetPaper){
	  if (isDetPaperCut) m_trackeff_vs_eta_vlowpt->Fill(genEta,1.0);
	} else {
	  if (isDetPaperCut) m_trackeff_vs_eta_vlowpt->Fill(genEta,0.0);
	}
      }

      float logPt=-10;
      if (fabs(genEta)<m_maxTrackEta) {
        if (genPt>0) logPt=log10(genPt);
	if (matchedDetPaper){
	  if (isDetPaperCut) {
	    m_trackeff_vs_pt->Fill(genPt,1.0);
	    m_trackeff_vs_pt_lowpt->Fill(genPt,1.0);
	    if (fabs(genEta)<0.8) m_trackeff_vs_pt_loweta->Fill(genPt,1.0);
	    else if (fabs(genEta)<1.5) m_trackeff_vs_pt_medeta->Fill(genPt,1.0);
	    else m_trackeff_vs_pt_higheta->Fill(genPt,1.0);
            // Efficiency vs log(pt)
            m_trackeff_vs_logpt->Fill(logPt,1.0);
            if (fabs(genEta)<0.8) m_trackeff_vs_logpt_loweta->Fill(logPt,1.0);
            else if (fabs(genEta)<1.5) m_trackeff_vs_logpt_medeta->Fill(logPt,1.0);
            else m_trackeff_vs_logpt_higheta->Fill(logPt,1.0);
	    if(matchedJet){
	      m_trackinjeteff_vs_pt->Fill(genPt,1.0);
	    }
	  }
	} else {
	  if (isDetPaperCut) {
	    m_trackeff_vs_pt->Fill(genPt,0.0);
	    m_trackeff_vs_pt_lowpt->Fill(genPt,0.0);
	    if (fabs(genEta)<0.8) m_trackeff_vs_pt_loweta->Fill(genPt,0.0);
	    else if (fabs(genEta)<1.5) m_trackeff_vs_pt_medeta->Fill(genPt,0.0);
	    else m_trackeff_vs_pt_higheta->Fill(genPt,0.0);
            // Efficiency vs log(pt)
            m_trackeff_vs_logpt->Fill(logPt,0.0);
            if (fabs(genEta)<0.8) m_trackeff_vs_logpt_loweta->Fill(logPt,0.0);
            else if (fabs(genEta)<1.5) m_trackeff_vs_logpt_medeta->Fill(logPt,0.0);
            else m_trackeff_vs_logpt_higheta->Fill(logPt,0.0);
	    if(matchedJet){
	      m_trackinjeteff_vs_pt->Fill(genPt,0.0);
	    }
	  }
	}
      }

      // InDetRecStatistics prob cut of 50%
      // 1 GeV cut for tracking efficiency versus eta
      if (genPt>m_minTrackPtEff){
	if (matchedIDRS){
	  if (isPrimary) {
	    m_trackeff_primary_vs_eta->Fill(genEta,1.0);
	    m_trackeff_primary_vs_phi->Fill(genPhi*57.296,1.0);
	  }
	  if (isSecondary) {
	    m_trackeff_secondary_vs_eta->Fill(genEta,1.0);
	    m_trackeff_secondary_vs_phi->Fill(genPhi*57.296,1.0);
	    m_trackeff_secondary_vs_radius->Fill(startVertex.perp(),1.0);
	  }
	} else {
	  if (isPrimary) m_trackeff_primary_vs_eta->Fill(genEta,0.0);
	  if (isPrimary) m_trackeff_primary_vs_phi->Fill(genPhi*57.296,0.0);
	  if (isSecondary) {
	    m_trackeff_secondary_vs_eta->Fill(genEta,0.0);
	    m_trackeff_secondary_vs_phi->Fill(genPhi*57.296,0.0);
	    m_trackeff_secondary_vs_radius->Fill(startVertex.perp(),0.0);
	  }
	}
      } else if (genPt>0.5){
	if (matchedIDRS){
	  if (isPrimary) {
	    m_trackeff_primary_vs_eta_lowpt->Fill(genEta,1.0);
	    m_trackeff_secondary_vs_eta_lowpt->Fill(genEta,1.0);
	  }
	} else {
	  if (isPrimary) m_trackeff_primary_vs_eta_lowpt->Fill(genEta,0.0);
	  if (isSecondary) m_trackeff_secondary_vs_eta_lowpt->Fill(genEta,0.0);
	}
      } else if (genPt>0.1){
	if (matchedIDRS){
	  if (isPrimary) m_trackeff_primary_vs_eta_vlowpt->Fill(genEta,1.0);
	  if (isSecondary) m_trackeff_secondary_vs_eta_vlowpt->Fill(genEta,1.0);
	} else {
	  if (isPrimary) m_trackeff_primary_vs_eta_vlowpt->Fill(genEta,0.0);
	  if (isSecondary) m_trackeff_secondary_vs_eta_vlowpt->Fill(genEta,0.0);
	}
      }
      if (fabs(genEta)<m_maxTrackEta) {
        if (genPt>0) logPt=log10(genPt);
	if (matchedIDRS){
	  if (isPrimary) {
	    m_trackeff_primary_vs_pt->Fill(genPt,1.0);
	    m_trackeff_primary_vs_pt_lowpt->Fill(genPt,1.0);
	    if (fabs(genEta)<0.8) m_trackeff_primary_vs_pt_loweta->Fill(genPt,1.0);
	    else if (fabs(genEta)<1.5) m_trackeff_primary_vs_pt_medeta->Fill(genPt,1.0);
	    else m_trackeff_primary_vs_pt_higheta->Fill(genPt,1.0);
            // Efficiency vs log(pt)
            m_trackeff_primary_vs_logpt->Fill(logPt,1.0);
            if (fabs(genEta)<0.8) m_trackeff_primary_vs_logpt_loweta->Fill(logPt,1.0);
            else if (fabs(genEta)<1.5) m_trackeff_primary_vs_logpt_medeta->Fill(logPt,1.0);
            else m_trackeff_primary_vs_logpt_higheta->Fill(logPt,1.0);
	  }
	  if (isSecondary) m_trackeff_secondary_vs_pt->Fill(genPt,1.0);
	} else {
	  if (isPrimary) {
	    m_trackeff_primary_vs_pt->Fill(genPt,0.0);
	    m_trackeff_primary_vs_pt_lowpt->Fill(genPt,0.0);
	    if (fabs(genEta)<0.8) m_trackeff_primary_vs_pt_loweta->Fill(genPt,0.0);
	    else if (fabs(genEta)<1.5) m_trackeff_primary_vs_pt_medeta->Fill(genPt,0.0);
	    else m_trackeff_primary_vs_pt_higheta->Fill(genPt,0.0);
            // Efficiency vs log(pt)
            m_trackeff_primary_vs_logpt->Fill(logPt,0.0);
            if (fabs(genEta)<0.8) m_trackeff_primary_vs_logpt_loweta->Fill(logPt,0.0);
            else if (fabs(genEta)<1.5) m_trackeff_primary_vs_logpt_medeta->Fill(logPt,0.0);
            else m_trackeff_primary_vs_logpt_higheta->Fill(logPt,0.0);
	  }
	  if (isSecondary) m_trackeff_secondary_vs_pt->Fill(genPt,0.0);
	}
      }
      // BH
      //delete generatedTrackPerigee;
    }
    m_nprimperevent->Fill(nprimperevent);
    m_nsecperevent->Fill(nsecperevent);
    m_nprimperevent05->Fill(nprimperevent05);
    m_nsecperevent05->Fill(nsecperevent05);

  }

  if (msgLvl(MSG::VERBOSE))
  msg() << "Number of Tracks: all = " << nTracks<< "   selected = " << nTrackSelected<< endmsg;

  m_ntrack->Fill(nTracks);
  m_ntracksel->Fill(nTrackSelected);
  m_nparticle->Fill(nChargedParticle);
  m_nparticlesel->Fill(nChargedParticleSelected);
  m_nbarcode0->Fill(nBarCode0);
  m_nbarcode0match->Fill(nBarCode0Match);
  m_nbarcode100k->Fill(nBarCode100k);
  m_nbarcodegood->Fill(nBarCodeGood);
  m_nbadmatch->Fill(nBadMatch);
  m_nlowprob->Fill(nLowProb);

  return;
}


void IDStandardPerformance::MakeDataPlots(const DataVector<Trk::Track>* trks) { //CB

  int nTracks=0;
  int nTrackSelected=0;
  int nTrackRejected=0;

  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
  DataVector<Trk::Track>::const_iterator trksItr2  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE2 = trks->end();

  for (; trksItr != trksItrE; ++trksItr) {

    // ---------------------------------------------
    //
    // Analyze the hit content of tracks
    //
    // ---------------------------------------------

    nTracks++;

    // ------------------------
    // Track selection tool
    // ------------------------
    if (m_useTrackSelection) {
      if (m_trackSelectorTool->decision(**trksItr)) {
	nTrackSelected++;
	if (msgLvl(MSG::VERBOSE)) {
	  msg() << "track selected!"<<endmsg;
	}
      } else {
	nTrackRejected++;
	if (msgLvl(MSG::VERBOSE)) {
	  msg() << "track rejected!"<<endmsg;
	}
	continue;
      }
    } else {
      nTrackSelected++;
    }
    if (msgLvl(MSG::VERBOSE)) {msg() << "Selected the track!" << endmsg;}

    float trkd0          = -999;
    float trkz0          = -999;
    float trkphi         = -999;
    float trktheta       = -999;
    float trketa         = -999;
    float trkpt          = -999;
    //float trkcot         = -999;
    float trkqOverPt     = -999;
    float charge         = 0;
    float trkd0c         = -999;
    float trkd0err       = -999;
    float trkz0err       = -999;
    float trkphierr      = -999;
    float trkthetaerr    = -999;
    //float trkcoterr      = -999;
    float trkqopterr     = -999;

    // Get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    // const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();

    const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();
    const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
    if (startPerigee == NULL) {
      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee => skip track"<<endmsg;
      continue;
    }
    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );

    if (!(startPerigee->covariance())) {
      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
    } else {
      AmgVector(5) perigeeParams = startPerigee->parameters();
      trkd0    = perigeeParams[Trk::d0];
      trkz0    = perigeeParams[Trk::z0];
      trkphi   = perigeeParams[Trk::phi0];
      trktheta = perigeeParams[Trk::theta];
      //trkcot   = 1./tan(trktheta);
      trketa   = - log (tan(trktheta / 2.0));
      trkqOverPt  = perigeeParams[Trk::qOverP]*1000./sin(trktheta);
      //trkpt    = abs(1.0 / trkqOverPt);
      trkpt    = fabs(1.0 / trkqOverPt);
      //std::cout <<"trkpt = "<<trkpt<<"theta = "<<trktheta<<" sin(trktheta) = "<<sin(trktheta)<<" qOverPt = "<<trkqOverPt<<"perigeeParams[Trk::qOverP] = "<< perigeeParams[Trk::qOverP]<<std::endl;
      if (trkqOverPt >-10e-6 && trkqOverPt <10e-6) charge=-2;//unphysical default if no magnetic field, (as can happen in Data) CB
      else if (trkqOverPt<0) charge=-1;
      else charge=+1;
      
      const AmgSymMatrix(5) * ErrorMat = startPerigee->covariance();
      double mtheta = (startPerigee->parameters())[Trk::theta];
      double mqp = (startPerigee->parameters())[Trk::qOverP];
      Trk::JacobianThetaPToCotThetaPt TheJac(mtheta,mqp);
      AmgSymMatrix(5) covVert;
      covVert = ErrorMat->similarity(TheJac);
      trkd0err = Amg::error(covVert,0);
      trkz0err = Amg::error(covVert,1);
      trkphierr = Amg::error(covVert,2);
      //trkcoterr = Amg::error(covVert,3);
      trkqopterr = Amg::error(covVert,4)*1000.;
      trkthetaerr = Amg::error((*ErrorMat),Trk::theta);
    }

    if (trkphi<0) trkphi+=2*m_Pi;

    float chi2oDoF = -1;
    if (fitQual) {
      float chisquared = fitQual->chiSquared();
      int DoF = fitQual->numberDoF();
      if(DoF>0) chi2oDoF = chisquared/(float)DoF;
    }
   
    m_recpT->Fill(trkpt);
    m_recpTlow->Fill(trkpt);
    m_recphi->Fill(trkphi);
    m_receta->Fill(trketa);
    m_recd0->Fill(trkd0);
    m_recz0->Fill(trkz0);
    m_recd0c->Fill(trkd0c); // Currently just -999
    m_recchi2->Fill(chi2oDoF);
    m_reccharge->Fill(charge);

    if (trkpt>1) m_d0sig_trackpT1->Fill(trkd0/trkd0err);
    if (trkpt>2) m_d0sig_trackpT2->Fill(trkd0/trkd0err);
    if (trkpt>5) m_d0sig_trackpT5->Fill(trkd0/trkd0err);
    if (trkpt>10) m_d0sig_trackpT10->Fill(trkd0/trkd0err);

    if (msgLvl(MSG::VERBOSE)) msg() << "track parameters: d0=" <<trkd0 << " +/-"<< trkd0err<<" , z0=" <<trkz0 << " +/-"<< trkz0err
      << " , q/pt=" <<trkqOverPt << " +/-"<< trkqopterr << " , phi=" <<trkphi << " +/-"<< trkphierr<< " , theta=" <<trktheta<< " +/-"<< trkthetaerr<<endmsg;

    // Now check for duplicates/close-by tracks
    if (msgLvl(MSG::VERBOSE)) {
      for (; trksItr2 != trksItrE2 && trksItr2 != trksItr; ++trksItr2) {
	if (m_useTrackSelection) {
	  if (!m_trackSelectorTool->decision(**trksItr2)) {
	    continue;
	  }
	}
	const Trk::Perigee* startPerigee2 = (*trksItr2)->perigeeParameters();
	//const Trk::MeasuredPerigee* measPer2 = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee2 );
	if (!(startPerigee2->covariance())) {
	  msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
	  continue;
	}
	AmgVector(5) perigeeParams2 = startPerigee2->parameters();

	float trketa2   = - log(tan(perigeeParams2[Trk::theta] / 2.0));
	float trkphi2   = perigeeParams2[Trk::phi0];
	if (trkphi2<0) trkphi2+=2*m_Pi;
	float dphi=acos(cos(trkphi-trkphi2));
	float deta=trketa-trketa2;
	float dr=sqrt(dphi*dphi+deta*deta);
	if (msgLvl(MSG::VERBOSE)) msg() << "Distance of this track to next closest track: "<<dr<<endmsg;
      }
    }
  }

  if (msgLvl(MSG::VERBOSE))
    msg() << "Number of Tracks: all = " << nTracks<< "   selected = " << nTrackSelected<< endmsg;
  m_ntrack->Fill(nTracks);
  m_ntracksel->Fill(nTrackSelected);

  return;
}



void IDStandardPerformance::MakeHitPlots(const DataVector<Trk::Track>* trks){
  // This function determines general track properties and hit efficiencies and can be run both on data and MC

  const int nPixelLayers=m_PIX_Mgr->numerology().numLayers();
  const int nPixelDisks=m_PIX_Mgr->numerology().numDisks();

  //MsgStream log( msgSvc(), name() );

  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();

  for (; trksItr != trksItrE; ++trksItr) {

    //int trkAuthor = (*trksItr)->info().author();
    //if (msgLvl(MSG::VERBOSE)) msg() << (*trksItr)->info().dumpInfo() << endmsg;

    if (m_useTrackSelection) {
      if (!m_trackSelectorTool->decision(**trksItr)) {
        continue;
      }
    }
    // ---------------------------------------------
    //
    // Analyze the hit content of tracks
    //
    // ---------------------------------------------

    // Get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    // const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();
    const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
    if (startPerigee == NULL) {
      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee"<<endmsg;
      continue;
    }
    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );

    if (!(startPerigee->covariance())) {
      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
      continue;
    }
    AmgVector(5) perigeeParams = startPerigee->parameters();

    //float trkz0    = perigeeParams[Trk::z0];
    //trkphi         = perigeeParams[Trk::phi0];
    //trktheta       = perigeeParams[Trk::theta];
    //trkcot         = 1./tan(trktheta);
    float trketa   = - log(tan(perigeeParams[Trk::theta] / 2.0));
    //float trkpt    = fabs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
    float trkpt    = fabs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
    float trkphi   = perigeeParams[Trk::phi0];

    //for each track see if we can get updator tool
    //if (m_updator) {
    if (! m_updatorHandle.empty()){
      m_isUnbiased = 1;
    } else {
      m_isUnbiased = 0;
    }

    // ----------------------------------------------------------------
    // Use TrackSummary object to get hit content of tracks
    // ----------------------------------------------------------------
    //Trk::Track& nonConstTrack = const_cast<Trk::Track&>(**trksItr);
    //m_trkSummaryTool->updateTrack(nonConstTrack);
    //Trk::TrackSummary* summary = new Trk::TrackSummary(*(nonConstTrack.trackSummary()));
    auto_ptr<const Trk::TrackSummary> summary(m_trkSummaryTool->createSummary(**trksItr));

    if (msgLvl(MSG::VERBOSE)) {
      msg() << "Analyze Hit Content using TrackSummary object" << endmsg;
      msg() << "track hits: PIX: " <<summary->get(Trk::numberOfPixelHits) << "  SCT: " << summary->get(Trk::numberOfSCTHits)<< "   TRT:" <<summary->get(Trk::numberOfTRTHits)<<endmsg;
    }

    float eta_histo = trketa;
    bool havesummary = false;
    if (m_plotsVsAbsEta) eta_histo = fabs(trketa);
    // Invalid entries means all contents of the summary are -1
    if (summary.get() and summary->get(Trk::numberOfSCTHoles) >=0) {
      havesummary = true;
      // IBL 
      m_HitContent_NiblHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerHits));
      m_HitContent_NiblOutliers->Fill(summary->get(Trk::numberOfInnermostPixelLayerOutliers));
      m_HitContent_NiblSharedHits->Fill(summary->get(Trk::numberOfInnermostPixelLayerSharedHits));
      m_HitContent_vs_eta_NiblHits->Fill(eta_histo,summary->get(Trk::numberOfInnermostPixelLayerHits));
      m_HitContent_vs_eta_NiblOutliers->Fill(eta_histo,summary->get(Trk::numberOfInnermostPixelLayerOutliers));
      m_HitContent_vs_eta_NiblSharedHits->Fill(eta_histo,summary->get(Trk::numberOfInnermostPixelLayerSharedHits));
      // B-layer
      m_HitContent_NBlayerHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerHits));
      m_HitContent_NBlayerOutliers->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerOutliers));
      m_HitContent_NBlayerSharedHits->Fill(summary->get(Trk::numberOfNextToInnermostPixelLayerSharedHits));
      m_HitContent_vs_eta_NBlayerHits->Fill(eta_histo,summary->get(Trk::numberOfNextToInnermostPixelLayerHits));
      m_HitContent_vs_eta_NBlayerOutliers->Fill(eta_histo,summary->get(Trk::numberOfNextToInnermostPixelLayerOutliers));
      m_HitContent_vs_eta_NBlayerSharedHits->Fill(eta_histo,summary->get(Trk::numberOfNextToInnermostPixelLayerSharedHits));
      // Pixel
      m_HitContent_NPixelHits->Fill(summary->get(Trk::numberOfPixelHits));
      m_HitContent_NPixelHoles->Fill(summary->get(Trk::numberOfPixelHoles));
      // BH doesnot exist yet in 14.4.0
      //m_HitContent_NPixelOutliers->Fill(summary->get(Trk::numberOfPixelOutliers));
      m_HitContent_NPixelContribLayers->Fill(summary->get(Trk::numberOfContribPixelLayers));
      m_HitContent_NPixelSharedHits->Fill(summary->get(Trk::numberOfPixelSharedHits));
      m_HitContent_NPixelGangedHits->Fill(summary->get(Trk::numberOfGangedPixels));
      m_HitContent_NPixelGangedHitsFlaggedFakes->Fill(summary->get(Trk::numberOfGangedFlaggedFakes));
      m_HitContent_vs_eta_NPixelHits->Fill(eta_histo,summary->get(Trk::numberOfPixelHits));
      m_HitContent_vs_eta_NPixelHoles->Fill(eta_histo,summary->get(Trk::numberOfPixelHoles));
      m_HitContent_vs_phi_NPixelHoles->Fill(trkphi,summary->get(Trk::numberOfPixelHoles));
      // BH doesnot exist yet in 14.4.0
      //m_HitContent_vs_eta_NPixelOutliers->Fill(eta_histo,summary->get(Trk::numberOfPixelOutliers));
      m_HitContent_vs_eta_NPixelContribLayers->Fill(eta_histo,summary->get(Trk::numberOfContribPixelLayers));
      m_HitContent_vs_eta_NPixelSharedHits->Fill(eta_histo,summary->get(Trk::numberOfPixelSharedHits));
      m_HitContent_vs_eta_NPixelGangedHits->Fill(eta_histo,summary->get(Trk::numberOfGangedPixels));
      m_HitContent_vs_eta_NPixelGangedHitsFlaggedFakes->Fill(eta_histo,summary->get(Trk::numberOfGangedFlaggedFakes));
      // SCT
      m_HitContent_NSCTHits->Fill(summary->get(Trk::numberOfSCTHits));
      m_HitContent_NSCTHoles->Fill(summary->get(Trk::numberOfSCTHoles));
      m_HitContent_NSCTDoubleHoles->Fill(summary->get(Trk::numberOfSCTDoubleHoles));
      m_HitContent_NSCTSharedHits->Fill(summary->get(Trk::numberOfSCTSharedHits));
      m_HitContent_vs_eta_NSCTHits->Fill(eta_histo,summary->get(Trk::numberOfSCTHits));
      m_HitContent_vs_eta_NSCTHoles->Fill(eta_histo,summary->get(Trk::numberOfSCTHoles));
      m_HitContent_vs_eta_NSCTDoubleHoles->Fill(eta_histo,summary->get(Trk::numberOfSCTDoubleHoles));
      m_HitContent_vs_eta_NSCTSharedHits->Fill(eta_histo,summary->get(Trk::numberOfSCTSharedHits));

      // Hit map plot a la InDetRecStatistics
      m_nHits_blay_2d->Fill(trketa,trkphi,summary->get(Trk::numberOfNextToInnermostPixelLayerHits)); //correctly looking at B-Layer for Run-2
      m_nExpectedHits_blay_2d->Fill(trketa,trkphi,summary->get(Trk::expectNextToInnermostPixelLayerHit)); //correctly looking at B-Layer for Run-2
      m_nHits_pixel_2d->Fill(trketa,trkphi,summary->get(Trk::numberOfPixelHits));
      m_nHits_SCT_2d->Fill(trketa,trkphi,summary->get(Trk::numberOfSCTHits));
    }

    // TRT-only tracks have TrackSummary information only for TRT
    if (summary.get() and summary->get(Trk::numberOfTRTHits) >=0) {
      havesummary = true;
      // TRT
      m_HitContent_NTRTHits->Fill(summary->get(Trk::numberOfTRTHits));
      m_HitContent_NTRTOutliers->Fill(summary->get(Trk::numberOfTRTOutliers));
      m_HitContent_NTRTHighThresholdHits->Fill(summary->get(Trk::numberOfTRTHighThresholdHits));
      m_HitContent_NTRTHighThresholdOutliers->Fill(summary->get(Trk::numberOfTRTHighThresholdOutliers));
      m_HitContent_vs_eta_NTRTHits->Fill(eta_histo,summary->get(Trk::numberOfTRTHits));
      m_HitContent_vs_eta_NTRTOutliers->Fill(eta_histo,summary->get(Trk::numberOfTRTOutliers));
      m_HitContent_vs_eta_NTRTHighThresholdHits->Fill(eta_histo,summary->get(Trk::numberOfTRTHighThresholdHits));
      m_HitContent_vs_eta_NTRTHighThresholdOutliers->Fill(eta_histo,summary->get(Trk::numberOfTRTHighThresholdOutliers));

      m_nHits_TRT_2d->Fill(trketa,trkphi,summary->get(Trk::numberOfTRTHits));
    }
    
    if (!havesummary && msgLvl(MSG::VERBOSE)) msg() << "Could not find TrackSummary object"<<endmsg;

    // Only use tracks>800 MeV for the hit efficiencies and residuals
    // to avoid crashes that sometimes occur for low pt tracks for the hole search tool
    if (msgLvl(MSG::VERBOSE)) msg() << "Calculate hit efficiencies and residuals for track : pT = " << trkpt << "  eta = " << trketa<<endmsg;
    if (trkpt>0.8) {
      auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**trksItr));

      // Loop over all hits on track
      DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin();
      for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {
        if (not *TSOSItr ) {
          msg(MSG::ERROR) << "pointer to TSOS is NULL: this should never happen" << (*TSOSItr) <<endmsg;
          continue;
        }
        Identifier surfaceID;
        const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
        const Trk::RIO_OnTrack* hit = mesb ? dynamic_cast<const Trk::RIO_OnTrack*>(mesb) : 0;
        if (mesb && !hit) continue;  // skip pseudomeasurements etc.
        // hits, outliers
        if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
          surfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
        } else { // holes, perigee
          if (not (*TSOSItr)->trackParameters() ) {
            msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
            continue;
          }
          surfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
        }
        bool anyhit = (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)
          || (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier)
          || (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole);
        int ilayB=-1; // barrel
        int ilayE=-1; // endcaps
        int itrt=-1;

        if (anyhit) {
          // Pixel and SCT barrel
          if (m_idHelper->is_pixel(surfaceID)) {
            if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ) {
              if (msgLvl(MSG::VERBOSE)) {
                msg() << "Pix Hit : Layer = " << m_pixelID->layer_disk(surfaceID) << "  B/EC "
                      << m_pixelID->barrel_ec(surfaceID) << "  " << m_pixelID->eta_module(surfaceID)
                      << "  " << m_pixelID->phi_module(surfaceID) << endmsg;
              }
            } else if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole) ) {
              if (msgLvl(MSG::VERBOSE)) {
                msg() << "Pix Hole: Layer = " << m_pixelID->layer_disk(surfaceID)<<"  B/EC "
                      << m_pixelID->barrel_ec(surfaceID) << "  "
                      << m_pixelID->eta_module(surfaceID) << "  "
                      << m_pixelID->phi_module(surfaceID) << endmsg;
              }
            } else if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ) {
              if (msgLvl(MSG::VERBOSE)) {
                msg() << "Pix Outl: Layer = " << m_pixelID->layer_disk(surfaceID) << "  B/EC "
                      << m_pixelID->barrel_ec(surfaceID) << "  "
                      << m_pixelID->eta_module(surfaceID) << "  "
                      << m_pixelID->phi_module(surfaceID) << endmsg;
              }
            }
          } else if (m_idHelper->is_sct(surfaceID)){
            if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){
              if (msgLvl(MSG::VERBOSE)) msg() << "SCT Hit : Layer = "<<m_sctID->layer_disk(surfaceID)<< "  B/EC " <<
                      m_sctID->barrel_ec(surfaceID) <<
                      " side = " <<  m_sctID->side(surfaceID)<< "  " << m_sctID->eta_module(surfaceID)<< "  " << m_sctID->phi_module(surfaceID)<< endmsg;
            } else if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole) ){
              if (msgLvl(MSG::VERBOSE)) msg() << "SCT Hole: Layer = "<<m_sctID->layer_disk(surfaceID)<< "  B/EC " <<
                      m_sctID->barrel_ec(surfaceID) <<
                      " side = " <<  m_sctID->side(surfaceID)<< "  " << m_sctID->eta_module(surfaceID)<< "  " << m_sctID->phi_module(surfaceID)<<endmsg;
            } else if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ){
              if (msgLvl(MSG::VERBOSE)) msg() << "SCT Outl: Layer = "<<m_sctID->layer_disk(surfaceID)<< "  B/EC " <<
                      m_sctID->barrel_ec(surfaceID) <<
                      " side = " <<  m_sctID->side(surfaceID)<< "  " << m_sctID->eta_module(surfaceID)<< "  " << m_sctID->phi_module(surfaceID)<<endmsg;
            }
          } else if (!m_doUpgrade && m_idHelper->is_trt(surfaceID)){
            if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){
              if (msgLvl(MSG::VERBOSE)) msg() << "TRT Hit : Layer = "<<m_trtID->barrel_ec(surfaceID)<<endmsg;
            } else if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ){
              if (msgLvl(MSG::VERBOSE)) msg() << "TRT Outl: Layer = "<<m_trtID->barrel_ec(surfaceID)<<endmsg;
            }
          }
          if (m_idHelper->is_pixel(surfaceID)){
              // for the default ATLAS geometry 0 is barrel and +/-1 is endcap
            if (m_pixelID->barrel_ec(surfaceID) == 0){
              ilayB = m_pixelID->layer_disk(surfaceID);
            } else {
              ilayE = m_pixelID->layer_disk(surfaceID);
            }
          } else if (m_idHelper->is_sct(surfaceID)){
            if (!m_doUpgrade) {
              // for the default ATLAS geometry 0 is barrel and +/-1 is endcap
              if (m_sctID->barrel_ec(surfaceID) == 0){
                ilayB=nPixelLayers+m_sctID->layer_disk(surfaceID);
              } else {
                ilayE=nPixelDisks+m_sctID->layer_disk(surfaceID);
              }
            } else {
              // for the upgraded detector +/-1 is barrel and +/-2 is endcap
              if (abs(m_sctID->barrel_ec(surfaceID)) <2){
                ilayB=nPixelLayers+m_sctID->layer_disk(surfaceID);
              } else if (abs(m_sctID->barrel_ec(surfaceID)) == 2){
                ilayE=nPixelDisks+m_sctID->layer_disk(surfaceID);
              }
            }
          } else if (!m_doUpgrade && m_idHelper->is_trt(surfaceID)){
            if (abs(m_trtID->barrel_ec(surfaceID))==1){
              itrt=1; // TRT barrel
            } else {
              itrt=2; // TRT endcap
            }
          } else {
            if (!m_doUpgrade) {
              msg(MSG::ERROR) << "Spurious hit that is not categorised: Pixel " << m_pixelID->barrel_ec(surfaceID)
                << "  SCT: " << m_sctID->barrel_ec(surfaceID) << "  TRT: " << m_trtID->barrel_ec(surfaceID) <<endmsg;
            } else {
              msg(MSG::ERROR) << "Spurious hit that is not categorised: Pixel " << m_pixelID->barrel_ec(surfaceID)
                << "  SCT: " << m_sctID->barrel_ec(surfaceID)  <<endmsg;
            }
          }

        // now get hit efficiency versus eta
          if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement) ){
            if (ilayB==0) { // IBL
              m_eff_hit_vs_eta_ibl_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB==1) { // B-layer
              m_eff_hit_vs_eta_blay_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB>=0) { // entire pixel and SCT barrel detectors
              if (ilayB<nPixelLayers) m_eff_hit_vs_eta_pix_barrel->Fill(fabs(trketa),1);
              else m_eff_hit_vs_eta_sct_barrel->Fill(fabs(trketa),1);
            }
            if (ilayE>=0){ // pixel and SCT endcap
              if (ilayE<nPixelDisks) m_eff_hit_vs_eta_pix_endcap->Fill(fabs(trketa),1);
              else m_eff_hit_vs_eta_sct_endcap->Fill(fabs(trketa),1);
            }
          // TRT barrel and endcap
            if (itrt==1) m_eff_hit_vs_eta_trt_barrel->Fill(fabs(trketa),1);
            else if (itrt==2) m_eff_hit_vs_eta_trt_endcap->Fill(fabs(trketa),1);
          } else {
            if (ilayB==0) { // IBL
              m_eff_hit_vs_eta_ibl_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB==1) { // B-layer
              m_eff_hit_vs_eta_blay_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB>=0) {// entire pixel and SCT barrel detectors
              if (ilayB<nPixelLayers) m_eff_hit_vs_eta_pix_barrel->Fill(fabs(trketa),0);
              else m_eff_hit_vs_eta_sct_barrel->Fill(fabs(trketa),0);
            }
            if (ilayE>=0){// pixel and SCT endcap
              if (ilayE<nPixelDisks) m_eff_hit_vs_eta_pix_endcap->Fill(fabs(trketa),0);
              else m_eff_hit_vs_eta_sct_endcap->Fill(fabs(trketa),0);
            }
          // TRT barrel and endcap
            if (itrt==1) m_eff_hit_vs_eta_trt_barrel->Fill(fabs(trketa),0);
            else if (itrt==2) m_eff_hit_vs_eta_trt_endcap->Fill(fabs(trketa),0);
          }
        // now get fraction of holes versus eta
          if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole) ){
            if (ilayB==0) {
              m_frac_hole_vs_eta_ibl_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB==1) {
              m_frac_hole_vs_eta_blay_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB>=0) {
              if (ilayB<nPixelLayers) m_frac_hole_vs_eta_pix_barrel->Fill(fabs(trketa),1);
              else m_frac_hole_vs_eta_sct_barrel->Fill(fabs(trketa),1);
            }
            if (ilayE>=0) {
              if (ilayE<nPixelDisks) m_frac_hole_vs_eta_pix_endcap->Fill(fabs(trketa),1);
              else m_frac_hole_vs_eta_sct_endcap->Fill(fabs(trketa),1);
            }
            if (itrt==1) m_frac_hole_vs_eta_trt_barrel->Fill(fabs(trketa),1);
            else if (itrt==2) m_frac_hole_vs_eta_trt_endcap->Fill(fabs(trketa),1);
          } else {
            if (ilayB==0) {
              m_frac_hole_vs_eta_ibl_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB==1) {
              m_frac_hole_vs_eta_blay_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB>=0) {
              if (ilayB<nPixelLayers) m_frac_hole_vs_eta_pix_barrel->Fill(fabs(trketa),0);
              else m_frac_hole_vs_eta_sct_barrel->Fill(fabs(trketa),0);
            }
            if (ilayE>=0) {
              if (ilayE<nPixelDisks) m_frac_hole_vs_eta_pix_endcap->Fill(fabs(trketa),0);
              else m_frac_hole_vs_eta_sct_endcap->Fill(fabs(trketa),0);
            }
            if (itrt==1) m_frac_hole_vs_eta_trt_barrel->Fill(fabs(trketa),0);
            else if (itrt==2) m_frac_hole_vs_eta_trt_endcap->Fill(fabs(trketa),0);
          }
        // now get fraction of outliers versus eta
          if ( (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier) ){
            if (ilayB==0) {
              m_frac_outlier_vs_eta_ibl_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB==1) {
              m_frac_outlier_vs_eta_blay_barrel->Fill(fabs(trketa),1);
            }
            if (ilayB>=0) {
              if (ilayB<nPixelLayers) m_frac_outlier_vs_eta_pix_barrel->Fill(fabs(trketa),1);
              else m_frac_outlier_vs_eta_sct_barrel->Fill(fabs(trketa),1);
            }
            if (ilayE>=0) {
              if (ilayE<nPixelDisks) m_frac_outlier_vs_eta_pix_endcap->Fill(fabs(trketa),1);
              else m_frac_outlier_vs_eta_sct_endcap->Fill(fabs(trketa),1);
            }
            if (itrt==1) m_frac_outlier_vs_eta_trt_barrel->Fill(fabs(trketa),1);
            else if (itrt==2) m_frac_outlier_vs_eta_trt_endcap->Fill(fabs(trketa),1);
          } else {
            if (ilayB==0) {
              m_frac_outlier_vs_eta_ibl_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB==1) {
              m_frac_outlier_vs_eta_blay_barrel->Fill(fabs(trketa),0);
            }
            if (ilayB>=0) {
              if (ilayB<nPixelLayers) m_frac_outlier_vs_eta_pix_barrel->Fill(fabs(trketa),0);
              else m_frac_outlier_vs_eta_sct_barrel->Fill(fabs(trketa),0);
            }
            if (ilayE>=0) {
              if (ilayE<nPixelDisks) m_frac_outlier_vs_eta_pix_endcap->Fill(fabs(trketa),0);
              else m_frac_outlier_vs_eta_sct_endcap->Fill(fabs(trketa),0);
            }
            if (itrt==1) m_frac_outlier_vs_eta_trt_barrel->Fill(fabs(trketa),0);
            else if (itrt==2) m_frac_outlier_vs_eta_trt_endcap->Fill(fabs(trketa),0);
          }
        }
      // Make hit map plot (a la InDetRecStatistics)
        if (mesb){
          if (hit && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)){
          // Pixel or SCT
            if (m_idHelper->is_pixel(surfaceID) ||  m_idHelper->is_sct(surfaceID)) {
              const Amg::Vector3D& gp  = hit->globalPosition();
              m_nHits_ALL_RZ->Fill( gp.z(),sqrt(pow(gp.x(),2)+pow(gp.y(),2)) );
            } // Pixel or SCT
            else if (m_idHelper->is_trt(surfaceID)){
              const InDet::TRT_DriftCircleOnTrack *trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(hit);
              if (trtcircle) {
                const Amg::Vector3D& gp = trtcircle->globalPosition();
                m_nHits_ALL_RZ->Fill( gp.z(),sqrt(pow(gp.x(),2)+pow(gp.y(),2)) );
              } // TRT circle exist
            } // TRT hit
          } // If hit
        } // End hit map plot
      // now calculate residuals
        const Trk::TrackParameters* biasedTrackParameters = (*TSOSItr)->trackParameters();

        if (mesb && biasedTrackParameters) {
          // track parameters at outliers are already unbiased by definition
          const Trk::TrackParameters *trackParameters = (!(*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier)) ? getUnbiasedTrackParameters(biasedTrackParameters,mesb) : biasedTrackParameters;
          if (!m_residualPullCalculator.empty()) {
          // fill general residuals and pulls
          // for the residuals and pulls always use the ROT.
          // --------------------------------- -----
            float residualLocX = -1000.;
            float residualLocY = -1000.;
            float pullLocX     = -1000.;
            float pullLocY     = -1000.;
	    int phiWidth = -1;
	    int zWidth   = -1;
	    // float pullLocY     = -1000.;
            if (hit && m_isUnbiased && trkpt>m_minTrackPtRes) {
	      // Cluster width determination
	      if(m_idHelper->is_pixel(surfaceID) ||  m_idHelper->is_sct(surfaceID)) {
		const InDet::SiCluster* pCluster = dynamic_cast <const InDet::SiCluster*>(hit->prepRawData());
		if(pCluster){
		  InDet::SiWidth width = pCluster->width();
		  phiWidth = int(width.colRow().x());
		  zWidth = int(width.colRow().y());
		}
		// Switched for checking:
		//phiWidth = int(width.colRow().y());
		//zWidth = int(width.colRow().x());
		if (m_idHelper->is_pixel(surfaceID))
		  {
		    m_cluster_width_phi_pixel->Fill(phiWidth);
		    m_cluster_width_eta_pixel->Fill(zWidth);
		    m_cluster_z_phi_pixel->Fill(zWidth, phiWidth);
		    m_cluster_width_phi_pixel_eta->Fill(trketa, phiWidth);
		    m_cluster_width_eta_pixel_eta->Fill(trketa, zWidth);
		  }
		if (m_idHelper->is_sct(surfaceID))
		  {
		    m_cluster_width_phi_sct->Fill(phiWidth);
		    m_cluster_z_phi_sct->Fill(zWidth, phiWidth);
		    m_cluster_width_phi_sct_eta->Fill(trketa, phiWidth);
		  }
	      } //simple cluster info done.
	      // Track incidence angle code.
	      const InDetDD::SiDetectorElement* element = NULL;
	      const Trk::PrepRawData* prd = hit->prepRawData();
	      if (m_idHelper->is_pixel(surfaceID))
		{
		  const InDet::PixelCluster* sc =
		    dynamic_cast<const InDet::PixelCluster*>(prd);
		  if (sc)
		    element = sc->detectorElement();
		  //element = m_PIX_Mgr->getDetectorElement(surfaceID);
		}
	      if (!element && m_idHelper->is_sct(surfaceID))
		{
		  const InDet::SiCluster* sc = dynamic_cast<const InDet::SiCluster*>(prd);
		  if (sc)
		    element = sc->detectorElement();
		}
	      if (!element) msg(MSG::DEBUG) << "No element for track incidence angles!" << endmsg;
	      float PixTrkAngle = -1000;
	      float PixTrkThetaI = -1000;
	      if (element)
		{
		  Amg::Vector3D my_track = trackParameters->momentum();
		  Amg::Vector3D my_normal = element->normal();
		  Amg::Vector3D my_phiax = element->phiAxis();
		  Amg::Vector3D my_etaax = element->etaAxis();
		  // track component on etaAxis:
		  float trketacomp = my_track.dot(my_etaax);
                  // track component on phiAxis:
		  float trkphicomp = my_track.dot(my_phiax);
		  // track component on the normal to the module
		  float trknormcomp = my_track.dot(my_normal);
		  // Track angle
		  PixTrkAngle = atan2(trkphicomp,trknormcomp);
		  PixTrkThetaI = atan2(trketacomp,trknormcomp);
		  // reducing the angle in the right quadrant
		  if(PixTrkThetaI > M_PI / 2 ) PixTrkThetaI -= M_PI;
		  else if(PixTrkThetaI < - M_PI / 2 ) PixTrkThetaI += M_PI;
		  PixTrkThetaI = ( M_PI / 2 ) - PixTrkThetaI;
		  if(PixTrkAngle > M_PI / 2 ) PixTrkAngle -= M_PI;
		  else if(PixTrkAngle < - M_PI / 2 ) PixTrkAngle += M_PI;
		  PixTrkAngle = ( M_PI / 2 ) - PixTrkAngle;
		}
	      detAreaTypes detArea = n_detAreaTypes;
	      Trk::ResidualPull::ResidualType resType = m_isUnbiased
                                                      ? Trk::ResidualPull::Unbiased
                                                      : Trk::ResidualPull::Biased;
              const auto_ptr<const Trk::ResidualPull> residualPull(m_residualPullCalculator->residualPull(hit,
                                                             trackParameters,
                                                             resType));
              fillPixelTrackPullHistos(surfaceID, *TSOSItr, residualPull);

              if (msgLvl(MSG::VERBOSE)) msg() << "obtained Hit Residual and Pull " << endmsg;
              residualLocX = 1000*residualPull->residual()[Trk::loc1]; // residuals in microns
              pullLocX = residualPull->pull()[Trk::loc1]; // residuals in microns
              detArea = pix_b;
              if (ilayB>=0 && ilayB<nPixelLayers) { // Pixel
                m_residualx_pixel_barrel->Fill(residualLocX);//set 1
		if (phiWidth==1) m_residualx_pixel_barrel_1hit->Fill(residualLocX);//set 2
		if (phiWidth>=2) m_residualx_pixel_barrel_2ormorehits->Fill(residualLocX);//set 3
                m_residualx_pixel_barrel_l[ilayB]->Fill(residualLocX);//set 5
                m_residualpullx_pixel_barrel->Fill(pullLocX);//set 4
                m_residualpullx_pixel_barrel_l[ilayB]->Fill(pullLocX);//set 6
                if (residualPull->dimension() >= 2) {
                  residualLocY = 1000*residualPull->residual()[Trk::loc2];
                  pullLocY = residualPull->pull()[Trk::loc2];
                  m_residualy_pixel_barrel->Fill(residualLocY);//set 1
		  if (zWidth==1) m_residualy_pixel_barrel_1hit->Fill(residualLocY);//set 2
		  if (zWidth>=2) m_residualy_pixel_barrel_2ormorehits->Fill(residualLocY);//set 3
                  m_residualy_pixel_barrel_l[ilayB]->Fill(residualLocY);//set 5
                  m_residualpully_pixel_barrel->Fill(pullLocY);//set 4
                  m_residualpully_pixel_barrel_l[ilayB]->Fill(pullLocY);// set 6
                }
              } else if (ilayB>=nPixelLayers) { // SCT
		detArea = sct_b;
                m_residualx_sct_barrel->Fill(residualLocX);
		if (phiWidth==1) m_residualx_sct_barrel_1hit->Fill(residualLocX);
		if (phiWidth>=2) m_residualx_sct_barrel_2ormorehits->Fill(residualLocX);
                m_residualx_sct_barrel_l[ilayB-nPixelLayers]->Fill(residualLocX);
                m_residualpullx_sct_barrel->Fill(pullLocX);
                m_residualpullx_sct_barrel_l[ilayB-nPixelLayers]->Fill(pullLocX);
              }
              if (ilayE>=0 && ilayE<nPixelDisks) { // Pixel
		detArea = pix_e;
                m_residualx_pixel_endcap->Fill(residualLocX);
		if (phiWidth==1) m_residualx_pixel_endcap_1hit->Fill(residualLocX);
		if (phiWidth>=2) m_residualx_pixel_endcap_2ormorehits->Fill(residualLocX);
                m_residualx_pixel_endcap_d[ilayE]->Fill(residualLocX);
                m_residualpullx_pixel_endcap->Fill(pullLocX);
                m_residualpullx_pixel_endcap_d[ilayE]->Fill(pullLocX);
                if (residualPull->dimension() >= 2) {
                  residualLocY = 1000*residualPull->residual()[Trk::loc2];
                  pullLocY = residualPull->pull()[Trk::loc2];
                  m_residualy_pixel_endcap->Fill(residualLocY);
		  if (zWidth==1) m_residualy_pixel_endcap_1hit->Fill(residualLocY);
		  if (zWidth>=2) m_residualy_pixel_endcap_2ormorehits->Fill(residualLocY);
                  m_residualy_pixel_endcap_d[ilayE]->Fill(residualLocY);
                  m_residualpully_pixel_endcap->Fill(pullLocY);
                  m_residualpully_pixel_endcap_d[ilayE]->Fill(pullLocY);
                }
              } else if (ilayE>nPixelDisks) { // SCT
		detArea = sct_e;
                m_residualx_sct_endcap->Fill(residualLocX);
		if (phiWidth==1) m_residualx_sct_endcap_1hit->Fill(residualLocX);
		if (phiWidth>=2) m_residualx_sct_endcap_2ormorehits->Fill(residualLocX);
                m_residualx_sct_endcap_d[ilayE-nPixelDisks]->Fill(residualLocX);
                m_residualpullx_sct_endcap->Fill(pullLocX);
                m_residualpullx_sct_endcap_d[ilayE-nPixelDisks]->Fill(pullLocX);
              }
              if (itrt==1) {
                m_residualx_trt_barrel->Fill(residualLocX);
                m_residualpullx_trt_barrel->Fill(pullLocX);
              } else if (itrt==2) {
                m_residualx_trt_endcap->Fill(residualLocX);
                m_residualpullx_trt_endcap->Fill(pullLocX);
              }
	      FillTrackIncidencePlots(detArea, phiWidth, zWidth, PixTrkAngle, PixTrkThetaI);

            }
          }
          if (trackParameters != biasedTrackParameters) delete trackParameters;
        }
      }
    }
  }
  return;
}
  void IDStandardPerformance::HitBasedMatching(const DataVector<Trk::Track>* trks,const DataVector<Trk::Track>* pixel_trks, const DataVector<Trk::Track>* sct_trks, const DataVector<Trk::Track>* trt_trks){
	  float trt_hit_threshold = 0.8;

	  if (msgLvl(MSG::DEBUG)) msg() <<"trks->size() = "<<trks->size()<<"\t";
	  if (msgLvl(MSG::DEBUG)) msg() <<"pix trks->size() = "<<pixel_trks->size()<<"\t";
	  if (msgLvl(MSG::DEBUG)) msg() <<"sct trks->size() = "<<sct_trks->size()<<"\t";
	  if (msgLvl(MSG::DEBUG)) msg() <<"trt trks->size() = "<<trt_trks->size()<<"\n";

	  DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
	  DataVector<Trk::Track>::const_iterator trksItrE = trks->end();

	  DataVector<Trk::Track>::const_iterator PixtrksItr  = pixel_trks->begin();
	  DataVector<Trk::Track>::const_iterator PixtrksItrE = pixel_trks->end();

	  DataVector<Trk::Track>::const_iterator SCTtrksItr  = sct_trks->begin();
	  DataVector<Trk::Track>::const_iterator SCTtrksItrE = sct_trks->end();

	  DataVector<Trk::Track>::const_iterator TRTtrksItr  = trt_trks->begin();
	  DataVector<Trk::Track>::const_iterator TRTtrksItrE = trt_trks->end();


	  //apply trackc selection stuff to tracks container, leave tracklets alone
	  
	  /*
	  if (trks->size()==0) return; //this breaks the trackSelectorTool..
	  if (m_useTrackSelection) {
	    if (!m_trackSelectorTool->decision(**trksItr)) {
	      //continue;  //track doesn't pass selection criteria, not much use then
	    }
	  }
	  */

	  Identifier TrkSurfaceID;


	  //iterate over pixel hits on tracks
	  //make vectors of eta, phi and disk layer coord. of hits
	  //std::vector < std::vector<int> > pix_layer,pix_b_ec,pix_eta, pix_phi;
	  //std::vector < std::vector<int> > sct_layer,sct_b_ec,sct_eta, sct_phi;
	  //std::vector < std::vector<int> > trt_layer,trt_b_ec,trt_eta, trt_phi;
	  std::vector < std::vector<int> > pix_coords[4];
	  std::vector < std::vector<int> > sct_coords[5];
	  std::vector < std::vector<int> > trt_coords[2];
	  std::vector < std::vector<int> > trk_pix_coords[4];//layer, barrel/ec, eta, phi module
	  std::vector < std::vector<int> > trk_sct_coords[5];//layer, barrel/ec, side, eta, phi module
	  std::vector < std::vector<int> > trk_trt_coords[2];//barrel/ec, phi module
	  std::vector<float> pix_trk_pt;
	  std::vector<float> pix_trk_eta;
	  std::vector<float> pix_trk_phi;
	  std::vector<float> sct_trk_pt;
	  std::vector<float> sct_trk_eta;
	  std::vector<float> sct_trk_phi;
	  std::vector<float> trt_trk_phi;
	  std::vector<float> trt_trk_pt;


	  //get pixel hits
	  for (; PixtrksItr != PixtrksItrE; ++PixtrksItr) {

	    auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**PixtrksItr));
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin();
	    Identifier SurfaceID;



	    const Trk::Perigee* startPerigee = (*PixtrksItr)->perigeeParameters();
	    if (startPerigee == NULL) {
	      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee"<<endmsg;
	      continue;
	    }
	    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );
	    if (!(startPerigee->covariance())) {
	      msg(MSG::WARNING) << "No covariance nmatrix assigned to the track" << endmsg;
	      continue;
	    }
	    AmgVector(5) perigeeParams = startPerigee->parameters();
	    
	    
	    //these are for track perigee params
	    float trketa   = - log(tan(perigeeParams[Trk::theta] / 2.0));
	    //float trkpt    = abs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    float trkpt    = fabs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    float trkphi   = perigeeParams[Trk::phi0];
	    
	    float trk_d0    = perigeeParams[Trk::d0];
	  
	    float doMax = 5.0;
	    float etaMax = 2.5;
  
	    //track selection on the pixel tracklets... 
	    if (m_useTrackSelection) {
	      if (fabs(trk_d0) > doMax || fabs(trketa) > etaMax)  continue;
	    }
	    
	    pix_coords[0].push_back(std::vector<int>());
	    pix_coords[1].push_back(std::vector<int>());
	    pix_coords[2].push_back(std::vector<int>());
	    pix_coords[3].push_back(std::vector<int>());
	    
	    pix_trk_eta.push_back(trketa);
	    pix_trk_pt.push_back(trkpt);
	    pix_trk_phi.push_back(trkphi);
	     
	    for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {
	      /*
		 pix_layer.push_back(std::vector<int>());
		 pix_b_ec.push_back(std::vector<int>());
		 pix_eta.push_back(std::vector<int>());
		 pix_phi.push_back(std::vector<int>());
	       */


	      if (not *TSOSItr ) {
		msg(MSG::ERROR) << "pointer to TSOS is NULL: this should never happen" << (*TSOSItr) <<endmsg;
		continue;
	      }

	      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
	      // hits, outliers
	      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
		SurfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	      } 
	      else { // holes, perigee
		if (not (*TSOSItr)->trackParameters() ) {
		  msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
		  continue;
		}
		SurfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
	      }

	      bool anyhit = (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement);


	      if (anyhit) {

		if (m_idHelper->is_pixel(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {

		  pix_coords[0].at(pix_coords[0].size()-1).push_back(m_pixelID->layer_disk(SurfaceID));
		  pix_coords[1].at(pix_coords[1].size()-1).push_back(m_pixelID->barrel_ec(SurfaceID));
		  pix_coords[2].at(pix_coords[2].size()-1).push_back(m_pixelID->eta_module(SurfaceID));
		  pix_coords[3].at(pix_coords[3].size()-1).push_back(m_pixelID->phi_module(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<"Pixel Tracklet Hit : Layer = " << m_pixelID->layer_disk(SurfaceID) << "  B/EC " << m_pixelID->barrel_ec(SurfaceID) << "  " << m_pixelID->eta_module(SurfaceID) << "  " << m_pixelID->phi_module(SurfaceID) <<"\tPhysical eta = "<<trketa<<" and phi "<<trkphi<<endmsg;
		}
	      }
	    }//end of loop over pixel hits 
	  }//end of loop over pixel tracks
	 
	  //get SCT tracks
	  for (; SCTtrksItr != SCTtrksItrE; ++SCTtrksItr) {

	    auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**SCTtrksItr));
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin();
	    Identifier SurfaceID;



	    const Trk::Perigee* startPerigee = (*SCTtrksItr)->perigeeParameters();
	    if (startPerigee == NULL) {
	      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee"<<endmsg;
	      continue;
	    }
	    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );
	    if (!(startPerigee->covariance())) {
	      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
	      continue;
	    }
	    AmgVector(5) perigeeParams = startPerigee->parameters();
	    
	    //these are for track perigee params, not SCT track perigee params
	    float trketa   =  - log(tan(perigeeParams[Trk::theta] / 2.0));
	    //float trkpt    = abs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    float trkpt    = fabs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    float trkphi   = perigeeParams[Trk::phi0];
	    
	    float trk_d0    = perigeeParams[Trk::d0];
	  
	    float doMax = 5.0;
	    float etaMax = 2.5;
	    
	    if (m_useTrackSelection) {
	      if (fabs(trk_d0) > doMax || fabs(trketa) > etaMax)  continue;
	    }
	    
	    sct_coords[0].push_back(std::vector<int>());
	    sct_coords[1].push_back(std::vector<int>());
	    sct_coords[2].push_back(std::vector<int>());
	    sct_coords[3].push_back(std::vector<int>());
	    sct_coords[4].push_back(std::vector<int>());

	    sct_trk_eta.push_back(trketa);
	    sct_trk_pt.push_back(trkpt);
	    sct_trk_phi.push_back(trkphi);
	    
	    for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {
	      /*
		 sct_layer.push_back(std::vector<int>());
		 sct_b_ec.push_back(std::vector<int>());
		 sct_eta.push_back(std::vector<int>());
		 sct_phi.push_back(std::vector<int>());
	       */


	      if (not *TSOSItr ) {
		msg(MSG::ERROR) << "pointer to TSOS is NULL: this should never happen" << (*TSOSItr) <<endmsg;
		continue;
	      }

	      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
	      // hits, outliers
	      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
		SurfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	      } 
	      else { // holes, perigee
		if (not (*TSOSItr)->trackParameters() ) {
		  msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
		  continue;
		}
		SurfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
	      }

	      bool anyhit = (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement);

	      if (anyhit) {

		if (m_idHelper->is_sct(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
		  /*
		     sct_layer[sct_layer.size()-1].push_back(m_sctID->layer_disk(SurfaceID));
		     sct_b_ec[sct_layer.size()-1].push_back(m_sctID->barrel_ec(SurfaceID)); 
		     sct_eta[sct_layer.size()-1].push_back(m_sctID->eta_module(SurfaceID)); 
		     sct_phi[sct_layer.size()-1].push_back(m_sctID->phi_module(SurfaceID)); 
		   */

		  sct_coords[0].at(sct_coords[0].size()-1).push_back(m_sctID->layer_disk(SurfaceID));
		  sct_coords[1].at(sct_coords[1].size()-1).push_back(m_sctID->barrel_ec(SurfaceID));
		  sct_coords[2].at(sct_coords[2].size()-1).push_back(m_sctID->eta_module(SurfaceID));
		  sct_coords[3].at(sct_coords[3].size()-1).push_back(m_sctID->phi_module(SurfaceID));
		  sct_coords[4].at(sct_coords[4].size()-1).push_back(m_sctID->side(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<" SCT Hit : side = "<<m_sctID->side(SurfaceID) <<" Layer = " << m_sctID->layer_disk(SurfaceID) << "  B/EC " << m_sctID->barrel_ec(SurfaceID) << "  " << m_sctID->eta_module(SurfaceID) << "  " << m_sctID->phi_module(SurfaceID) <<endmsg;
		}
	      }
	    }//end of loop over sct hits 
	  }//end of loop over sct tracks

	  //get TRT tracks
	  for (; TRTtrksItr != TRTtrksItrE; ++TRTtrksItr) {

	    auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**TRTtrksItr));
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin();
	    Identifier SurfaceID;

	    //need some kind of selcetion cuts
	    trt_coords[0].push_back(std::vector<int>());
	    trt_coords[1].push_back(std::vector<int>());

	    const Trk::Perigee* startPerigee = (*TRTtrksItr)->perigeeParameters();
	    if (startPerigee == NULL) {
	      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee"<<endmsg;
	      continue;
	    }
	    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );
	    if (!(startPerigee->covariance())) {
	      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
	      continue;
	    }
	    AmgVector(5) perigeeParams = startPerigee->parameters();
	    
	    //these are for the track perigee params, not TRT track
	    float trkphi   = perigeeParams[Trk::phi0];
	    //float trkpt    = abs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    float trkpt    = fabs(sin(perigeeParams[Trk::theta])*1000. / perigeeParams[Trk::qOverP]);
	    trt_trk_phi.push_back(trkphi);
	    trt_trk_pt.push_back(trkpt);

	    for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {
	      /*
		 trt_layer.push_back(std::vector<int>());
		 trt_b_ec.push_back(std::vector<int>());
		 trt_eta.push_back(std::vector<int>());
		 trt_phi.push_back(std::vector<int>());
	       */


	      if (not *TSOSItr ) {
		msg(MSG::ERROR) << "pointer to TSOS is NULL: this should never happen" << (*TSOSItr) <<endmsg;
		continue;
	      }

	      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
	      // hits, outliers
	      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
		SurfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	      } 
	      else { // holes, perigee
		if (not (*TSOSItr)->trackParameters() ) {
		  msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
		  continue;
		}
		SurfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
	      }

	      bool anyhit = (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement);
	//	|| (*TSOSItr)->type(Trk::TrackStateOnSurface::Outlier)
	//	|| (*TSOSItr)->type(Trk::TrackStateOnSurface::Hole);


	      if (anyhit) {

		if (m_idHelper->is_trt(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
		  trt_coords[0].at(trt_coords[0].size()-1).push_back(m_trtID->barrel_ec(SurfaceID));
		  trt_coords[1].at(trt_coords[1].size()-1).push_back(m_trtID->phi_module(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<" TRT Hit :   B/EC " << m_trtID->barrel_ec(SurfaceID) <<  "  " << m_trtID->phi_module(SurfaceID) <<endmsg;
		}
	      }
	    }//end of loop over trt hits 
	  }//end of loop over trt tracks


	  int ntrks = 0;
	  //actual track hits
	  for (; trksItr != trksItrE; ++trksItr) {
	    if (m_useTrackSelection) {
	      if (!m_trackSelectorTool->decision(**trksItr)) {
		continue;
	      }
	    }
	  
	    
	    ntrks++;
	    int patternRecoInfo = -1;
	    bool done = 0;
	    int more_than_one = 0;
	    int recinfo1 = -1, recinfo2 = -1; 
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiSPSeededFinder"<<endmsg; patternRecoInfo = 0;if (done){ more_than_one++;recinfo2 = 0;} else {recinfo1 = 0;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiCTBTracking)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiCTBTracking"<<endmsg; patternRecoInfo = 1;if (done){ more_than_one++;recinfo2 = 1;} else {recinfo1 = 1;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::InDetAmbiguitySolver)) {if (msgLvl(MSG::DEBUG)) msg() <<"InDetAmbiguitySolver"<<endmsg; patternRecoInfo = 2;if (done){ more_than_one++;recinfo2 = 2;} else {recinfo1 = 2;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::InDetExtensionProcessor)) {if (msgLvl(MSG::DEBUG)) msg() <<"InDetExtensionProcessor"<<endmsg; patternRecoInfo = 3;if (done){ more_than_one++;recinfo2 = 3;} else {recinfo1 = 3;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TRTSeededTrackFinder)) {if (msgLvl(MSG::DEBUG)) msg() <<"TRTSeededTrackFinder"<<endmsg; patternRecoInfo = 4;if (done){ more_than_one++;recinfo2 = 4;} else {recinfo1 = 4;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::Muonboy)) {if (msgLvl(MSG::DEBUG)) msg() <<"Muonboy"<<endmsg; patternRecoInfo = 5;if (done){ more_than_one++;recinfo2 = 5;} else {recinfo1 = 5;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::MuGirlUnrefitted )) {if (msgLvl(MSG::DEBUG)) msg() <<"MuGirlUnrefitted "<<endmsg; patternRecoInfo = 6;if (done){ more_than_one++;recinfo2 = 6;} else {recinfo1 = 6;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::STACO)) {if (msgLvl(MSG::DEBUG)) msg() <<"STACO"<<endmsg; patternRecoInfo = 7;if (done){ more_than_one++;recinfo2 = 7;} else {recinfo1 = 7;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::StacoLowPt)) {if (msgLvl(MSG::DEBUG)) msg() <<"StacoLowPt"<<endmsg; patternRecoInfo = 8;if (done){ more_than_one++;recinfo2 = 8;} else {recinfo1 = 8;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::MuTag)) {if (msgLvl(MSG::DEBUG)) msg() <<"MuTag"<<endmsg; patternRecoInfo = 9;if (done){ more_than_one++;recinfo2 = 9;} else {recinfo1 = 9;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::MooreToTrackTool)) {if (msgLvl(MSG::DEBUG)) msg() <<"MooreToTrackTool"<<endmsg; patternRecoInfo = 10;if (done){ more_than_one++;recinfo2 = 10;} else {recinfo1 = 10;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TrigIDSCAN )) {if (msgLvl(MSG::DEBUG)) msg() <<"TrigIDSCAN "<<endmsg; patternRecoInfo = 11;if (done){ more_than_one++;recinfo2 = 11;} else {recinfo1 = 11;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TrigSiTrack)) {if (msgLvl(MSG::DEBUG)) msg() <<"TrigSiTrack"<<endmsg; patternRecoInfo = 12;if (done){ more_than_one++;recinfo2 = 12;} else {recinfo1 = 12;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TrigTRTxK)) {if (msgLvl(MSG::DEBUG)) msg() <<"TrigTRTxK"<<endmsg; patternRecoInfo = 13;if (done){ more_than_one++;recinfo2 = 13;} else {recinfo1 = 13;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TrigTRTLUT)) {if (msgLvl(MSG::DEBUG)) msg() <<"TrigTRTLUT"<<endmsg; patternRecoInfo = 14;if (done){ more_than_one++;recinfo2 = 14;} else {recinfo1 = 14;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::Fatras)) {if (msgLvl(MSG::DEBUG)) msg() <<"Fatras"<<endmsg; patternRecoInfo = 15;if (done){ more_than_one++;recinfo2 = 15;} else {recinfo1 = 15;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::iPatLegacyCnv)) {if (msgLvl(MSG::DEBUG)) msg() <<"iPatLegacyCnv"<<endmsg; patternRecoInfo = 16;if (done){ more_than_one++;recinfo2 = 16;} else {recinfo1 = 16;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::xKalmanLegacyCnv)) {if (msgLvl(MSG::DEBUG)) msg() <<"xKalmanLegacyCnv"<<endmsg; patternRecoInfo = 17;if (done){ more_than_one++;recinfo2 = 17;} else {recinfo1 = 17;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool)) {if (msgLvl(MSG::DEBUG)) msg() <<"SimpleAmbiguityProcessorTool"<<endmsg; patternRecoInfo = 18;if (done){ more_than_one++;recinfo2 =18 ;} else {recinfo1 = 18;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::InDetAmbiTrackSelectionTool)) {if (msgLvl(MSG::DEBUG)) msg() <<"InDetAmbiTrackSelectionTool"<<endmsg; patternRecoInfo = 19;if (done){ more_than_one++;recinfo2 = 19;} else {recinfo1 = 19;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TRTStandalone)) {if (msgLvl(MSG::DEBUG)) msg() <<"TRTStandalone"<<endmsg; patternRecoInfo = 20;if (done){ more_than_one++;recinfo2 = 20;} else {recinfo1 = 20;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::MuidStandAlone)) {if (msgLvl(MSG::DEBUG)) msg() <<"MuidStandAlone"<<endmsg; patternRecoInfo = 21;if (done){ more_than_one++;recinfo2 = 21;} else {recinfo1 = 21;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::TRTSeededSingleSpTrackFinder)) {if (msgLvl(MSG::DEBUG)) msg() <<"TRTSeededSingleSpTrackFinder"<<endmsg; patternRecoInfo = 22;if (done){ more_than_one++;recinfo2 = 22;} else {recinfo1 = 22;} done = 1;}
	    
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::iPatRec)) {if (msgLvl(MSG::DEBUG)) msg() <<"iPatRec"<<endmsg; patternRecoInfo = 29;if (done){ more_than_one++;recinfo2 = 29;} else {recinfo1 = 29;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::FatrasSimulation)) {if (msgLvl(MSG::DEBUG)) msg() <<"FatrasSimulation"<<endmsg; patternRecoInfo = 31;if (done){ more_than_one++;recinfo2 = 31;} else {recinfo1 = 31;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiSpacePointsSeedMaker_Cosmic"<<endmsg; patternRecoInfo = 34;if (done){ more_than_one++;recinfo2 = 34;} else {recinfo1 = 34;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiSpacePointsSeedMaker_HeavyIon"<<endmsg; patternRecoInfo = 35;if (done){ more_than_one++;recinfo2 = 35;} else {recinfo1 = 35;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiSpacePointsSeedMaker_LowMomentum"<<endmsg; patternRecoInfo = 36;if (done){ more_than_one++;recinfo2 = 36;} else {recinfo1 = 36;} done = 1;}
	    if ((*trksItr)->info().patternRecoInfo(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas)) {if (msgLvl(MSG::DEBUG)) msg() <<"SiSpacePointsSeedMaker_BeamGas"<<endmsg; patternRecoInfo = 37;if (done){ more_than_one++;recinfo2 = 37;} else {recinfo1 = 37;} done = 1;}
	    
	    m_patternRecoInfo->Fill(patternRecoInfo);
	    if (more_than_one>0) {
	      m_patternRecoInfo_npat->Fill(more_than_one+1);
	      m_patternRecoInfo_p1p2->Fill(recinfo1,recinfo2);
	      
	    }
	      
	      
	    
	    auto_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(**trksItr));
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator TSOSItr = trackWithHoles->trackStateOnSurfaces()->begin();
	    Identifier SurfaceID;
	    const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
	   
	    
	    if (startPerigee == NULL) {
	      if (msgLvl(MSG::VERBOSE)) msg() << "no start of perigee"<<endmsg;
	      continue;
	    }
	    //const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>( startPerigee );
	    if (!(startPerigee->covariance())) {
	      msg(MSG::WARNING) << "No covariance matrix assigned to the track" << endmsg;
	      continue;
	    }

	    AmgVector(5) perigeeParams = startPerigee->parameters();
	    
	    float trketa   =  - log(tan(perigeeParams[Trk::theta] / 2.0));
	    float trkphi   = perigeeParams[Trk::phi0];
	    
	    

	    trk_pix_coords[0].push_back(std::vector<int>());
	    trk_pix_coords[1].push_back(std::vector<int>());
	    trk_pix_coords[2].push_back(std::vector<int>());
	    trk_pix_coords[3].push_back(std::vector<int>());
	    
	    trk_sct_coords[0].push_back(std::vector<int>());
	    trk_sct_coords[1].push_back(std::vector<int>());
	    trk_sct_coords[2].push_back(std::vector<int>());
	    trk_sct_coords[3].push_back(std::vector<int>());
	    trk_sct_coords[4].push_back(std::vector<int>());
	    
	    trk_trt_coords[0].push_back(std::vector<int>());
	    trk_trt_coords[1].push_back(std::vector<int>());

	    for (; TSOSItr != trackWithHoles->trackStateOnSurfaces()->end(); ++TSOSItr) {


	      if (not *TSOSItr ) {
		msg(MSG::ERROR) << "pointer to TSOS is NULL: this should never happen" << (*TSOSItr) <<endmsg;
		continue;
	      }

	      const Trk::MeasurementBase* mesb=(*TSOSItr)->measurementOnTrack();
	      // hits, outliers
	      if (mesb && mesb->associatedSurface().associatedDetectorElement()) {
		SurfaceID = mesb->associatedSurface().associatedDetectorElement()->identify();
	      } 
	      else { // holes, perigee
		if (not (*TSOSItr)->trackParameters() ) {
		  msg(MSG::INFO) << "pointer of TSOS to track parameters or associated surface is null" << endmsg;
		  continue;
		}
		SurfaceID = (*TSOSItr)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
	      }

	      bool anyhit = (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement);

	      if (anyhit) {

		if (m_idHelper->is_pixel(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
		  trk_pix_coords[0].at(trk_pix_coords[0].size()-1).push_back(m_pixelID->layer_disk(SurfaceID));
		  trk_pix_coords[1].at(trk_pix_coords[1].size()-1).push_back(m_pixelID->barrel_ec(SurfaceID));
		  trk_pix_coords[2].at(trk_pix_coords[2].size()-1).push_back(m_pixelID->eta_module(SurfaceID));
		  trk_pix_coords[3].at(trk_pix_coords[3].size()-1).push_back(m_pixelID->phi_module(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<" Track(Pixel) Hit : Layer = " << m_pixelID->layer_disk(SurfaceID) << "  B/EC " << m_pixelID->barrel_ec(SurfaceID) << "  " << m_pixelID->eta_module(SurfaceID) << "  " << m_pixelID->phi_module(SurfaceID) <<"\tPhysical eta = "<<trketa<<" and phi "<<trkphi<<endmsg;

		}
		
		if (m_idHelper->is_sct(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
		  trk_sct_coords[0].at(trk_sct_coords[0].size()-1).push_back(m_sctID->layer_disk(SurfaceID));
		  trk_sct_coords[1].at(trk_sct_coords[1].size()-1).push_back(m_sctID->barrel_ec(SurfaceID));
		  trk_sct_coords[2].at(trk_sct_coords[2].size()-1).push_back(m_sctID->eta_module(SurfaceID));
		  trk_sct_coords[3].at(trk_sct_coords[3].size()-1).push_back(m_sctID->phi_module(SurfaceID));
		  trk_sct_coords[4].at(trk_sct_coords[4].size()-1).push_back(m_sctID->side(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<" Track(SCT) Hit : Layer = " << m_sctID->layer_disk(SurfaceID) << "  B/EC " << m_sctID->barrel_ec(SurfaceID) << "  " << m_sctID->eta_module(SurfaceID) << "  " << m_sctID->phi_module(SurfaceID) <<"\tPhysical eta = "<<trketa<<" and phi "<<trkphi<<endmsg;

		}
		
		if (m_idHelper->is_trt(SurfaceID) && (*TSOSItr)->type(Trk::TrackStateOnSurface::Measurement)) {
		  trk_trt_coords[0].at(trk_trt_coords[0].size()-1).push_back(m_trtID->barrel_ec(SurfaceID));
		  trk_trt_coords[1].at(trk_trt_coords[1].size()-1).push_back(m_trtID->phi_module(SurfaceID));
		  if (msgLvl(MSG::DEBUG)) msg() <<"HitBasedPlots "<<" Track(TRT) Hit : B/EC " << m_trtID->barrel_ec(SurfaceID) << "  " << m_trtID->phi_module(SurfaceID) <<"Physical phi "<<trkphi<<endmsg;

		}
	      }
	    }//end of loop over pixel hits 
	  }//end of loop over tracks

	  
	  //fill ntracks
	  //m_nTrks->Fill(trk_pix_coords[0].size());
	  m_nTrks->Fill(ntrks);
	  m_nPixelTrks->Fill(pix_coords[0].size());
	  m_nSCTTrks->Fill(sct_coords[0].size());
	  m_nTRTTrks->Fill(trt_coords[1].size());
	  
	  bool match = 0; 
	  bool done = 0;
	  size_t nmatch =0;
	  for (int i=pix_coords[0].size()-1;i>=0;i--){//loop over pixel tracklets
	    //if (pix_coords[0].at(i).size()<3) continue;//each pixel tracklet must have three hits
	    done = 0;
	for (int j=trk_pix_coords[0].size()-1;j>=0 && !done;j--){//loop over tracks
	      match=1;
	      nmatch = 0;
	      for (unsigned int k=0;k<pix_coords[0].at(i).size() && match;k++){//loop over hits on pixel tracklets
		for (unsigned int l=0;l<trk_pix_coords[0].at(j).size() && match;l++){//loop over pixel hits on tracks
		 
		  if(pix_coords[0].at(i).at(k) == trk_pix_coords[0].at(j).at(l)
		      &&pix_coords[1].at(i).at(k) == trk_pix_coords[1].at(j).at(l)
		      &&pix_coords[2].at(i).at(k) == trk_pix_coords[2].at(j).at(l)
		      &&pix_coords[3].at(i).at(k) == trk_pix_coords[3].at(j).at(l)) nmatch++;//a track hit and pixel hit are shared
		 
		  
		}
	      }

	      if (nmatch == pix_coords[0].at(i).size()){ // any cand. in tracks shares all hits with this cand. of pixels
		m_Pix_Track_Etaeff->Fill(pix_trk_eta.at(i),1);
		m_Pix_Track_Phieff->Fill(pix_trk_phi.at(i),1);
		m_Pix_Track_Pteff->Fill(pix_trk_pt.at(i),1);
		if (pix_trk_pt.at(i)<10) m_Pix_Track_Ptlow_eff->Fill(pix_trk_pt.at(i),1);
		
		done = 1;//this pixel tracklet has been matched, don't try to match it to any others
	      }
	      }
	    if (!done){//zero cand. in tracks shares all hits with this cand. of pixel tracks
	      m_Pix_Track_Etaeff->Fill(pix_trk_eta.at(i),0);
	      m_Pix_Track_Phieff->Fill(pix_trk_phi.at(i),0);
	      m_Pix_Track_Pteff->Fill(pix_trk_pt.at(i),0);
	      if (pix_trk_pt.at(i)<10) m_Pix_Track_Ptlow_eff->Fill(pix_trk_pt.at(i),0);
	    }

	  }
	 
	  
	  for (int i=sct_coords[0].size()-1;i>=0;i--){//loop over sct tracklets
	    done = 0;
	  for (int j=trk_sct_coords[0].size()-1;j>=0 && !done;j--){//loop over tracks
	      match=1;
	      nmatch = 0;
	      for (unsigned int k=0;k<sct_coords[0].at(i).size() && match;k++){//loop over hits on sct tracklets
		for (unsigned int l=0;l<trk_sct_coords[0].at(j).size() && match;l++){//loop over sct hits on tracks
		 
		  if(sct_coords[0].at(i).at(k) == trk_sct_coords[0].at(j).at(l)
		      &&sct_coords[1].at(i).at(k) == trk_sct_coords[1].at(j).at(l)
		      &&sct_coords[2].at(i).at(k) == trk_sct_coords[2].at(j).at(l)
		      &&sct_coords[3].at(i).at(k) == trk_sct_coords[3].at(j).at(l)
		      &&sct_coords[4].at(i).at(k) == trk_sct_coords[4].at(j).at(l)
		      ) nmatch++;//a track hit and sct hit are shared
		 
		  
		}
	      }
	      if (nmatch == sct_coords[0].at(i).size()){ // any cand. in tracks shares all hits with this cand. of scts
		m_SCT_Track_Etaeff->Fill(sct_trk_eta.at(i),1);
		m_SCT_Track_Phieff->Fill(sct_trk_phi.at(i),1);
		m_SCT_Track_Pteff->Fill(sct_trk_pt.at(i),1);
		if (sct_trk_pt.at(i)<10) m_SCT_Track_Ptlow_eff->Fill(sct_trk_pt.at(i),1);
		done = 1;//this sct tracklet has been matched, don't try to match it to any others
	      }
	      }
	    if (!done){//zero cand. in tracks shares all hits with this cand. of sct tracks
	      m_SCT_Track_Etaeff->Fill(sct_trk_eta.at(i),0);
	      m_SCT_Track_Phieff->Fill(sct_trk_phi.at(i),0);
	      m_SCT_Track_Pteff->Fill(sct_trk_pt.at(i),0);
	      if (sct_trk_pt.at(i)<10) m_SCT_Track_Ptlow_eff->Fill(sct_trk_pt.at(i),0);
	    }
	  }
	  
	  for (int i=trt_coords[0].size()-1;i>=0;i--){//loop over trt tracklets
	    done = 0;
	  for (int j=trk_trt_coords[0].size()-1;j>=0 && !done;j--){//loop over tracks
	      match=1;
	      nmatch = 0;
	      for (unsigned int k=0;k<trt_coords[0].at(i).size() && match;k++){//loop over hits on sct tracklets
		for (unsigned int l=0;l<trk_trt_coords[0].at(j).size() && match;l++){//loop over sct hits on tracks
		 
		  if(trt_coords[0].at(i).at(k) == trk_trt_coords[0].at(j).at(l)
		     &&trt_coords[1].at(i).at(k) == trk_trt_coords[1].at(j).at(l)
		      ) nmatch++;//a track hit and sct hit are shared
		 
		  
		}
	      }
	      //what eta, phi measurement to fill plot with? 
	      if (float(nmatch) >trt_hit_threshold*float(trt_coords[0].at(i).size())){ // any cand. in tracks shares all hits with this cand. of trts
		//m_TRT_Track_Etaeff->Fill(trk_eta.at(i),1);
		m_TRT_Track_Phieff->Fill(trt_trk_phi.at(i),1);
		m_TRT_Track_Pteff->Fill(trt_trk_pt.at(i),1);
		if (trt_trk_pt.at(i)<10) m_TRT_Track_Ptlow_eff->Fill(trt_trk_pt.at(i),1);
		
		trk_trt_coords[0].at(j).clear();
		trk_trt_coords[1].at(j).clear();
		trk_trt_coords[0].erase(trk_trt_coords[0].begin()+j);
		trk_trt_coords[1].erase(trk_trt_coords[1].begin()+j);
		done = 1;//this trt tracklet has been matched, don't try to match it to any others
		//if (msgLvl(MSG::DEBUG)) msg() <<"YESSSSS trt\t"<<trt_trk_pt.at(i)<<"\t"<<trt_trk_phi.at(i)<<endmsg;
	      }
	      }
	    if (!done){//zero cand. in tracks shares all hits with this cand. of trt tracks
	      m_TRT_Track_Phieff->Fill(trt_trk_phi.at(i),0);
	      m_TRT_Track_Pteff->Fill(trt_trk_pt.at(i),0);
	      if (trt_trk_pt.at(i)<10) m_TRT_Track_Ptlow_eff->Fill(trt_trk_pt.at(i),0);
	    }
	  }

	  return;
      }

void IDStandardPerformance::FillTrackIncidencePlots(detAreaTypes detArea, int phiWidth, int zWidth, float PixTrkAngle, float PixTrkThetaI)
{
  if (PixTrkThetaI == -1000. || PixTrkAngle == -1000.) return;
  if (phiWidth <= 0 || zWidth <= 0)
    {
      return;
    }
  if (detArea < 0 || detArea >= n_detAreaTypes) return;
  if (phiWidth > 4) phiWidth = 4;
  if (zWidth > 4) zWidth = 4;
  m_cluster_angle_phi[detArea][phiWidth-1]->Fill(PixTrkAngle);
  m_cluster_angle_eta[detArea][zWidth-1]->Fill(PixTrkThetaI);
  return;
}

const Trk::TrackParameters*
IDStandardPerformance::getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters
                                                , const Trk::MeasurementBase* measurement ) {
  const Trk::TrackParameters *unbiasedTrkParameters = 0;

  // ------------------------------------
  // try to get measured track parameters
  // ------------------------------------

  //const Trk::MeasuredTrackParameters *measuredTrkParameters =
  //dynamic_cast<const Trk::MeasuredTrackParameters*>(trkParameters);

  if (!m_updatorHandle.empty() && (m_isUnbiased==1) ) {
    if ( trkParameters->covariance() ) {
      // Get unbiased state
      unbiasedTrkParameters =
    m_updatorHandle->removeFromState( *trkParameters,
                    measurement->localParameters(),
                    measurement->localCovariance());

      if (!unbiasedTrkParameters) {
    msg(MSG::WARNING) << "Could not get unbiased track parameters, "
        <<"use normal parameters" << endmsg;
    m_isUnbiased = 0;
      }
    } else if (!m_UpdatorWarning) {
      // warn only once!
      msg(MSG::WARNING) << "TrackParameters contain no covariance: "
      <<"Unbiased track states can not be calculated "
      <<"(ie. pulls and residuals will be too small)" << endmsg;
      m_UpdatorWarning = true;
      m_isUnbiased = 0;
    } else {
      m_isUnbiased = 0;
    }// end if no measured track parameter
  }
  return unbiasedTrkParameters;
}

// Set min and max of a histogram, but increase them if an
// entry of the histograms is below or above the min/max

void IDStandardPerformance::SetSafeMinimumMaximum(TH1* h, float min, float max) {
  float hmin = 0.9 * h->GetMinimum();
  float hmax = 1.1 * h->GetMaximum();

  if (hmin < min)
    h->SetMinimum(hmin);
  else
    h->SetMinimum(min);

  if (hmax > max)
    h->SetMaximum(hmax);
  else
    h->SetMaximum(max);
  return;
}


void
IDStandardPerformance::fillPixelTrackPullHistos(const Identifier& elementID
                                              , const Trk::TrackStateOnSurface* trackState
                                              , const auto_ptr<const Trk::ResidualPull>& trackPull)
{
  const InDetDD::SiDetectorElement* element = m_PIX_Mgr->getDetectorElement(elementID);
  if (not element) {
    return;
  }
  if (not m_idHelper->is_pixel(elementID)) {
    msg(MSG::FATAL) << "This is not a pixel" << endmsg;
  }
  bool isBarrel = m_pixelID->is_barrel(elementID);
  int layer = m_pixelID->layer_disk(elementID);
  const Amg::Vector3D trackMomentum = trackState->trackParameters()->momentum();
  double globalEta = trackMomentum.eta();
  // the incident angle should be computed in the x-y plane only.
  Amg::Vector3D xyMomentum(trackMomentum);
  xyMomentum[Trk::locZ] = 0;
  Amg::Vector3D xyNormal(element->normal());
  xyNormal[Trk::locZ] = 0;
  // double incidentAngle = element->normal().angle(trackState->trackParameters()->momentum());
  //double incidentAngle = xyNormal.angle(xyMomentum);
  double incidentAngle = Amg::angle(xyNormal,xyMomentum);
  // CLHEP::Hep3Vector stripPlaneComponent = trackMomentum - element->normal()*trackMomentum.dot(element->normal());

  // we decompose the track momentum into the three local coordinates of the pixels
  double trackNormalComponent = trackMomentum.dot(element->normal());
  double trackEtaComponent = trackMomentum.dot(element->etaAxis());
  double trackPhiComponent = trackMomentum.dot(element->phiAxis());
  // the SCTHittEffMonTool claims that eta and phi are now the following:
  double eta = atan2(trackEtaComponent, trackNormalComponent);
  double phi = atan2(trackPhiComponent, trackNormalComponent);
  if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "PIXEL: element x y z "
            << element->normal().x()
            << " " << element->normal().y()
            << " " << element->normal().z()
            << endmsg;
  }
  if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "PIXEL: track x y z "
            << trackState->trackParameters()->momentum().x()
            << " " << trackState->trackParameters()->momentum().y()
            << " " << trackState->trackParameters()->momentum().z()
            << endmsg;
  }
  if (msgLvl(MSG::VERBOSE)) {
    Amg::Vector3D momVect =  trackState->trackParameters()->momentum();
    Amg::Vector3D normVect = element->normal();
      msg(MSG::VERBOSE) << "PIXEL: track incident angle "
			<<  Amg::angle(momVect,normVect)
            << endmsg;
  }
  double pullPhi = trackPull->pull()[Trk::locRPhi];
  double pullZ = trackPull->pull()[Trk::locZ];
  double residualPhi = trackPull->residual()[Trk::locRPhi];
  double residualZ = trackPull->residual()[Trk::locZ];
  m_pullPhi_vs_incident_pixel->Fill(incidentAngle, pullPhi);
  m_residualPhi_vs_incident_pixel->Fill(incidentAngle, residualPhi);
  if (isBarrel) {
    m_pullPhi_vs_phi_pixelBarrel->Fill(phi, pullPhi);
    m_pullPhi_vs_incident_pixel_barrel_l[layer]->Fill(incidentAngle, pullPhi);
    m_residualPhi_vs_incident_pixel_barrel_l[layer]->Fill(incidentAngle, residualPhi);
    m_pullZ_vs_incident_pixel_barrel_l[layer]->Fill(incidentAngle, pullZ);
    m_residualZ_vs_incident_pixel_barrel_l[layer]->Fill(incidentAngle, residualZ);

    m_pullPhi_vs_eta_pixel_barrel_l[layer]->Fill(eta, pullPhi);
    m_residualPhi_vs_eta_pixel_barrel_l[layer]->Fill(eta, residualPhi);
    m_pullZ_vs_eta_pixel_barrel_l[layer]->Fill(eta, pullZ);
    m_residualZ_vs_eta_pixel_barrel_l[layer]->Fill(eta, residualZ);

    m_pullPhi_vs_phi_pixel_barrel_l[layer]->Fill(phi, pullPhi);
    m_residualPhi_vs_phi_pixel_barrel_l[layer]->Fill(phi, residualPhi);
    m_pullZ_vs_phi_pixel_barrel_l[layer]->Fill(phi, pullZ);
    m_residualZ_vs_phi_pixel_barrel_l[layer]->Fill(phi, residualZ);
    // bins are 0,0.5,1.0,1.5,2.0,2.5 -- eta * 10 / 5
    int etaBin = static_cast<int>(floor(fabs(globalEta)*2));
    if (etaBin < 5) {
        m_pullPhi_vs_phi_pixel_barrel_etaBins[etaBin][layer]->Fill(phi, pullPhi);
        m_residualPhi_vs_phi_pixel_barrel_etaBins[etaBin][layer]->Fill(phi, residualPhi);
    }
  } else {
    m_pullPhi_vs_phi_pixelEndcap->Fill(phi, pullPhi);
    m_pullPhi_vs_incident_pixel_endcap_d[layer]->Fill(incidentAngle, pullPhi);
    m_residualPhi_vs_incident_pixel_endcap_d[layer]->Fill(incidentAngle, residualPhi);
    m_pullZ_vs_incident_pixel_endcap_d[layer]->Fill(incidentAngle, pullZ);
    m_residualZ_vs_incident_pixel_endcap_d[layer]->Fill(incidentAngle, residualZ);

    m_pullPhi_vs_eta_pixel_endcap_d[layer]->Fill(eta, pullPhi);
    m_residualPhi_vs_eta_pixel_endcap_d[layer]->Fill(eta, residualPhi);
    m_pullZ_vs_eta_pixel_endcap_d[layer]->Fill(eta, pullZ);
    m_residualZ_vs_eta_pixel_endcap_d[layer]->Fill(eta, residualZ);

    m_pullPhi_vs_phi_pixel_endcap_d[layer]->Fill(phi, pullPhi);
    m_residualPhi_vs_phi_pixel_endcap_d[layer]->Fill(phi, residualPhi);
    m_pullZ_vs_phi_pixel_endcap_d[layer]->Fill(phi, pullZ);
    m_residualZ_vs_phi_pixel_endcap_d[layer]->Fill(phi, residualZ);
  }
}
