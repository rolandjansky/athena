/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigBjetHypo/TrigBjetFex.h"
#include "TrigBjetHypo/TrigBjetTagger.h"
#include "TrigBjetHypo/TuningLikelihood.h"

#include "TaggerHelper.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "VxVertex/VxContainer.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"

#include "TrigSteeringEvent/TrigOperationalInfo.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetEvent/TrigVertex.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFex::TrigBjetFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_trackJetFinderTool("TrigTrackJetFinderTool",this),      	
  m_trigL2BjetColl(0),
  m_trigEFBjetColl(0),
  m_trigBjetTagger(0),
  m_constTrigBjetTagger(0),
  m_trigBjetPrmVtxInfo(0),
  m_trigBjetSecVtxInfo(0),
  m_trigBjetJetInfo(0),
  m_totTracks(0),
  m_totSelTracks(0),
  m_tuningLikelihoodIP1D(0),
  m_tuningLikelihoodIP2D(0),
  m_tuningLikelihoodIP3D(0),
  m_tuningLikelihoodIP1D_lowSiHits(0),
  m_tuningLikelihoodIP2D_lowSiHits(0),
  m_tuningLikelihoodIP3D_lowSiHits(0),
  m_tuningLikelihoodMVtx(0),
  m_tuningLikelihoodEVtx(0),
  m_tuningLikelihoodNVtx(0),
  m_tuningLikelihoodSV(0)
{
  declareProperty ("AlgoId",             m_algo);
  declareProperty ("Instance",           m_instance);
  declareProperty ("Taggers",            m_taggers);

  declareProperty ("par_0_MC",           m_par_0_MC);
  declareProperty ("par_1_MC",           m_par_1_MC);
  declareProperty ("par_0_DT",           m_par_0_DT);
  declareProperty ("par_1_DT",           m_par_1_DT);

  declareProperty ("SizeIP1D",           m_sizeIP1D);
  declareProperty ("bIP1D",              m_bIP1D);
  declareProperty ("uIP1D",              m_uIP1D);
  declareProperty ("SizeIP2D",           m_sizeIP2D);
  declareProperty ("bIP2D",              m_bIP2D);
  declareProperty ("uIP2D",              m_uIP2D);
  declareProperty ("SizeIP3D",           m_sizeIP3D);
  declareProperty ("bIP3D",              m_bIP3D);
  declareProperty ("uIP3D",              m_uIP3D);

  declareProperty ("useLowSiHits",                 m_useLowSiHits = false);
  declareProperty ("SizeIP1D_lowSiHits",           m_sizeIP1D_lowSiHits);
  declareProperty ("bIP1D_lowSiHits",              m_bIP1D_lowSiHits);
  declareProperty ("uIP1D_lowSiHits",              m_uIP1D_lowSiHits);
  declareProperty ("SizeIP2D_lowSiHits",           m_sizeIP2D_lowSiHits);
  declareProperty ("bIP2D_lowSiHits",              m_bIP2D_lowSiHits);
  declareProperty ("uIP2D_lowSiHits",              m_uIP2D_lowSiHits);
  declareProperty ("SizeIP3D_lowSiHits",           m_sizeIP3D_lowSiHits);
  declareProperty ("bIP3D_lowSiHits",              m_bIP3D_lowSiHits);
  declareProperty ("uIP3D_lowSiHits",              m_uIP3D_lowSiHits);

  declareProperty ("SizeMVtx",           m_sizeMVtx);
  declareProperty ("bMVtx",              m_bMVtx);
  declareProperty ("uMVtx",              m_uMVtx);
  declareProperty ("SizeEVtx",           m_sizeEVtx);
  declareProperty ("bEVtx",              m_bEVtx);
  declareProperty ("uEVtx",              m_uEVtx);
  declareProperty ("SizeNVtx",           m_sizeNVtx);
  declareProperty ("bNVtx",              m_bNVtx);
  declareProperty ("uNVtx",              m_uNVtx);
  declareProperty ("SizeSV",             m_sizeSV);
  declareProperty ("bSV",                m_bSV);
  declareProperty ("uSV",                m_uSV);

  declareProperty ("UseBeamSpotFlag",    m_useBeamSpotFlag    = false);
  declareProperty ("SetBeamSpotWidth",   m_setBeamSpotWidth   = 0.05);

  declareProperty ("UseParamFromData",   m_useParamFromData   = false);

  declareProperty ("UseErrIPParam",      m_useErrIPParam      = false);
  declareProperty ("L2PrmVtxAtEF",       m_l2PrmVtxAtEF       = true);
  declareProperty ("UseEtaPhiTrackSel",  m_useEtaPhiTrackSel  = false);

  declareProperty ("UseJetDirection",    m_useJetDirection);
  declareProperty ("RetrieveHLTJets",    m_retrieveHLTJets    = true);
  declareProperty ("TagHLTJets",         m_tagHLTJets         = 0);

  declareProperty ("TrkSel_Chi2",        m_trkSelChi2         = 0.001);
  declareProperty ("TrkSel_BLayer",      m_trkSelBLayer       = 1);
  declareProperty ("TrkSel_PixHits",     m_trkSelPixHits      = 2);
  declareProperty ("TrkSel_SiHits",      m_trkSelSiHits       = 4);
  declareProperty ("TrkSel_D0",          m_trkSelD0           = 1*CLHEP::mm);
  declareProperty ("TrkSel_Z0",          m_trkSelZ0           = 2*CLHEP::mm);
  declareProperty ("TrkSel_Pt",          m_trkSelPt           = 1*CLHEP::GeV);

  declareMonitoredStdContainer("trk_a0",            m_mon_trk_a0,        AutoClear);
  declareMonitoredStdContainer("trk_a0_sel",        m_mon_trk_a0_sel,    AutoClear);
  declareMonitoredStdContainer("trk_S(a0)_sel",     m_mon_trk_Sa0_sel,   AutoClear);
  declareMonitoredStdContainer("trk_z0",            m_mon_trk_z0,        AutoClear);
  declareMonitoredStdContainer("trk_z0_sel",        m_mon_trk_z0_sel,    AutoClear);
  declareMonitoredStdContainer("trk_z0_sel_PV",     m_mon_trk_z0_sel_PV, AutoClear);
  declareMonitoredStdContainer("trk_S(z0)_sel",     m_mon_trk_Sz0_sel,   AutoClear);
  declareMonitoredStdContainer("trk_prob",          m_mon_trk_prob,      AutoClear);

  declareMonitoredVariable    ("roi_nTracks",       m_totTracks);
  declareMonitoredVariable    ("roi_nTracks_sel",   m_totSelTracks);
  declareMonitoredStdContainer("roi_stepsToSelect", m_listCutApplied, AutoClear);
  declareMonitoredObject      ("roi_selectedTracks", *this, &TrigBjetFex::totSelectedTracks);

  declareMonitoredVariable    ("roi_deltaEtaJet",       m_deltaEtaJet,       AutoClear);
  declareMonitoredVariable    ("roi_deltaPhiJet",       m_deltaPhiJet,       AutoClear);
  declareMonitoredVariable    ("roi_deltaEtaTrkJet",    m_deltaEtaTrkJet,    AutoClear);
  declareMonitoredVariable    ("roi_deltaPhiTrkJet",    m_deltaPhiTrkJet,    AutoClear);
  declareMonitoredVariable    ("roi_deltaEtaJetTrkJet", m_deltaEtaJetTrkJet, AutoClear);
  declareMonitoredVariable    ("roi_deltaPhiJetTrkJet", m_deltaPhiJetTrkJet, AutoClear);

  declareMonitoredObject("X(IP1D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP1D);
  declareMonitoredObject("X(IP2D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP2D);
  declareMonitoredObject("X(IP3D)", m_constTrigBjetTagger, &TrigBjetTagger::getXIP3D);
  declareMonitoredObject("X(SVTX)", m_constTrigBjetTagger, &TrigBjetTagger::getXSVTX);
  declareMonitoredObject("X(COMB)", m_constTrigBjetTagger, &TrigBjetTagger::getXCOMB);
  declareMonitoredObject("X(CHI2)", m_constTrigBjetTagger, &TrigBjetTagger::getXCHI2);

  m_taggerHelper = new TaggerHelper(msg(), msgLvl());
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFex::~TrigBjetFex() {

  if (m_taggerHelper)            delete m_taggerHelper;
  if (m_trigBjetTagger)          delete m_trigBjetTagger;
  if (m_trigBjetPrmVtxInfo)      delete m_trigBjetPrmVtxInfo;
  if (m_trigBjetSecVtxInfo)      delete m_trigBjetSecVtxInfo;
  if (m_trigBjetJetInfo)         delete m_trigBjetJetInfo;
  if (m_tuningLikelihoodIP1D)    delete m_tuningLikelihoodIP1D;
  if (m_tuningLikelihoodIP2D)    delete m_tuningLikelihoodIP2D;
  if (m_tuningLikelihoodIP3D)    delete m_tuningLikelihoodIP3D;
  if (m_tuningLikelihoodIP1D_lowSiHits)    delete m_tuningLikelihoodIP1D_lowSiHits;
  if (m_tuningLikelihoodIP2D_lowSiHits)    delete m_tuningLikelihoodIP2D_lowSiHits;
  if (m_tuningLikelihoodIP3D_lowSiHits)    delete m_tuningLikelihoodIP3D_lowSiHits;
  if (m_tuningLikelihoodMVtx)    delete m_tuningLikelihoodMVtx;
  if (m_tuningLikelihoodEVtx)    delete m_tuningLikelihoodEVtx;
  if (m_tuningLikelihoodNVtx)    delete m_tuningLikelihoodNVtx;
  if (m_tuningLikelihoodSV)      delete m_tuningLikelihoodSV;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltInitialize() {
  
  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBjetFex, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endreq; 
    msg() << MSG::DEBUG << " Instance = "            << m_instance << endreq;
 
    msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endreq; 
    msg() << MSG::DEBUG << " SetBeamSpotWidth = "    << m_setBeamSpotWidth << endreq;

    msg() << MSG::DEBUG << " UseParamFromData = "    << m_useParamFromData << endreq; 

    msg() << MSG::DEBUG << " Taggers = "             << m_taggers << endreq; 
    msg() << MSG::DEBUG << " UseErrIPParam = "       << m_useErrIPParam << endreq; 
    msg() << MSG::DEBUG << " UseJetDirection = "     << m_useJetDirection << endreq; 
    msg() << MSG::DEBUG << " RetrieveHLTJets = "     << m_retrieveHLTJets << endreq; 
    msg() << MSG::DEBUG << " TagHLTJets = "          << m_tagHLTJets << endreq;
    msg() << MSG::DEBUG << " L2PrmVtxAtEF = "        << m_l2PrmVtxAtEF << endreq;
    msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel << endreq;

    msg() << MSG::DEBUG << " JetProb 0 MC = "      << m_par_0_MC << endreq; 
    msg() << MSG::DEBUG << " JetProb 1 MC = "      << m_par_1_MC << endreq; 
    msg() << MSG::DEBUG << " JetProb 0 DT = "      << m_par_0_DT << endreq; 
    msg() << MSG::DEBUG << " JetProb 1 DT = "      << m_par_1_DT << endreq; 

    msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endreq; 
    msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endreq; 
    msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endreq; 
    msg() << MSG::DEBUG << " SizeIP2D = "          << m_sizeIP2D << endreq;
    msg() << MSG::DEBUG << " bIP2D = "             << m_bIP2D << endreq; 
    msg() << MSG::DEBUG << " uIP2D = "             << m_uIP2D << endreq;  
    msg() << MSG::DEBUG << " SizeIP3D = "          << m_sizeIP3D << endreq; 
    msg() << MSG::DEBUG << " bIP3D = "             << m_bIP3D << endreq; 
    msg() << MSG::DEBUG << " uIP3D = "             << m_uIP3D << endreq; 

    msg() << MSG::DEBUG << " SizeIP1D_lowSiHits = "          << m_sizeIP1D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " bIP1D_lowSiHits = "             << m_bIP1D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " uIP1D_lowSiHits = "             << m_uIP1D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " SizeIP2D_lowSiHits = "          << m_sizeIP2D_lowSiHits << endreq;
    msg() << MSG::DEBUG << " bIP2D_lowSiHits = "             << m_bIP2D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " uIP2D_lowSiHits = "             << m_uIP2D_lowSiHits << endreq;  
    msg() << MSG::DEBUG << " SizeIP3D_lowSiHits = "          << m_sizeIP3D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " bIP3D_lowSiHits = "             << m_bIP3D_lowSiHits << endreq; 
    msg() << MSG::DEBUG << " uIP3D_lowSiHits = "             << m_uIP3D_lowSiHits << endreq; 

    msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endreq; 
    msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endreq; 
    msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endreq; 

    msg() << MSG::DEBUG << " TrkSel_Chi2 = "     << m_trkSelChi2 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_BLayer = "   << m_trkSelBLayer << endreq; 
    msg() << MSG::DEBUG << " TrkSel_SiHits = "   << m_trkSelSiHits << endreq; 
    msg() << MSG::DEBUG << " TrkSel_D0 = "       << m_trkSelD0 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_Z0 = "       << m_trkSelZ0 << endreq; 
    msg() << MSG::DEBUG << " TrkSel_Pt = "       << m_trkSelPt << endreq; 

    msg() << MSG::DEBUG << " SizeMVtx = "       << m_sizeMVtx << endreq; 
    msg() << MSG::DEBUG << " bMVtx = "          << m_bMVtx << endreq; 
    msg() << MSG::DEBUG << " uMVtx = "          << m_uMVtx << endreq;
    msg() << MSG::DEBUG << " SizeEVtx = "       << m_sizeEVtx << endreq; 
    msg() << MSG::DEBUG << " bEVtx = "          << m_bEVtx << endreq; 
    msg() << MSG::DEBUG << " uEVtx = "          << m_uEVtx << endreq;  
    msg() << MSG::DEBUG << " SizeNVtx = "       << m_sizeNVtx << endreq; 
    msg() << MSG::DEBUG << " bNVtx = "          << m_bNVtx << endreq; 
    msg() << MSG::DEBUG << " uNVtx = "          << m_uNVtx << endreq;
    msg() << MSG::DEBUG << " SizeSV = "         << m_sizeSV << endreq; 
    msg() << MSG::DEBUG << " bSV = "            << m_bSV << endreq; 
    msg() << MSG::DEBUG << " uSV = "            << m_uSV << endreq;  
  }

  m_trigBjetTagger = new TrigBjetTagger(this, msg(), msgLvl());
  m_constTrigBjetTagger  = const_cast<const TrigBjetTagger*>(m_trigBjetTagger);

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Retrieving tuning likelihoods." << endreq;

  m_tuningLikelihoodIP1D = new TuningLikelihood(&m_sizeIP1D[0], &m_bIP1D[0], &m_uIP1D[0], m_sizeIP1D.size());
  m_trigBjetTagger->fillLikelihoodMap("IP1D", m_tuningLikelihoodIP1D);
    
  m_tuningLikelihoodIP2D = new TuningLikelihood(&m_sizeIP2D[0], &m_bIP2D[0], &m_uIP2D[0], m_sizeIP2D.size());   
  m_trigBjetTagger->fillLikelihoodMap("IP2D", m_tuningLikelihoodIP2D);

  m_tuningLikelihoodIP3D = new TuningLikelihood(&m_sizeIP3D[0], &m_bIP3D[0], &m_uIP3D[0], m_sizeIP3D.size()); 
  m_trigBjetTagger->fillLikelihoodMap("IP3D", m_tuningLikelihoodIP3D);

  if (m_useLowSiHits) {
    m_tuningLikelihoodIP1D_lowSiHits = new TuningLikelihood(&m_sizeIP1D_lowSiHits[0], &m_bIP1D_lowSiHits[0], &m_uIP1D_lowSiHits[0], m_sizeIP1D_lowSiHits.size());
    m_trigBjetTagger->fillLikelihoodMap("IP1D_lowSiHits", m_tuningLikelihoodIP1D_lowSiHits);

    m_tuningLikelihoodIP2D_lowSiHits = new TuningLikelihood(&m_sizeIP2D_lowSiHits[0], &m_bIP2D_lowSiHits[0], &m_uIP2D_lowSiHits[0], m_sizeIP2D_lowSiHits.size());
    m_trigBjetTagger->fillLikelihoodMap("IP2D_lowSiHits", m_tuningLikelihoodIP2D_lowSiHits);

    m_tuningLikelihoodIP3D_lowSiHits = new TuningLikelihood(&m_sizeIP3D_lowSiHits[0], &m_bIP3D_lowSiHits[0], &m_uIP3D_lowSiHits[0], m_sizeIP3D_lowSiHits.size());
    m_trigBjetTagger->fillLikelihoodMap("IP3D_lowSiHits", m_tuningLikelihoodIP3D_lowSiHits);
  }

  m_tuningLikelihoodMVtx = new TuningLikelihood(&m_sizeMVtx[0], &m_bMVtx[0], &m_uMVtx[0], m_sizeMVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("MVTX", m_tuningLikelihoodMVtx);
    
  m_tuningLikelihoodEVtx = new TuningLikelihood(&m_sizeEVtx[0], &m_bEVtx[0], &m_uEVtx[0], m_sizeEVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("EVTX", m_tuningLikelihoodEVtx);

  m_tuningLikelihoodNVtx = new TuningLikelihood(&m_sizeNVtx[0], &m_bNVtx[0], &m_uNVtx[0], m_sizeNVtx.size()); 
  m_trigBjetTagger->fillLikelihoodMap("NVTX", m_tuningLikelihoodNVtx);

  m_tuningLikelihoodSV = new TuningLikelihood(&m_sizeSV[0], &m_bSV[0], &m_uSV[0], m_sizeSV.size()); 
  m_trigBjetTagger->fillLikelihoodMap("SVTX", m_tuningLikelihoodSV);

  //* Retrieve TrigTrackJetFinder tool *//
  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    msg() << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endreq;
    return HLT::BAD_JOB_SETUP;
  } else
    msg() << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endreq;
  
  m_trigBjetPrmVtxInfo = new TrigBjetPrmVtxInfo();
  m_trigBjetSecVtxInfo = new TrigBjetSecVtxInfo();
  m_trigBjetJetInfo    = new TrigBjetJetInfo();

  return HLT::OK;
}



//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getCollection(const TrigInDetTrackCollection*& pointerToL2TrackCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const TrigInDetTrackCollection*> vectorOfL2TrackCollections;

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfL2TrackCollections, ""); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get InDetTrackCollection from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfL2TrackCollections.size() << " InDetTrackCollection" << endreq;

  std::vector<const TrigInDetTrackCollection*>::iterator pTrackColl    = vectorOfL2TrackCollections.begin();
  std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfL2TrackCollections.end();

  for ( ; pTrackColl != lastTrackColl; pTrackColl++) { 
    if ((*pTrackColl)->size() != 0) {
      if ((*pTrackColl)->front()->algorithmId() == m_algo) {
	if (msgLvl() <= MSG::DEBUG && m_algo == 1) 
	  msg() << MSG::DEBUG << "Selected collection with SiTrack label" << endreq;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 2) 
	  msg() << MSG::DEBUG << "Selected collection with IdScan label" << endreq;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 5) 
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy A)" << endreq;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 6) 
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy B)" << endreq;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 8) 
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy F)" << endreq;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 13) 
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy F with refit)" << endreq;
	break;
      }
    }
  } 
  
  if (pTrackColl == lastTrackColl) {
    pointerToL2TrackCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToL2TrackCollections = *pTrackColl;
    return HLT::OK;
  }    
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getCollection(const Rec::TrackParticleContainer*& pointerToEFTrackCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const Rec::TrackParticleContainer*> vectorOfEFTrackCollections;

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFTrackCollections, ""); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFTrackCollections.size() << " TrackParticleContainer" << endreq;
  
  std::vector<const Rec::TrackParticleContainer*>::iterator pTrackColl    = vectorOfEFTrackCollections.begin();
  std::vector<const Rec::TrackParticleContainer*>::iterator lastTrackColl = vectorOfEFTrackCollections.end();
  
  if (pTrackColl == lastTrackColl) {
    pointerToEFTrackCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToEFTrackCollections = *pTrackColl;
    return HLT::OK;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getPrmVtxCollection(const TrigVertexCollection*& pointerToL2PrmVtxCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const TrigVertexCollection*> vectorOfL2PrmVtxCollections;
 
  std::string key = "";

  if (m_instance == "L2")      key = "T2HistoPrmVtx";
  else if (m_instance == "EF") key = "EFHistoPrmVtx";

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfL2PrmVtxCollections, key);
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfL2PrmVtxCollections.size() << " TrigVertexCollection" << endreq;

  std::vector<const TrigVertexCollection*>::iterator pPrmVtxColl    = vectorOfL2PrmVtxCollections.begin();
  std::vector<const TrigVertexCollection*>::iterator lastPrmVtxColl = vectorOfL2PrmVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 

      if ((*pPrmVtxColl)->size() != 0)
	msg() << MSG::VERBOSE << "TrigVertexCollection with label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
    }

    pPrmVtxColl = vectorOfL2PrmVtxCollections.begin();
  }

  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 

    if ((*pPrmVtxColl)->size() != 0) {

      if (m_instance == "L2") {
	if (m_algo == 1 || m_algo == 6) {
	  if ((*pPrmVtxColl)->front()->algorithmId() == 3) {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
	    break;
	  }
	} else if (m_algo == 2 || m_algo == 5) {
	  if ((*pPrmVtxColl)->front()->algorithmId() == 10) {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
	    break;
	  }
	} else if (m_algo == 8) {
	  if ((*pPrmVtxColl)->front()->algorithmId() == 19) {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
	    break;
	  }
	} else if (m_algo == 13) {
	  if ((*pPrmVtxColl)->front()->algorithmId() == 23) {
	    if (msgLvl() <= MSG::DEBUG)
	      msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
	    break;
	  }
	}
      } else if (m_instance == "EF") {
	if ((*pPrmVtxColl)->front()->algorithmId() == 11) {
	  if (msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Selected collection with TrigT2HistoPrmVtx label " << (*pPrmVtxColl)->front()->algorithmId() << endreq;
	  break;
	}
      }
    }
  } 
  
  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToL2PrmVtxCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToL2PrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getPrmVtxCollection(const VxContainer*& pointerToEFPrmVtxCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const VxContainer*> vectorOfEFPrmVtxCollections;
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFPrmVtxCollections, ""); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get VxContainer from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFPrmVtxCollections.size() << " VxContainer" << endreq;

  std::vector<const VxContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const VxContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 

      if ((*pPrmVtxColl)->size() != 0)
	msg() << MSG::VERBOSE << "VxContainer with label " << (*pPrmVtxColl)->front()->vertexType() << endreq;
    }

    pPrmVtxColl = vectorOfEFPrmVtxCollections.begin();
  }

  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 

    if ((*pPrmVtxColl)->size() != 0) {
      if ((*pPrmVtxColl)->front()->vertexType() == 1) {
	if (msgLvl() <= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "Selected collection with Primary Vertex label" << (*pPrmVtxColl)->front()->vertexType() << endreq;
	break;
      }
    }
  }

  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToEFPrmVtxCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToEFPrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getSecVtxCollection(const TrigVertexCollection*& pointerToL2SecVtxCollection, const HLT::TriggerElement* outputTE) {

  std::vector<const TrigVertexCollection*> vectorOfSecVtxCollections;
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfSecVtxCollections, "secVrt"); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got " << vectorOfSecVtxCollections.size() << " TrigVertexCollection" << endreq;

  for (int i = 0; i < (int) vectorOfSecVtxCollections.size(); i++) {

    if (!vectorOfSecVtxCollections[i] || vectorOfSecVtxCollections[i]->size() == 0) continue;
    
    // comment this if to make it work on old releases
    if (vectorOfSecVtxCollections[i]->front()->algorithmId() == TrigVertex::BJETSECVTXID) { // DANILO -- comment in early versions
      pointerToL2SecVtxCollection = vectorOfSecVtxCollections[i];
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                                        << "Selected Sec Vtx collection with algo id = "
                                        << pointerToL2SecVtxCollection->front()->algorithmId() << endreq;
      break;
    }
  }

  if (!pointerToL2SecVtxCollection) {
    return HLT::ERROR;
  } else
    return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getSecVtxCollection(const Trk::VxSecVertexInfoContainer*& pointerToSecVtxCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const Trk::VxSecVertexInfoContainer*> vectorOfSecVtxCollections;
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfSecVtxCollections, "SecVtxInfo"); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get VxSecVertexInfoContainer from the trigger element" << endreq;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got " << vectorOfSecVtxCollections.size() << " VxSecVertexInfoContainer" << endreq;

  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator pSecVtxColl    = vectorOfSecVtxCollections.begin();
  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator lastSecVtxColl = vectorOfSecVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pSecVtxColl != lastSecVtxColl; pSecVtxColl++) { 

      if ((*pSecVtxColl)->size() != 0) {
       
	const std::vector<xAOD::Vertex_v1*> vectorOfVxCandidates = (*pSecVtxColl)->front()->vertices();

        if (vectorOfVxCandidates.size()!=0)
          msg() << MSG::VERBOSE << "VxSecVertexInfoContainer with label " << (*vectorOfVxCandidates.begin())->vertexType() << endreq;
      }
    }

    pSecVtxColl = vectorOfSecVtxCollections.begin();
  }  

  for ( ; pSecVtxColl != lastSecVtxColl; pSecVtxColl++) { 

    if ((*pSecVtxColl)->size() != 0) {

      const std::vector<xAOD::Vertex_v1*> vectorOfVxCandidates = (*pSecVtxColl)->front()->vertices();

      if (vectorOfVxCandidates.size()!=0)
        if ((*vectorOfVxCandidates.begin())->vertexType() == 2) {
         
          if (msgLvl() <= MSG::DEBUG) 
            msg() << MSG::DEBUG << "Selected collection with Secondary Vertex label " << (*vectorOfVxCandidates.begin())->vertexType() << endreq;
          break;
        }
    }
  }

  if (pSecVtxColl == lastSecVtxColl) {
    pointerToSecVtxCollections = 0;
    return HLT::ERROR;
  } else {
    pointerToSecVtxCollections = *pSecVtxColl;
    return HLT::OK;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getSecVtxInfo(const TrigVertexCollection*& pointerToL2SecVtxCollections) {
   
   if(!pointerToL2SecVtxCollections) {
      msg() << MSG::DEBUG << "Pointer to L2 secondary vertex collections is zero" << endreq;
      return HLT::OK;
   }
   
   const TrigVertex* sv  = pointerToL2SecVtxCollections->at(0);
   
   if(!sv) {
     msg() << MSG::DEBUG << "Pointer to first L2 secondary vertex in collection is zero" << endreq;
      return HLT::OK;
   }
   
   m_trigBjetSecVtxInfo->setVtxMass(sv->mass());
   m_trigBjetSecVtxInfo->setDecayLengthSignificance(sv->decayLengthSignificance()); // DANILO -- comment for early versions
   m_trigBjetSecVtxInfo->setEnergyFraction(sv->energyFraction());
   m_trigBjetSecVtxInfo->setN2TrkVtx(sv->nTwoTracksSecVtx());

   TrackInVertexList* pointerToTracksInSV = sv->tracks();
   
   if(pointerToTracksInSV)
     m_trigBjetSecVtxInfo->setNTrksInVtx((int)pointerToTracksInSV->size());

   m_trigBjetSecVtxInfo->isValid(true);
   
   return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getSecVtxInfo(const Trk::VxSecVertexInfoContainer*& pointerToEFSecVtxCollections ,
					  const VxContainer*& pointerToEFPrmVtxCollections, 
					  const TrigVertexCollection*& pointerToPrmVtxCollections) {

   if(!pointerToEFSecVtxCollections) {
      msg() << MSG::DEBUG << "No secondary vertex collection sent when extracting sec vtx info" << endreq;
      return HLT::OK;
   }

   const Trk::VxSecVertexInfo* m_secVertexInfo = (*pointerToEFSecVtxCollections)[0];
   if(!m_secVertexInfo) {
      msg() << MSG::DEBUG << "No secondary vertex when extracting sec vtx info" << endreq;
      return HLT::OK;
   }

   const Trk::VxSecVKalVertexInfo* myVKalSecVertexInfo = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(m_secVertexInfo);
   
   if(!myVKalSecVertexInfo) {
      msg() << MSG::DEBUG << "The cast to VKal secndary vertex went wrong, the pointer is zero" << endreq;
      return HLT::OK;
   }
   
   const std::vector<xAOD::Vertex_v1*> & myVertices = myVKalSecVertexInfo->vertices();
   
   if(myVertices.size()>0) {
      
      m_trigBjetSecVtxInfo->setVtxMass(myVKalSecVertexInfo->mass());
      m_trigBjetSecVtxInfo->setEnergyFraction(myVKalSecVertexInfo->energyFraction());
      m_trigBjetSecVtxInfo->setN2TrkVtx(myVKalSecVertexInfo->n2trackvertices());

      int NTracksInSV=0;
      std::vector<xAOD::Vertex_v1*>::const_iterator verticesIt=myVertices.begin();
      std::vector<xAOD::Vertex_v1*>::const_iterator verticesEnd=myVertices.end();
      
      if(myVertices.size()>1) 
	msg() << MSG::DEBUG << "Secondary vertex from InDetVKalVxInJet has more than one vertex, is this ok?" << endreq;
      
      for( ; verticesIt!=verticesEnd ; ++verticesIt) {

	if(!(*verticesIt)) {
	  msg() << MSG::DEBUG << "Secondary vertex from InDetVKalVxInJet has zero pointer. Skipping this vtx.." << endreq;
	  continue;
	}

	msg() << MSG::DEBUG << "VxCandidate at (" 
	      << (*verticesIt)->position().x() << "," 
	      << (*verticesIt)->position().y() << "," 
	      << (*verticesIt)->position().z() << endreq;

	std::vector<Trk::VxTrackAtVertex> myTracks = (*verticesIt)->vxTrackAtVertex();

	if(myTracks.size() > 0)
	  NTracksInSV+=myTracks.size();

      }

      m_trigBjetSecVtxInfo->setNTrksInVtx(NTracksInSV);
      
      //Calculate decay length and significance here
      //Use the same utilities as in InDetVKalVxInJet
      //Some gymnastics needed...ugh...
      
      //bool prmVtxFound=false; // UNUSED
      const Trk::RecVertex* pPrmVrt=0;

      if(m_l2PrmVtxAtEF) {
         
	if(pointerToPrmVtxCollections) {
            
	  float x, y, z, exx, exy, exz, eyy, eyz, ezz;
	  exy = eyz = exz = 0; // cross-correlation 0 for now

	  z = pointerToPrmVtxCollections->at(0)->z();
	  ezz = sqrt(pointerToPrmVtxCollections->at(0)->cov()[5]);  // FIX for covariance definition - added sqrt
          //ezz = 0;
	  x = m_trigBjetPrmVtxInfo->xBeamSpot();
	  y = m_trigBjetPrmVtxInfo->yBeamSpot();

	  exx = m_trigBjetPrmVtxInfo->xBeamSpotWidth();
	  eyy = m_trigBjetPrmVtxInfo->yBeamSpotWidth();
            
	  AmgSymMatrix(3) cov;
            
	  cov(0,0) = exx*exx;  // FIX for covariance definition - added quadrature 
 	  cov(1,1) = eyy*eyy;  // FIX for covariance definition - added quadrature 
	  cov(2,2) = ezz*ezz;  // FIX for covariance definition - added quadrature 
	  cov(0,1) = cov(1,0) = exy; 
	  cov(0,2) = cov(2,0) = exz;
	  cov(1,2) = cov(2,1) = eyz;

//	  Trk::CovarianceMatrix* cov = new Trk::CovarianceMatrix(err);
//	  pPrmVrt = new Trk::RecVertex(HepGeom::Point3D<double>(x,y,z), Trk::ErrorMatrix(cov),
//				       pointerToPrmVtxCollections->at(0)->ndof(), pointerToPrmVtxCollections->at(0)->chi2());
	  //AmgSymMatrix(3) cov = err;
	  pPrmVrt = new Trk::RecVertex(Amg::Vector3D(x,y,z), cov,
				       pointerToPrmVtxCollections->at(0)->ndof(), pointerToPrmVtxCollections->at(0)->chi2());
            
	  //prmVtxFound = true; // UNUSED
	}         
      } else {
         
	if(pointerToEFPrmVtxCollections) {

	  const Trk::VxCandidate* PrmVtxCand = (*pointerToEFPrmVtxCollections)[0];
            
	  if(PrmVtxCand) {
               
	    const Trk::RecVertex& PrmVrt = PrmVtxCand->recVertex();
	    pPrmVrt = &PrmVrt;
	    //prmVtxFound = true; // UNUSED
	  }
	}
      }

      if(pPrmVrt) {
         
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Primary vertex for decay length (" 
		<< pPrmVrt->position().x() << "," 
		<< pPrmVrt->position().y() << "," 
		<< pPrmVrt->position().z() << ") and error (" 
		<< pPrmVrt->covariancePosition()(0,0) << "," 
		<< pPrmVrt->covariancePosition()(1,1) << "," 
		<< pPrmVrt->covariancePosition()(2,2) << ")" << endreq; 
	}
	
	CLHEP::HepLorentzVector jetDirection;

	if (m_useJetDirection == 1) {
	  jetDirection.setX(cos(m_trigBjetJetInfo->phiJet()));
	  jetDirection.setY(sin(m_trigBjetJetInfo->phiJet()));
	  jetDirection.setZ(sinh(m_trigBjetJetInfo->etaJet()));
	} else if (m_useJetDirection == 2) {
	  jetDirection.setX(cos(m_trigBjetJetInfo->phiTrkJet()));
	  jetDirection.setY(sin(m_trigBjetJetInfo->phiTrkJet()));
	  jetDirection.setZ(sinh(m_trigBjetJetInfo->etaTrkJet()));
	} else if (m_useJetDirection == 3) {
	  jetDirection.setX(cos(m_trigBjetJetInfo->phiRoI()));
	  jetDirection.setY(sin(m_trigBjetJetInfo->phiRoI()));
	  jetDirection.setZ(sinh(m_trigBjetJetInfo->etaRoI()));
	}

	//Gymnastics to get sec vtx position and error matrix in correct format for later
	const Amg::Vector3D SecVrt = myVertices[0]->position();

	std::vector<double> SecVrtErr(6,0);
	SecVrtErr[0] = myVertices[0]->covariancePosition()(0,0);
	SecVrtErr[1] = myVertices[0]->covariancePosition()(0,1);
	SecVrtErr[2] = myVertices[0]->covariancePosition()(1,1); 
	SecVrtErr[3] = myVertices[0]->covariancePosition()(0,2);
	SecVrtErr[4] = myVertices[0]->covariancePosition()(1,2);
	SecVrtErr[5] = myVertices[0]->covariancePosition()(2,2); 
	 
	const Trk::RecVertex & PrmVrt = *pPrmVrt;
         
	//Calcualte the jet-vrt direction for the signed decay length calculation
	double JetVrtDir =
	  jetDirection.px()*(SecVrt.x()-PrmVrt.position().x())
	  + jetDirection.py()*(SecVrt.y()-PrmVrt.position().y())
	  + jetDirection.pz()*(SecVrt.z()-PrmVrt.position().z());
         
	//Decay length
	float sign3D = 0.;
	double dist3D = m_taggerHelper->VrtVrtDist(PrmVrt,SecVrt,SecVrtErr,sign3D);
      
	if(JetVrtDir < 0) sign3D = -sign3D;

	m_trigBjetSecVtxInfo->setDecayLengthSignificance(sign3D);
    
	//Toggle
	m_trigBjetSecVtxInfo->isValid(true);

	double distance = (SecVrt - PrmVrt.position()).mag();
	if(msgLvl() <= MSG::DEBUG) {
	  if(fabs(distance-dist3D)>0.0001) {
	    msg() << MSG::DEBUG << "decay length distance do not match among tools: tool " << dist3D << " manual " << distance << endreq;
	  }
	}
         
	if(msgLvl() <= MSG::DEBUG) {
	  double dist2D = (SecVrt - PrmVrt.position()).perp();            
	  msg() << MSG::DEBUG << "Calculating secondary vertex decay length with primary vertex at (" 
		<< PrmVrt.position().x() << "," << PrmVrt.position().y() << "," << PrmVrt.position().z() << ") and sec vtx at ("
		<< SecVrt.x() << "," << SecVrt.y() << "," << SecVrt.z() 
		<< ") and jet direction (px,py,pz) = ("
		<< jetDirection.px() << "," << jetDirection.py() << "," << jetDirection.pz() <<  
	    ") which gives 3D decay length " << distance << " (VrtVrtDist tool " << dist3D << ") and 3D significance " << sign3D 
		<< " and 2D(R/phi) decay length " << dist2D << endreq;
	}

	//Clean up if I created a vertex object earlier which is not pointer to collection
	if(m_l2PrmVtxAtEF && pPrmVrt)
	  delete pPrmVrt;
      } 
   }

   return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigBjetFex::l2TrackSel(const TrigInDetTrack*& track, unsigned int i) {

  float zv = m_trigBjetPrmVtxInfo->zPrmVtx();

  float d0  = track->param()->a0();
  float z0  = track->param()->z0();
  float theta = 2*TMath::ATan(TMath::Exp(-track->param()->eta()));
  d0=track->param()->a0(); 
  z0=track->param()->z0();

  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "l2TrackSel method" << endreq;    
    msg() << MSG::VERBOSE << " Track number " << i+1 << " to be selected must be:" << endreq;
    msg() << MSG::VERBOSE << "  Pt " << track->param()->pT() << " >= " << m_trkSelPt << endreq;
    msg() << MSG::VERBOSE << "  d0 " << fabs(d0) << " <= " << m_trkSelD0 << endreq;
    msg() << MSG::VERBOSE << "  z0*sin(theta) " << fabs(z0-zv)*TMath::Sin(theta) << " <= " << m_trkSelZ0 << endreq;
    msg() << MSG::VERBOSE << "  SiHit " << track->NPixelSpacePoints()+track->NSCT_SpacePoints() << " >= " << m_trkSelSiHits << endreq;
    msg() << MSG::VERBOSE << "  isBLayer " << ((track->HitPattern() & 0x1) == 0 ? "1" : "0") << " = 0" << endreq;
    msg() << MSG::VERBOSE << "  Prob(chi2) " << TMath::Prob(track->chi2(),(track->NPixelSpacePoints()+track->NSCT_SpacePoints())*3-5) << " > " << m_trkSelChi2 << endreq;
  }

  if(m_useEtaPhiTrackSel) {

    if (fabs(track->param()->eta() - m_trigBjetJetInfo->etaRoI()) > 0.2) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (eta matching)" << endreq;;
      m_listCutApplied.push_back(2); return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track->param()->phi0()) - m_trigBjetJetInfo->phiRoI())) > 0.2) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (phi matching)" << endreq;;
      m_listCutApplied.push_back(3); return false;
    }
  }

  if (fabs(track->param()->pT()) < m_trkSelPt) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (pT cut)" << endreq;
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(d0) > m_trkSelD0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (d0 cut)" << endreq;
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > m_trkSelZ0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (z0 cut)" << endreq;
    m_listCutApplied.push_back(6); return false;
  }

  if (track->HitPattern()) {

    if ((track->HitPattern() & 0x1) == 0) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (missing b-layer hit)" << endreq;
      m_listCutApplied.push_back(7); return false;
    }

    if (track->NPixelSpacePoints()+track->NSCT_SpacePoints() < m_trkSelSiHits) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (too few silicon hits)" << endreq;
      m_listCutApplied.push_back(8); return false;
    }

    if (TMath::Prob(track->chi2(),(track->NPixelSpacePoints()+track->NSCT_SpacePoints())*3-5) <= m_trkSelChi2) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (chi2 cut)" << endreq;
      m_listCutApplied.push_back(9); return false;
    }
  }

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " selected" << endreq;

  m_listCutApplied.push_back(10);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigBjetFex::efTrackSel(const Rec::TrackParticle*& track, unsigned int i) {

  float zv = m_trigBjetPrmVtxInfo->zPrmVtx();

  const Trk::TrackSummary *summary = track->trackSummary();
  const Trk::FitQuality *quality   = track->fitQuality();

  int numberOfSiHits = summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits);
  float theta = track->measuredPerigee()->parameters()[Trk::theta];
  float qOverPt = track->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
  float pT = (1/qOverPt);
  
  float d0 = track->measuredPerigee()->parameters()[Trk::d0];
  float z0 = track->measuredPerigee()->parameters()[Trk::z0];

  // FIX FOR REDEFINED IP REFERENCE (ATR-9051)
  //m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0], track->measuredPerigee()->parameters()[Trk::z0], 
  //                     d0,z0,track->phi(), track->eta(), pT, m_trigBjetPrmVtxInfo->xBeamSpot(), m_trigBjetPrmVtxInfo->yBeamSpot()); 
  d0=track->measuredPerigee()->parameters()[Trk::d0]; 
  z0=track->measuredPerigee()->parameters()[Trk::z0];//+m_trigBjetPrmVtxInfo->zBeamSpot(); 
  // END FIX 

  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "efTrackSel method" << endreq;
    msg() << MSG::VERBOSE << "  Track number " << i+1 << " to be selected must be:" << endreq;
    msg() << MSG::VERBOSE << "    Pt " << fabs(pT) << " >= " << m_trkSelPt << endreq;
    msg() << MSG::VERBOSE << "    d0 " << fabs(d0) << " <= " << m_trkSelD0 << endreq;
    msg() << MSG::VERBOSE << "    z0*sin(theta) " << fabs(z0-zv)*TMath::Sin(theta) << " <= " << m_trkSelZ0 << endreq;
    msg() << MSG::VERBOSE << "    bLayer " << summary->get(Trk::numberOfBLayerHits) << " >= " << m_trkSelBLayer << endreq;
    msg() << MSG::VERBOSE << "    pixelHit " << summary->get(Trk::numberOfPixelHits) << " >= " << m_trkSelPixHits << endreq;
    msg() << MSG::VERBOSE << "    SiHit " << (summary->get(Trk::numberOfPixelHits)+ summary->get(Trk::numberOfSCTHits)) << " >= " 
	  << m_trkSelSiHits << endreq;
    msg() << MSG::VERBOSE << "    Prob(chi2) " << TMath::Prob(quality->chiSquared(), (int)numberOfSiHits*3-5) << " > " << m_trkSelChi2 << endreq;
  }

  if(m_useEtaPhiTrackSel) {

    if (fabs(track->eta() - m_trigBjetJetInfo->etaRoI()) > 0.2) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " is not selected (eta matching)" << endreq;
      m_listCutApplied.push_back(2); return false;
    }

    if (fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track->phi()) - m_trigBjetJetInfo->phiRoI())) > 0.2) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " is not selected (phi matching)" << endreq;
      m_listCutApplied.push_back(3); return false;
    }
  }

  if (fabs(pT) < m_trkSelPt) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (pT cut)" << endreq;
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(d0) > m_trkSelD0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (d0 cut)" << endreq;
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > m_trkSelZ0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (z0 cut)" << endreq;
    m_listCutApplied.push_back(6); return false;
  }

  if (summary->get(Trk::numberOfBLayerHits) < m_trkSelBLayer) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (missing b-layer hit)" << endreq;
    m_listCutApplied.push_back(7); return false;
  }

  if (summary->get(Trk::numberOfPixelHits) < m_trkSelPixHits) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (too few pixel hits)" << endreq;
    m_listCutApplied.push_back(8); return false;
  }

  if (numberOfSiHits < m_trkSelSiHits) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (too few silicon hits)" << endreq;
    m_listCutApplied.push_back(9); return false;
  }

  if (TMath::Prob(quality->chiSquared(), (int)numberOfSiHits*3-5) <= m_trkSelChi2) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  track " << i+1 << " not selected (chi2 cut)" << endreq;
    m_listCutApplied.push_back(10); return false;
  }
  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    track " << i+1 << " is selected" << endreq;

  m_listCutApplied.push_back(11);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBjetFex" << endreq;

  m_totSelTracks = 0;
  m_totTracks    = 0;

  m_trigBjetPrmVtxInfo->clear();
  m_trigBjetSecVtxInfo->clear();
  m_trigBjetJetInfo->clear();
  std::vector<TrigBjetTrackInfo> trigBjetTrackInfoVector;
  m_trigBjetTrackInfoVector = &trigBjetTrackInfoVector;

  //* Get RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) == HLT::OK) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using TE: " << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi() << ", Eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "No feature for this Trigger Element" << endreq;
    
    return HLT::NAV_ERROR;
  }

  //* Get the initial RoI descriptor *//
  const TrigRoiDescriptor* initialRoI = 0;
  if (m_instance == "L2") {
    if (getFeature(inputTE,  initialRoI, "initialRoI") == HLT::OK) {
      if (initialRoI) {
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Using initialRoI: " << "RoI id " << initialRoI->roiId()
		<< ", Phi = " <<  initialRoI->phi() << ", Eta = " << initialRoI->eta() << endreq;
	}
      } else {
	initialRoI=roiDescriptor; // L1.5 Jets...
      }
    } else {
      if (msgLvl() <= MSG::DEBUG) 
	msg() <<  MSG::DEBUG << "No feature for this Trigger Element" << endreq;
      
      return HLT::NAV_ERROR;
    }
  }

  //* Get LVL2 jets *//
  const TrigT2Jet* trigT2Jet = 0;
  if (m_retrieveHLTJets && m_instance == "L2") {
    if (getFeature(outputTE, trigT2Jet, "TrigT2CaloJet") == HLT::OK) {
      if (trigT2Jet) {
	if (msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Using LVL2 jet: " << "RoI id " << trigT2Jet->RoIword()
		<< ", Phi = " << trigT2Jet->phi() << ", Eta = " << trigT2Jet->eta() 
		<< ", Et = " << trigT2Jet->et() << endreq;
	}
      }
    }
  }

  //* Get EF jets *//
  float m_et_EFjet = 0;
  if (m_retrieveHLTJets && m_instance == "EF") {
    std::vector<const TrigOperationalInfo*> m_vectorOperationalInfo;
    if (getFeatures(outputTE, m_vectorOperationalInfo, "EFJetInfo") != HLT::OK) {
      if (msgLvl() <= MSG::WARNING) {
	msg() << MSG::WARNING << "Failed to get TrigOperationalInfo" << endreq;
      }
      return HLT::MISSING_FEATURE;
    } else {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Number of TrigOperationalInfo objects: " << m_vectorOperationalInfo.size() << endreq;
      }
    }

    std::vector<const TrigOperationalInfo*>::const_iterator m_operationalInfo;
    for (m_operationalInfo=m_vectorOperationalInfo.begin(); m_operationalInfo!=m_vectorOperationalInfo.end(); ++m_operationalInfo) {    
      
      if ((*m_operationalInfo)->defined("EFJetEt")==1) {
	
	unsigned int m_etSize = (*m_operationalInfo)->infos().first.size();    
	if (m_etSize!=1) {
	  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "More than one Et threshold associated to the same EF jet" << endreq;
	  return HLT::NAV_ERROR;
	}
    
	m_et_EFjet = (*m_operationalInfo)->get("EFJetEt");
      }
    }
  }

  if (m_instance == "L2") {
    m_trigBjetJetInfo->setEtaPhiJet(roiDescriptor->eta(), m_taggerHelper->phiCorr(roiDescriptor->phi()));
    m_trigBjetJetInfo->setEtaPhiRoI(initialRoI->eta(), m_taggerHelper->phiCorr(initialRoI->phi()));
    if (trigT2Jet) {
      if(m_tagHLTJets==1)
	m_trigBjetJetInfo->setEtJet(-trigT2Jet->et());
      else 
	m_trigBjetJetInfo->setEtJet(trigT2Jet->et());
    }
  } else if (m_instance == "EF") {
    m_trigBjetJetInfo->setEtaPhiJet(roiDescriptor->eta(), m_taggerHelper->phiCorr(roiDescriptor->phi()));
    m_trigBjetJetInfo->setEtaPhiRoI(roiDescriptor->eta(), m_taggerHelper->phiCorr(roiDescriptor->phi()));
    m_trigBjetJetInfo->setEtJet(m_et_EFjet);
  }

  //* Retrieve beamspot information *//
  IBeamCondSvc* m_iBeamCondSvc; 
  StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc);

  if (sc.isFailure() || m_iBeamCondSvc == 0) {
    m_iBeamCondSvc = 0;

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;

  } else {

    Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();

    int m_beamSpotBitMap = m_iBeamCondSvc->beamStatus();

    //* Check if beam spot is from online algorithms *//
    int m_beamSpotStatus = ((m_beamSpotBitMap & 0x4) == 0x4);
    
    //* Check if beam spot fit converged *//
    if (m_beamSpotStatus)
      m_beamSpotStatus = ((m_beamSpotBitMap & 0x3) == 0x3);

    if(msgLvl() <= MSG::DEBUG)
      
      msg() << MSG::DEBUG << "Beam spot from service: x=" << m_beamSpot.x() << ", y=" << m_beamSpot.y() << ", z=" << m_beamSpot.z() 
	    << ", tiltXZ=" << m_iBeamCondSvc->beamTilt(0) << ", tiltYZ=" << m_iBeamCondSvc->beamTilt(1) 
	    << ", sigmaX=" << m_iBeamCondSvc->beamSigma(0) << ", sigmaY=" << m_iBeamCondSvc->beamSigma(1) << ", sigmaZ=" << m_iBeamCondSvc->beamSigma(2) 
	    << ", status=" << m_beamSpotStatus << endreq;

    m_trigBjetPrmVtxInfo->setBeamSpot(m_beamSpot.x(), m_beamSpot.y(), m_beamSpot.z());
    m_trigBjetPrmVtxInfo->setBeamSpotTilt(m_iBeamCondSvc->beamTilt(0), m_iBeamCondSvc->beamTilt(1));
    m_trigBjetPrmVtxInfo->setBeamSpotWidth(m_iBeamCondSvc->beamSigma(0), m_iBeamCondSvc->beamSigma(1), m_iBeamCondSvc->beamSigma(2));
    m_trigBjetPrmVtxInfo->setBeamSpotStatus(m_beamSpotStatus);

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endreq;

  }

  //* Create collections *//
  if(m_instance == "L2")
    m_trigL2BjetColl = new TrigL2BjetContainer();
  if(m_instance == "EF")
    m_trigEFBjetColl = new TrigEFBjetContainer();
 
  //* Create pointers to collections *// 
  const TrigInDetTrackCollection*    pointerToL2TrackCollections = 0;
  const Rec::TrackParticleContainer* pointerToEFTrackCollections = 0;

  //* Create pointers to TrigVertex collection *// 
  const TrigVertexCollection*          pointerToPrmVtxCollections = 0;
  const TrigVertexCollection*          pointerToL2SecVtxCollections = 0;
  const VxContainer*                   pointerToEFPrmVtxCollections = 0;
  const Trk::VxSecVertexInfoContainer* pointerToEFSecVtxCollections = 0;

  if (m_instance == "L2")  { 
  
    //* Get LVL2 track collection *//
    if (getCollection(pointerToL2TrackCollections, outputTE) != HLT::OK) {
      msg() << MSG::DEBUG << "No LVL2 track collection retrieved" << endreq;
    } else if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "LVL2 track collection retrieved" << endreq;

    //* Get primary vertex collection *//
    if (getPrmVtxCollection(pointerToPrmVtxCollections, outputTE) != HLT::OK) {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
    } else if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Primary vertex collection retrieved" << endreq;

    //* Get secondary vertex collection *//
    if (getSecVtxCollection(pointerToL2SecVtxCollections, outputTE) != HLT::OK) {
      msg() << MSG::DEBUG << "No secondary vertex collection retrieved" << endreq;
    } else if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Secondary vertex collection retrieved" << endreq;
  }
  
  else if (m_instance == "EF") {

    //* Get EF track collection *//
    if (getCollection(pointerToEFTrackCollections, outputTE) != HLT::OK) {
      msg() << MSG::DEBUG << "No EF track collection retrieved" << endreq;
    } else if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "EF track collection retrieved" << endreq;

    //* Get primary vertex collection *//
    if (m_l2PrmVtxAtEF) {
      if (getPrmVtxCollection(pointerToPrmVtxCollections, outputTE) != HLT::OK) {
	msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
      } else if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Primary vertex collection retrieved" << endreq;
    } else {
      if (getPrmVtxCollection(pointerToEFPrmVtxCollections, outputTE) != HLT::OK) {
	msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
      } else if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Primary vertex collection retrieved" << endreq;
    }

    //* Get secondary vertex collection *//
    if (getSecVtxCollection(pointerToEFSecVtxCollections, outputTE) != HLT::OK) {
      msg() << MSG::DEBUG << "No secondary vertex collection retrieved" << endreq;
    } else if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Secondary vertex collection retrieved" << endreq;
  }

  //* Apply beam spot correction for tilt *//
  float m_xPrmVtx=0, m_yPrmVtx=0, m_zPrmVtx=0;

  if (m_instance == "L2" || m_l2PrmVtxAtEF) {
    // Protect against null pointers
    if(pointerToPrmVtxCollections) {
      // Protect against empty vectors
      if(pointerToPrmVtxCollections->size()>0) {
	const TrigVertex* prmVertex = (*pointerToPrmVtxCollections)[0];
	m_zPrmVtx = prmVertex->z();
      } else {
	msg() << MSG::DEBUG << "Empty primary vertex collection retrieved" << endreq;
	m_zPrmVtx = 0;
      }
    } else {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
      m_zPrmVtx = 0;
    }

  } else {
    // Protect against null pointers
    if(pointerToEFPrmVtxCollections) {
      // Protect against empty vectors
      if(pointerToEFPrmVtxCollections->size()>0) {
	const Trk::VxCandidate* prmVertex = (*pointerToEFPrmVtxCollections)[0];
	m_xPrmVtx = (float)prmVertex->recVertex().position().x();
	m_yPrmVtx = (float)prmVertex->recVertex().position().y();
	m_zPrmVtx = (float)prmVertex->recVertex().position().z();
      } else {
	msg() << MSG::DEBUG << "Empty primary vertex collection retrieved" << endreq;
	m_xPrmVtx = 0;
	m_yPrmVtx = 0;
	m_zPrmVtx = 0;
      }
    } else {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endreq;
      m_xPrmVtx = 0;
      m_yPrmVtx = 0;
      m_zPrmVtx = 0;
    }
  }
    
  float m_xBeamSpot = m_trigBjetPrmVtxInfo->xBeamSpot() + tan(m_trigBjetPrmVtxInfo->xBeamSpotTilt()) * (m_zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float m_yBeamSpot = m_trigBjetPrmVtxInfo->yBeamSpot() + tan(m_trigBjetPrmVtxInfo->yBeamSpotTilt()) * (m_zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float m_zBeamSpot = m_trigBjetPrmVtxInfo->zBeamSpot();

  m_trigBjetPrmVtxInfo->setBeamSpot(m_xBeamSpot, m_yBeamSpot, m_zBeamSpot);
  m_trigBjetPrmVtxInfo->setPrmVtx(m_xPrmVtx, m_yPrmVtx, m_zPrmVtx);

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endreq;

  m_trackJetFinderTool->clear();
  if (msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "Set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() << endreq;
  m_trackJetFinderTool->inputPrimaryVertexZ(m_trigBjetPrmVtxInfo->zPrmVtx());
  if (msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "Done set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() << endreq;

  if (m_instance == "L2")  {

    m_totTracks = m_taggerHelper->getTrackNumber(pointerToL2TrackCollections);

    for (unsigned int j = 0; j < m_totTracks; j++) {

      const TrigInDetTrack* track = (*pointerToL2TrackCollections)[j];

      m_mon_trk_a0.push_back(track->param()->a0());
      m_mon_trk_z0.push_back(track->param()->z0());

      if (l2TrackSel(track, j)) {
	m_totSelTracks++;
	TrigBjetTrackInfo trigBjetTrackInfo(track);
	float d0Corr=0, z0Corr=0;
 	d0Corr=track->param()->a0(); 
 	z0Corr=track->param()->z0();
	trigBjetTrackInfo.setIPCorr(d0Corr, z0Corr);
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "  " << trigBjetTrackInfo << endreq;
	trigBjetTrackInfoVector.push_back(trigBjetTrackInfo);
  if (msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "Adding track to rackjet tool " << endreq;
	m_trackJetFinderTool->addTrack(track, j);
      }
    }

  } else if (m_instance == "EF") {

    m_totTracks = m_taggerHelper->getTrackNumber(pointerToEFTrackCollections);

    for (unsigned int j = 0; j < m_totTracks; j++) {
      
      const Rec::TrackParticle* track = (*pointerToEFTrackCollections)[j];
      
      m_mon_trk_a0.push_back(track->measuredPerigee()->parameters()[Trk::d0]);
      m_mon_trk_z0.push_back(track->measuredPerigee()->parameters()[Trk::z0]);

      if (efTrackSel(track, j)) {
	m_totSelTracks++;
	TrigBjetTrackInfo trigBjetTrackInfo(track);

	float d0Corr=0, z0Corr=0;
	d0Corr=track->measuredPerigee()->parameters()[Trk::d0]; 
	z0Corr=track->measuredPerigee()->parameters()[Trk::z0];
	trigBjetTrackInfo.setIPCorr(d0Corr, z0Corr);
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "  " << trigBjetTrackInfo << endreq;
	trigBjetTrackInfoVector.push_back(trigBjetTrackInfo);
	m_trackJetFinderTool->addTrack(track, j);
      }
    }
  }

  std::vector<int> tracksTrackJet;
  float etaTrackJet, phiTrackJet;
  
  m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);

  if (etaTrackJet != -99 && phiTrackJet != -99) {
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(etaTrackJet, phiTrackJet);
    
  } else {
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(m_trigBjetJetInfo->etaRoI(), m_trigBjetJetInfo->phiRoI());
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "eta Jet = eta RoI" << endreq;
  }
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetJetInfo << endreq;

  //* For monitoring *//
  m_deltaEtaJet       = m_trigBjetJetInfo->etaRoI()-m_trigBjetJetInfo->etaJet();
  m_deltaPhiJet       = m_trigBjetJetInfo->phiRoI()-m_trigBjetJetInfo->phiJet();
  m_deltaEtaTrkJet    = m_trigBjetJetInfo->etaRoI()-m_trigBjetJetInfo->etaTrkJet();
  m_deltaPhiTrkJet    = m_trigBjetJetInfo->phiRoI()-m_trigBjetJetInfo->phiTrkJet();
  m_deltaEtaJetTrkJet = m_trigBjetJetInfo->etaJet()-m_trigBjetJetInfo->etaTrkJet();
  m_deltaPhiJetTrkJet = m_trigBjetJetInfo->phiJet()-m_trigBjetJetInfo->phiTrkJet();

  std::vector<std::string>::iterator pTagger    = m_taggers.begin();
  std::vector<std::string>::iterator lastTagger = m_taggers.end();

  if (m_useBeamSpotFlag && !m_trigBjetPrmVtxInfo->beamSpotStatus()) {

    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Beam spot status flag set to " << m_trigBjetPrmVtxInfo->beamSpotStatus() << ". Discriminant weights are not computed." << endreq;
      msg() << MSG::DEBUG << "Beam spot flag set to " << m_useBeamSpotFlag << ". Discriminant weights are not computed." << endreq;
    }

    m_listCutApplied.push_back(0);
    m_trigBjetTagger->getWeights();

  } else if (m_trigBjetPrmVtxInfo->xBeamSpotWidth()>m_setBeamSpotWidth || m_trigBjetPrmVtxInfo->yBeamSpotWidth()>m_setBeamSpotWidth) {

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Beam spot width is more than " << m_setBeamSpotWidth << "um. Discriminant weights are not computed." << endreq;

    m_listCutApplied.push_back(1);
    m_trigBjetTagger->getWeights();

  } else {
    
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Computing discriminant weights using taggers: " << m_taggers 
	    << " and using calibration from " << (m_useParamFromData==0 ? "MC" : "data") << " for CHI2" << endreq;

    //Look for a sec vertex?
    bool retrieveSV = false;
    for ( ; pTagger != lastTagger; pTagger++) {
       if ((*pTagger).find("VTX") != std::string::npos) {
          retrieveSV = true;
          break;
       }
    }
    
    if(retrieveSV) {
       
      if (m_instance == "L2")  {

	//* Get secondary vertex information at LVL2 from TrigBjetFex::getSecVtxInfo *//
	if (getSecVtxInfo(pointerToL2SecVtxCollections) != HLT::OK)
	  if (msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "No L2 SV information retrieved from TrigBjetFex::getSecVtxInfo" << endreq;
       
      } else if (m_instance == "EF") {

	//* Get secondary vertex information at EF from TrigBjetFex::getSecVtxInfo *//
	if (getSecVtxInfo(pointerToEFSecVtxCollections, pointerToEFPrmVtxCollections, pointerToPrmVtxCollections) != HLT::OK)
	  if (msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "No EF SV information retrieved from TrigBjetFex::getSecVtxInfo" << endreq;
      }

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << *m_trigBjetSecVtxInfo << endreq;

    }

    m_trigBjetTagger->getWeights(m_trigBjetTrackInfoVector, m_trigBjetPrmVtxInfo, m_trigBjetSecVtxInfo, m_trigBjetJetInfo);
  }
  
  if (msgLvl() <= MSG::DEBUG) {
     
    const EventInfo* pEventInfo;
     
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
      msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
    } 
    else {
      msg() << MSG::DEBUG << "Bjet slice summary (Run " << pEventInfo->event_ID()->run_number() 
	    << "; Event " << pEventInfo->event_ID()->event_number() << ")" << endreq;
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId()
	    << ", Phi = "   << roiDescriptor->phi()
	    << ", Eta = "   << roiDescriptor->eta() << endreq;
      msg() << MSG::DEBUG << "REGTEST:  Tracks: " << m_totTracks << " reconstructed and " << m_totSelTracks <<" selected" << endreq;
      
      if (m_instance == "EF") {
	if (pointerToPrmVtxCollections) {
	  msg() << MSG::DEBUG << "REGTEST:  Primary vertex: " << pointerToPrmVtxCollections->size() << " reconstructed"
		<< ", (x,y,z) = (" << m_trigBjetPrmVtxInfo->xPrmVtx() << "," << m_trigBjetPrmVtxInfo->yPrmVtx() << "," << m_trigBjetPrmVtxInfo->zPrmVtx() << ")" << endreq;
	}
	if (pointerToEFSecVtxCollections)
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: " << pointerToEFSecVtxCollections->size() << " reconstructed" << endreq;
	else
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: 0 reconstructed" << endreq;
      } else {
	msg() << MSG::DEBUG << "REGTEST:  Primary vertex: " << pointerToPrmVtxCollections->size() << " reconstructed"
	      << ", z = " << m_trigBjetPrmVtxInfo->zPrmVtx() << endreq;
	if (pointerToL2SecVtxCollections)
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: " << pointerToL2SecVtxCollections->size() << " reconstructed" << endreq;
	else
	  msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: 0 reconstructed" << endreq;
      }

      msg() << MSG::DEBUG << "REGTEST:  SV L/sigma(L) " <<  m_trigBjetSecVtxInfo->decayLengthSignificance() << "  SV mass " <<  m_trigBjetSecVtxInfo->vtxMass() 
	    << "  SV efrac " <<  m_trigBjetSecVtxInfo->energyFraction() << "   SV 2-track vertex multiplicity " << m_trigBjetSecVtxInfo->n2TrkVtx() << endreq; 
      
      msg() << MSG::DEBUG << "REGTEST: List weights stored probability and likelihood objects:" << endreq;

      pTagger = m_taggers.begin();
      for ( ; pTagger != lastTagger; pTagger++)
	msg() << MSG::DEBUG << "REGTEST:  X(" << (*pTagger) << ") = " << m_trigBjetTagger->taggersXMap((*pTagger))  << endreq;

    }
  }

  //* Create TrigL2Bjet and attach feature *//
  if (m_instance == "L2") {

    TrigL2Bjet* trigL2Bjet = new TrigL2Bjet(roiDescriptor->roiId(), m_trigBjetJetInfo->etaJet(), m_trigBjetJetInfo->phiJet(),
					    0, 0, 0, m_trigBjetPrmVtxInfo->zPrmVtx(), m_trigBjetJetInfo->etJet(),
					    m_trigBjetTagger->taggersXMap("COMB"),  m_trigBjetTagger->taggersXMap("IP1D"), 
					    m_trigBjetTagger->taggersXMap("IP2D"),  m_trigBjetTagger->taggersXMap("IP3D"),
					    m_trigBjetTagger->taggersXMap("CHI2"), 
					    m_trigBjetSecVtxInfo->decayLengthSignificance(), m_trigBjetSecVtxInfo->vtxMass(), 
					    m_trigBjetSecVtxInfo->energyFraction(), m_trigBjetSecVtxInfo->n2TrkVtx()); 
    
    trigL2Bjet->validate(true);
    m_trigL2BjetColl->push_back(trigL2Bjet);

    if (!m_trigL2BjetColl) {
      msg() << MSG::ERROR << "Feature TrigL2BjetContainer not found" << endreq;
      return HLT::BAD_JOB_SETUP;
    }
    
    HLT::ErrorCode stat = attachFeature(outputTE, m_trigL2BjetColl, "L2BjetFex");
    
    if (stat != HLT::OK) {
      if (msgLvl() <= MSG::DEBUG) 
    	msg() << MSG::DEBUG << "Failed to attach TrigL2BjetContainer to navigation" << endreq;
      return stat;
    }
  }

  //* Create TrigEFBjet and attach feature *//
  if (m_instance == "EF") {
 
    TrigEFBjet* trigEFBjet = new TrigEFBjet(roiDescriptor->roiId(), m_trigBjetJetInfo->etaJet(), m_trigBjetJetInfo->phiJet(),
					    0, 0, 0, m_trigBjetPrmVtxInfo->zPrmVtx(), m_trigBjetJetInfo->etJet(),
					    m_trigBjetTagger->taggersXMap("COMB"),  m_trigBjetTagger->taggersXMap("IP1D"), 
					    m_trigBjetTagger->taggersXMap("IP2D"),  m_trigBjetTagger->taggersXMap("IP3D"), 
					    m_trigBjetTagger->taggersXMap("CHI2"), 
					    m_trigBjetSecVtxInfo->decayLengthSignificance(), m_trigBjetSecVtxInfo->vtxMass(), 
					    m_trigBjetSecVtxInfo->energyFraction(), m_trigBjetSecVtxInfo->n2TrkVtx()); 
    
    trigEFBjet->validate(true);
    m_trigEFBjetColl->push_back(trigEFBjet);

    if (!m_trigEFBjetColl) {
      msg() << MSG::ERROR << "Feature TrigEFBjetContainer not found" << endreq;
      return HLT::BAD_JOB_SETUP;
    }
    
    HLT::ErrorCode stat = attachFeature(outputTE, m_trigEFBjetColl, "EFBjetFex");
    
    if (stat != HLT::OK) {
      if ( msgLvl() <= MSG::DEBUG) 
    	msg() << MSG::DEBUG << "Failed to attach TrigEFBjetContainer to navigation" << endreq;
      return stat;
    }
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetFex" << endreq;

  return HLT::OK;
}



