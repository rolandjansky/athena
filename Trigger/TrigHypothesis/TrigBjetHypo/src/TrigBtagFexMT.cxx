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
  ATH_MSG_DEBUG( "declareProperty review:"                   );
  /*
  ATH_MSG_DEBUG( "   "     << m_useBeamSpotFlag              );
  ATH_MSG_DEBUG( "   "     << m_TaggerBaseNames              );
  ATH_MSG_DEBUG( "   "     << m_JetContainerKey              );
  ATH_MSG_DEBUG( "   "     << m_VertexContainerKey           );
  ATH_MSG_DEBUG( "   "     << m_BackUpVertexContainerKey     );
  ATH_MSG_DEBUG( "   "     << m_TrackParticleContainerKey    );
  ATH_MSG_DEBUG( "   "     << m_outputBTaggingContainerKey   );
  ATH_MSG_DEBUG( "   "     << m_outputVertexContainerKey     );
  ATH_MSG_DEBUG( "   "     << m_outputBtagVertexContainerKey );
  */

  ATH_MSG_DEBUG( "Initialising ReadHandleKeys" );
  ATH_CHECK( m_JetContainerKey.initialize()           );
  ATH_CHECK( m_VertexContainerKey.initialize()        );
  ATH_CHECK( m_trkContainerKey.initialize() );

  ATH_CHECK( m_outputBTaggingContainerKey.initialize() );
  //  ATH_CHECK( m_outputBtagVertexContainerKey.initialize() );
  //  ATH_CHECK( m_outputVertexContainerKey.initialize() );

  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBtagFexMT::execute() {

  const EventContext& ctx = getContext();
  
  ATH_MSG_DEBUG( "Executing TrigBtagFexMT" );
  ATH_MSG_DEBUG("--- BTAGFEX: EXECUTING");
  

  // Test retrieval of JetContainer
  SG::ReadHandle<xAOD::JetContainer> jetContainer (m_JetContainerKey);
  if (!jetContainer.isValid()) {
    ATH_MSG_ERROR(" cannot retrieve JetContainer with key " << m_JetContainerKey.key());
    return StatusCode::FAILURE;
  }
  if (jetContainer->size() == 0) {
    ATH_MSG_DEBUG("    BTAGFEX: JetContainer --> empty!");
  }
  else {
    ATH_MSG_DEBUG("    BTAGFEX: JetContainer --> n = " << jetContainer->size());
  }

  // Test retrieval of VertexContainer
  const xAOD::VertexContainer* vxContainer (0);
  StatusCode sc = evtStore()->retrieve(vxContainer, m_VertexContainerKey.key());
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" cannot retrieve Vertex Container with key " << m_VertexContainerKey.key());
    return StatusCode::SUCCESS;
  }
  if (vxContainer->size()==0) {
    ATH_MSG_DEBUG("    BTAGFEX: VertexContainer --> empty!");
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG("    BTAGFEX: VertexContainer --> n = " << vxContainer->size());
  }



  // Test retrieval of Track Particles
  SG::ReadHandle< xAOD::TrackParticleContainer > trkContainerHandle = SG::makeHandle( m_trkContainerKey,ctx );
  CHECK( trkContainerHandle.isValid() );
  ATH_MSG_DEBUG("Retrieved " << trkContainerHandle->size() << " Tracks");

  const xAOD::TrackParticleContainer *trkContainer =  trkContainerHandle.get();
  
  for ( auto trk : *trkContainer ) {
    ATH_MSG_DEBUG( "  *** pt=" << trk->p4().Et() );
  }


  // Creating dummy B-Tagging container in order to avoid
  // warnings from the SGInputLoader
  std::unique_ptr< xAOD::BTaggingContainer > outputBtagging = std::make_unique< xAOD::BTaggingContainer >();
  std::unique_ptr< xAOD::BTaggingAuxContainer > outputBtaggingAux = std::make_unique< xAOD::BTaggingAuxContainer >();
  outputBtagging->setStore( outputBtaggingAux.get() );

  SG::WriteHandle< xAOD::BTaggingContainer > btaggingHandle = SG::makeHandle( m_outputBTaggingContainerKey,ctx );
  CHECK( btaggingHandle.record( std::move( outputBtagging ),std::move( outputBtaggingAux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << btaggingHandle->size() << " btagging objects" );


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

