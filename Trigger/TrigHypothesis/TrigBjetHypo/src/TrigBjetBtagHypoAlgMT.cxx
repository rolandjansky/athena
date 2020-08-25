/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetBtagHypoAlgMT.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


TrigBjetBtagHypoAlgMT::TrigBjetBtagHypoAlgMT( const std::string& name, 
						ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetBtagHypoAlgMT::initialize() {

  ATH_CHECK( m_hypoTools.retrieve() );
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_CHECK( m_bTaggedJetKey.initialize() );
  ATH_CHECK( m_bTagKey.initialize() );
  ATH_CHECK( m_trackKey.initialize() );
  ATH_CHECK( m_inputPrmVtx.initialize() );

  renounce( m_bTaggedJetKey );
  renounce( m_bTagKey );
  renounce( m_trackKey );
  renounce( m_inputPrmVtx );

  return StatusCode::SUCCESS;
}


StatusCode TrigBjetBtagHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients 
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = nullptr;
  CHECK( retrievePreviousDecisionContainer( context,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // Retrive Precision tracks from Event Views. We get them all in this way!
  ElementLinkVector< xAOD::TrackParticleContainer > trackELs;
  CHECK( retrieveObjectFromEventView( context,trackELs,m_trackKey,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Retrieved " << trackELs.size() << " precision tracks..." );

  // online monitoring for tracks
  auto monitor_for_track_count = Monitored::Scalar( "track_count", trackELs.size() );
  CHECK( monitor_tracks( trackELs ) );

  for ( const ElementLink< xAOD::TrackParticleContainer >& trackLink : trackELs )
    ATH_MSG_DEBUG( "   * pt=" << (*trackLink)->p4().Et() << 
		   " eta=" << (*trackLink)->eta() <<
		   " phi=" << (*trackLink)->phi() );


  // Retrieve Jets
  ElementLinkVector< xAOD::JetContainer > jetELs;
  CHECK( retrieveCollectionFromNavigation( TrigCompositeUtils::featureString(), jetELs, prevDecisionContainer ) );  
  ATH_MSG_DEBUG( "Retrieved " << jetELs.size() << " Jets of key " << TrigCompositeUtils::featureString() );

  // online monitoring for jets
  auto monitor_for_jet_count = Monitored::Scalar( "jet_count", jetELs.size() );
  CHECK( monitor_jets( jetELs ) );


  // Retrieve Vertices
  ElementLinkVector< xAOD::VertexContainer > vertexELs;
  CHECK( retrieveObjectFromStoreGate( context,vertexELs,m_inputPrmVtx ) );
  ATH_MSG_DEBUG( "Retrieved " << vertexELs.size() <<" vertices..." );

  // opnline monitoring for vertex
  auto monitor_for_vertex_count = Monitored::Scalar( "vertex_count", vertexELs.size() );  


  auto monitor_group_for_events = Monitored::Group( m_monTool, monitor_for_track_count, monitor_for_jet_count, monitor_for_vertex_count );


  // ==========================================================================================================================
  //    ** Prepare Outputs
  // ==========================================================================================================================

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context );
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ========================================================================================================================== 
  //    ** Prepare Decisions
  // ========================================================================================================================== 
  
  const unsigned int nDecisions = prevDecisionContainer->size();

  // Create output decisions
  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
									     prevDecisionContainer->at(index),
									     "", context );

    // Attaching links to the output decisions
    // Retrieved jets from view on which we have run flavour tagging
    ElementLinkVector< xAOD::JetContainer > bTaggedJetEL;
    CHECK( retrieveCollectionFromView( context,
				       bTaggedJetEL,
				       m_bTaggedJetKey,
				       prevDecisionContainer->at(index) ) );

    if ( bTaggedJetEL.size() != 1 ) {
      ATH_MSG_ERROR( "Did not find only 1 b-tagged jet object from View!" );
      return StatusCode::FAILURE;
    }

    toAdd->setObjectLink< xAOD::JetContainer >( TrigCompositeUtils::featureString(),bTaggedJetEL.front() );     



    // Retrieve Flavour Tagging object from view
    ElementLinkVector< xAOD::BTaggingContainer > bTaggingEL;
    CHECK( retrieveCollectionFromView( context,
				       bTaggingEL,
				       m_bTagKey,
				       prevDecisionContainer->at(index) ) );
    
    if ( bTaggingEL.size() != 1 ) {
      ATH_MSG_ERROR( "Did not find only 1 b-tagging object from View!" );
      return StatusCode::FAILURE;
    }

    toAdd->setObjectLink< xAOD::BTaggingContainer >( m_bTaggingLink.value(),bTaggingEL.front() );

    // online monitoring for btagging, with a check to ensure the PV is marked
    ElementLink< xAOD::VertexContainer > vertexEL;
        CHECK( retrieveObjectFromNavigation(  m_prmVtxLink.value(), vertexEL, prevDecisionContainer->at(index) ) );
        
    if ( (*vertexEL)->vertexType() == xAOD::VxType::VertexType::PriVtx ) {
      CHECK( monitor_btagging( bTaggingEL ) );
    }




    // Add to Decision collection
    newDecisions.push_back( toAdd );
  }


  // ==========================================================================================================================
  //    ** Prepare input to Hypo Tools  
  // ==========================================================================================================================


  std::vector< TrigBjetBtagHypoTool::TrigBjetBtagHypoToolInfo > bTagHypoInputs;

  for ( unsigned int index(0); index<nDecisions; index++ ) {

    // Extract the IDs of the b-jet chains which are active.
    // Previous decision IDs.
    const TrigCompositeUtils::Decision *previousDecision = prevDecisionContainer->at(index);

    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
      TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
      TrigCompositeUtils::decisionIDs( previousDecision ).end() };

    
    // Retrieve PV from navigation
    ElementLink< xAOD::VertexContainer > vertexEL;
    CHECK( retrieveObjectFromNavigation(  m_prmVtxLink.value(), vertexEL, previousDecision ) );

    // Retrieve b-tagging code
    ElementLinkVector< xAOD::BTaggingContainer > bTaggingELs;
    CHECK( retrieveCollectionFromView< xAOD::BTaggingContainer >( context,
								  bTaggingELs,
								  m_bTagKey,
								  previousDecision ) );
    CHECK( bTaggingELs.size() == 1 );
    
    // Put everything in place
    TrigBjetBtagHypoTool::TrigBjetBtagHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    infoToAdd.btaggingEL = bTaggingELs.front();
    infoToAdd.vertexEL = vertexEL;
    infoToAdd.decision = newDecisions.at( index );
    bTagHypoInputs.push_back( infoToAdd );
  }

  // ==========================================================================================================================

  // Run on Trigger Chains
  for ( const ToolHandle< TrigBjetBtagHypoTool >& tool : m_hypoTools ) 
    CHECK( tool->decide( bTagHypoInputs ) ); 
  
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );
  ATH_MSG_DEBUG( "CHECKING OUTPUT DECISION HANDLE" );
  CHECK( hypoBaseOutputProcessing(handle) );

  // ==========================================================================================================================  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetBtagHypoAlgMT::monitor_jets( const ElementLinkVector<xAOD::JetContainer >& jetELs ) const {

  auto monitor_for_jet_pt = Monitored::Collection( "jet_pt", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->pt() / 1000.0 /*Gev*/; } );
  auto monitor_for_jet_eta = Monitored::Collection( "jet_eta", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->eta(); } );
  auto monitor_for_jet_phi = Monitored::Collection( "jet_phi", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->phi(); } );

  auto monitor_group_for_jets = Monitored::Group( m_monTool, 
     monitor_for_jet_pt, monitor_for_jet_eta, monitor_for_jet_phi
  );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetBtagHypoAlgMT::monitor_tracks( const ElementLinkVector< xAOD::TrackParticleContainer >& trackELs ) const {

  auto monitor_for_track_Et = Monitored::Collection( "track_Et", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->p4().Et() / 1000.0 /*Gev*/; } );
  auto monitor_for_track_eta = Monitored::Collection( "track_eta", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->eta(); } ); 
  auto monitor_for_track_phi = Monitored::Collection( "track_phi", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->phi(); } );

  // Monitors for d0 and z0 track impact parameter variables
  auto monitor_for_track_d0 = Monitored::Collection( "track_d0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->d0(); } );

  auto monitor_for_track_d0err = Monitored::Collection( "track_d0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
	    return Amg::error( (*trackLink)->definingParametersCovMatrix(), 0);
    } );

  auto monitor_for_track_d0sig = Monitored::Collection( "track_d0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->d0() / Amg::error( (*trackLink)->definingParametersCovMatrix(), 0);
    } );

  auto monitor_for_track_z0 = Monitored::Collection( "track_z0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->z0(); } );

  auto monitor_for_track_z0err = Monitored::Collection( "track_z0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
	    return Amg::error( (*trackLink)->definingParametersCovMatrix(), 1);
    } );

  auto monitor_for_track_z0sig = Monitored::Collection( "track_z0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->z0() / Amg::error( (*trackLink)->definingParametersCovMatrix(), 1);
    } );

  auto monitor_group_for_tracks = Monitored::Group( m_monTool, 
    monitor_for_track_Et, monitor_for_track_eta, monitor_for_track_phi,
    monitor_for_track_d0, monitor_for_track_d0err, monitor_for_track_d0sig,
    monitor_for_track_z0, monitor_for_track_z0err, monitor_for_track_z0sig
  );

  return StatusCode::SUCCESS;
}


