/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBtagFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "TrigBjetHypo/TrigBtagFex.h"

// ONLINE INFRASTRUCTURE
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h" // TO BE REMOVED

// BEAMSPOT
#include "InDetBeamSpotService/IBeamCondSvc.h"

// EDM
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"

#include "TrigBjetHypo/TrigBjetTagger.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigBjetHypo/TuningLikelihood.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TaggerHelper.h"					/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigInDetEvent/TrigInDetTrackCollection.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "Particle/TrackParticleContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigInDetEvent/TrigVertexCollection.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "VxVertex/VxContainer.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "VxSecVertex/VxSecVertexInfo.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "VxSecVertex/VxSecVKalVertexInfo.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigCaloEvent/TrigT2Jet.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigSteeringEvent/TrigRoiDescriptor.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "EventInfo/EventInfo.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "EventInfo/EventID.h"					/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "EventPrimitives/EventPrimitives.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "EventPrimitives/EventPrimitivesHelpers.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigParticle/TrigEFBjetContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigSteeringEvent/TrigOperationalInfo.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "InDetBeamSpotService/IBeamCondSvc.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "TrigInDetEvent/TrigVertex.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODBTagging/BTaggingContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODBTagging/BTagging.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODBTagging/BTaggingAuxContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODJet/Jet.h"					/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODJet/JetContainer.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODTracking/VertexContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODTracking/VertexAuxContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
								/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODTracking/VertexContainer.h"			/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODTracking/Vertex.h"				/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODTracking/TrackParticleContainer.h"		/* TO BE REMOVED AS SOON AS UNNECESSARY */ 
#include "xAODBase/IParticle.h"					/* TO BE REMOVED AS SOON AS UNNECESSARY */ 




//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBtagFex::TrigBtagFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_bTagTrackAssocTool("Analysis::BTagTrackAssociation"),
  m_bTagSecVtxTool("Analysis::BTagSecVertexing"),

  m_trackJetFinderTool("TrigTrackJetFinderTool",this),      	
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
  declareProperty("setupOfflineTools",  m_setupOfflineTools=false);

  declareProperty("BTagTool",           m_bTagTool);
  declareProperty("BTagTrackAssocTool", m_bTagTrackAssocTool);
  declareProperty("BTagSecVertexing",   m_bTagSecVtxTool);

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
  declareProperty ("HistoPrmVtxAtEF",    m_histoPrmVtxAtEF       = true);
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
  declareMonitoredObject      ("roi_selectedTracks", *this, &TrigBtagFex::totSelectedTracks);

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


