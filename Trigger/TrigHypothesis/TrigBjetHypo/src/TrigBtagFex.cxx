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

// BEAMSPOT
#include "InDetBeamSpotService/IBeamCondSvc.h"

// EDM

#include "xAODBase/IParticle.h"

#include "xAODJet/Jet.h"
 #include "xAODJet/JetContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagTool.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigBtagFex::TrigBtagFex(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator),
  m_bTagTrackAssocTool("Analysis::BTagTrackAssociation"),
  m_bTagSecVtxTool("Analysis::BTagSecVertexing")

{
  declareProperty("setupOfflineTools",  m_setupOfflineTools = false);

  declareProperty("TaggerBaseNames",    m_TaggerBaseNames);
  declareProperty("TrackKey",           m_trackKey = "");
  declareProperty("JetKey",             m_jetKey = "");
  declareProperty("PriVtxKey",          m_priVtxKey = "xPrimVx");
  declareProperty("UsePriVtxKeyBackup", m_usePriVtxKeyBackup = true);
  declareProperty("PriVtxKeyBackup",    m_priVtxKeyBackup = "EFHistoPrmVtx");

  declareProperty("BTagTool",           m_bTagTool);
  declareProperty("BTagTrackAssocTool", m_bTagTrackAssocTool);
  declareProperty("BTagSecVertexing",   m_bTagSecVtxTool);

  declareProperty ("UseBeamSpotFlag",    m_useBeamSpotFlag    = false);

  // Run-2 monitoring

  declareMonitoredVariable("sv1_mass", m_mon_sv_mass, AutoClear);
  declareMonitoredVariable("sv1_evtx", m_mon_sv_evtx, AutoClear);
  declareMonitoredVariable("sv1_nvtx", m_mon_sv_nvtx, AutoClear);
  declareMonitoredVariable("sv1_ntkv", m_mon_sv_ntkv, AutoClear);
  declareMonitoredVariable("sv1_sig3", m_mon_sv_sig3, AutoClear);
  declareMonitoredVariable("sv1_Lxy",  m_mon_sv_Lxy,  AutoClear);
  declareMonitoredVariable("sv1_L3d",  m_mon_sv_L3d,  AutoClear);
  declareMonitoredVariable("sv1_dR",   m_mon_sv_dR,   AutoClear);


  declareMonitoredVariable("tag_IP2D",    m_mon_tag_IP2D,    AutoClear);
  declareMonitoredVariable("tag_IP3D",    m_mon_tag_IP3D,    AutoClear);
  declareMonitoredVariable("tag_SV1",     m_mon_tag_SV1,     AutoClear);
  declareMonitoredVariable("tag_IP3DSV1", m_mon_tag_IP3DSV1, AutoClear);
  declareMonitoredVariable("tag_MV2c00",  m_mon_tag_MV2c00,  AutoClear);
  declareMonitoredVariable("tag_MV2c10",  m_mon_tag_MV2c10,  AutoClear);
  declareMonitoredVariable("tag_MV2c20",  m_mon_tag_MV2c20,  AutoClear);
  declareMonitoredVariable("tag_IP2_c",   m_mon_tag_IP2_c,   AutoClear);
  declareMonitoredVariable("tag_IP2_cu",  m_mon_tag_IP2_cu,  AutoClear);
  declareMonitoredVariable("tag_IP3_c",   m_mon_tag_IP3_c,   AutoClear);
  declareMonitoredVariable("tag_IP3_cu",  m_mon_tag_IP3_cu,  AutoClear);

  //MV2 taggers variables
  declareMonitoredVariable("jf_n2tv",   m_mon_jf_n2tv,   AutoClear);
  declareMonitoredVariable("jf_ntrkv",  m_mon_jf_ntrkv,  AutoClear);
  declareMonitoredVariable("jf_nvtx",   m_mon_jf_nvtx,   AutoClear);
  declareMonitoredVariable("jf_nvtx1t", m_mon_jf_nvtx1t, AutoClear);
  declareMonitoredVariable("jf_mass",   m_mon_jf_mass,   AutoClear);
  declareMonitoredVariable("jf_efrc",   m_mon_jf_efrc,   AutoClear);
  declareMonitoredVariable("jf_dR",     m_mon_jf_dR,     AutoClear);
  declareMonitoredVariable("jf_sig3",   m_mon_jf_sig3,   AutoClear);

  declareMonitoredVariable("jet_pt",    m_mon_jet_pt,    AutoClear);
  declareMonitoredVariable("jet_eta",   m_mon_jet_eta,   AutoClear);
  declareMonitoredVariable("sv_bool",   m_mon_sv_bool,   AutoClear);
  declareMonitoredVariable("jf_bool",   m_mon_jf_bool,   AutoClear);

}