StatusCode TrigBjetBtagHypoAlgMT::monitor_flavor_tag_variable( const ElementLinkVector< xAOD::BTaggingContainer >& bTaggingEL, const std::string& var_name ) const {
  auto monitor_pu = Monitored::Collection( "btag_"+var_name+"_pu", bTaggingEL,
    [var_name](const ElementLink< xAOD::BTaggingContainer >& bTagLink) { 
      double pu = -1; 
      (*bTagLink)->pu( var_name, pu );
      return pu; 
    } );

  auto monitor_pb = Monitored::Collection( "btag_"+var_name+"_pb", bTaggingEL,
    [var_name](const ElementLink< xAOD::BTaggingContainer >& bTagLink) { 
      double pb = -1;
      (*bTagLink)->pb( var_name, pb );
      return pb;
    } );

  auto monitor_pc = Monitored::Collection( "btag_"+var_name+"_pc", bTaggingEL,
    [var_name](const ElementLink< xAOD::BTaggingContainer >& bTagLink) { 
      double pc = -1;
      (*bTagLink)->pc( var_name, pc );
      return pc;
    } );

  auto monitor_group_for_flavor_tag_var = Monitored::Group( m_monTool, monitor_pu, monitor_pb, monitor_pc );

  return StatusCode::SUCCESS;
}
  