TrigBtagFex::~TrigBtagFex() {

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


HLT::ErrorCode TrigBtagFex::hltInitialize() {
  
  //* Get message service *//
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigBtagFex, version " << PACKAGE_VERSION << endreq;

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
    msg() << MSG::DEBUG << " HistoPrmVtxAtEF = "        << m_histoPrmVtxAtEF << endreq;
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

  if(m_setupOfflineTools) {

    // Retrieve the offline track association tool
    if(!m_bTagTrackAssocTool.empty()) {
      if(m_bTagTrackAssocTool.retrieve().isFailure()) {
	msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTrackAssocTool << endreq;
	return HLT::BAD_JOB_SETUP;
      } else
	msg() << MSG::INFO << "Retrieved tool " << m_bTagTrackAssocTool << endreq;
    } else if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No track association tool to retrieve" << endreq;
  
    // Retrieve the bTagSecVtxTool
    if(m_bTagSecVtxTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagSecVtxTool << endreq;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagSecVtxTool << endreq;
  
    // Retrieve the main BTagTool
    if(m_bTagTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTool << endreq;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagTool << endreq;

  }
  
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBtagFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBtagFex" << endreq;

  // RETRIEVE INPUT CONTAINERS

  //* Get EF jet *//
  const xAOD::JetContainer* jets = 0;
  if(getFeature(inputTE, jets) == HLT::OK && jets!=0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::JetContainer: " << "nJets = " << jets->size() << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - No xAOD::JetContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  //* Get primary vertex *//
  const xAOD::VertexContainer* vertexes = 0;
  if(getFeature(outputTE, vertexes, "EFHistoPrmVtx") == HLT::OK && vertexes != 0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: " << "nVertexes = " << vertexes->size() << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::VertexContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  //* Get tracks *// 
  const xAOD::TrackParticleContainer* tracks = 0;
  if(getFeature(outputTE, tracks, "") == HLT::OK && tracks != 0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::TrackParticleContainer: " << "nTracks = " << tracks->size() << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::TrackParticleContainer" << endreq;
    return HLT::MISSING_FEATURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS

  //* Prepare jet tagging - get primary vertex *//
  //xAOD::VertexContainer::const_iterator vtxitr = vertexes->begin();
  //const xAOD::Vertex* primaryVertex = *vtxitr;

  //* Prepare jet tagging - create temporary jet copy *//
  //xAOD::JetContainer::const_iterator jetitr=jets->begin();
  //xAOD::Jet* jet = new xAOD::Jet(**jetitr);

  //* Prepare jet tagging - create SV output *//
  xAOD::VertexAuxContainer trigVertexAuxContainer;
  xAOD::VertexContainer* trigVertexContainer = new xAOD::VertexContainer();
  trigVertexContainer->setStore(&trigVertexAuxContainer);
  xAOD::Vertex* trigVertex = new xAOD::Vertex();
  trigVertexContainer->push_back(trigVertex);

  //* Prepare jet tagging - create JF output *//
  xAOD::BTagVertexAuxContainer trigBTagVertexAuxContainer;
  xAOD::BTagVertexContainer* trigBTagVertexContainer = new xAOD::BTagVertexContainer();
  trigBTagVertexContainer->setStore(&trigBTagVertexAuxContainer);
  xAOD::BTagVertex* trigBTagVertex = new xAOD::BTagVertex();
  trigBTagVertexContainer->push_back(trigBTagVertex);

  //* Prepare jet tagging - create BTagging output *//
  xAOD::BTaggingAuxContainer trigBTaggingAuxContainer;
  xAOD::BTaggingContainer* trigBTaggingContainer = new xAOD::BTaggingContainer();
  trigBTaggingContainer->setStore(&trigBTaggingAuxContainer);
  xAOD::BTagging* trigBTagging = new xAOD::BTagging();
  trigBTaggingContainer->push_back(trigBTagging);

  // EXECUTE OFFLINE TOOLS

  //* Execute track associator *// TODO: do this with offline track associator - temporarily done by hand
  std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks;
  for(auto trkitr = tracks->begin(); trkitr != tracks->end(); ++trkitr) {
    ElementLink<xAOD::TrackParticleContainer> EL;
    EL.toContainedElement(*tracks, *trkitr);
    associationLinks.push_back(EL);
  }
  trigBTagging->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >("BTagTrackToJetAssociator") = associationLinks;

  // UNCOMMENT  //* Execute secondary vertexing *//
  // UNCOMMENT  StatusCode sc = m_bTagSecVtxTool->BTagSecVtx_exec(*jet, trigBTagging, trigVertexContainer, trigBTagVertexContainer, primaryVertex);
  // UNCOMMENT  if(sc.isFailure()) {
  // UNCOMMENT    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed to reconstruct sec vtx" << endreq;
  // UNCOMMENT  }

  // UNCOMMENT  //* Tag jet *//
  // UNCOMMENT  sc = m_bTagTool->tagJet(*jet, trigBTagging, primaryVertex);
  // UNCOMMENT  if(sc.isFailure()) {
  // UNCOMMENT    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed in taggers call" << endreq;
  // UNCOMMENT  }

  // DEBUG DUMPS

  //* Dump results *//
  msg() << MSG::DEBUG << "IP2D u/b: " << trigBTagging->IP2D_pu() << "/" << trigBTagging->IP2D_pb()
	<< "   IP3D u/b: " << trigBTagging->IP3D_pu() << "/" << trigBTagging->IP3D_pb()
	<< "   SV1 u/b: " << trigBTagging->SV1_pu() << "/" << trigBTagging->SV1_pb()
	<< "   MV1 var: " << trigBTagging->MV1_discriminant() << endreq;

  // ATTACH FEATURES

  //* Attach SV output as feature *//
  if(attachFeature(outputTE, trigVertexContainer, "BjetSecondaryVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::VertexContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::VertexContainer" << endreq;
    return HLT::NAV_ERROR;
  }

  //* Attach JF output as feature *//
  if(attachFeature(outputTE, trigBTagVertexContainer, "BjetVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTagVertexContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTagVertexContainer" << endreq;
    return HLT::NAV_ERROR;
  }

  //* Attach BTagContainer as feature *//
  if(attachFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTaggingContainer" << endreq;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTaggingContainer" << endreq;
    return HLT::NAV_ERROR;
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBtagFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBtagFex" << endreq;

  return HLT::OK;
}



