/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
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

#include "TrigParticle/TrigEFBjetContainer.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigInDetEvent/TrigVertex.h"

#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFex::TrigBjetFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_trackJetFinderTool("TrigTrackJetFinderTool",this),      	
  m_trigEFBjetColl(0),
  m_trigBTaggingContainer(nullptr),
  m_trigBjetTagger(0),
  m_constTrigBjetTagger(0),
  m_trigBjetPrmVtxInfo(0),
  m_trigBjetSecVtxInfo(0),
  m_trigBjetJetInfo(0),
  m_trigBjetTrackInfoVector(nullptr),
  m_totTracks(0),
  m_totSelTracks(0),
  m_tuningLikelihoodIP1D_Grade1(0),
  m_tuningLikelihoodIP2D_Grade1(0),
  m_tuningLikelihoodIP3D_Grade1(0),
  m_tuningLikelihoodIP1D_Grade2(0),
  m_tuningLikelihoodIP2D_Grade2(0),
  m_tuningLikelihoodIP3D_Grade2(0),
  m_tuningLikelihoodMVtx(0),
  m_tuningLikelihoodEVtx(0),
  m_tuningLikelihoodNVtx(0),
  m_tuningLikelihoodSV(0)
{
  declareProperty ("AlgoId",             m_algo);
  declareProperty ("Instance",           m_instance);
  declareProperty ("Taggers",            m_taggers);
  declareProperty ("JetKey",             m_jetKey     = ""); //"" needed for default config, SplitJet for new config
  declareProperty ("PriVtxKey",          m_priVtxKey  = "EFHistoPrmVtx");
  declareProperty ("TrackKey",           m_trackKey   = ""); //"" needed for default config, Bjet_IDTrig for new config?

  declareProperty ("par_0_MC",           m_par_0_MC);
  declareProperty ("par_1_MC",           m_par_1_MC);
  declareProperty ("par_0_DT",           m_par_0_DT);
  declareProperty ("par_1_DT",           m_par_1_DT);

  declareProperty ("SizeIP1D_Grade1",    m_sizeIP1D_Grade1);
  declareProperty ("bIP1D_Grade1",       m_bIP1D_Grade1);
  declareProperty ("uIP1D_Grade1",       m_uIP1D_Grade1);
  declareProperty ("SizeIP2D_Grade1",    m_sizeIP2D_Grade1);
  declareProperty ("bIP2D_Grade1",       m_bIP2D_Grade1);
  declareProperty ("uIP2D_Grade1",       m_uIP2D_Grade1);
  declareProperty ("SizeIP3D_Grade1",    m_sizeIP3D_Grade1);
  declareProperty ("bIP3D_Grade1",       m_bIP3D_Grade1);
  declareProperty ("uIP3D_Grade1",       m_uIP3D_Grade1);

  declareProperty ("useGrading",         m_useGrading = true);
  declareProperty ("SizeIP1D_Grade2",    m_sizeIP1D_Grade2);
  declareProperty ("bIP1D_Grade2",       m_bIP1D_Grade2);
  declareProperty ("uIP1D_Grade2",       m_uIP1D_Grade2);
  declareProperty ("SizeIP2D_Grade2",    m_sizeIP2D_Grade2);
  declareProperty ("bIP2D_Grade2",       m_bIP2D_Grade2);
  declareProperty ("uIP2D_Grade2",       m_uIP2D_Grade2);
  declareProperty ("SizeIP3D_Grade2",    m_sizeIP3D_Grade2);
  declareProperty ("bIP3D_Grade2",       m_bIP3D_Grade2);
  declareProperty ("uIP3D_Grade2",       m_uIP3D_Grade2);

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
  declareProperty ("HistoPrmVtxAtEF",    m_histoPrmVtxAtEF    = true);
  declareProperty ("UseEtaPhiTrackSel",  m_useEtaPhiTrackSel  = false);

  declareProperty ("UseJetDirection",    m_useJetDirection);
  declareProperty ("RetrieveHLTJets",    m_retrieveHLTJets    = true);
  declareProperty ("TagHLTJets",         m_tagHLTJets         = 0);

  declareProperty ("TrkSelGrade1_Chi2",            m_trkSelGrade1_Chi2            = 0.001);
  declareProperty ("TrkSelGrade1_Innermost",       m_trkSelGrade1_Innermost       = 1);
  declareProperty ("TrkSelGrade1_NextToInnermost", m_trkSelGrade1_NextToInnermost = 1);
  declareProperty ("TrkSelGrade1_PixHits",         m_trkSelGrade1_PixHits         = 2);
  declareProperty ("TrkSelGrade1_SiHits",          m_trkSelGrade1_SiHits          = 4);
  declareProperty ("TrkSelGrade1_D0",              m_trkSelGrade1_D0              = 1*CLHEP::mm);
  declareProperty ("TrkSelGrade1_Z0",              m_trkSelGrade1_Z0              = 2*CLHEP::mm);
  declareProperty ("TrkSelGrade1_Pt",              m_trkSelGrade1_Pt              = 1*CLHEP::GeV);
  declareProperty ("TrkSelGrade1_Eta",             m_trkSelGrade1_Eta             = 0.4);
  declareProperty ("TrkSelGrade1_Phi",             m_trkSelGrade1_Phi             = 0.4);
  declareProperty ("TrkSelGrade1_R",               m_trkSelGrade1_R               = 0.4);

  declareProperty ("TrkSelGrade2_Chi2",            m_trkSelGrade2_Chi2            = 0.001);
  declareProperty ("TrkSelGrade2_Innermost",       m_trkSelGrade2_Innermost       = 1);
  declareProperty ("TrkSelGrade2_NextToInnermost", m_trkSelGrade2_NextToInnermost = 1);
  declareProperty ("TrkSelGrade2_PixHits",         m_trkSelGrade2_PixHits         = 2);
  declareProperty ("TrkSelGrade2_SiHits",          m_trkSelGrade2_SiHits          = 4);
  declareProperty ("TrkSelGrade2_D0",              m_trkSelGrade2_D0              = 1*CLHEP::mm);
  declareProperty ("TrkSelGrade2_Z0",              m_trkSelGrade2_Z0              = 2*CLHEP::mm);
  declareProperty ("TrkSelGrade2_Pt",              m_trkSelGrade2_Pt              = 1*CLHEP::GeV);
  declareProperty ("TrkSelGrade2_Eta",             m_trkSelGrade2_Eta             = 0.4);
  declareProperty ("TrkSelGrade2_Phi",             m_trkSelGrade2_Phi             = 0.4);
  declareProperty ("TrkSelGrade2_R",               m_trkSelGrade2_R               = 0.4);

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

  // Run-2 monitoring

  declareMonitoredVariable("sv_mass", m_mon_sv_mass, AutoClear);
  declareMonitoredVariable("sv_evtx", m_mon_sv_evtx, AutoClear);
  declareMonitoredVariable("sv_nvtx", m_mon_sv_nvtx, AutoClear);

  declareMonitoredVariable("tag_IP2D",    m_mon_tag_IP2D,    AutoClear);
  declareMonitoredVariable("tag_IP3D",    m_mon_tag_IP3D,    AutoClear);
  declareMonitoredVariable("tag_SV1",     m_mon_tag_SV1,     AutoClear);
  declareMonitoredVariable("tag_IP3DSV1", m_mon_tag_IP3DSV1, AutoClear);
  declareMonitoredVariable("tag_MV2c00",  m_mon_tag_MV2c00,  AutoClear);
  declareMonitoredVariable("tag_MV2c10",  m_mon_tag_MV2c10,  AutoClear);
  declareMonitoredVariable("tag_MV2c20",  m_mon_tag_MV2c20,  AutoClear);

  m_taggerHelper = new TaggerHelper(msg(), msgLvl());
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetFex::~TrigBjetFex() {

  if (m_taggerHelper)            delete m_taggerHelper;
  if (m_trigBjetTagger)          delete m_trigBjetTagger;
  if (m_trigBjetPrmVtxInfo)      delete m_trigBjetPrmVtxInfo;
  if (m_trigBjetSecVtxInfo)      delete m_trigBjetSecVtxInfo;
  if (m_trigBjetJetInfo)         delete m_trigBjetJetInfo;
  if (m_tuningLikelihoodIP1D_Grade1)    delete m_tuningLikelihoodIP1D_Grade1;
  if (m_tuningLikelihoodIP2D_Grade1)    delete m_tuningLikelihoodIP2D_Grade1;
  if (m_tuningLikelihoodIP3D_Grade1)    delete m_tuningLikelihoodIP3D_Grade1;
  if (m_tuningLikelihoodIP1D_Grade2)    delete m_tuningLikelihoodIP1D_Grade2;
  if (m_tuningLikelihoodIP2D_Grade2)    delete m_tuningLikelihoodIP2D_Grade2;
  if (m_tuningLikelihoodIP3D_Grade2)    delete m_tuningLikelihoodIP3D_Grade2;
  if (m_tuningLikelihoodMVtx)    delete m_tuningLikelihoodMVtx;
  if (m_tuningLikelihoodEVtx)    delete m_tuningLikelihoodEVtx;
  if (m_tuningLikelihoodNVtx)    delete m_tuningLikelihoodNVtx;
  if (m_tuningLikelihoodSV)      delete m_tuningLikelihoodSV;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltInitialize() {
  
  // Get message service
      if (msgLvl() <= MSG::INFO) 
      msg() << MSG::INFO << "Initializing TrigBjetFex, version " << PACKAGE_VERSION << endmsg;

    // declareProperty overview
        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "declareProperty review:" << endmsg;

	  msg() << MSG::DEBUG << "JetKey = "               << m_jetKey << endmsg;
	  msg() << MSG::DEBUG << "PriVtxKey = "            << m_priVtxKey << endmsg;

          msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endmsg; 
          msg() << MSG::DEBUG << " Instance = "            << m_instance << endmsg;
 
          msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endmsg; 
          msg() << MSG::DEBUG << " SetBeamSpotWidth = "    << m_setBeamSpotWidth << endmsg;

          msg() << MSG::DEBUG << " UseParamFromData = "    << m_useParamFromData << endmsg; 

          msg() << MSG::DEBUG << " Taggers = "             << m_taggers << endmsg; 
          msg() << MSG::DEBUG << " UseErrIPParam = "       << m_useErrIPParam << endmsg; 
          msg() << MSG::DEBUG << " UseJetDirection = "     << m_useJetDirection << endmsg; 
          msg() << MSG::DEBUG << " RetrieveHLTJets = "     << m_retrieveHLTJets << endmsg; 
          msg() << MSG::DEBUG << " TagHLTJets = "          << m_tagHLTJets << endmsg;
          msg() << MSG::DEBUG << " HistoPrmVtxAtEF = "     << m_histoPrmVtxAtEF << endmsg;
          msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel << endmsg;

          msg() << MSG::VERBOSE << " JetProb 0 MC = "      << m_par_0_MC << endmsg; 
          msg() << MSG::VERBOSE << " JetProb 1 MC = "      << m_par_1_MC << endmsg; 
          msg() << MSG::VERBOSE << " JetProb 0 DT = "      << m_par_0_DT << endmsg; 
          msg() << MSG::VERBOSE << " JetProb 1 DT = "      << m_par_1_DT << endmsg; 

          msg() << MSG::VERBOSE << " SizeIP1D_Grade1 = "  << m_sizeIP1D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " bIP1D_Grade1 = "     << m_bIP1D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " uIP1D_Grade1 = "     << m_uIP1D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " SizeIP2D_Grade1 = "  << m_sizeIP2D_Grade1 << endmsg;
          msg() << MSG::VERBOSE << " bIP2D_Grade1 = "     << m_bIP2D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " uIP2D_Grade1 = "     << m_uIP2D_Grade1 << endmsg;  
          msg() << MSG::VERBOSE << " SizeIP3D_Grade1 = "  << m_sizeIP3D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " bIP3D_Grade1 = "     << m_bIP3D_Grade1 << endmsg; 
          msg() << MSG::VERBOSE << " uIP3D_Grade1 = "     << m_uIP3D_Grade1 << endmsg; 

          msg() << MSG::VERBOSE << " SizeIP1D_Grade2 = "  << m_sizeIP1D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " bIP1D_Grade2 = "     << m_bIP1D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " uIP1D_Grade2 = "     << m_uIP1D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " SizeIP2D_Grade2 = "  << m_sizeIP2D_Grade2 << endmsg;
          msg() << MSG::VERBOSE << " bIP2D_Grade2 = "     << m_bIP2D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " uIP2D_Grade2 = "     << m_uIP2D_Grade2 << endmsg;  
          msg() << MSG::VERBOSE << " SizeIP3D_Grade2 = "  << m_sizeIP3D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " bIP3D_Grade2 = "     << m_bIP3D_Grade2 << endmsg; 
          msg() << MSG::VERBOSE << " uIP3D_Grade2 = "     << m_uIP3D_Grade2 << endmsg; 

          msg() << MSG::DEBUG << " TrkSelGrade1_Chi2 = "            << m_trkSelGrade1_Chi2 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_Innermost = "       << m_trkSelGrade1_Innermost << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_NextToInnermost = " << m_trkSelGrade1_NextToInnermost << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_PixHits = "         << m_trkSelGrade1_PixHits << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_SiHits = "          << m_trkSelGrade1_SiHits << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_D0 = "              << m_trkSelGrade1_D0 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_Z0 = "              << m_trkSelGrade1_Z0 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_Pt = "              << m_trkSelGrade1_Pt << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_Eta = "             << m_trkSelGrade1_Eta << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_Phi = "             << m_trkSelGrade1_Phi << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade1_R = "               << m_trkSelGrade1_R << endmsg; 

          msg() << MSG::DEBUG << " TrkSelGrade2_Chi2 = "            << m_trkSelGrade2_Chi2 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_Innermost = "       << m_trkSelGrade2_Innermost << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_NextToInnermost = " << m_trkSelGrade2_NextToInnermost << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_PixHits = "         << m_trkSelGrade2_PixHits << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_SiHits = "          << m_trkSelGrade2_SiHits << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_D0 = "              << m_trkSelGrade2_D0 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_Z0 = "              << m_trkSelGrade2_Z0 << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_Pt = "              << m_trkSelGrade2_Pt << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_Eta = "             << m_trkSelGrade2_Eta << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_Phi = "             << m_trkSelGrade2_Phi << endmsg; 
          msg() << MSG::DEBUG << " TrkSelGrade2_R = "               << m_trkSelGrade2_R << endmsg; 

          msg() << MSG::VERBOSE << " SizeMVtx = "       << m_sizeMVtx << endmsg; 
          msg() << MSG::VERBOSE << " bMVtx = "          << m_bMVtx << endmsg; 
          msg() << MSG::VERBOSE << " uMVtx = "          << m_uMVtx << endmsg;
          msg() << MSG::VERBOSE << " SizeEVtx = "       << m_sizeEVtx << endmsg; 
          msg() << MSG::VERBOSE << " bEVtx = "          << m_bEVtx << endmsg; 
          msg() << MSG::VERBOSE << " uEVtx = "          << m_uEVtx << endmsg;  
          msg() << MSG::VERBOSE << " SizeNVtx = "       << m_sizeNVtx << endmsg; 
          msg() << MSG::VERBOSE << " bNVtx = "          << m_bNVtx << endmsg; 
          msg() << MSG::VERBOSE << " uNVtx = "          << m_uNVtx << endmsg;
          msg() << MSG::VERBOSE << " SizeSV = "         << m_sizeSV << endmsg; 
          msg() << MSG::VERBOSE << " bSV = "            << m_bSV << endmsg; 
          msg() << MSG::VERBOSE << " uSV = "            << m_uSV << endmsg;  
        }

    m_trigBjetTagger = new TrigBjetTagger(this, msg(), msgLvl());
    m_constTrigBjetTagger  = const_cast<const TrigBjetTagger*>(m_trigBjetTagger);

    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Retrieving tuning likelihoods." << endmsg;

    m_tuningLikelihoodIP1D_Grade1 = new TuningLikelihood(&m_sizeIP1D_Grade1[0], &m_bIP1D_Grade1[0], &m_uIP1D_Grade1[0], m_sizeIP1D_Grade1.size());
    m_trigBjetTagger->fillLikelihoodMap("IP1D_Grade1", m_tuningLikelihoodIP1D_Grade1);
    
    m_tuningLikelihoodIP2D_Grade1 = new TuningLikelihood(&m_sizeIP2D_Grade1[0], &m_bIP2D_Grade1[0], &m_uIP2D_Grade1[0], m_sizeIP2D_Grade1.size());   
    m_trigBjetTagger->fillLikelihoodMap("IP2D_Grade1", m_tuningLikelihoodIP2D_Grade1);

    m_tuningLikelihoodIP3D_Grade1 = new TuningLikelihood(&m_sizeIP3D_Grade1[0], &m_bIP3D_Grade1[0], &m_uIP3D_Grade1[0], m_sizeIP3D_Grade1.size()); 
    m_trigBjetTagger->fillLikelihoodMap("IP3D_Grade1", m_tuningLikelihoodIP3D_Grade1);

    m_tuningLikelihoodIP1D_Grade2 = new TuningLikelihood(&m_sizeIP1D_Grade2[0], &m_bIP1D_Grade2[0], &m_uIP1D_Grade2[0], m_sizeIP1D_Grade2.size());
    m_trigBjetTagger->fillLikelihoodMap("IP1D_Grade2", m_tuningLikelihoodIP1D_Grade2);
    
    m_tuningLikelihoodIP2D_Grade2 = new TuningLikelihood(&m_sizeIP2D_Grade2[0], &m_bIP2D_Grade2[0], &m_uIP2D_Grade2[0], m_sizeIP2D_Grade2.size());
    m_trigBjetTagger->fillLikelihoodMap("IP2D_Grade2", m_tuningLikelihoodIP2D_Grade2);
    
    m_tuningLikelihoodIP3D_Grade2 = new TuningLikelihood(&m_sizeIP3D_Grade2[0], &m_bIP3D_Grade2[0], &m_uIP3D_Grade2[0], m_sizeIP3D_Grade2.size());
    m_trigBjetTagger->fillLikelihoodMap("IP3D_Grade2", m_tuningLikelihoodIP3D_Grade2);

    m_tuningLikelihoodMVtx = new TuningLikelihood(&m_sizeMVtx[0], &m_bMVtx[0], &m_uMVtx[0], m_sizeMVtx.size()); 
    m_trigBjetTagger->fillLikelihoodMap("MVTX", m_tuningLikelihoodMVtx);
    
    m_tuningLikelihoodEVtx = new TuningLikelihood(&m_sizeEVtx[0], &m_bEVtx[0], &m_uEVtx[0], m_sizeEVtx.size()); 
    m_trigBjetTagger->fillLikelihoodMap("EVTX", m_tuningLikelihoodEVtx);

    m_tuningLikelihoodNVtx = new TuningLikelihood(&m_sizeNVtx[0], &m_bNVtx[0], &m_uNVtx[0], m_sizeNVtx.size()); 
    m_trigBjetTagger->fillLikelihoodMap("NVTX", m_tuningLikelihoodNVtx);

    m_tuningLikelihoodSV = new TuningLikelihood(&m_sizeSV[0], &m_bSV[0], &m_uSV[0], m_sizeSV.size()); 
    m_trigBjetTagger->fillLikelihoodMap("SVTX", m_tuningLikelihoodSV);

    // Retrieve TrigTrackJetFinder tool
        StatusCode sc = m_trackJetFinderTool.retrieve();
    if(sc.isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endmsg;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endmsg;
  
    m_trigBjetPrmVtxInfo = new TrigBjetPrmVtxInfo();
    m_trigBjetSecVtxInfo = new TrigBjetSecVtxInfo();
    m_trigBjetJetInfo    = new TrigBjetJetInfo();

    return HLT::OK;
}



//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getCollection(const xAOD::TrackParticleContainer*& pointerToEFTrackCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const xAOD::TrackParticleContainer*> vectorOfEFTrackCollections;

  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFTrackCollections, m_trackKey); 

  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrackParticleContainer from the trigger element" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFTrackCollections.size() << " TrackParticleContainer" << endmsg;
  
  std::vector<const xAOD::TrackParticleContainer*>::iterator pTrackColl    = vectorOfEFTrackCollections.begin();
  std::vector<const xAOD::TrackParticleContainer*>::iterator lastTrackColl = vectorOfEFTrackCollections.end();
  
  if (pTrackColl == lastTrackColl) {
    pointerToEFTrackCollections = 0;
    return HLT::ERROR;
  } 
  else {
    pointerToEFTrackCollections = *pTrackColl;
    return HLT::OK;
  }

}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getPrmVtxCollection(const xAOD::VertexContainer*& pointerToEFPrmVtxCollections, const HLT::TriggerElement* outputTE, std::string priVtxKey) {

  std::vector<const xAOD::VertexContainer*> vectorOfEFPrmVtxCollections;

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Retrieving PV with key = " << priVtxKey << std::endl;
  
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFPrmVtxCollections, priVtxKey);
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get xAOD::VertexContainer from the trigger element" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfEFPrmVtxCollections.size() << " xAOD::VertexContainer" << endmsg;
  
  std::vector<const xAOD::VertexContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const xAOD::VertexContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();
  
  // This part is message only
  if (msgLvl() <= MSG::VERBOSE) {
    
    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
      
      if ((*pPrmVtxColl)->size() != 0)
	      msg() << MSG::VERBOSE << "xAOD::VertexContainer with label " << (*pPrmVtxColl)->front()->vertexType() << endmsg;
    }
    pPrmVtxColl = vectorOfEFPrmVtxCollections.begin();
  }
  
  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Size of pPrmVtxColl = " << (*pPrmVtxColl)->size() << endmsg;
    
    if ((*pPrmVtxColl)->size() != 0) {
      
      if ((*pPrmVtxColl)->front()->vertexType() == xAOD::VxType::PriVtx) { // was == 1
        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << "Selected collection with Primary Vertex label " << (*pPrmVtxColl)->front()->vertexType() << endmsg;
	  msg() << MSG::DEBUG << "First PV has z-position = " <<  (*pPrmVtxColl)->front()->z() << endmsg;
	}
        break;
      }
    }
  }
  
  // Message up to here, or better to say, this selects primary vertex from the collection
  
  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToEFPrmVtxCollections = 0;
    msg() << MSG::DEBUG << "No primary vertex collection found" << endmsg;
    return HLT::ERROR;
  } 
  else {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Found a primary vertex collection." << endmsg;
    }
    pointerToEFPrmVtxCollections = *pPrmVtxColl;
    return HLT::OK;
  }
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::getSecVtxCollection(const Trk::VxSecVertexInfoContainer*& pointerToSecVtxCollections, const HLT::TriggerElement* outputTE) {

  std::vector<const Trk::VxSecVertexInfoContainer*> vectorOfSecVtxCollections;
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfSecVtxCollections, "SecVtxInfo"); 
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get VxSecVertexInfoContainer from the trigger element" << endmsg;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got " << vectorOfSecVtxCollections.size() << " VxSecVertexInfoContainer" << endmsg;

  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator pSecVtxColl    = vectorOfSecVtxCollections.begin();
  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator lastSecVtxColl = vectorOfSecVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pSecVtxColl != lastSecVtxColl; pSecVtxColl++) { 

      if ((*pSecVtxColl)->size() != 0) {
       
        const std::vector<xAOD::Vertex*> vectorOfVxCandidates = (*pSecVtxColl)->front()->vertices();

        if (vectorOfVxCandidates.size()!=0)
          msg() << MSG::VERBOSE << "VxSecVertexInfoContainer with label " << (*vectorOfVxCandidates.begin())->vertexType() << endmsg;
      }
    }

    pSecVtxColl = vectorOfSecVtxCollections.begin();
  }  

  for ( ; pSecVtxColl != lastSecVtxColl; pSecVtxColl++) { 

    if ((*pSecVtxColl)->size() != 0) {

      const std::vector<xAOD::Vertex*> vectorOfVxCandidates = (*pSecVtxColl)->front()->vertices();

      if (vectorOfVxCandidates.size()!=0)
        if ((*vectorOfVxCandidates.begin())->vertexType() == 2) {
         
          if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Selected collection with Secondary Vertex label " << (*vectorOfVxCandidates.begin())->vertexType() << endmsg;
	          msg() << MSG::DEBUG << "First SV has z-position = " <<  (*vectorOfVxCandidates.begin())->z() << endmsg;
	        }
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


  //HLT::ErrorCode TrigBjetFex::getSecVtxInfo(const Trk::VxSecVertexInfoContainer*& pointerToEFSecVtxCollections , const xAOD::VertexContainer*& pointerToEFPrmVtxCollections, const TrigVertexCollection*& pointerToPrmVtxCollections) {

HLT::ErrorCode TrigBjetFex::getSecVtxInfo(const Trk::VxSecVertexInfoContainer*& pointerToEFSecVtxCollections, 
					  const xAOD::VertexContainer*& pointerToEFPrmVtxCollections) {

  if(!pointerToEFSecVtxCollections) {
    msg() << MSG::DEBUG << "No secondary vertex collection sent when extracting sec vtx info" << endmsg;
    return HLT::OK;
  }

  const Trk::VxSecVertexInfo* secVertexInfo = (*pointerToEFSecVtxCollections)[0];
  if(!secVertexInfo) {
    msg() << MSG::DEBUG << "No secondary vertex when extracting sec vtx info" << endmsg;
    return HLT::OK;
  }

  const Trk::VxSecVKalVertexInfo* myVKalSecVertexInfo = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(secVertexInfo);
   
  if(!myVKalSecVertexInfo) {
    msg() << MSG::DEBUG << "The cast to VKal secndary vertex went wrong, the pointer is zero" << endmsg;
    return HLT::OK;
  }
   
  const std::vector<xAOD::Vertex*> & myVertices = myVKalSecVertexInfo->vertices();
   
  if(myVertices.size()>0) {
      
    m_trigBjetSecVtxInfo->setVtxMass(myVKalSecVertexInfo->mass());
    m_trigBjetSecVtxInfo->setEnergyFraction(myVKalSecVertexInfo->energyFraction());
    m_trigBjetSecVtxInfo->setN2TrkVtx(myVKalSecVertexInfo->n2trackvertices());

    int NTracksInSV=0;
    std::vector<xAOD::Vertex*>::const_iterator verticesIt=myVertices.begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd=myVertices.end();
      
    if(myVertices.size()>1) 
      msg() << MSG::DEBUG << "Secondary vertex from InDetVKalVxInJet has more than one vertex, is this ok?" << endmsg;
      
    for( ; verticesIt!=verticesEnd ; ++verticesIt) {

      if(!(*verticesIt)) {
        msg() << MSG::DEBUG << "Secondary vertex from InDetVKalVxInJet has zero pointer. Skipping this vtx.." << endmsg;
        continue;
      }

      msg() << MSG::DEBUG << "VxCandidate at (" 
            << (*verticesIt)->position().x() << "," 
            << (*verticesIt)->position().y() << "," 
            << (*verticesIt)->position().z() << endmsg;

      std::vector<Trk::VxTrackAtVertex> myTracks = (*verticesIt)->vxTrackAtVertex();

      if(myTracks.size() > 0)
        NTracksInSV+=myTracks.size();

    }

    m_trigBjetSecVtxInfo->setNTrksInVtx(NTracksInSV);
      
    //Calculate decay length and significance here
    //Use the same utilities as in InDetVKalVxInJet
    //Some gymnastics needed...ugh...
      
    //bool prmVtxFound=false; // UNUSED
    //      const Trk::RecVertex* pPrmVrt=0;
    const xAOD::Vertex* pPrmVrt=0;

    if(m_histoPrmVtxAtEF) {

      if(pointerToEFPrmVtxCollections) {
	
	const xAOD::Vertex* PrmVtxCand = (*pointerToEFPrmVtxCollections)[0];
	
        if(PrmVtxCand) {
          // const Trk::RecVertex& PrmVrt = PrmVtxCand->recVertex();
          //This is probably wrong, but I have no idea what to put
	  // KL - this line crashes - reverting to what was in -19 tag
          pPrmVrt = PrmVtxCand;
	  //const xAOD::Vertex& pPrmVrt = *PrmVtxCand; 
          //       pPrmVrt = &PrmVrt;
          //prmVtxFound = true; // UNUSED
        }
      }
    }
    //////////////

    if(pPrmVrt) {
         
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Primary vertex for decay length (" 
              << pPrmVrt->position().x() << "," 
              << pPrmVrt->position().y() << "," 
              << pPrmVrt->position().z() << ") and error (" 
              << pPrmVrt->covariancePosition()(0,0) << "," 
              << pPrmVrt->covariancePosition()(1,1) << "," 
              << pPrmVrt->covariancePosition()(2,2) << ")" << endmsg; 
      }
	
      // Needs some Eigen migration magic here
      //CLHEP::HepLorentzVector jetDirection;
      TVector3 jetDirection;

      if (m_useJetDirection == 1) {
        jetDirection.SetX(cos(m_trigBjetJetInfo->phiJet()));
        jetDirection.SetY(sin(m_trigBjetJetInfo->phiJet()));
        jetDirection.SetZ(sinh(m_trigBjetJetInfo->etaJet()));
      } else if (m_useJetDirection == 2) {
        jetDirection.SetX(cos(m_trigBjetJetInfo->phiTrkJet()));
        jetDirection.SetY(sin(m_trigBjetJetInfo->phiTrkJet()));
        jetDirection.SetZ(sinh(m_trigBjetJetInfo->etaTrkJet()));
      } else if (m_useJetDirection == 3) {
        jetDirection.SetX(cos(m_trigBjetJetInfo->phiRoI()));
        jetDirection.SetY(sin(m_trigBjetJetInfo->phiRoI()));
        jetDirection.SetZ(sinh(m_trigBjetJetInfo->etaRoI()));
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
	 
      const xAOD::Vertex& PrmVrt = *pPrmVrt;
         
      //Calcualte the jet-vrt direction for the signed decay length calculation
      double JetVrtDir =
        jetDirection.Px()*(SecVrt.x()-PrmVrt.position().x())
        + jetDirection.Py()*(SecVrt.y()-PrmVrt.position().y())
        + jetDirection.Pz()*(SecVrt.z()-PrmVrt.position().z());
         
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
          msg() << MSG::DEBUG << "decay length distance do not match among tools: tool " << dist3D << " manual " << distance << endmsg;
        }
      }
         
      if(msgLvl() <= MSG::DEBUG) {
        double dist2D = (SecVrt - PrmVrt.position()).perp();            
        msg() << MSG::DEBUG << "Calculating secondary vertex decay length with primary vertex at (" 
              << PrmVrt.position().x() << "," << PrmVrt.position().y() << "," << PrmVrt.position().z() << ") and sec vtx at ("
              << SecVrt.x() << "," << SecVrt.y() << "," << SecVrt.z() 
              << ") and jet direction (px,py,pz) = ("
              << jetDirection.Px() << "," << jetDirection.Py() << "," << jetDirection.Pz() <<  
          ") which gives 3D decay length " << distance << " (VrtVrtDist tool " << dist3D << ") and 3D significance " << sign3D 
              << " and 2D(R/phi) decay length " << dist2D << endmsg;
      }

      //Clean up if I created a vertex object earlier which is not pointer to collection
      //if(m_histoPrmVtxAtEF && pPrmVrt)
      //  delete pPrmVrt;
    } 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigBjetFex::trackSel(const xAOD::TrackParticle*& track, unsigned int i, int grade,
			   float trkSelChi2, int trkSelInnermost, int trkSelNextToInnermost, int trkSelPixHits, int trkSelSiHits, float trkSelD0, float trkSelZ0, float trkSelPt,
			   float trkSelEta, float trkSelPhi, float trkSelR) {

  float zv = m_trigBjetPrmVtxInfo->zPrmVtx();

  //const Trk::TrackSummary *summary = track->trackSummary();
  //const Trk::FitQuality *quality   = track->fitQuality();

  //uint8_t nBlayerHits = 0;
  uint8_t nInnermostHits = 0;
  uint8_t nNextToInnermostHits = 0;
  uint8_t nPixHits    = 0;  
  uint8_t nSCTHits    = 0; 

  //track->summaryValue(nBlayerHits,          xAOD::numberOfInnermostPixelLayerHits);
  track->summaryValue(nInnermostHits,       xAOD::numberOfInnermostPixelLayerHits);
  track->summaryValue(nNextToInnermostHits, xAOD::numberOfNextToInnermostPixelLayerHits);
  track->summaryValue(nPixHits,             xAOD::numberOfPixelHits);
  track->summaryValue(nSCTHits,             xAOD::numberOfSCTHits);

  int   nSiHits = nPixHits + nSCTHits; //summary->get(Trk::numberOfPixelHits)+summary->get(Trk::numberOfSCTHits);
  float theta   = track->theta();
  float qOverPt = track->qOverP()/TMath::Sin(theta); 
  float pT      = (1/qOverPt);
  float d0      = track->d0();
  float z0      = track->z0() + m_trigBjetPrmVtxInfo->zBeamSpot();

  float dEta = fabs(track->eta() - m_trigBjetJetInfo->etaRoI());
  float dPhi = fabs(m_taggerHelper->phiCorr(m_taggerHelper->phiCorr(track->phi()) - m_trigBjetJetInfo->phiRoI()));
  float dR   = sqrt(dEta*dEta+dPhi*dPhi);

  // FIX FOR REDEFINED IP REFERENCE (ATR-9051)
  //m_taggerHelper->IPCorr(track->measuredPerigee()->parameters()[Trk::d0], track->measuredPerigee()->parameters()[Trk::z0], 
  //                     d0,z0,track->phi(), track->eta(), pT, m_trigBjetPrmVtxInfo->xBeamSpot(), m_trigBjetPrmVtxInfo->yBeamSpot()); 
  //d0=track->measuredPerigee()->parameters()[Trk::d0]; // THIS LINE WAS DOING NOTHING!
  //z0=track->measuredPerigee()->parameters()[Trk::z0]; // THIS LINE WAS DOING NOTHING! //+m_trigBjetPrmVtxInfo->zBeamSpot(); 
  // END FIX 

  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "TrackSel method (grade " << grade << ")" << endmsg;
    msg() << MSG::VERBOSE << "  Track number "           << i+1  << " to be selected must be:" << endmsg;
    msg() << MSG::VERBOSE << "    dEta "                 << dEta                          << " <= " << trkSelEta << endmsg;
    msg() << MSG::VERBOSE << "    dPhi "                 << dPhi                          << " <= " << trkSelPhi << endmsg;
    msg() << MSG::VERBOSE << "    dR "                   << dR                            << " <= " << trkSelR << endmsg;
    msg() << MSG::VERBOSE << "    pt "                   << fabs(pT)                      << " >= " << trkSelPt << endmsg;
    msg() << MSG::VERBOSE << "    d0 "                   << fabs(d0)                      << " <= " << trkSelD0 << endmsg;
    msg() << MSG::VERBOSE << "    z0*sin(theta) "        << fabs(z0-zv)*TMath::Sin(theta) << " <= " << trkSelZ0 << endmsg;
    msg() << MSG::VERBOSE << "    innermostLayer "       << (int)nInnermostHits           << " >= " << trkSelInnermost << endmsg;
    msg() << MSG::VERBOSE << "    nextToInnermostLayer " << (int)nNextToInnermostHits     << " >= " << trkSelNextToInnermost << endmsg;
    msg() << MSG::VERBOSE << "    pixelHit "             << (int)nPixHits                 << " >= " << trkSelPixHits << endmsg;
    msg() << MSG::VERBOSE << "    siHit "                << (int)nSiHits                  << " >= " << trkSelSiHits << endmsg;
    msg() << MSG::VERBOSE << "    prob(chi2) "           << TMath::Prob(track->chiSquared(), (int)nSiHits*3-5) << " > " << trkSelChi2 << endmsg;
  }
  
  if (dEta > trkSelEta) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " is not selected (delta eta matching)" << endmsg;
    m_listCutApplied.push_back(2); return false;
  }

  if (dPhi > trkSelPhi) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " is not selected (delta phi matching)" << endmsg;
    m_listCutApplied.push_back(3); return false;
  }

  if (dR > trkSelR) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " is not selected (delta R matching)" << endmsg;
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(pT) < trkSelPt) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (pT cut)" << endmsg;
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(d0) > trkSelD0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (d0 cut)" << endmsg;
    m_listCutApplied.push_back(6); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > trkSelZ0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (z0 cut)" << endmsg;
    m_listCutApplied.push_back(7); return false;
  }

  if (nInnermostHits < trkSelInnermost) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (missing innermost layer hit)" << endmsg;
    m_listCutApplied.push_back(8); return false;
  }

  if (nNextToInnermostHits < trkSelNextToInnermost) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (missing next to innermost layer hit)" << endmsg;
    m_listCutApplied.push_back(9); return false;
  }

  if (nPixHits < trkSelPixHits) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (too few pixel hits)" << endmsg;
    m_listCutApplied.push_back(10); return false;
  }

  if (nSiHits < trkSelSiHits) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (too few silicon hits)" << endmsg;
    m_listCutApplied.push_back(11); return false;
  }

  if (TMath::Prob(track->chiSquared(), (int)nSiHits*3-5) <= trkSelChi2) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  grade " << grade << " track " << i+1 << " not selected (chi2 cut)" << endmsg;
    m_listCutApplied.push_back(12); return false;
  }
  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    grade " << grade << " track " << i+1 << " is selected" << endmsg;

  m_listCutApplied.push_back(13);
  return true;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE) {
  ATH_MSG_DEBUG( "Executing TrigBjetFex" );

  m_totSelTracks = 0;
  m_totTracks    = 0;

  m_trigBjetPrmVtxInfo->clear();
  m_trigBjetSecVtxInfo->clear();
  m_trigBjetJetInfo->clear();
  std::vector<TrigBjetTrackInfo> trigBjetTrackInfoVector;
  m_trigBjetTrackInfoVector = &trigBjetTrackInfoVector;

  // -----------------------------------
  // Get RoI descriptor
  // -----------------------------------
  const TrigRoiDescriptor* roiDescriptor = 0;

  if ((getFeature(outputTE, roiDescriptor, m_jetKey) == HLT::OK) && roiDescriptor) {
    if (msgLvl() <= MSG::DEBUG && roiDescriptor != 0) {
      msg() << MSG::DEBUG << "Using TE: " << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi() << ", Eta = " << roiDescriptor->eta() << endmsg;
    }
  }
  else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "No feature for this Trigger Element" << endmsg;
    
    return HLT::NAV_ERROR;
  }

  //xAOD jets from TE

  float et_EFjet = 0;
 
  msg() << MSG::DEBUG << "pass 1 " << et_EFjet << endmsg;

  const xAOD::JetContainer* jets(0);
  //HLT::ErrorCode ec = getFeature(outputTE, jets,"EFJet");
  HLT::ErrorCode ec = getFeature(outputTE, jets, m_jetKey);

  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } 
  else {
    msg() << MSG::DEBUG << "Obtained JetContainer with name = " << m_jetKey << endmsg;
  }

  msg() << MSG::DEBUG << "pass 2 " << &jets << endmsg;

  if(jets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    return HLT::ERROR;
  }

  std::vector<const xAOD::Jet*> theJets(jets->begin(), jets->end());

  msg() << MSG::DEBUG << "pass 2 jet size: " << theJets.size() << endmsg;

  std::size_t njets = theJets.size();
  if( njets == 0 ){
    ATH_MSG_DEBUG("JetCollection is empty");
    return HLT::OK;
  } else {
    ATH_MSG_DEBUG("JetCollection contains " << njets <<"jets");
  }

  if(njets > 1)
    ATH_MSG_DEBUG("Something is wrong, it should not be more than one jet");

  for (const xAOD::Jet* aJet : theJets) {
    double etjet = aJet->p4().Et();
    double etajet = aJet->p4().Eta();

    et_EFjet = aJet->p4().Et()*0.001;

    msg() << MSG::DEBUG << "et  " << etjet << " and eta " << etajet << " Et EFjet " << et_EFjet << endmsg;

  }

    m_trigBjetJetInfo->setEtaPhiJet(roiDescriptor->eta(), m_taggerHelper->phiCorr(roiDescriptor->phi()));
    m_trigBjetJetInfo->setEtaPhiRoI(roiDescriptor->eta(), m_taggerHelper->phiCorr(roiDescriptor->phi()));
    m_trigBjetJetInfo->setEtJet(et_EFjet);

  // -----------------------------------
  // Retrieve beamspot information
  // -----------------------------------
  IBeamCondSvc* iBeamCondSvc; 
  StatusCode sc = service("BeamCondSvc", iBeamCondSvc);

  if (sc.isFailure() || iBeamCondSvc == 0) {
    iBeamCondSvc = 0;

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;

  } 
  else {
    
    Amg::Vector3D beamSpot = iBeamCondSvc->beamPos();
    
    int beamSpotBitMap = iBeamCondSvc->beamStatus();
    
    // Check if beam spot is from online algorithms
    int beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);
    
    // Check if beam spot fit converged
    if (beamSpotStatus)
      beamSpotStatus = ((beamSpotBitMap & 0x3) == 0x3);
    
    if(msgLvl() <= MSG::DEBUG)
      
      msg() << MSG::DEBUG << "Beam spot from service: x=" << beamSpot.x() << ", y=" << beamSpot.y() << ", z=" << beamSpot.z() 
            << ", tiltXZ=" << iBeamCondSvc->beamTilt(0) << ", tiltYZ=" << iBeamCondSvc->beamTilt(1) 
            << ", sigmaX=" << iBeamCondSvc->beamSigma(0) << ", sigmaY=" << iBeamCondSvc->beamSigma(1) << ", sigmaZ=" << iBeamCondSvc->beamSigma(2) 
            << ", status=" << beamSpotStatus << endmsg;

    m_trigBjetPrmVtxInfo->setBeamSpot(beamSpot.x(), beamSpot.y(), beamSpot.z());
    m_trigBjetPrmVtxInfo->setBeamSpotTilt(iBeamCondSvc->beamTilt(0), iBeamCondSvc->beamTilt(1));
    m_trigBjetPrmVtxInfo->setBeamSpotWidth(iBeamCondSvc->beamSigma(0), iBeamCondSvc->beamSigma(1), iBeamCondSvc->beamSigma(2));
    m_trigBjetPrmVtxInfo->setBeamSpotStatus(beamSpotStatus);

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endmsg;

  }

  // -----------------------------------
  // Create collections
  // -----------------------------------
  m_trigEFBjetColl = new TrigEFBjetContainer();

  xAOD::BTaggingAuxContainer trigBjetAuxContainer;
  m_trigBTaggingContainer = new xAOD::BTaggingContainer();
  m_trigBTaggingContainer->setStore(&trigBjetAuxContainer);
 
  // Create pointers to collections
  const xAOD::TrackParticleContainer* pointerToEFTrackCollections = 0;

  // Create pointers to TrigVertex collection
  //const TrigVertexCollection*          pointerToPrmVtxCollections = 0;
  const xAOD::VertexContainer*          pointerToEFPrmVtxCollections = 0;
  const Trk::VxSecVertexInfoContainer*  pointerToEFSecVtxCollections = 0;

  // -----------------------------------
  // Get EF track collection 
  // -----------------------------------
  HLT::ErrorCode status = getFeature(outputTE, pointerToEFTrackCollections, m_trackKey);


  if (status != HLT::OK) {
    msg() << MSG::DEBUG << "No HLT track collection retrieved" << endmsg;
  } else if (pointerToEFTrackCollections == nullptr) {
    msg() << MSG::DEBUG << "Null pointer to HLT track collection" << endmsg;
  } else if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "HLT track collection retrieved with size = " << pointerToEFTrackCollections->size() << endmsg;
  }

  // -----------------------------------
  // Get secondary vertex collection 
  // -----------------------------------
  if (getSecVtxCollection(pointerToEFSecVtxCollections, outputTE) != HLT::OK) {
    msg() << MSG::DEBUG << "No secondary vertex collection retrieved" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Secondary vertex collection retrieved" << endmsg;
  
  // -----------------------------------
  // Get primary vertex collection
  // -----------------------------------

  float xPrmVtx=0, yPrmVtx=0, zPrmVtx=0;

  if (m_histoPrmVtxAtEF) {   // PV from TrigT2HistoPrmVtx

    if (getPrmVtxCollection(pointerToEFPrmVtxCollections, outputTE, m_priVtxKey) != HLT::OK) {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved with name " << m_priVtxKey << endmsg;
      // If the ID PV-finding fails then use the PV from T2HistoPrmVtx instead
      // This is not ideal... investigate why ID PV finding fails
      if (m_priVtxKey == "xPrimVx" && getPrmVtxCollection(pointerToEFPrmVtxCollections, outputTE, "EFHistoPrmVtx") != HLT::OK) {
	      msg() << MSG::DEBUG << "No primary vertex collection retrieved with name EFHistoPrmVtx either..." << endmsg;
      }
      else if (msgLvl() <= MSG::DEBUG) {
	      msg() << MSG::DEBUG << "Didn't manage to find " << m_priVtxKey << " PV, so using EFHistoPrmVtx instead." << endmsg;
      }
    } 
    else if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Primary vertex collection retrieved" << endmsg;
    }

    // Protect against null pointers
    if(pointerToEFPrmVtxCollections) {
      // Protect against empty vectors
      if(pointerToEFPrmVtxCollections->size()>0) {
	      const xAOD::Vertex* prmVertex = (*pointerToEFPrmVtxCollections)[0];
        zPrmVtx = prmVertex->z();
      } else {
        msg() << MSG::DEBUG << "Empty primary vertex collection retrieved" << endmsg;
        zPrmVtx = 0;
      }
    } else {
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endmsg;
      zPrmVtx = 0;
    }
  } else  {  // PV from ID tracking

    // Protect against null pointers
    //
    /** pointerToEFPrmVtxCollections is necessarily NULL at this point, from line 989
    // and the fact that m_histoPrmVtxAtEF is false in this logic branch, so the pointer
    // was not modified from NULL.
    //
    if(pointerToEFPrmVtxCollections) {

      // Protect against empty vectors
      if(pointerToEFPrmVtxCollections->size()>0) {

        const xAOD::Vertex* prmVertex = (*pointerToEFPrmVtxCollections)[0];
        xPrmVtx = (float)prmVertex->x();
        yPrmVtx = (float)prmVertex->y();
        zPrmVtx = (float)prmVertex->z();
      } 
      else {
        msg() << MSG::DEBUG << "Empty primary vertex collection retrieved" << endmsg;
        xPrmVtx = 0;
        yPrmVtx = 0;
        zPrmVtx = 0;
      }
    } 
    else 
    **/{
      msg() << MSG::DEBUG << "No primary vertex collection retrieved" << endmsg;
      xPrmVtx = 0;
      yPrmVtx = 0;
      zPrmVtx = 0;
    }
  }

  // -----------------------------------
  // Apply beam spot correction for tilt
  // ----------------------------------- 
    
  float xBeamSpot = m_trigBjetPrmVtxInfo->xBeamSpot() + tan(m_trigBjetPrmVtxInfo->xBeamSpotTilt()) * (zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float yBeamSpot = m_trigBjetPrmVtxInfo->yBeamSpot() + tan(m_trigBjetPrmVtxInfo->yBeamSpotTilt()) * (zPrmVtx-m_trigBjetPrmVtxInfo->zBeamSpot());
  float zBeamSpot = m_trigBjetPrmVtxInfo->zBeamSpot();

  m_trigBjetPrmVtxInfo->setBeamSpot(xBeamSpot, yBeamSpot, zBeamSpot);
  m_trigBjetPrmVtxInfo->setPrmVtx(xPrmVtx, yPrmVtx, zPrmVtx);

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetPrmVtxInfo << endmsg;

  m_trackJetFinderTool->clear();
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() << endmsg;
  m_trackJetFinderTool->inputPrimaryVertexZ(m_trigBjetPrmVtxInfo->zPrmVtx());
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Done set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() << endmsg;

  m_totTracks = m_taggerHelper->getTrackNumber(pointerToEFTrackCollections);
  
  for (unsigned int j = 0; j < m_totTracks; j++) {
    
    const xAOD::TrackParticle* track = pointerToEFTrackCollections == nullptr ? nullptr : (*pointerToEFTrackCollections)[j];
    if (track == nullptr) continue;

    m_mon_trk_a0.push_back(track->d0());
    m_mon_trk_z0.push_back(track->z0() + m_trigBjetPrmVtxInfo->zBeamSpot());
    
    bool passGrade1 = trackSel(track, j, 1, m_trkSelGrade1_Chi2, m_trkSelGrade1_Innermost, m_trkSelGrade1_NextToInnermost, m_trkSelGrade1_PixHits, m_trkSelGrade1_SiHits,
			       m_trkSelGrade1_D0, m_trkSelGrade1_Z0, m_trkSelGrade1_Pt, m_trkSelGrade1_Eta, m_trkSelGrade1_Phi, m_trkSelGrade1_R);

    bool passGrade2 = trackSel(track, j, 2, m_trkSelGrade2_Chi2, m_trkSelGrade2_Innermost, m_trkSelGrade2_NextToInnermost, m_trkSelGrade2_PixHits, m_trkSelGrade2_SiHits,
			       m_trkSelGrade2_D0, m_trkSelGrade2_Z0, m_trkSelGrade2_Pt, m_trkSelGrade2_Eta, m_trkSelGrade2_Phi, m_trkSelGrade2_R);

    if (passGrade1 || (m_useGrading && passGrade2)) {
      m_totSelTracks++;
      TrigBjetTrackInfo trigBjetTrackInfo(track);

      if(m_useGrading && passGrade2) trigBjetTrackInfo.setGrade(2);
      if(passGrade1) trigBjetTrackInfo.setGrade(1);
     
      float d0Corr=0, z0Corr=0;
      d0Corr=track->d0(); 
      z0Corr=track->z0()+m_trigBjetPrmVtxInfo->zBeamSpot();
      trigBjetTrackInfo.setIPCorr(d0Corr, z0Corr);
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "  " << trigBjetTrackInfo << endmsg;
      trigBjetTrackInfoVector.push_back(trigBjetTrackInfo);

      //m_trackJetFinderTool->addTrack(track, j);
    }
  }

  std::vector<int> tracksTrackJet;
  float etaTrackJet, phiTrackJet;
  
  m_trackJetFinderTool->findJet(tracksTrackJet, etaTrackJet, phiTrackJet);

  if (etaTrackJet != -99 && phiTrackJet != -99) {
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(etaTrackJet, phiTrackJet);
    
  } 
  else {
    
    m_trigBjetJetInfo->setEtaPhiTrkJet(m_trigBjetJetInfo->etaRoI(), m_trigBjetJetInfo->phiRoI());
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "eta Jet = eta RoI" << endmsg;
  }
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << *m_trigBjetJetInfo << endmsg;

  // -----------------------------------
  // For monitoring
  // -----------------------------------
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
      msg() << MSG::DEBUG << "Beam spot status flag set to " << m_trigBjetPrmVtxInfo->beamSpotStatus() << ". Discriminant weights are not computed." << endmsg;
      msg() << MSG::DEBUG << "Beam spot flag set to " << m_useBeamSpotFlag << ". Discriminant weights are not computed." << endmsg;
    }

    m_listCutApplied.push_back(0);
    m_trigBjetTagger->getWeights();

  } else if (m_trigBjetPrmVtxInfo->xBeamSpotWidth()>m_setBeamSpotWidth || m_trigBjetPrmVtxInfo->yBeamSpotWidth()>m_setBeamSpotWidth) {

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Beam spot width is more than " << m_setBeamSpotWidth << "um. Discriminant weights are not computed." << endmsg;

    m_listCutApplied.push_back(1);
    m_trigBjetTagger->getWeights();

  } else {
    
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Computing discriminant weights using taggers: " << m_taggers 
            << " and using calibration from " << (m_useParamFromData==0 ? "MC" : "data") << " for CHI2" << endmsg;

    //Look for a sec vertex?
    bool retrieveSV = false;
    for ( ; pTagger != lastTagger; pTagger++) {
      if ((*pTagger).find("VTX") != std::string::npos) {
        retrieveSV = true;
        break;
      }
    }
    
    if(retrieveSV) {
       
      // Get secondary vertex information at EF from TrigBjetFex::getSecVtxInfo
      //if (getSecVtxInfo(pointerToEFSecVtxCollections, pointerToEFPrmVtxCollections, pointerToPrmVtxCollections) != HLT::OK)
      if (getSecVtxInfo(pointerToEFSecVtxCollections, pointerToEFPrmVtxCollections) != HLT::OK)
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "No EF SV information retrieved from TrigBjetFex::getSecVtxInfo" << endmsg;
      
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << *m_trigBjetSecVtxInfo << endmsg;
      
    }

    m_trigBjetTagger->getWeights(m_trigBjetTrackInfoVector, m_trigBjetPrmVtxInfo, m_trigBjetSecVtxInfo, m_trigBjetJetInfo);
  }
  
  if (msgLvl() <= MSG::DEBUG) {
     
    const EventInfo* pEventInfo;
     
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
      msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
    } 
    else {
      msg() << MSG::DEBUG << "Bjet slice summary (Run " << pEventInfo->event_ID()->run_number() 
            << "; Event " << pEventInfo->event_ID()->event_number() << ")" << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId()
            << ", Phi = "   << roiDescriptor->phi()
            << ", Eta = "   << roiDescriptor->eta() << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  Tracks: " << m_totTracks << " reconstructed and " << m_totSelTracks <<" selected" << endmsg;
      
      if (pointerToEFPrmVtxCollections) {
        msg() << MSG::DEBUG << "REGTEST:  Primary vertex: " << pointerToEFPrmVtxCollections->size() << " reconstructed"
              << ", (x,y,z) = (" << m_trigBjetPrmVtxInfo->xPrmVtx() << "," << m_trigBjetPrmVtxInfo->yPrmVtx() << "," << m_trigBjetPrmVtxInfo->zPrmVtx() << ")" << endmsg;
      }
      if (pointerToEFSecVtxCollections)
        msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: " << pointerToEFSecVtxCollections->size() << " reconstructed" << endmsg;
      else
        msg() << MSG::DEBUG << "REGTEST:  Secondary vertex: 0 reconstructed" << endmsg;
      
      msg() << MSG::DEBUG << "REGTEST:  SV L/sigma(L) " <<  m_trigBjetSecVtxInfo->decayLengthSignificance() << "  SV mass " <<  m_trigBjetSecVtxInfo->vtxMass() 
            << "  SV efrac " <<  m_trigBjetSecVtxInfo->energyFraction() << "   SV 2-track vertex multiplicity " << m_trigBjetSecVtxInfo->n2TrkVtx() << endmsg; 
      
      msg() << MSG::DEBUG << "REGTEST: List weights stored probability and likelihood objects:" << endmsg;

      pTagger = m_taggers.begin();
      for ( ; pTagger != lastTagger; pTagger++)
        msg() << MSG::DEBUG << "REGTEST:  X(" << (*pTagger) << ") = " << m_trigBjetTagger->taggersXMap((*pTagger))  << endmsg;

    }
  }

  // -----------------------------------
  // Create TrigEFBjet and attach feature
  // -----------------------------------
  TrigEFBjet* trigEFBjet = new TrigEFBjet(roiDescriptor->roiId(), m_trigBjetJetInfo->etaJet(), m_trigBjetJetInfo->phiJet(),
					  0, 0, 0, m_trigBjetPrmVtxInfo->zPrmVtx(), m_trigBjetJetInfo->etJet(),
					  m_trigBjetTagger->taggersXMap("COMB"),  m_trigBjetTagger->taggersXMap("IP1D"), 
					  m_trigBjetTagger->taggersXMap("IP2D"),  m_trigBjetTagger->taggersXMap("IP3D"), 
					  m_trigBjetTagger->taggersXMap("CHI2"), 
					  m_trigBjetSecVtxInfo->decayLengthSignificance(), m_trigBjetSecVtxInfo->vtxMass(), 
					  m_trigBjetSecVtxInfo->energyFraction(), m_trigBjetSecVtxInfo->n2TrkVtx()); 
  
  
  if (!m_trigEFBjetColl) {
    msg() << MSG::ERROR << "Feature TrigEFBjetContainer not found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  trigEFBjet->validate(true);
  m_trigEFBjetColl->push_back(trigEFBjet);
  
  HLT::ErrorCode stat = attachFeature(outputTE, m_trigEFBjetColl, "EFBjetFex");
  
  if (stat != HLT::OK) {
    if ( msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Failed to attach TrigEFBjetContainer to navigation" << endmsg;
    return stat;
  }

  // -----------------------------------
  // Create xAOD::BTagging and attach feature
  // -----------------------------------
  if (!m_trigBTaggingContainer) {
    msg() << MSG::ERROR << "Feature BTaggingContainer not found" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  xAOD::BTagging * newBTag = new xAOD::BTagging();

  m_trigBTaggingContainer->push_back(newBTag);

  // Set results
  newBTag->setSV1_pu(m_trigBjetTagger->taggersPuMap("MVTX")*m_trigBjetTagger->taggersPuMap("NVTX")*m_trigBjetTagger->taggersPuMap("EVTX"));
  newBTag->setSV1_pb(m_trigBjetTagger->taggersPbMap("MVTX")*m_trigBjetTagger->taggersPbMap("NVTX")*m_trigBjetTagger->taggersPbMap("EVTX"));

  newBTag->setIP2D_pu(m_trigBjetTagger->taggersPuMap("IP2D"));
  newBTag->setIP2D_pb(m_trigBjetTagger->taggersPbMap("IP2D"));

  newBTag->setIP3D_pu(m_trigBjetTagger->taggersPuMap("IP3D"));
  newBTag->setIP3D_pb(m_trigBjetTagger->taggersPbMap("IP3D"));

  // Set additional SV1 variables
  newBTag->setVariable<float>("SV1", "masssvx",         m_trigBjetSecVtxInfo->vtxMass());
  newBTag->setVariable<float>("SV1", "efracsvx",        m_trigBjetSecVtxInfo->energyFraction());
  newBTag->setVariable<int>  ("SV1", "N2Tpair",         m_trigBjetSecVtxInfo->n2TrkVtx());
  newBTag->setVariable<float>("SV1", "significance3d",  m_trigBjetSecVtxInfo->decayLengthSignificance());

  // Create element links to the TrackParticles and to the Jet
  auto& trkAssociationLinks = newBTag->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >("BTagTrackToJetAssociator");
  if(pointerToEFTrackCollections) {
    trkAssociationLinks.resize (pointerToEFTrackCollections->size());
    for(size_t i = 0; i < pointerToEFTrackCollections->size(); ++i) {
      trkAssociationLinks[i].toIndexedElement (*pointerToEFTrackCollections, i);
    }
  }
  //auto& jetAssociationLinks = newBTag->auxdata<std::vector<ElementLink<xAOD::JetContainer> > >("BTagBtagToJetAssociator");
  auto& jetAssociationLinks = newBTag->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("BTagBtagToJetAssociator");
  if(jets) {
    jetAssociationLinks.resize (jets->size());
    for(size_t i = 0; i < jets->size(); ++i) {
      jetAssociationLinks[i].toIndexedElement (*jets, i);
    }
  }

  // Fill monitoring variables
  newBTag->variable<float>("SV1", "masssvx",  m_mon_sv_mass);
  newBTag->variable<float>("SV1", "efracsvx", m_mon_sv_evtx);
  newBTag->variable<int>  ("SV1", "N2Tpair",  m_mon_sv_nvtx);

  m_mon_tag_IP2D    = newBTag->IP2D_loglikelihoodratio();
  m_mon_tag_IP3D    = newBTag->IP3D_loglikelihoodratio();
  m_mon_tag_SV1     = newBTag->SV1_loglikelihoodratio();
  m_mon_tag_IP3DSV1 = newBTag->SV1plusIP3D_discriminant();

//  std::cout << "TEMP-BJETMON " << m_mon_sv_mass << " / "  << m_mon_sv_evtx << " / "  << m_mon_sv_nvtx << " / " 
//	    << m_mon_tag_IP2D << " / "  << m_mon_tag_IP3D << " / "  << m_mon_tag_SV1 << " / " << m_mon_tag_IP3DSV1 << std::endl;


  msg() << MSG::DEBUG << "IP2D u/b: " << m_trigBjetTagger->taggersPuMap("IP2D") << "/" << m_trigBjetTagger->taggersPbMap("IP2D")
	<< "   IP3D u/b: " << m_trigBjetTagger->taggersPuMap("IP3D") << "/" << m_trigBjetTagger->taggersPbMap("IP3D")
	<< "   SV1 u/b: " << m_trigBjetTagger->taggersPuMap("MVTX")*m_trigBjetTagger->taggersPuMap("NVTX")*m_trigBjetTagger->taggersPuMap("EVTX") << "/"
	<< m_trigBjetTagger->taggersPbMap("MVTX")*m_trigBjetTagger->taggersPbMap("NVTX")*m_trigBjetTagger->taggersPbMap("EVTX") << endmsg;

  
  stat = attachFeature(outputTE, m_trigBTaggingContainer, "HLTBjetFex");
  
  if (stat != HLT::OK) {
    if ( msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Failed to attach BTaggingContainer to navigation" << endmsg;
    return stat;
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigBjetFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigBjetFex" << endmsg;

  return HLT::OK;
}




//  LocalWords:  endmsg
