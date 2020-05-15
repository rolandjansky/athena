/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetBtagHypoAlgMT.h"


TrigBjetBtagHypoAlgMT::TrigBjetBtagHypoAlgMT( const std::string& name, 
						ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetBtagHypoAlgMT::initialize() {

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_bTagKey          );
  ATH_MSG_DEBUG(  "   " << m_trackKey         );
  ATH_MSG_DEBUG(  "   " << m_inputPrmVtx      );
  ATH_MSG_DEBUG(  "   " << m_prmVtxLink       );


  ATH_CHECK( m_hypoTools.retrieve() );

  CHECK( m_bTagKey.initialize() );
  CHECK( m_trackKey.initialize() );
  CHECK( m_inputPrmVtx.initialize() );

  renounce( m_bTagKey );
  renounce( m_trackKey );
  renounce( m_inputPrmVtx );

  renounce( m_bTagKey );

  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

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

  // Retrieve b-tagging
  ElementLinkVector< xAOD::BTaggingContainer > btaggingELs;
  // Retrieve b-tagging here

  // Retrive Precision tracks from Event Views. We get them all in this way!
  ElementLinkVector< xAOD::TrackParticleContainer > trackELs;
  CHECK( retrieveObjectFromEventView( context,trackELs,m_trackKey,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Retrieved " << trackELs.size() << " precision tracks..." );
  auto monitor_for_track_count = Monitored::Scalar( "track_count", trackELs.size() );
  CHECK( monitor_tracks(trackELs) );

  for ( const ElementLink< xAOD::TrackParticleContainer >& trackLink : trackELs )
    ATH_MSG_DEBUG( "   * pt=" << (*trackLink)->p4().Et() << 
		   " eta=" << (*trackLink)->eta() <<
		   " phi=" << (*trackLink)->phi() );


  // Retrieve Jets
  ElementLinkVector< xAOD::JetContainer > jetELs;
  for ( const TrigCompositeUtils::Decision* previousDecision: *prevDecisionContainer ) {
    const std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > jetLinks = 
      TrigCompositeUtils::findLinks< xAOD::JetContainer >( previousDecision, TrigCompositeUtils::featureString(), TrigDefs::lastFeatureOfType);

    for ( size_t linkIndex = 0; linkIndex < jetLinks.size(); linkIndex++ ) {
      ATH_CHECK( jetLinks.at(linkIndex).isValid() );
      jetELs.push_back( jetLinks.at(linkIndex).link );
    }
  }
  ATH_MSG_DEBUG( "Retrieved " << jetELs.size() << " Jets of key " << TrigCompositeUtils::featureString() );
  auto monitor_for_jet_pt = Monitored::Collection( "jet_pt", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->pt(); } );
  auto monitor_for_jet_eta = Monitored::Collection( "jet_eta", jetELs,
    [](const ElementLink< xAOD::JetContainer >& jetLink) { return (*jetLink)->eta(); } );
  auto monitor_for_jet_count = Monitored::Scalar( "jet_count", jetELs.size() );
  auto monitor_group_for_jets = Monitored::Group( m_monTool, monitor_for_jet_pt, monitor_for_jet_eta );


  // Retrieve Vertices
  ElementLinkVector< xAOD::VertexContainer > vertexELs;
  CHECK( retrieveObjectFromStoreGate( context,vertexELs,m_inputPrmVtx ) );
  ATH_MSG_DEBUG( "Retrieved " << vertexELs.size() <<" vertices..." );
  auto monitor_for_vertex_count = Monitored::Scalar( "vertex_count", vertexELs.size() );  

  auto monitor_group_for_events = Monitored::Group( m_monTool, monitor_for_track_count, monitor_for_jet_count, monitor_for_vertex_count );


  // ==========================================================================================================================
  //    ** Prepare Outputs
  // ==========================================================================================================================

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context );
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ========================================================================================================================== 
  //    ** Compute Decisions
  // ========================================================================================================================== 
  
  const unsigned int nDecisions = prevDecisionContainer->size();

  // Create output decisions
  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
									     prevDecisionContainer->at(index),
									     "", context );
    newDecisions.push_back( toAdd );
  }

  // Adding Links
  //  CHECK( attachLinkToDecisions( context,newDecisions ) );

  for ( unsigned int index(0); index<nDecisions; index++ ) {
    // Adding b-tagging links to output decisions // TMP
    // Adding a dummy-link (a link to self) for now to satisfy validation.
    ElementLink<TrigCompositeUtils::DecisionContainer> dummyFeatureLink(*outputDecisions, index, context);
    newDecisions.at( index )->setObjectLink<TrigCompositeUtils::DecisionContainer>(TrigCompositeUtils::featureString(), dummyFeatureLink);
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

    TrigBjetBtagHypoTool::TrigBjetBtagHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    //    infoToAdd.btaggingEL = btaggingELs.at( index ); // TMP
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


StatusCode TrigBjetBtagHypoAlgMT::attachLinksToDecision( const EventContext&,
							 TrigCompositeUtils::Decision&,
							 int, int ) const {
  
  return StatusCode::SUCCESS;
}


StatusCode TrigBjetBtagHypoAlgMT::monitor_tracks( 
        const ElementLinkVector< xAOD::TrackParticleContainer >& trackELs ) const {

  auto monitor_for_track_Et = Monitored::Collection( "track_Et", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->p4().Et(); } );
  auto monitor_for_track_eta = Monitored::Collection( "track_eta", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->eta(); } ); 
  auto monitor_for_track_phi = Monitored::Collection( "track_phi", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->phi(); } );

  // Monitors for d0 and z0 track impact parameter variables
  auto monitor_for_track_d0 = Monitored::Collection( "track_d0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->d0(); } );

  auto monitor_for_track_d0err = Monitored::Collection( "track_d0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
    } );

  auto monitor_for_track_d0sig = Monitored::Collection( "track_d0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->d0() / (*trackLink)->definingParametersCovMatrix()( Trk::d0, Trk::d0 );
    } );

  auto monitor_for_track_z0 = Monitored::Collection( "track_z0", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) { return (*trackLink)->z0(); } );

  auto monitor_for_track_z0err = Monitored::Collection( "track_z0err", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
    } );

  auto monitor_for_track_z0sig = Monitored::Collection( "track_z0sig", trackELs,
    [](const ElementLink< xAOD::TrackParticleContainer >& trackLink) {
      return (*trackLink)->z0() / (*trackLink)->definingParametersCovMatrix()( Trk::z0, Trk::z0 );
    } );

  auto monitor_group_for_tracks = Monitored::Group( m_monTool, 
    monitor_for_track_Et, monitor_for_track_eta, monitor_for_track_phi,
    monitor_for_track_d0, monitor_for_track_d0err, monitor_for_track_d0sig,
    monitor_for_track_z0, monitor_for_track_z0err, monitor_for_track_z0sig
  );

  return StatusCode::SUCCESS;
}