// ----------------------------------------------------------------------------------------------------------------- 


TrigBtagFex::~TrigBtagFex() {

}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltInitialize() {

  // Get message service 
  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigBtagFex, version " << PACKAGE_VERSION << endmsg;

  // declareProperty overview 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    // msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endmsg;
    // msg() << MSG::DEBUG << " Instance = "            << m_instance << endmsg;

    msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endmsg;
    // msg() << MSG::DEBUG << " SetBeamSpotWidth = "    << m_setBeamSpotWidth << endmsg;

    // msg() << MSG::DEBUG << " UseParamFromData = "    << m_useParamFromData << endmsg;

    // msg() << MSG::DEBUG << " Using Offline Tools = " << m_setupOfflineTools << endmsg;
    // msg() << MSG::DEBUG << " Taggers = "             << m_taggers << endmsg;
    msg() << MSG::DEBUG << " Offline Taggers = "     << m_TaggerBaseNames << endmsg;
    // msg() << MSG::DEBUG << " UseErrIPParam = "       << m_useErrIPParam << endmsg;
    // msg() << MSG::DEBUG << " UseJetDirection = "     << m_useJetDirection << endmsg;
    // msg() << MSG::DEBUG << " RetrieveHLTJets = "     << m_retrieveHLTJets << endmsg;
    // msg() << MSG::DEBUG << " TagHLTJets = "          << m_tagHLTJets << endmsg;
    // msg() << MSG::DEBUG << " HistoPrmVtxAtEF = "        << m_histoPrmVtxAtEF << endmsg;
    // msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << m_useEtaPhiTrackSel << endmsg;

    // msg() << MSG::DEBUG << " JetProb 0 MC = "      << m_par_0_MC << endmsg;
    // msg() << MSG::DEBUG << " JetProb 1 MC = "      << m_par_1_MC << endmsg;
    // msg() << MSG::DEBUG << " JetProb 0 DT = "      << m_par_0_DT << endmsg;
    // msg() << MSG::DEBUG << " JetProb 1 DT = "      << m_par_1_DT << endmsg;

    // msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endmsg;
    // msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endmsg;
    // msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endmsg;
    // msg() << MSG::DEBUG << " SizeIP2D = "          << m_sizeIP2D << endmsg;
    // msg() << MSG::DEBUG << " bIP2D = "             << m_bIP2D << endmsg;
    // msg() << MSG::DEBUG << " uIP2D = "             << m_uIP2D << endmsg;
    // msg() << MSG::DEBUG << " SizeIP3D = "          << m_sizeIP3D << endmsg;
    // msg() << MSG::DEBUG << " bIP3D = "             << m_bIP3D << endmsg;
    // msg() << MSG::DEBUG << " uIP3D = "             << m_uIP3D << endmsg;

    // msg() << MSG::DEBUG << " SizeIP1D_lowSiHits = "          << m_sizeIP1D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " bIP1D_lowSiHits = "             << m_bIP1D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " uIP1D_lowSiHits = "             << m_uIP1D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " SizeIP2D_lowSiHits = "          << m_sizeIP2D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " bIP2D_lowSiHits = "             << m_bIP2D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " uIP2D_lowSiHits = "             << m_uIP2D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " SizeIP3D_lowSiHits = "          << m_sizeIP3D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " bIP3D_lowSiHits = "             << m_bIP3D_lowSiHits << endmsg;
    // msg() << MSG::DEBUG << " uIP3D_lowSiHits = "             << m_uIP3D_lowSiHits << endmsg;

    // msg() << MSG::DEBUG << " SizeIP1D = "          << m_sizeIP1D << endmsg;
    // msg() << MSG::DEBUG << " bIP1D = "             << m_bIP1D << endmsg;
    // msg() << MSG::DEBUG << " uIP1D = "             << m_uIP1D << endmsg;

    // msg() << MSG::DEBUG << " TrkSel_Chi2 = "     << m_trkSelChi2 << endmsg;
    // msg() << MSG::DEBUG << " TrkSel_BLayer = "   << m_trkSelBLayer << endmsg;
    // msg() << MSG::DEBUG << " TrkSel_SiHits = "   << m_trkSelSiHits << endmsg;
    // msg() << MSG::DEBUG << " TrkSel_D0 = "       << m_trkSelD0 << endmsg;
    // msg() << MSG::DEBUG << " TrkSel_Z0 = "       << m_trkSelZ0 << endmsg;
    // msg() << MSG::DEBUG << " TrkSel_Pt = "       << m_trkSelPt << endmsg;

    // msg() << MSG::DEBUG << " SizeMVtx = "       << m_sizeMVtx << endmsg;
    // msg() << MSG::DEBUG << " bMVtx = "          << m_bMVtx << endmsg;
    // msg() << MSG::DEBUG << " uMVtx = "          << m_uMVtx << endmsg;
    // msg() << MSG::DEBUG << " SizeEVtx = "       << m_sizeEVtx << endmsg;
    // msg() << MSG::DEBUG << " bEVtx = "          << m_bEVtx << endmsg;
    // msg() << MSG::DEBUG << " uEVtx = "          << m_uEVtx << endmsg;
    // msg() << MSG::DEBUG << " SizeNVtx = "       << m_sizeNVtx << endmsg;
    // msg() << MSG::DEBUG << " bNVtx = "          << m_bNVtx << endmsg;
    // msg() << MSG::DEBUG << " uNVtx = "          << m_uNVtx << endmsg;
    // msg() << MSG::DEBUG << " SizeSV = "         << m_sizeSV << endmsg;
    // msg() << MSG::DEBUG << " bSV = "            << m_bSV << endmsg;
    // msg() << MSG::DEBUG << " uSV = "            << m_uSV << endmsg;
  }

  if(m_setupOfflineTools) {

    // Retrieve the offline track association tool
    if(!m_bTagTrackAssocTool.empty()) {
      if(m_bTagTrackAssocTool.retrieve().isFailure()) {
	msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTrackAssocTool << endmsg;
	return HLT::BAD_JOB_SETUP;
      } else
	msg() << MSG::INFO << "Retrieved tool " << m_bTagTrackAssocTool << endmsg;
    } else if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No track association tool to retrieve" << endmsg;

    // Retrieve the bTagSecVtxTool
    if(m_bTagSecVtxTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagSecVtxTool << endmsg;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagSecVtxTool << endmsg;

    // Retrieve the main BTagTool
    if(m_bTagTool.retrieve().isFailure()) {
      msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTool << endmsg;
      return HLT::BAD_JOB_SETUP;
    } else
      msg() << MSG::INFO << "Retrieved tool " << m_bTagTool << endmsg;

  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigBtagFex" << endmsg;

  // RETRIEVE INPUT CONTAINERS

  // Get EF jet 
  const xAOD::JetContainer* jets = nullptr;
  if(getFeature(inputTE, jets, m_jetKey) == HLT::OK && jets != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::JetContainer: " << "nJets = " << jets->size() << endmsg;
  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - No xAOD::JetContainer" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // Get primary vertex 
  const xAOD::VertexContainer* vertexes = nullptr;
  const xAOD::Vertex* primaryVertex     = nullptr;
  bool usePVBackup=true;
  if (getFeature(outputTE, vertexes, m_priVtxKey) == HLT::OK && vertexes != nullptr) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: " << m_priVtxKey << " has nVertexes = " << vertexes->size() << endmsg;
    primaryVertex = getPrimaryVertex(vertexes);
    if (primaryVertex){
      usePVBackup=false;
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKey << endmsg;
    }
  }

  if(m_usePriVtxKeyBackup && usePVBackup) {
    vertexes = nullptr;
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKey << " - proceeding with backup option" << endmsg;
    if (getFeature(outputTE, vertexes, m_priVtxKeyBackup) == HLT::OK && vertexes != nullptr) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: " << m_priVtxKeyBackup << " has nVertexes = " << vertexes->size() << endmsg;
      primaryVertex = getPrimaryVertex(vertexes);	
      if (primaryVertex){
	usePVBackup=false;
	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKeyBackup << endmsg;
      }
    }
  }
  
  if(usePVBackup) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKeyBackup << " - aborting..." << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // Get tracks 
  const xAOD::TrackParticleContainer* tracks = nullptr;
  if(getFeature(outputTE, tracks, m_trackKey) == HLT::OK && tracks != nullptr) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "INPUT - xAOD::TrackParticleContainer: " << "nTracks = " << tracks->size() << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "INPUT - No xAOD::TrackParticleContainer" << endmsg;
    return HLT::MISSING_FEATURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS

  // Prepare jet tagging - get primary vertex 
  //auto vtxitr = vertexes->begin();
  //const auto primaryVertex = *vtxitr;

  // Prepare jet tagging - create temporary jet copy 
  auto jetitr=jets->begin();
  xAOD::Jet jet;
  jet.makePrivateStore(**jetitr);

  // Prepare jet tagging - create SV output 
  auto    trigVertexContainer = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer   trigSecVertexAuxContainer;
  trigVertexContainer->setStore(&trigSecVertexAuxContainer);

  // Prepare jet tagging - create JF output 
  auto   trigBTagVertexContainer = new xAOD::BTagVertexContainer();
  xAOD::VertexAuxContainer     trigBTagSecVertexAuxContainer;
  trigBTagVertexContainer->setStore(&trigBTagSecVertexAuxContainer);

  // Prepare jet tagging - create BTagging output 
  xAOD::BTaggingAuxContainer trigBTaggingAuxContainer;
  auto trigBTaggingContainer = new xAOD::BTaggingContainer();
  trigBTaggingContainer->setStore(&trigBTaggingAuxContainer);
  auto trigBTagging = new xAOD::BTagging();
  trigBTaggingContainer->push_back(trigBTagging);

  // EXECUTE OFFLINE TOOLS

  if(m_setupOfflineTools) {

    // Execute track association 
    if (!m_bTagTrackAssocTool.empty()) {
      StatusCode jetIsAssociated;

      // Link the BTagging object to the jet for track association
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      linkBTagger.toContainedElement(*trigBTaggingContainer, trigBTagging);
      jet.setBTaggingLink(linkBTagger);

      std::vector<xAOD::Jet*> jetsList;
      jetsList.push_back(&jet);
      if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "#BTAG# Track association tool is not empty" << endmsg;
      // We must pass the tracks explicitly to the track associator
      jetIsAssociated = m_bTagTrackAssocTool->BTagTrackAssociation_exec(&jetsList, tracks);

      if ( jetIsAssociated.isFailure() ) {
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "#BTAG# Failed to associate tracks to jet" << endmsg;
	return StatusCode::FAILURE;
      }
    }
    else {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Empty track association tool" << endmsg;
    }

    // Execute secondary vertexing 
    StatusCode sc = m_bTagSecVtxTool->BTagSecVtx_exec(jet, trigBTagging, trigVertexContainer, trigBTagVertexContainer, primaryVertex);
    if(sc.isFailure()) {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed to reconstruct sec vtx" << endmsg;
    }

    // Tag jet 
    sc = m_bTagTool->tagJet(jet, trigBTagging, primaryVertex);
    if(sc.isFailure()) {
      if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "#BTAG# Failed in taggers call" << endmsg;
    }
  }

  // Fill monitoring variables

  sv_mass_check = 0.0 ; 
  
  trigBTagging->variable<float>("SV1", "masssvx",  sv_mass_check);

  if(sv_mass_check == -1 )     { m_mon_sv_bool = 0;}
  else if(sv_mass_check == 0 ) { m_mon_sv_bool = 1;}
  else{m_mon_sv_bool = 2; }  

  sv_check = trigBTagging->auxdata<float>("SV1_Lxy");

  if( sv_check != -100.0 ) 
    {  
      trigBTagging->variable<float>("SV1", "masssvx",  m_mon_sv_mass);
      trigBTagging->variable<float>("SV1", "efracsvx", m_mon_sv_evtx);
      trigBTagging->variable<int>  ("SV1", "N2Tpair",  m_mon_sv_nvtx);
      trigBTagging->variable<int>  ("SV1", "NGTinSvx", m_mon_sv_ntkv);
      trigBTagging->variable<float>("SV1", "normdist", m_mon_sv_sig3);
      if(m_mon_sv_sig3 > 200) m_mon_sv_sig3 = 200;
      m_mon_sv_Lxy = trigBTagging->auxdata<float>("SV1_Lxy");
      if(m_mon_sv_Lxy > 100) m_mon_sv_Lxy = 100;
      m_mon_sv_L3d = trigBTagging->auxdata<float>("SV1_L3d");
      if(m_mon_sv_L3d > 100) m_mon_sv_L3d = 100;
      m_mon_sv_dR  = trigBTagging->auxdata<float>("SV1_deltaR");
      if(m_mon_sv_dR > 1) m_mon_sv_dR = 1;
      m_mon_tag_SV1     = trigBTagging->SV1_loglikelihoodratio();
    }
  else
    {
      m_mon_sv_mass= -999; 
      m_mon_sv_evtx= -999; 
      m_mon_sv_nvtx= -999; 
      m_mon_sv_ntkv= -999;
      m_mon_sv_Lxy= -999;
      m_mon_sv_L3d= -999;
      m_mon_sv_sig3= -999;
      m_mon_sv_dR= -999; 
      m_mon_tag_SV1= -999;
    } //set default values for jets that SV is not found

  
  //Check JetFitter algorithm
  m_mon_jf_deta = -10.;
  m_mon_jf_dphi = -10.;
  trigBTagging->variable<float>  ("JetFitter", "deltaeta",  m_mon_jf_deta);
  trigBTagging->variable<float>  ("JetFitter", "deltaphi",  m_mon_jf_dphi); 
  jf_check = std::hypot( m_mon_jf_deta,m_mon_jf_dphi);
  if( jf_check > 14 ){ jf_check = -1. ; }


  if(jf_check == -1 ) { m_mon_jf_bool = 0;}
  else{ m_mon_jf_bool = 1;}

  if( jf_check != -1 )
    {
      trigBTagging->variable<int>  ("JetFitter", "N2Tpair",  m_mon_jf_n2tv);
      if(m_mon_jf_n2tv > 20) m_mon_jf_n2tv = 20;
      trigBTagging->variable<int>  ("JetFitter", "nTracksAtVtx",  m_mon_jf_ntrkv);
      trigBTagging->variable<int>  ("JetFitter", "nVTX",  m_mon_jf_nvtx);
      trigBTagging->variable<int>  ("JetFitter", "nSingleTracks",  m_mon_jf_nvtx1t);
      trigBTagging->variable<float>  ("JetFitter", "mass",  m_mon_jf_mass);
      if(m_mon_jf_mass > 9000) m_mon_jf_mass = 9000;
      trigBTagging->variable<float>  ("JetFitter", "energyFraction",  m_mon_jf_efrc);
   
      m_mon_jf_dR = std::hypot( m_mon_jf_deta,m_mon_jf_dphi);
      if( m_mon_jf_dR > 14 ){ m_mon_jf_dR = -1. ; }
      trigBTagging->variable<float>  ("JetFitter", "significance3d",  m_mon_jf_sig3); 
    }
  else
    {
      m_mon_jf_n2tv= -999;
      m_mon_jf_ntrkv= -999; 
      m_mon_jf_nvtx= -999; 
      m_mon_jf_nvtx1t= -999; 
      m_mon_jf_mass= -999; 
      m_mon_jf_efrc= -999; 
      m_mon_jf_dR= -999; 
      m_mon_jf_sig3= -999; 
    } 

  //Check IPxD/MV2 variables
  m_mon_tag_IP2D    = trigBTagging->IP2D_loglikelihoodratio();
  m_mon_tag_IP3D    = trigBTagging->IP3D_loglikelihoodratio();
  m_mon_tag_IP3DSV1 = trigBTagging->SV1plusIP3D_discriminant();
  m_mon_tag_MV2c00  = trigBTagging->auxdata<double>("MV2c00_discriminant");
  m_mon_tag_MV2c10  = trigBTagging->auxdata<double>("MV2c10_discriminant");
  m_mon_tag_MV2c20  = trigBTagging->auxdata<double>("MV2c20_discriminant");
  if( trigBTagging->IP2D_pc() != 0 && trigBTagging->IP2D_pb() != 0 ) m_mon_tag_IP2_c   = log(( trigBTagging->IP2D_pb() )/( trigBTagging->IP2D_pc() ));
  else m_mon_tag_IP2_c   = -999.;
  if( trigBTagging->IP2D_pu() != 0 && trigBTagging->IP2D_pc() != 0 ) m_mon_tag_IP2_cu  = log(( trigBTagging->IP2D_pc() )/( trigBTagging->IP2D_pu() ));
  else m_mon_tag_IP2_cu  = -999.;
  if( trigBTagging->IP3D_pc() != 0 && trigBTagging->IP3D_pb() != 0 ) m_mon_tag_IP3_c   = log(( trigBTagging->IP3D_pb() )/( trigBTagging->IP3D_pc() ));
  else m_mon_tag_IP3_c   = -999.;
  if( trigBTagging->IP3D_pu() != 0 && trigBTagging->IP3D_pc() != 0 ) m_mon_tag_IP3_cu  = log(( trigBTagging->IP3D_pc() )/( trigBTagging->IP3D_pu() ));
  else m_mon_tag_IP3_cu  = -999.;

  m_mon_jet_pt  =  jet.pt()  ;
  m_mon_jet_eta =  jet.eta() ;


  // Dump results 
  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "IP2D u/b: " << trigBTagging->IP2D_pu() << "/" << trigBTagging->IP2D_pb()
          << "   IP3D u/b: " << trigBTagging->IP3D_pu() << "/" << trigBTagging->IP3D_pb()
          << "   SV1 u/b: " << trigBTagging->SV1_pu() << "/" << trigBTagging->SV1_pb()
          << "   MV2c20 var: " << trigBTagging->auxdata<double>("MV2c20_discriminant") 
          << "   MV2c10 var: " << trigBTagging->auxdata<double>("MV2c10_discriminant") << endmsg;

  // ATTACH FEATURES AND CLEAR TEMPORARY OBJECTS

  // Create element link from the BTagging to the Jet container
  auto &jetAssociationLinks = trigBTagging->auxdata<std::vector<ElementLink<xAOD::IParticleContainer> > >("BTagBtagToJetAssociator");
  if (jets) {
    jetAssociationLinks.resize (jets->size());
    for(size_t i = 0; i < jets->size(); ++i) {
      jetAssociationLinks[i].toIndexedElement (*jets, i);
    }
  }

  // Attach BTagContainer as feature 
  if(attachFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTaggingContainer" << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTaggingContainer" << endmsg;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigVertexContainer, "HLT_BjetSecondaryVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::VertexContainer" << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::VertexContainer" << endmsg;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigBTagVertexContainer, "HLT_BjetVertexFex") == HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "OUTPUT - Attached xAOD::BTagVertexContainer" << endmsg;
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTagVertexContainer" << endmsg;
    return HLT::NAV_ERROR;
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------------------------------------------------- 


HLT::ErrorCode TrigBtagFex::hltFinalize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Finalizing TrigBtagFex" << endmsg;

  return HLT::OK;
}

const xAOD::Vertex* TrigBtagFex::getPrimaryVertex(const xAOD::VertexContainer* vertexContainer)
{
  // vertex types are listed on L328 of                                                                                                                                                                                              
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTracking/trunk/xAODTracking/TrackingPrimitives.h                                                                                                                    
  for( auto vtx_itr : *vertexContainer )
    {
      if(vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) { continue; }
      return vtx_itr;
    }
  if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "No primary vertex found." << endmsg;

  return 0;
}

