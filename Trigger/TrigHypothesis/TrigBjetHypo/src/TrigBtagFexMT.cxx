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

#include "AthenaMonitoringKernel/Monitored.h"
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

  ATH_CHECK( m_outputBTaggingContainerKey.initialize() );
  //  ATH_CHECK( m_outputBtagVertexContainerKey.initialize() );
  //  ATH_CHECK( m_outputVertexContainerKey.initialize() );

  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

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
  auto monitor_for_jet_count = Monitored::Scalar( "jet_count", jetContainer->size() );

  //auto monitor_for_jet_pt = Monitored::Collection( "jet_pt", *jetContainer, []( const xAOD::Jet *jet ) { return jet->pt(); } );
  auto monitor_for_jet_pt = Monitored::Collection( "jet_pt", *jetContainer, &xAOD::Jet::pt );
  auto monitor_for_jet_eta = Monitored::Collection( "jet_eta", *jetContainer, &xAOD::Jet::eta );

  for ( const xAOD::Jet* jet : *jetContainer ) {
    ATH_MSG_DEBUG( "    BTAGFEX:    ** pt=" << jet->pt() << " eta=" << jet->eta() << " phi=" << jet->phi() );
  }
  auto monitor_group_for_jets = Monitored::Group( m_monTool, monitor_for_jet_pt, monitor_for_jet_eta );


  // Test retrieval of Track Particles
  ATH_MSG_DEBUG( "Attempting to retrieve TrackParticleContainer with key " << m_trkContainerKey.key() );
  SG::ReadHandle< xAOD::TrackParticleContainer > trkContainerHandle = SG::makeHandle< xAOD::TrackParticleContainer >( m_trkContainerKey,ctx );
  CHECK( trkContainerHandle.isValid() );
  const xAOD::TrackParticleContainer *trkContainer =  trkContainerHandle.get();
  ATH_MSG_DEBUG("Retrieved " << trkContainerHandle->size() << " Tracks");
  auto monitor_for_track_count = Monitored::Scalar( "track_count", trkContainer->size() );

  auto monitor_for_track_Et = Monitored::Collection( "track_Et", *trkContainer, []( const xAOD::TrackParticle *trk ) { return trk->p4().Et(); } );
  auto monitor_for_track_eta = Monitored::Collection( "track_eta", *trkContainer, &xAOD::TrackParticle::eta );
  auto monitor_for_track_phi = Monitored::Collection( "track_phi", *trkContainer, &xAOD::TrackParticle::phi );

  // Monitors for d0 and z0 track impact parameter variables
  auto monitor_for_track_d0 = Monitored::Collection( "track_d0", *trkContainer, &xAOD::TrackParticle::d0 );
  auto monitor_for_track_d0err = Monitored::Collection( "track_d0err", *trkContainer, []( const xAOD::TrackParticle *trk ) {
    return trk->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
  } );
  auto monitor_for_track_d0sig = Monitored::Collection( "track_d0sig", *trkContainer, []( const xAOD::TrackParticle *trk ) {
    return trk->d0() / trk->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
  } );

  auto monitor_for_track_z0 = Monitored::Collection( "track_z0", *trkContainer, &xAOD::TrackParticle::z0 );
  auto monitor_for_track_z0err = Monitored::Collection( "track_z0err", *trkContainer, []( const xAOD::TrackParticle *trk ) {
    return trk->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
  } );
  auto monitor_for_track_z0sig = Monitored::Collection( "track_z0sig", *trkContainer, []( const xAOD::TrackParticle *trk ) {
    return trk->z0() / trk->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
  } );

  for ( const xAOD::TrackParticle *trk : *trkContainer ) {
    ATH_MSG_DEBUG( "  *** pt=" << trk->p4().Et() * 1e-3 << " eta=" << trk->eta() << " phi=" << trk->phi() );
  }
  auto monitor_group_for_tracks = Monitored::Group( m_monTool, 
    monitor_for_track_Et, monitor_for_track_eta, monitor_for_track_phi,
    monitor_for_track_d0, monitor_for_track_d0err, monitor_for_track_d0sig,
    monitor_for_track_z0, monitor_for_track_z0err, monitor_for_track_z0sig
  );


  // Test retrieval of VertexContainer
  ATH_MSG_DEBUG( "Attempting to retrieve VertexContainer with key " << m_VertexContainerKey.key() );
  SG::ReadHandle< xAOD::VertexContainer > vxContainerHandle = SG::makeHandle< xAOD::VertexContainer >( m_VertexContainerKey, ctx );
  CHECK( vxContainerHandle.isValid() );  
  const xAOD::VertexContainer* vxContainer = vxContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << vxContainer->size() <<" vertices..." );
  auto monitor_for_vertex_count = Monitored::Scalar( "vertex_count", vxContainer->size() );

  for ( const xAOD::Vertex *pv : *vxContainer ) {
    ATH_MSG_DEBUG( "   ** PV x=" << pv->x()<< " y=" << pv->y() << " z=" << pv->z() );
  }


  // Creating dummy B-Tagging container in order to avoid
  // warnings from the SGInputLoader
  std::unique_ptr< xAOD::BTaggingContainer > outputBtagging = std::make_unique< xAOD::BTaggingContainer >();
  std::unique_ptr< xAOD::BTaggingAuxContainer > outputBtaggingAux = std::make_unique< xAOD::BTaggingAuxContainer >();
  outputBtagging->setStore( outputBtaggingAux.get() );

  SG::WriteHandle< xAOD::BTaggingContainer > btaggingHandle = SG::makeHandle( m_outputBTaggingContainerKey,ctx );
  CHECK( btaggingHandle.record( std::move( outputBtagging ),std::move( outputBtaggingAux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << btaggingHandle->size() << " btagging objects" );

  auto monitor_group_for_events = Monitored::Group( m_monTool, monitor_for_jet_count, monitor_for_track_count, monitor_for_vertex_count );

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

