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
  ATH_MSG_DEBUG( "   "     << m_useBeamSpotFlag              );
  ATH_MSG_DEBUG( "   "     << m_TaggerBaseNames              );
  ATH_MSG_DEBUG( "   "     << m_JetContainerKey              );
  ATH_MSG_DEBUG( "   "     << m_VertexContainerKey           );
  ATH_MSG_DEBUG( "   "     << m_BackUpVertexContainerKey     );
  ATH_MSG_DEBUG( "   "     << m_TrackParticleContainerKey    );
  ATH_MSG_DEBUG( "   "     << m_outputBTaggingContainerKey   );
  ATH_MSG_DEBUG( "   "     << m_outputVertexContainerKey     );
  ATH_MSG_DEBUG( "   "     << m_outputBtagVertexContainerKey );
  
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

