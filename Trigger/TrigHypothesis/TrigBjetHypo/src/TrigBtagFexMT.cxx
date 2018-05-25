/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBtagFexMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// ************************************************

#include "src/TrigBtagFexMT.h"

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


TrigBtagFexMT::TrigBtagFexMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}


// ----------------------------------------------------------------------------------------------------------------- 

TrigBtagFexMT::~TrigBtagFexMT() {}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBtagFexMT::initialize() {

  // Get message service 
  ATH_MSG_INFO( "Initializing TrigBtagFexMT, version " << PACKAGE_VERSION );

  // declareProperty overview 
  ATH_MSG_DEBUG( "declareProperty review:" );
  ATH_MSG_DEBUG( " UseBeamSpotFlag = "     << m_useBeamSpotFlag );
  ATH_MSG_DEBUG( " Offline Taggers = "     << m_TaggerBaseNames );
  
  ATH_MSG_DEBUG( "Initialising ReadHandleKeys" );
  ATH_CHECK( m_JetContainerKey.initialize()           );
  ATH_CHECK( m_VertexContainerKey.initialize()        );
  ATH_CHECK( m_BackUpVertexContainerKey.initialize()  );
  ATH_CHECK( m_TrackParticleContainerKey.initialize() );

  ATH_CHECK( m_outputBTaggingContainerKey.initialize() );
  ATH_CHECK( m_outputBtagVertexContainerKey.initialize() );
  ATH_CHECK( m_outputVertexContainerKey.initialize() );

  if ( m_setupOfflineTools == true ) {
    // Retrieve the offline track association tool
    if ( not m_bTagTrackAssocTool.name().empty() )
      ATH_CHECK( m_bTagTrackAssocTool.retrieve() );
    ATH_CHECK( m_bTagSecVtxTool.retrieve() );
    ATH_CHECK( m_bTagTool.retrieve() );
  }

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBtagFexMT::execute() {
  ATH_MSG_DEBUG( "Executing TrigBtagFexMT" );

  // RETRIEVE INPUT CONTAINERS
  const EventContext& ctx = getContext();
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_JetContainerKey,ctx );
  SG::ReadHandle< xAOD::VertexContainer > prmVtxContainerHandle = SG::makeHandle( m_VertexContainerKey,ctx );
  //  SG::ReadHandle< xAOD::VertexContainer > backupPrmVtxContainerHandle = SG::makeHandle( m_BackUpVertexContainerKey,ctx );
  SG::ReadHandle< xAOD::TrackParticleContainer > trkParticlesHandle = SG::makeHandle( m_TrackParticleContainerKey,ctx );

  // Temporary comment these lines
  //  const xAOD::VertexContainer* vertexes = nullptr;
  //  const xAOD::Vertex* primaryVertex     = nullptr;

  bool usePVBackup = true;

  /*
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
  */

  if ( usePVBackup ) {
    ATH_MSG_INFO( "INPUT - xAOD::VertexContainer: NO valid vertex found in " << m_BackUpVertexContainerKey << " - aborting..." );
    return StatusCode::FAILURE;
  }

  // PREPARE PROCESSING AND OUTPUT CONTAINERS
  // Prepare jet tagging - create temporary jet copy  
  xAOD::JetContainer::const_iterator jetitr = jetContainerHandle->begin();
  xAOD::Jet jet;
  jet.makePrivateStore(**jetitr);

  // Prepare jet tagging - create SV output 
  std::unique_ptr< xAOD::VertexContainer > trigVertexContainer( new xAOD::VertexContainer() );
  std::unique_ptr< xAOD::VertexAuxContainer > trigSecVertexAuxContainer( new xAOD::VertexAuxContainer() );
  trigVertexContainer->setStore( trigSecVertexAuxContainer.get() );

  // Prepare jet tagging - create JF output 
  std::unique_ptr< xAOD::BTagVertexContainer > trigBTagVertexContainer( new xAOD::BTagVertexContainer() );
  std::unique_ptr< xAOD::VertexAuxContainer > trigBTagSecVertexAuxContainer( new xAOD::VertexAuxContainer() );
  trigBTagVertexContainer->setStore( trigBTagSecVertexAuxContainer.get() );

  // Prepare jet tagging - create BTagging output 
  std::unique_ptr< xAOD::BTaggingContainer > trigBTaggingContainer( new xAOD::BTaggingContainer() );
  std::unique_ptr< xAOD::BTaggingAuxContainer > trigBTaggingAuxContainer( new xAOD::BTaggingAuxContainer() );
  trigBTaggingContainer->setStore( trigBTaggingAuxContainer.get() );

  xAOD::BTagging *trigBTagging = new xAOD::BTagging();
  trigBTaggingContainer->push_back(trigBTagging);

  // EXECUTE OFFLINE TOOLS
  if ( m_setupOfflineTools == true ) {
    /*  
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
    */
  }

  // Dump results 
  ATH_MSG_DEBUG( "IP2D u/b: " << trigBTagging->IP2D_pu() << "/" << trigBTagging->IP2D_pb()
		 << "   IP3D u/b: " << trigBTagging->IP3D_pu() << "/" << trigBTagging->IP3D_pb()
		 << "   SV1 u/b: " << trigBTagging->SV1_pu() << "/" << trigBTagging->SV1_pb()
		 << "   MV2c20 var: " << trigBTagging->auxdata<double>("MV2c20_discriminant") 
		 << "   MV2c10 var: " << trigBTagging->auxdata<double>("MV2c10_discriminant") );

  // ATTACH FEATURES AND CLEAR TEMPORARY OBJECTS

  // Temporary comment these lines
  // Create element link from the BTagging to the Jet container
  //  auto &jetAssociationLinks = trigBTagging->auxdata< std::vector< ElementLink< xAOD::IParticleContainer > > >("BTagBtagToJetAssociator");
  /*
  if ( jets ) {
    jetAssociationLinks.resize ( jets->size() );
    for( size_t i = 0; i < jets->size(); ++i ) {
      jetAssociationLinks[i].toIndexedElement (*jets, i);
    }
  }
  */

  SG::WriteHandle< xAOD::BTaggingContainer > outputBTaggingContainerHandle = SG::makeHandle( m_outputBTaggingContainerKey,ctx );
  ATH_CHECK( outputBTaggingContainerHandle.record( std::move(trigBTaggingContainer),std::move(trigBTaggingAuxContainer) ) );

  SG::WriteHandle< xAOD::VertexContainer > outputVtxContainerHandle = SG::makeHandle( m_outputVertexContainerKey,ctx );
  ATH_CHECK( outputVtxContainerHandle.record( std::move(trigVertexContainer),std::move(trigSecVertexAuxContainer) ) );

  SG::WriteHandle< xAOD::BTagVertexContainer > outputBtagVtxContainerHandle = SG::makeHandle( m_outputBtagVertexContainerKey,ctx );  
  ATH_CHECK( outputBtagVtxContainerHandle.record( std::move(trigBTagVertexContainer),std::move(trigBTagSecVertexAuxContainer) ) );

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 


StatusCode TrigBtagFexMT::finalize() {
  ATH_MSG_INFO( "Finalizing TrigBtagFex" );
  return StatusCode::SUCCESS;
}

const xAOD::Vertex* TrigBtagFexMT::getPrimaryVertex(const xAOD::VertexContainer* vertexContainer) {
  // vertex types are listed on L328 of 
  // https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTracking/trunk/xAODTracking/TrackingPrimitives.h

  for( auto vtx_itr : *vertexContainer ) {
    if(vtx_itr->vertexType() != xAOD::VxType::VertexType::PriVtx) { continue; }
    return vtx_itr;
  }
  ATH_MSG_WARNING( "No primary vertex found." );
  return nullptr;
}

