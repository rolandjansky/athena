/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT.h"
#include "AthViews/ViewHelper.h"

TrigBjetEtHypoAlgMT::TrigBjetEtHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}


StatusCode TrigBjetEtHypoAlgMT::initialize() {

  ATH_MSG_DEBUG(  "declareProperty review:"    );
  ATH_MSG_DEBUG(  "   " << m_roiLink           );
  ATH_MSG_DEBUG(  "   " << m_prmVtxLink        );

  ATH_CHECK( m_hypoTools.retrieve()   );

  CHECK( m_inputJetsKey.initialize()       );
  CHECK( m_inputRoIKey.initialize()        );
  CHECK( m_inputPrmVtx.initialize()        );

  renounce( m_inputJetsKey );
  renounce( m_inputRoIKey );
  if ( m_retrieveVertexFromEventView == true ) renounce( m_inputPrmVtx );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  // The decisions come from the Jet group
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = nullptr;
  CHECK( retrievePreviousDecisionContainer( context,prevDecisionContainer ) );
  ATH_MSG_DEBUG( "Found a total of " << prevDecisionContainer->size() << " previous decisions!" );

  // Retrieve Jets
  // These are the jets coming from the Jet selector in which RoIs have been constructed
  // with PV constraint
  ElementLinkVector< xAOD::JetContainer > jetELs;
  CHECK( retrieveObjectFromEventView( context,jetELs,m_inputJetsKey,prevDecisionContainer ) );

  ATH_MSG_DEBUG( "Found " << jetELs.size()<< " jets."  );
  for ( const ElementLink< xAOD::JetContainer >& jetLink : jetELs ) 
    ATH_MSG_DEBUG( "   -- Jet pt=" << (*jetLink)->p4().Et() 
		   <<" eta="<< (*jetLink)->eta() 
		   << " phi="<< (*jetLink)->phi() );
  
  // The following is just for debugging. 
  // Retrieve RoI (to be linked to the output decision)
  ElementLinkVector< TrigRoiDescriptorCollection > roiELs;
  CHECK( retrieveObjectFromEventView( context,roiELs,m_inputRoIKey,prevDecisionContainer ) );
  
  ATH_MSG_DEBUG( "Found " << roiELs.size() << " rois." );
  for ( const ElementLink< TrigRoiDescriptorCollection >& roiLink : roiELs )
    ATH_MSG_DEBUG( "   -- RoI eta=" << (*roiLink)->eta() << 
		   " phi=" << (*roiLink)->phi() );

  // The following is just for debugging.
  // Retrieve PVs 
  ElementLinkVector< xAOD::VertexContainer > vertexELs;
  if ( m_retrieveVertexFromEventView == true )
    CHECK( retrieveObjectFromEventView( context,vertexELs,m_inputPrmVtx,prevDecisionContainer ) );
  else
    CHECK( retrieveObjectFromStoreGate( context,vertexELs,m_inputPrmVtx ) );

  ATH_MSG_DEBUG( "Found " << vertexELs.size() <<" vertexes." );
  for ( const ElementLink< xAOD::VertexContainer >& vertexLink : vertexELs )
    ATH_MSG_DEBUG( "  -- PV x="<<  (*vertexLink)->x() << 
		   " y=" << (*vertexLink)->y() << 
		   " z=" << (*vertexLink)->z() );

  // ========================================================================================================================== 
  //    ** Prepare Outputs
  // ========================================================================================================================== 

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context ); 
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ==========================================================================================================================
  //    ** Compute Decisions
  // ==========================================================================================================================

  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  /*
  for ( const TrigCompositeUtils::Decision* inputDecision : *prevDecisionContainer ) {
    
    int index = 0;
    if ( index < 0 ) continue;
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
                                                                             inputDecision,
                                                                             "", context );

    // Adding Links 
    CHECK( attachLinksToDecision( context,*toAdd,index ) ); 
    newDecisions.push_back( toAdd );    
  }
  */
  
  // Create output decisions
  for ( unsigned int index(0); index < jetELs.size(); index++ ) {

    // Find correct previous decision
    const TrigCompositeUtils::Decision *previousDecision = prevDecisionContainer->at(0);

    // Create new decisions
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
									     previousDecision,
									     "", context );

    // find index of PV 
    int PVindex = -1;

    for ( unsigned int pvi(0); pvi < vertexELs.size(); pvi++ ) {
      const ElementLink< xAOD::VertexContainer >& vertexLink = vertexELs.at( pvi );
      if ( (*vertexLink)->vertexType() != xAOD::VxType::VertexType::PriVtx ) continue;
      PVindex = pvi;
      break;
    }

    if ( PVindex == -1 ) {
      ATH_MSG_ERROR( "Primary Vertex could not be found!" );
      return StatusCode::FAILURE;
    }

    // Adding Links 
    CHECK( attachLinksToDecision( context,*toAdd,index,PVindex ) );
    newDecisions.push_back( toAdd );
  }


  // ==========================================================================================================================
  //    ** Prepare input to Hypo Tools
  // ========================================================================================================================== 

  std::vector< TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo > bJetHypoInputs;

  for ( TrigCompositeUtils::Decision* newDecision : newDecisions ) {
    // Check navigation is set properly
    if ( not newDecision->hasObjectCollectionLinks( TrigCompositeUtils::seedString() ) ) {
      ATH_MSG_ERROR( "Trying to extract the IDs of the b-jet chains which are active." );
      ATH_MSG_ERROR( "But output decision has no link to seed collection." );
      return StatusCode::FAILURE;
    }
    // Check jets are attached properly
    if ( not newDecision->hasObjectLink( TrigCompositeUtils::featureString() ) ) {
      ATH_MSG_ERROR( "Trying to extract the associate jet to the output deicions." );
      ATH_MSG_ERROR( "But output decision has no link to jet." );
      return StatusCode::FAILURE;
    }
    
    // Get Parent Decision
    const ElementLinkVector< TrigCompositeUtils::DecisionContainer > mySeeds = newDecision->objectCollectionLinks< TrigCompositeUtils::DecisionContainer >( TrigCompositeUtils::seedString() );
    // Get Associated Jet
    const ElementLink< xAOD::JetContainer > myJet = newDecision->objectLink< xAOD::JetContainer >( TrigCompositeUtils::featureString() ); 
    CHECK( myJet.isValid() );

    // Extract the IDs of the b-jet chains which are active.
    // Previous decision IDs.
    const TrigCompositeUtils::Decision *previousDecision = *mySeeds.back();

    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
      TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
	TrigCompositeUtils::decisionIDs( previousDecision ).end()
	};

    TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    infoToAdd.jetEL = myJet; 
    infoToAdd.decision = newDecision;
    bJetHypoInputs.push_back( infoToAdd );
  }

  // ========================================================================================================================== 

  // Run on Trigger Chains
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) 
    CHECK( tool->decide( bJetHypoInputs ) );
  
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );
  ATH_MSG_DEBUG( "CHECKING OUTPUT DECISION HANDLE" );
  CHECK( hypoBaseOutputProcessing(handle) );

  // ==========================================================================================================================  

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::attachLinksToDecision( const EventContext& context,
						       TrigCompositeUtils::Decision& toAdd,
						       int index,
						       int indexPrmVertex ) const {

  // Do the linking...
  // Link Jet
  CHECK( attachObjectLinkToDecisionFromEventView( context,
						  toAdd,
						  m_inputJetsKey,
						  TrigCompositeUtils::featureString(),
						  index ) );

  // Link RoI
  CHECK( attachObjectLinkToDecisionFromEventView( context,
						  toAdd,
						  m_inputRoIKey,
						  m_roiLink.value(),
						  index ) );

  // Link Vertex
  if ( m_retrieveVertexFromEventView )
    CHECK( attachObjectLinkToDecisionFromEventView( context,
						    toAdd,
						    m_inputPrmVtx,
						    m_prmVtxLink.value(),
						    indexPrmVertex ) );
  else
    CHECK( attachObjectLinkToDecisionFromStoreGate( toAdd,
						    m_inputPrmVtx,
						    m_prmVtxLink.value(),
						    indexPrmVertex ) );

  ATH_MSG_DEBUG("   ** Added object links to output decision");
  return StatusCode::SUCCESS;
}