StatusCode TrigBjetBtagHypoAlgMT::monitor_btagging( const ElementLinkVector< xAOD::BTaggingContainer >& bTaggingEL ) const {
  // Monitor IP2D, IP3D, SV1, DL1r, RNNIP
  CHECK( monitor_flavor_tag_variable(bTaggingEL, "IP2D") );
  CHECK( monitor_flavor_tag_variable(bTaggingEL, "IP3D") );
  CHECK( monitor_flavor_tag_variable(bTaggingEL, "SV1") );
  CHECK( monitor_flavor_tag_variable(bTaggingEL, "DL1r") );
  CHECK( monitor_flavor_tag_variable(bTaggingEL, "RNNIP") );

  // Monitor MV2c10
  auto monitor_for_MV2c10 = Monitored::Collection( "btag_MV2c10", bTaggingEL,
    [](const ElementLink< xAOD::BTaggingContainer >& bTagLink) { 
      double mv2c10_discriminant = -2;
      (*bTagLink)->MVx_discriminant( "MV2c10", mv2c10_discriminant );
      return mv2c10_discriminant;
    } );
  auto monitor_group_for_btagging = Monitored::Group( m_monTool, monitor_for_MV2c10);


  // Monitor tracks used in calculating btagging quantities
  for ( const ElementLink< xAOD::BTaggingContainer >& bTagLink : bTaggingEL ) {
    auto monitor_for_btag_valD0wrtPVofTracks = Monitored::Collection( "btag_valD0wrtPVofTracks",
      (*bTagLink)->auxdata< std::vector<float> >("IP3D_valD0wrtPVofTracks"));
    auto monitor_for_btag_sigD0wrtPVofTracks = Monitored::Collection( "btag_sigD0wrtPVofTracks",
      (*bTagLink)->auxdata< std::vector<float> >("IP3D_sigD0wrtPVofTracks"));
    auto monitor_for_btag_valZ0wrtPVofTracks = Monitored::Collection( "btag_valZ0wrtPVofTracks",
      (*bTagLink)->auxdata< std::vector<float> >("IP3D_valZ0wrtPVofTracks"));
    auto monitor_for_btag_sigZ0wrtPVofTracks = Monitored::Collection( "btag_sigZ0wrtPVofTracks",
      (*bTagLink)->auxdata< std::vector<float> >("IP3D_sigZ0wrtPVofTracks"));

    auto monitor_group_for_btag_track_vars = Monitored::Group( m_monTool,
      monitor_for_btag_valD0wrtPVofTracks, monitor_for_btag_sigD0wrtPVofTracks,
      monitor_for_btag_valZ0wrtPVofTracks, monitor_for_btag_sigZ0wrtPVofTracks
    );
  }



  return StatusCode::SUCCESS;
}

