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

#include "StoreGate/ReadDecorHandle.h"

// ----------------------------------------------------------------------------------------------------------------- 


TrigBtagFexMT::TrigBtagFexMT(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {
}


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
  ATH_CHECK( m_d0ContainerKey.initialize() );
  ATH_CHECK( m_z0SinThetaContainerKey.initialize() );
  ATH_CHECK( m_d0UncertaintyContainerKey.initialize() );
  ATH_CHECK( m_z0SinThetaUncertaintyContainerKey.initialize() );
  ATH_CHECK( m_trackposContainerKey.initialize() );
  ATH_CHECK( m_trackmomContainerKey.initialize() );

  ATH_CHECK( m_BTaggingContainerKey.initialize() );
  //  ATH_CHECK( m_outputBtagVertexContainerKey.initialize() );
  //  ATH_CHECK( m_outputVertexContainerKey.initialize() );


  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBtagFexMT::execute() {
  ATH_MSG_DEBUG( "Executing " << name() << "... " );
  
  const EventContext& ctx = getContext();

  // Test retrieval of JetContainer
  ATH_MSG_DEBUG( "Attempting to retrieve JetContainer with key " << m_JetContainerKey.key() );
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle< xAOD::JetContainer >( m_JetContainerKey,ctx );
  CHECK( jetContainerHandle.isValid() );
  const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << jetContainer->size() << " jets" );

  for ( const xAOD::Jet* jet : *jetContainer ) {
    ATH_MSG_DEBUG( "    BTAGFEX:    ** pt=" << jet->pt() << " eta=" << jet->eta() << " phi=" << jet->phi() );
  }


  // Test retrieval of Track Particles
  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_trkContainerKey.key() );
  SG::ReadHandle< xAOD::TrackParticleContainer > trkContainerHandle = SG::makeHandle< xAOD::TrackParticleContainer >( m_trkContainerKey,ctx );
  CHECK( trkContainerHandle.isValid() );
  const xAOD::TrackParticleContainer *trkContainer =  trkContainerHandle.get();
  ATH_MSG_DEBUG("Retrieved " << trkContainerHandle->size() << " Tracks");

  for ( const xAOD::TrackParticle *trk : *trkContainer ) 
    ATH_MSG_DEBUG( "  *** pt=" << trk->p4().Et() * 1e-3 <<
		   " eta=" << trk->eta() <<
		   " phi=" << trk->phi() <<
		   " d0=" << trk->d0() <<
		   " z0=" << trk->z0() );



  // Test retrieval of properties decorated in BTagTrackAugmenterAlg
  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_d0ContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, float > trkContainer_d0_Handle ( m_d0ContainerKey,ctx );
  CHECK( trkContainer_d0_Handle.isValid() );
  ATH_MSG_DEBUG("trkContainerHandle->size(): " << trkContainerHandle->size() << "   trkContainer_d0_Handle->size(): " << trkContainer_d0_Handle->size() );
  for ( long unsigned int i=0; i< trkContainer_d0_Handle->size(); i++)
    ATH_MSG_DEBUG("Track " << i <<": d0 =" << trkContainer_d0_Handle(i));

  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_z0SinThetaContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, float > trkContainer_z0SinTheta_Handle ( m_z0SinThetaContainerKey,ctx );
  CHECK( trkContainer_z0SinTheta_Handle.isValid() );
  for ( long unsigned int i=0; i< trkContainer_z0SinTheta_Handle->size(); i++)
    ATH_MSG_DEBUG("Track " << i <<": z0SinTheta =" << trkContainer_z0SinTheta_Handle(i));

  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_d0UncertaintyContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, float > trkContainer_d0Uncertainty_Handle ( m_d0UncertaintyContainerKey,ctx );
  CHECK( trkContainer_d0Uncertainty_Handle.isValid() );
  for ( long unsigned int i=0; i< trkContainer_d0Uncertainty_Handle->size(); i++)
    ATH_MSG_DEBUG("Track " << i << ": d0Uncertainty =" << trkContainer_d0Uncertainty_Handle(i));

  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_z0SinThetaUncertaintyContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, float > trkContainer_z0SinThetaUncertainty_Handle ( m_z0SinThetaUncertaintyContainerKey,ctx );
  CHECK( trkContainer_z0SinThetaUncertainty_Handle.isValid() );
  for ( long unsigned int i=0; i< trkContainer_z0SinThetaUncertainty_Handle->size(); i++)
    ATH_MSG_DEBUG("Track " << i << ": z0SinThetaUncertainty =" << trkContainer_z0SinThetaUncertainty_Handle(i));

  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_trackposContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, std::vector< float > > trkContainer_trackpos_Handle ( m_trackposContainerKey,ctx );
  CHECK( trkContainer_trackpos_Handle.isValid() );
  for ( long unsigned int i=0; i< trkContainer_trackpos_Handle->size(); i++){
    ATH_MSG_DEBUG("Track " << i << ": Displacement (x,y,z) = ");
    for (auto pos:  trkContainer_trackpos_Handle(i)){
      ATH_MSG_DEBUG( pos << ' ');
    }
  }
    
  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_trackmomContainerKey.key() );
  SG::ReadDecorHandle< xAOD::TrackParticleContainer, std::vector< float > > trkContainer_trackmom_Handle ( m_trackmomContainerKey,ctx );
  CHECK( trkContainer_trackmom_Handle.isValid() );
  for ( long unsigned int i=0; i< trkContainer_trackmom_Handle->size(); i++){
    ATH_MSG_DEBUG("Track " << i << ": Momentum = ");
    for (auto mom:  trkContainer_trackmom_Handle(i)){
      ATH_MSG_DEBUG( mom << ' ');
    }
  }

  // Test retrieval of VertexContainer
  ATH_MSG_DEBUG( "Attempting to retrieve VertexContainer with key " << m_VertexContainerKey.key() );
  SG::ReadHandle< xAOD::VertexContainer > vxContainerHandle = SG::makeHandle< xAOD::VertexContainer >( m_VertexContainerKey, ctx );
  CHECK( vxContainerHandle.isValid() );  
  const xAOD::VertexContainer* vxContainer = vxContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << vxContainer->size() <<" vertices..." );
  for ( const xAOD::Vertex *pv : *vxContainer ) {
    ATH_MSG_DEBUG( "   ** PV x=" << pv->x()<< " y=" << pv->y() << " z=" << pv->z() );
  }
/*
  // Test retrieval of B-Tagging container
  ATH_MSG_DEBUG( "Attempting to retrieve B-Tagging container with key " << m_BTaggingContainerKey.key() );
  SG::ReadHandle< xAOD::BTaggingContainer > btaggingHandle = SG::makeHandle< xAOD::BTaggingContainer >( m_BTaggingContainerKey,ctx );
  CHECK( btaggingHandle.isValid() );
  const xAOD::BTaggingContainer* btaggingContainer = btaggingHandle.get();
  ATH_MSG_DEBUG( "Exiting with " << btaggingContainer->size() <<" btagging objects" );

  xAOD::BTagging *toAdd = new xAOD::BTagging();
  outputBtagging->push_back( toAdd );

  SG::WriteHandle< xAOD::BTaggingContainer > btaggingHandle = SG::makeHandle( m_outputBTaggingContainerKey,ctx );
  CHECK( btaggingHandle.record( std::move( outputBtagging ),std::move( outputBtaggingAux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << btaggingHandle->size() << " btagging objects" );
*/
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


