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
      ATH_MSG_DEBUG( "declareProperty review:" );

      ATH_MSG_DEBUG( "JetKey = "               << m_jetKey );
      ATH_MSG_DEBUG( "PriVtxKey = "            << m_priVtxKey );
      
      ATH_MSG_DEBUG( " AlgoId = "              << m_algo );
      ATH_MSG_DEBUG( " Instance = "            << m_instance );
      
      ATH_MSG_DEBUG( " UseBeamSpotFlag = "     << m_useBeamSpotFlag );
      ATH_MSG_DEBUG( " SetBeamSpotWidth = "    << m_setBeamSpotWidth );
      
      ATH_MSG_DEBUG( " UseParamFromData = "    << m_useParamFromData );
      
      ATH_MSG_DEBUG( " Taggers = "             << m_taggers );
      ATH_MSG_DEBUG( " UseErrIPParam = "       << m_useErrIPParam );
      ATH_MSG_DEBUG( " UseJetDirection = "     << m_useJetDirection );
      ATH_MSG_DEBUG( " RetrieveHLTJets = "     << m_retrieveHLTJets );
      ATH_MSG_DEBUG( " TagHLTJets = "          << m_tagHLTJets );
      ATH_MSG_DEBUG( " HistoPrmVtxAtEF = "     << m_histoPrmVtxAtEF );
      ATH_MSG_DEBUG(  " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel );
      
      ATH_MSG_VERBOSE( " JetProb 0 MC = "      << m_par_0_MC );
      ATH_MSG_VERBOSE( " JetProb 1 MC = "      << m_par_1_MC );
      ATH_MSG_VERBOSE( " JetProb 0 DT = "      << m_par_0_DT );
      ATH_MSG_VERBOSE( " JetProb 1 DT = "      << m_par_1_DT );
      
      ATH_MSG_VERBOSE( " SizeIP1D_Grade1 = "  << m_sizeIP1D_Grade1 );
      ATH_MSG_VERBOSE( " bIP1D_Grade1 = "     << m_bIP1D_Grade1    );
      ATH_MSG_VERBOSE( " uIP1D_Grade1 = "     << m_uIP1D_Grade1    );
      ATH_MSG_VERBOSE( " SizeIP2D_Grade1 = "  << m_sizeIP2D_Grade1 );
      ATH_MSG_VERBOSE( " bIP2D_Grade1 = "     << m_bIP2D_Grade1    );
      ATH_MSG_VERBOSE( " uIP2D_Grade1 = "     << m_uIP2D_Grade1    );
      ATH_MSG_VERBOSE( " SizeIP3D_Grade1 = "  << m_sizeIP3D_Grade1 );
      ATH_MSG_VERBOSE( " bIP3D_Grade1 = "     << m_bIP3D_Grade1    );
      ATH_MSG_VERBOSE( " uIP3D_Grade1 = "     << m_uIP3D_Grade1    );
      
      ATH_MSG_VERBOSE( " SizeIP1D_Grade2 = "  << m_sizeIP1D_Grade2 );
      ATH_MSG_VERBOSE( " bIP1D_Grade2 = "     << m_bIP1D_Grade2 );
      ATH_MSG_VERBOSE( " uIP1D_Grade2 = "     << m_uIP1D_Grade2 );
      ATH_MSG_VERBOSE( " SizeIP2D_Grade2 = "  << m_sizeIP2D_Grade2 );
      ATH_MSG_VERBOSE( " bIP2D_Grade2 = "     << m_bIP2D_Grade2 );
      ATH_MSG_VERBOSE( " uIP2D_Grade2 = "     << m_uIP2D_Grade2 );
      ATH_MSG_VERBOSE( " SizeIP3D_Grade2 = "  << m_sizeIP3D_Grade2 );
      ATH_MSG_VERBOSE( " bIP3D_Grade2 = "     << m_bIP3D_Grade2 );
      ATH_MSG_VERBOSE( " uIP3D_Grade2 = "     << m_uIP3D_Grade2 );
      
      ATH_MSG_DEBUG( " TrkSelGrade1_Chi2 = "            << m_trkSelGrade1_Chi2 );
      ATH_MSG_DEBUG( " TrkSelGrade1_Innermost = "       << m_trkSelGrade1_Innermost );
      ATH_MSG_DEBUG( " TrkSelGrade1_NextToInnermost = " << m_trkSelGrade1_NextToInnermost );
      ATH_MSG_DEBUG( " TrkSelGrade1_PixHits = "         << m_trkSelGrade1_PixHits );
      ATH_MSG_DEBUG( " TrkSelGrade1_SiHits = "          << m_trkSelGrade1_SiHits );
      ATH_MSG_DEBUG( " TrkSelGrade1_D0 = "              << m_trkSelGrade1_D0 );
      ATH_MSG_DEBUG( " TrkSelGrade1_Z0 = "              << m_trkSelGrade1_Z0 );
      ATH_MSG_DEBUG( " TrkSelGrade1_Pt = "              << m_trkSelGrade1_Pt );
      ATH_MSG_DEBUG( " TrkSelGrade1_Eta = "             << m_trkSelGrade1_Eta );
      ATH_MSG_DEBUG( " TrkSelGrade1_Phi = "             << m_trkSelGrade1_Phi );
      ATH_MSG_DEBUG( " TrkSelGrade1_R = "               << m_trkSelGrade1_R );
      
      ATH_MSG_DEBUG( " TrkSelGrade2_Chi2 = "            << m_trkSelGrade2_Chi2 );
      ATH_MSG_DEBUG( " TrkSelGrade2_Innermost = "       << m_trkSelGrade2_Innermost );
      ATH_MSG_DEBUG( " TrkSelGrade2_NextToInnermost = " << m_trkSelGrade2_NextToInnermost );
      ATH_MSG_DEBUG( " TrkSelGrade2_PixHits = "         << m_trkSelGrade2_PixHits );
      ATH_MSG_DEBUG( " TrkSelGrade2_SiHits = "          << m_trkSelGrade2_SiHits );
      ATH_MSG_DEBUG( " TrkSelGrade2_D0 = "              << m_trkSelGrade2_D0 );
      ATH_MSG_DEBUG( " TrkSelGrade2_Z0 = "              << m_trkSelGrade2_Z0 );
      ATH_MSG_DEBUG( " TrkSelGrade2_Pt = "              << m_trkSelGrade2_Pt );
      ATH_MSG_DEBUG( " TrkSelGrade2_Eta = "             << m_trkSelGrade2_Eta );
      ATH_MSG_DEBUG( " TrkSelGrade2_Phi = "             << m_trkSelGrade2_Phi );
      ATH_MSG_DEBUG( " TrkSelGrade2_R = "               << m_trkSelGrade2_R );
      
      ATH_MSG_VERBOSE( " SizeMVtx = "       << m_sizeMVtx );
      ATH_MSG_VERBOSE( " bMVtx = "          << m_bMVtx    );
      ATH_MSG_VERBOSE( " uMVtx = "          << m_uMVtx    );
      ATH_MSG_VERBOSE( " SizeEVtx = "       << m_sizeEVtx );
      ATH_MSG_VERBOSE( " bEVtx = "          << m_bEVtx    );
      ATH_MSG_VERBOSE( " uEVtx = "          << m_uEVtx    );
      ATH_MSG_VERBOSE( " SizeNVtx = "       << m_sizeNVtx );
      ATH_MSG_VERBOSE( " bNVtx = "          << m_bNVtx    );
      ATH_MSG_VERBOSE( " uNVtx = "          << m_uNVtx    );
      ATH_MSG_VERBOSE( " SizeSV = "         << m_sizeSV   );
      ATH_MSG_VERBOSE( " bSV = "            << m_bSV      );
      ATH_MSG_VERBOSE( " uSV = "            << m_uSV      );

    m_trigBjetTagger = new TrigBjetTagger(this, msg(), msgLvl());
    m_constTrigBjetTagger  = const_cast<const TrigBjetTagger*>(m_trigBjetTagger);

    ATH_MSG_DEBUG( "Retrieving tuning likelihoods." );

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
    ATH_MSG_DEBUG( "Got " << vectorOfEFTrackCollections.size() << " TrackParticleContainer" );
  
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

  ATH_MSG_DEBUG( "Retrieving PV with key = " << priVtxKey );
  
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfEFPrmVtxCollections, priVtxKey);
  if (status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get xAOD::VertexContainer from the trigger element" << endmsg;
  } 
  else if (msgLvl() <= MSG::DEBUG) 
    ATH_MSG_DEBUG( "Got " << vectorOfEFPrmVtxCollections.size() << " xAOD::VertexContainer" );
  
  std::vector<const xAOD::VertexContainer*>::iterator pPrmVtxColl    = vectorOfEFPrmVtxCollections.begin();
  std::vector<const xAOD::VertexContainer*>::iterator lastPrmVtxColl = vectorOfEFPrmVtxCollections.end();
  
  // This part is message only
  if (msgLvl() <= MSG::VERBOSE) {
    
    for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
      
      if ((*pPrmVtxColl)->size() != 0)
	ATH_MSG_VERBOSE( "xAOD::VertexContainer with label " << (*pPrmVtxColl)->front()->vertexType() );
    }
    pPrmVtxColl = vectorOfEFPrmVtxCollections.begin();
  }
  
  for ( ; pPrmVtxColl != lastPrmVtxColl; pPrmVtxColl++) { 
    
    ATH_MSG_DEBUG( "Size of pPrmVtxColl = " << (*pPrmVtxColl)->size() );
    
    if ((*pPrmVtxColl)->size() != 0) {
      
      if ((*pPrmVtxColl)->front()->vertexType() == xAOD::VxType::PriVtx) { // was == 1
        if (msgLvl() <= MSG::DEBUG) {
	  ATH_MSG_DEBUG( "Selected collection with Primary Vertex label " << (*pPrmVtxColl)->front()->vertexType() );
	  ATH_MSG_DEBUG( "First PV has z-position = " <<  (*pPrmVtxColl)->front()->z() );
	}
        break;
      }
    }
  }
  
  // Message up to here, or better to say, this selects primary vertex from the collection
  
  if (pPrmVtxColl == lastPrmVtxColl) {
    pointerToEFPrmVtxCollections = 0;
    ATH_MSG_DEBUG( "No primary vertex collection found" );
    return HLT::ERROR;
  } 
  else {
    ATH_MSG_DEBUG( "Found a primary vertex collection." );
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
    ATH_MSG_DEBUG( "Got " << vectorOfSecVtxCollections.size() << " VxSecVertexInfoContainer" );

  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator pSecVtxColl    = vectorOfSecVtxCollections.begin();
  std::vector<const Trk::VxSecVertexInfoContainer*>::iterator lastSecVtxColl = vectorOfSecVtxCollections.end();

  if (msgLvl() <= MSG::VERBOSE) {

    for ( ; pSecVtxColl != lastSecVtxColl; pSecVtxColl++) { 

      if ((*pSecVtxColl)->size() != 0) {
       
        const std::vector<xAOD::Vertex*> vectorOfVxCandidates = (*pSecVtxColl)->front()->vertices();

        if (vectorOfVxCandidates.size()!=0)
	  ATH_MSG_VERBOSE( "VxSecVertexInfoContainer with label " << (*vectorOfVxCandidates.begin())->vertexType() );
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
	    ATH_MSG_DEBUG( "Selected collection with Secondary Vertex label " << (*vectorOfVxCandidates.begin())->vertexType() );
	    ATH_MSG_DEBUG( "First SV has z-position = " <<  (*vectorOfVxCandidates.begin())->z() );
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
    ATH_MSG_DEBUG( "No secondary vertex collection sent when extracting sec vtx info" );
    return HLT::OK;
  }

  const Trk::VxSecVertexInfo* secVertexInfo = (*pointerToEFSecVtxCollections)[0];
  if(!secVertexInfo) {
    ATH_MSG_DEBUG( "No secondary vertex when extracting sec vtx info" );
    return HLT::OK;
  }

  const Trk::VxSecVKalVertexInfo* myVKalSecVertexInfo = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(secVertexInfo);
   
  if(!myVKalSecVertexInfo) {
    ATH_MSG_DEBUG( "The cast to VKal secndary vertex went wrong, the pointer is zero" );
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
      ATH_MSG_DEBUG( "Secondary vertex from InDetVKalVxInJet has more than one vertex, is this ok?" );
      
    for( ; verticesIt!=verticesEnd ; ++verticesIt) {

      if(!(*verticesIt)) {
	ATH_MSG_DEBUG( "Secondary vertex from InDetVKalVxInJet has zero pointer. Skipping this vtx.." );
        continue;
      }

      ATH_MSG_DEBUG( "VxCandidate at (" 
		     << (*verticesIt)->position().x() << "," 
		     << (*verticesIt)->position().y() << "," 
		     << (*verticesIt)->position().z() );

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
         
      ATH_MSG_DEBUG( "Primary vertex for decay length (" 
		     << pPrmVrt->position().x() << "," 
		     << pPrmVrt->position().y() << "," 
		     << pPrmVrt->position().z() << ") and error (" 
		     << pPrmVrt->covariancePosition()(0,0) << "," 
		     << pPrmVrt->covariancePosition()(1,1) << "," 
		     << pPrmVrt->covariancePosition()(2,2) << ")" );
	
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
	  ATH_MSG_DEBUG( "decay length distance do not match among tools: tool " << dist3D << " manual " << distance );
        }
      }
         
      if(msgLvl() <= MSG::DEBUG) {
        double dist2D = (SecVrt - PrmVrt.position()).perp();            
	ATH_MSG_DEBUG( "Calculating secondary vertex decay length with primary vertex at (" 
		       << PrmVrt.position().x() << "," << PrmVrt.position().y() << "," << PrmVrt.position().z() << ") and sec vtx at ("
		       << SecVrt.x() << "," << SecVrt.y() << "," << SecVrt.z() 
		       << ") and jet direction (px,py,pz) = ("
		       << jetDirection.Px() << "," << jetDirection.Py() << "," << jetDirection.Pz() <<  
		       ") which gives 3D decay length " << distance << " (VrtVrtDist tool " << dist3D << ") and 3D significance " << sign3D 
		       << " and 2D(R/phi) decay length " << dist2D );
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

  //track->summaryValue(nBlayerHits,          xAOD::numberOfBLayerHits);
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

  ATH_MSG_VERBOSE( "TrackSel method (grade " << grade << ")" );
  ATH_MSG_VERBOSE( "  Track number "           << i+1  << " to be selected must be:" );
  ATH_MSG_VERBOSE( "    dEta "                 << dEta                          << " <= " << trkSelEta );
  ATH_MSG_VERBOSE( "    dPhi "                 << dPhi                          << " <= " << trkSelPhi );
  ATH_MSG_VERBOSE( "    dR "                   << dR                            << " <= " << trkSelR );
  ATH_MSG_VERBOSE( "    pt "                   << fabs(pT)                      << " >= " << trkSelPt );
  ATH_MSG_VERBOSE( "    d0 "                   << fabs(d0)                      << " <= " << trkSelD0 );
  ATH_MSG_VERBOSE( "    z0*sin(theta) "        << fabs(z0-zv)*TMath::Sin(theta) << " <= " << trkSelZ0 );
  ATH_MSG_VERBOSE( "    innermostLayer "       << (int)nInnermostHits           << " >= " << trkSelInnermost );
  ATH_MSG_VERBOSE( "    nextToInnermostLayer " << (int)nNextToInnermostHits     << " >= " << trkSelNextToInnermost );
  ATH_MSG_VERBOSE( "    pixelHit "             << (int)nPixHits                 << " >= " << trkSelPixHits );
  ATH_MSG_VERBOSE( "    siHit "                << (int)nSiHits                  << " >= " << trkSelSiHits );
  ATH_MSG_VERBOSE( "    prob(chi2) "           << TMath::Prob(track->chiSquared(), (int)nSiHits*3-5) << " > " << trkSelChi2 );
  
  if (dEta > trkSelEta) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " is not selected (delta eta matching)" );
    m_listCutApplied.push_back(2); return false;
  }

  if (dPhi > trkSelPhi) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " is not selected (delta phi matching)" );
    m_listCutApplied.push_back(3); return false;
  }

  if (dR > trkSelR) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " is not selected (delta R matching)" );
    m_listCutApplied.push_back(4); return false;
  }

  if (fabs(pT) < trkSelPt) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (pT cut)" );
    m_listCutApplied.push_back(5); return false;
  }

  if (fabs(d0) > trkSelD0) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (d0 cut)" );
    m_listCutApplied.push_back(6); return false;
  }

  if (fabs(z0-zv)*TMath::Sin(theta) > trkSelZ0) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (z0 cut)" );
    m_listCutApplied.push_back(7); return false;
  }

  if (nInnermostHits < trkSelInnermost) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (missing innermost layer hit)" );
    m_listCutApplied.push_back(8); return false;
  }

  if (nNextToInnermostHits < trkSelNextToInnermost) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (missing next to innermost layer hit)" );
    m_listCutApplied.push_back(9); return false;
  }

  if (nPixHits < trkSelPixHits) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (too few pixel hits)" );
    m_listCutApplied.push_back(10); return false;
  }

  if (nSiHits < trkSelSiHits) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (too few silicon hits)" );
    m_listCutApplied.push_back(11); return false;
  }

  if (TMath::Prob(track->chiSquared(), (int)nSiHits*3-5) <= trkSelChi2) {
    ATH_MSG_DEBUG( "  grade " << grade << " track " << i+1 << " not selected (chi2 cut)" );
    m_listCutApplied.push_back(12); return false;
  }
  
  ATH_MSG_DEBUG( "    grade " << grade << " track " << i+1 << " is selected" );

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
      ATH_MSG_DEBUG( "Using TE: " << "RoI id " << roiDescriptor->roiId()
		     << ", Phi = " <<  roiDescriptor->phi() << ", Eta = " << roiDescriptor->eta() );
    }
  }
  else {
    ATH_MSG_DEBUG( "No feature for this Trigger Element" );
    
    return HLT::NAV_ERROR;
  }

  //xAOD jets from TE

  float et_EFjet = 0;
 
  ATH_MSG_DEBUG( "pass 1 " << et_EFjet );

  const xAOD::JetContainer* jets(0);
  //HLT::ErrorCode ec = getFeature(outputTE, jets,"EFJet");
  HLT::ErrorCode ec = getFeature(outputTE, jets, m_jetKey);

  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } 
  else {
    ATH_MSG_DEBUG( "Obtained JetContainer with name = " << m_jetKey );
  }

  ATH_MSG_DEBUG( "pass 2 " << &jets );

  if(jets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    return HLT::ERROR;
  }

  std::vector<const xAOD::Jet*> theJets(jets->begin(), jets->end());

  ATH_MSG_DEBUG( "pass 2 jet size: " << theJets.size() );

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

    ATH_MSG_DEBUG( "et  " << etjet << " and eta " << etajet << " Et EFjet " << et_EFjet );

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
    
    ATH_MSG_DEBUG( "Beam spot from service: x=" << beamSpot.x() << ", y=" << beamSpot.y() << ", z=" << beamSpot.z() 
		   << ", tiltXZ=" << iBeamCondSvc->beamTilt(0) << ", tiltYZ=" << iBeamCondSvc->beamTilt(1) 
		   << ", sigmaX=" << iBeamCondSvc->beamSigma(0) << ", sigmaY=" << iBeamCondSvc->beamSigma(1) << ", sigmaZ=" << iBeamCondSvc->beamSigma(2) 
		   << ", status=" << beamSpotStatus );
    
    m_trigBjetPrmVtxInfo->setBeamSpot(beamSpot.x(), beamSpot.y(), beamSpot.z());
    m_trigBjetPrmVtxInfo->setBeamSpotTilt(iBeamCondSvc->beamTilt(0), iBeamCondSvc->beamTilt(1));
    m_trigBjetPrmVtxInfo->setBeamSpotWidth(iBeamCondSvc->beamSigma(0), iBeamCondSvc->beamSigma(1), iBeamCondSvc->beamSigma(2));
    m_trigBjetPrmVtxInfo->setBeamSpotStatus(beamSpotStatus);

    ATH_MSG_DEBUG( *m_trigBjetPrmVtxInfo );

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
    ATH_MSG_DEBUG( "No HLT track collection retrieved" );
  } else if (pointerToEFTrackCollections == nullptr) {
    ATH_MSG_DEBUG( "Null pointer to HLT track collection" );
  } else if (msgLvl() <= MSG::DEBUG) {
    ATH_MSG_DEBUG( "HLT track collection retrieved with size = " << pointerToEFTrackCollections->size() );
  }

  // -----------------------------------
  // Get secondary vertex collection 
  // -----------------------------------
  if (getSecVtxCollection(pointerToEFSecVtxCollections, outputTE) != HLT::OK) {
    ATH_MSG_DEBUG( "No secondary vertex collection retrieved" );
  } 
  else if (msgLvl() <= MSG::DEBUG)
    ATH_MSG_DEBUG( "Secondary vertex collection retrieved" );
  
  // -----------------------------------
  // Get primary vertex collection
  // -----------------------------------

  float xPrmVtx=0, yPrmVtx=0, zPrmVtx=0;

  if (m_histoPrmVtxAtEF) {   // PV from TrigT2HistoPrmVtx

    if (getPrmVtxCollection(pointerToEFPrmVtxCollections, outputTE, m_priVtxKey) != HLT::OK) {
      ATH_MSG_DEBUG( "No primary vertex collection retrieved with name " << m_priVtxKey );
      // If the ID PV-finding fails then use the PV from T2HistoPrmVtx instead
      // This is not ideal... investigate why ID PV finding fails
      if (m_priVtxKey == "xPrimVx" && getPrmVtxCollection(pointerToEFPrmVtxCollections, outputTE, "EFHistoPrmVtx") != HLT::OK) {
	ATH_MSG_DEBUG( "No primary vertex collection retrieved with name EFHistoPrmVtx either..." );
      }
      else if (msgLvl() <= MSG::DEBUG) {
	ATH_MSG_DEBUG( "Didn't manage to find " << m_priVtxKey << " PV, so using EFHistoPrmVtx instead." );
      }
    } 
    else if (msgLvl() <= MSG::DEBUG) {
      ATH_MSG_DEBUG( "Primary vertex collection retrieved" );
    }

    // Protect against null pointers
    if(pointerToEFPrmVtxCollections) {
      // Protect against empty vectors
      if(pointerToEFPrmVtxCollections->size()>0) {
	      const xAOD::Vertex* prmVertex = (*pointerToEFPrmVtxCollections)[0];
        zPrmVtx = prmVertex->z();
      } else {
	ATH_MSG_DEBUG( "Empty primary vertex collection retrieved" );
        zPrmVtx = 0;
      }
    } else {
      ATH_MSG_DEBUG( "No primary vertex collection retrieved" );
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
      ATH_MSG_DEBUG( "No primary vertex collection retrieved" );
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

  ATH_MSG_DEBUG( *m_trigBjetPrmVtxInfo );

  m_trackJetFinderTool->clear();
  ATH_MSG_DEBUG( "Set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() );
  m_trackJetFinderTool->inputPrimaryVertexZ(m_trigBjetPrmVtxInfo->zPrmVtx());
  ATH_MSG_DEBUG( "Done set input  z-vtx to trackjet tool " << m_trigBjetPrmVtxInfo->zPrmVtx() );

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
      ATH_MSG_DEBUG( "  " << trigBjetTrackInfo );
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
    
    ATH_MSG_DEBUG( "eta Jet = eta RoI" );
  }
  
  ATH_MSG_DEBUG( *m_trigBjetJetInfo );

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
      ATH_MSG_DEBUG( "Beam spot status flag set to " << m_trigBjetPrmVtxInfo->beamSpotStatus() << ". Discriminant weights are not computed." );
      ATH_MSG_DEBUG( "Beam spot flag set to " << m_useBeamSpotFlag << ". Discriminant weights are not computed." );
    }

    m_listCutApplied.push_back(0);
    m_trigBjetTagger->getWeights();

  } else if (m_trigBjetPrmVtxInfo->xBeamSpotWidth()>m_setBeamSpotWidth || m_trigBjetPrmVtxInfo->yBeamSpotWidth()>m_setBeamSpotWidth) {

    ATH_MSG_DEBUG( "Beam spot width is more than " << m_setBeamSpotWidth << "um. Discriminant weights are not computed." );

    m_listCutApplied.push_back(1);
    m_trigBjetTagger->getWeights();

  } else {
    
    ATH_MSG_DEBUG( "Computing discriminant weights using taggers: " << m_taggers 
		   << " and using calibration from " << (m_useParamFromData==0 ? "MC" : "data") << " for CHI2" );

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
	ATH_MSG_DEBUG( "No EF SV information retrieved from TrigBjetFex::getSecVtxInfo" );
      
      ATH_MSG_DEBUG( *m_trigBjetSecVtxInfo );
      
    }

    m_trigBjetTagger->getWeights(m_trigBjetTrackInfoVector, m_trigBjetPrmVtxInfo, m_trigBjetSecVtxInfo, m_trigBjetJetInfo);
  }
  
  if (msgLvl() <= MSG::DEBUG) {
     
    const EventInfo* pEventInfo;
     
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
      ATH_MSG_DEBUG( "Failed to get EventInfo " );
    } 
    else {
	ATH_MSG_DEBUG( "Bjet slice summary (Run " << pEventInfo->event_ID()->run_number() 
		       << "; Event " << pEventInfo->event_ID()->event_number() << ")" );
	ATH_MSG_DEBUG( "REGTEST:  RoI " << roiDescriptor->roiId()
		       << ", Phi = "   << roiDescriptor->phi()
		       << ", Eta = "   << roiDescriptor->eta() );
	ATH_MSG_DEBUG( "REGTEST:  Tracks: " << m_totTracks << " reconstructed and " << m_totSelTracks <<" selected" );
      
      if (pointerToEFPrmVtxCollections) {
	ATH_MSG_DEBUG( "REGTEST:  Primary vertex: " << pointerToEFPrmVtxCollections->size() << " reconstructed"
		       << ", (x,y,z) = (" << m_trigBjetPrmVtxInfo->xPrmVtx() << "," << m_trigBjetPrmVtxInfo->yPrmVtx() << "," << m_trigBjetPrmVtxInfo->zPrmVtx() << ")" );
      }
      if (pointerToEFSecVtxCollections)
	ATH_MSG_DEBUG( "REGTEST:  Secondary vertex: " << pointerToEFSecVtxCollections->size() << " reconstructed" );
      else
	ATH_MSG_DEBUG( "REGTEST:  Secondary vertex: 0 reconstructed" );
      
      ATH_MSG_DEBUG( "REGTEST:  SV L/sigma(L) " <<  m_trigBjetSecVtxInfo->decayLengthSignificance() << "  SV mass " <<  m_trigBjetSecVtxInfo->vtxMass() 
		     << "  SV efrac " <<  m_trigBjetSecVtxInfo->energyFraction() << "   SV 2-track vertex multiplicity " << m_trigBjetSecVtxInfo->n2TrkVtx() );
      
      ATH_MSG_DEBUG( "REGTEST: List weights stored probability and likelihood objects:" );

      pTagger = m_taggers.begin();
      for ( ; pTagger != lastTagger; pTagger++)
	ATH_MSG_DEBUG( "REGTEST:  X(" << (*pTagger) << ") = " << m_trigBjetTagger->taggersXMap((*pTagger))  );

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
    ATH_MSG_DEBUG( "Failed to attach TrigEFBjetContainer to navigation" );
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


  ATH_MSG_DEBUG( "IP2D u/b: " << m_trigBjetTagger->taggersPuMap("IP2D") << "/" << m_trigBjetTagger->taggersPbMap("IP2D")
		 << "   IP3D u/b: " << m_trigBjetTagger->taggersPuMap("IP3D") << "/" << m_trigBjetTagger->taggersPbMap("IP3D")
		 << "   SV1 u/b: " << m_trigBjetTagger->taggersPuMap("MVTX")*m_trigBjetTagger->taggersPuMap("NVTX")*m_trigBjetTagger->taggersPuMap("EVTX") << "/"
		 << m_trigBjetTagger->taggersPbMap("MVTX")*m_trigBjetTagger->taggersPbMap("NVTX")*m_trigBjetTagger->taggersPbMap("EVTX") );
		 
  
  stat = attachFeature(outputTE, m_trigBTaggingContainer, "HLTBjetFex");
  
  if (stat != HLT::OK) {
    ATH_MSG_DEBUG( "Failed to attach BTaggingContainer to navigation" );
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
