/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetBtagHypoAlgMT.h"


TrigBjetBtagHypoAlgMT::TrigBjetBtagHypoAlgMT( const std::string& name, 
						ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetBtagHypoAlgMT::initialize() {

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_bTagKey          );
  ATH_MSG_DEBUG(  "   " << m_trackKey         );

  //  ATH_MSG_DEBUG( "    " << m_trackLink        );

  ATH_CHECK( m_hypoTools.retrieve() );

  CHECK( m_bTagKey.initialize() );
  CHECK( m_trackKey.initialize() );

  renounce( m_bTagKey );
  renounce( m_trackKey );

  renounce( m_bTagKey );

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

  for ( const ElementLink< xAOD::TrackParticleContainer >& trackLink : trackELs )
    ATH_MSG_DEBUG( "   * pt=" << (*trackLink)->p4().Et() << 
		   " eta=" << (*trackLink)->eta() <<
		   " phi=" << (*trackLink)->phi() );

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
