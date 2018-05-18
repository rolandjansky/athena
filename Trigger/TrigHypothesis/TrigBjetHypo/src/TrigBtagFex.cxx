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
  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " UseBeamSpotFlag = "     << m_useBeamSpotFlag );
  ATH_MSG_DEBUG( " Offline Taggers = "     << m_TaggerBaseNames );
  
  if(m_setupOfflineTools) {
  
    // Retrieve the offline track association tool
    if(!m_bTagTrackAssocTool.empty()) {
      if(m_bTagTrackAssocTool.retrieve().isFailure()) {
	msg() << MSG::FATAL << "Failed to locate tool " << m_bTagTrackAssocTool << endmsg;
	return HLT::BAD_JOB_SETUP;
      } else
	msg() << MSG::INFO << "Retrieved tool " << m_bTagTrackAssocTool << endmsg;
    } else if(msgLvl() <= MSG::DEBUG)
      ATH_MSG_DEBUG( "No track association tool to retrieve" );

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

  ATH_MSG_DEBUG( "Executing TrigBtagFex" );

  // RETRIEVE INPUT CONTAINERS

  // Get EF jet 
  const xAOD::JetContainer* jets = nullptr;
  if(getFeature(inputTE, jets, m_jetKey) == HLT::OK && jets != nullptr) {
    ATH_MSG_DEBUG( "INPUT - xAOD::JetContainer: " << "nJets = " << jets->size() );
  } else {
    ATH_MSG_DEBUG( "INPUT - No xAOD::JetContainer" );
    return HLT::MISSING_FEATURE;
  }

  // Get primary vertex 
  const xAOD::VertexContainer* vertexes = nullptr;
  const xAOD::Vertex* primaryVertex     = nullptr;
  bool usePVBackup=true;
  if (getFeature(outputTE, vertexes, m_priVtxKey) == HLT::OK && vertexes != nullptr) {
    ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: " << m_priVtxKey << " has nVertexes = " << vertexes->size() );
    primaryVertex = getPrimaryVertex(vertexes);
    if (primaryVertex){
      usePVBackup=false;
      ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKey );
    }
  }

  if(m_usePriVtxKeyBackup && usePVBackup) {
    vertexes = nullptr;
    ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKey << " - proceeding with backup option" );
    if (getFeature(outputTE, vertexes, m_priVtxKeyBackup) == HLT::OK && vertexes != nullptr) {
      ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: " << m_priVtxKeyBackup << " has nVertexes = " << vertexes->size() );
      primaryVertex = getPrimaryVertex(vertexes);	
      if (primaryVertex){
	usePVBackup=false;
	ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: valid vertex found in " << m_priVtxKeyBackup );
      }
    }
  }
  
  if(usePVBackup) {
    ATH_MSG_DEBUG( "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_priVtxKeyBackup << " - aborting..." );
    return HLT::MISSING_FEATURE;
  }

  // Get tracks 
  const xAOD::TrackParticleContainer* tracks = nullptr;
  if(getFeature(outputTE, tracks, m_trackKey) == HLT::OK && tracks != nullptr) {
    ATH_MSG_DEBUG( "INPUT - xAOD::TrackParticleContainer: " << "nTracks = " << tracks->size() );
  } else {
    if(msgLvl() <= MSG::ERROR) ATH_MSG_ERROR( "INPUT - No xAOD::TrackParticleContainer" );
    return HLT::MISSING_FEATURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS

  // Prepare jet tagging - get primary vertex 
  //auto vtxitr = vertexes->begin();
  //const auto primaryVertex = *vtxitr;

  // Prepare jet tagging - create temporary jet copy 
  auto jetitr=jets->begin();
  xAOD::Jet jet;

  if ( m_jetKey != "GSCJet" ) {
    jet.makePrivateStore(**jetitr);
  } else {
    const xAOD::JetContainer* Splitjets = nullptr;
    if (getFeature(inputTE, Splitjets, "SplitJet") != HLT::OK || Splitjets == nullptr) {
      ATH_MSG_ERROR( "INPUT - No xAOD::JetContainer SplitJet for ALLTE TrigBtagFex" );
      return HLT::MISSING_FEATURE;
    }

    auto splitjetitr=Splitjets->begin();
    jet.makePrivateStore(**splitjetitr);
  }

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
      ATH_MSG_VERBOSE( "#BTAG# Track association tool is not empty" );
      // We must pass the tracks explicitly to the track associator
      jetIsAssociated = m_bTagTrackAssocTool->BTagTrackAssociation_exec(&jetsList, tracks);

      if ( jetIsAssociated.isFailure() ) {
	ATH_MSG_DEBUG( "#BTAG# Failed to associate tracks to jet" );
	return HLT::MISSING_FEATURE;
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
  trigBTagging->variable<float>("SV1", "masssvx",  m_mon_sv_mass);
  trigBTagging->variable<float>("SV1", "efracsvx", m_mon_sv_evtx);
  trigBTagging->variable<int>  ("SV1", "N2Tpair",  m_mon_sv_nvtx);

  m_mon_tag_IP2D    = trigBTagging->IP2D_loglikelihoodratio();
  m_mon_tag_IP3D    = trigBTagging->IP3D_loglikelihoodratio();
  m_mon_tag_SV1     = trigBTagging->SV1_loglikelihoodratio();
  m_mon_tag_IP3DSV1 = trigBTagging->SV1plusIP3D_discriminant();

  m_mon_tag_MV2c00 = trigBTagging->auxdata<double>("MV2c00_discriminant");
  m_mon_tag_MV2c10 = trigBTagging->auxdata<double>("MV2c10_discriminant");
  m_mon_tag_MV2c20 = trigBTagging->auxdata<double>("MV2c20_discriminant");

  // Dump results 
  ATH_MSG_DEBUG( "IP2D u/b: " << trigBTagging->IP2D_pu() << "/" << trigBTagging->IP2D_pb()
		 << "   IP3D u/b: " << trigBTagging->IP3D_pu() << "/" << trigBTagging->IP3D_pb()
		 << "   SV1 u/b: " << trigBTagging->SV1_pu() << "/" << trigBTagging->SV1_pb()
		 << "   MV2c20 var: " << trigBTagging->auxdata<double>("MV2c20_discriminant") 
		 << "   MV2c10 var: " << trigBTagging->auxdata<double>("MV2c10_discriminant") );

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
    ATH_MSG_DEBUG( "OUTPUT - Attached xAOD::BTaggingContainer" );
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::BTaggingContainer" << endmsg;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigVertexContainer, "HLT_BjetSecondaryVertexFex") == HLT::OK) {
    ATH_MSG_DEBUG( "OUTPUT - Attached xAOD::VertexContainer" );
  } else {
    if(msgLvl() <= MSG::ERROR) msg() << MSG::ERROR << "OUTPUT - Failed to attach xAOD::VertexContainer" << endmsg;
    return HLT::NAV_ERROR;
  }
  if(attachFeature(outputTE, trigBTagVertexContainer, "HLT_BjetVertexFex") == HLT::OK) {
    ATH_MSG_DEBUG( "OUTPUT - Attached xAOD::BTagVertexContainer" );
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

