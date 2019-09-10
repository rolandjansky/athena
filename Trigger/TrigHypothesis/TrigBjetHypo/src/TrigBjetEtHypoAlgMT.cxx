/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT.h"
#include "AthViews/ViewHelper.h"

TrigBjetEtHypoAlgMT::TrigBjetEtHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  TrigBjetHypoAlgBaseMT( name, pSvcLocator ) {}

TrigBjetEtHypoAlgMT::~TrigBjetEtHypoAlgMT() {}


StatusCode TrigBjetEtHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"    );
  ATH_MSG_DEBUG(  "   " << m_roiLink           );
  ATH_MSG_DEBUG(  "   " << m_prmVtxLink        );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve()   );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_inputJetsKey.initialize()       );
  CHECK( m_inputRoIKey.initialize()        );
  CHECK( m_inputPrmVtx.initialize()        );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::execute( const EventContext& context ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = nullptr;
  CHECK( retrievePreviousDecisionContainer( context,prevDecisionContainer ) );

  // Retrieve Jets
  ElementLinkVector< xAOD::JetContainer > jetELs;
  CHECK( retrieveJets( context,jetELs,m_inputJetsKey,prevDecisionContainer ) ); 

  ATH_MSG_DEBUG( "Found " << jetELs.size()<< " jets."  );
  for ( const ElementLink< xAOD::JetContainer >& jetLink : jetELs ) 
    ATH_MSG_DEBUG("   -- Jet pt=" << (*jetLink)->p4().Et() 
		  <<" eta="<< (*jetLink)->eta() 
		  << " phi="<< (*jetLink)->phi() );


  // Retrieve RoI (to be linked to the output decision)
  const TrigRoiDescriptorCollection *roiContainer = nullptr;
  if ( m_roiLink.value() != "Undefined" )
    CHECK( retrieveRoIs( context,roiContainer,m_inputRoIKey ) );

  // ========================================================================================================================== 
  //    ** Prepare Outputs
  // ========================================================================================================================== 

  // Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle = TrigCompositeUtils::createAndStore( decisionOutput(), context ); 
  TrigCompositeUtils::DecisionContainer *outputDecisions = handle.ptr();

  // ==========================================================================================================================
  //    ** Compute Decisions
  // ==========================================================================================================================

  // We need nDecisions (one per RoI if we run on Event Views). Each decision having m chains ( m=m_hypoTools.size() ) 
  const unsigned int nDecisions = jetELs.size();

  // Create output decisions
  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    TrigCompositeUtils::Decision *toAdd = TrigCompositeUtils::newDecisionIn( outputDecisions,
									     getPreviousDecision(prevDecisionContainer,index),
									     "", context );
    newDecisions.push_back( toAdd );
  }

  // Adding Links
  CHECK( attachLinkToDecisions( context,prevDecisionContainer,newDecisions) );

  // ==========================================================================================================================
  //    ** Prepare input to Hypo Tools
  // ========================================================================================================================== 

  std::vector< TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo > bJetHypoInputs;

  for ( unsigned int index(0); index<nDecisions; index++ ) {

    // Extract the IDs of the b-jet chains which are active.
    // Previous decision IDs.
    const TrigCompositeUtils::Decision *previousDecision = getPreviousDecision(prevDecisionContainer,index);

    TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
      TrigCompositeUtils::decisionIDs( previousDecision ).begin(),
	TrigCompositeUtils::decisionIDs( previousDecision ).end()
	};

    TrigBjetEtHypoTool::TrigBjetEtHypoToolInfo infoToAdd;
    infoToAdd.previousDecisionIDs = previousDecisionIDs;
    infoToAdd.jetEL = jetELs.at( index );
    infoToAdd.decision = newDecisions.at( index );
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

StatusCode TrigBjetEtHypoAlgMT::retrieveJets( const EventContext& context,
					      ElementLinkVector< xAOD::JetContainer >& jetELs,
					      const SG::ReadHandleKey< xAOD::JetContainer >& inputJetsKey,
					      const TrigCompositeUtils::DecisionContainer* prevDecisionContainer ) const {
  
  CHECK( retrieveObjectFromStoreGate( context,jetELs,inputJetsKey,prevDecisionContainer ) );
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::retrieveRoIs( const EventContext& context,
					      const TrigRoiDescriptorCollection*& roiContainer,
					      const SG::ReadHandleKey< TrigRoiDescriptorCollection >& inputRoIKey ) const {
  ATH_MSG_DEBUG( "Retrieving input TrigRoiDescriptorCollection with key: " << inputRoIKey );
  SG::ReadHandle< TrigRoiDescriptorCollection > roiContainerHandle = SG::makeHandle( inputRoIKey,context );
  CHECK( roiContainerHandle.isValid() );

  roiContainer = roiContainerHandle.get();
  ATH_MSG_DEBUG( "Retrieved " << roiContainer->size() <<" input RoIs" );
  for ( const TrigRoiDescriptor *roi : *roiContainer )
    ATH_MSG_DEBUG( "   ** eta="<< roi->eta() << " phi=" << roi->phi() );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::setJetLink( const EventContext& /*context*/,
					    const SG::ReadHandleKey< xAOD::JetContainer >& jetKey,
					    const unsigned int index,
					    const TrigCompositeUtils::DecisionContainer*& /*inputDecisions*/,
					    std::vector< TrigCompositeUtils::Decision* >& outputDecisions ) const {

  outputDecisions.at(index)->setObjectLink( TrigCompositeUtils::featureString(),ElementLink< xAOD::JetContainer >( jetKey.key(),index ) );           
  return StatusCode::SUCCESS;
}

const TrigCompositeUtils::Decision* TrigBjetEtHypoAlgMT::getPreviousDecision( const TrigCompositeUtils::DecisionContainer* decisionContainer,
									      unsigned int /*index*/ ) const {
  return decisionContainer->at(0);
}

StatusCode TrigBjetEtHypoAlgMT::attachLinkToDecisions( const EventContext& context,
						       const TrigCompositeUtils::DecisionContainer* prevDecisionContainer,
						       std::vector< TrigCompositeUtils::Decision* >& newDecisions ) const {
  const unsigned int nDecisions = newDecisions.size();

  for ( unsigned int index(0); index<nDecisions; index++ ) {
    // We want multiple output decision (one per RoI/Jet)    

    // A little bit tricky here, we may need to revise this in the future
    // In case what we want to link lives inside a view we have to do a few additional things 
    // in order to be able to link it to the output decision
    if ( m_roiLink.value() != "Undefined" && m_inputRoIKey.key() != "Undefined" )
      newDecisions.at( index )->setObjectLink( m_roiLink.value(),ElementLink< TrigRoiDescriptorCollection >( m_inputRoIKey.key(),index ) );
    
    if ( m_prmVtxLink.value() != "Undefined" && m_inputPrmVtx.key() != "Undefined")
      newDecisions.at( index )->setObjectLink( m_prmVtxLink.value(),ElementLink< xAOD::VertexContainer >( m_inputPrmVtx.key(),0 ) );
    
    CHECK( setJetLink( context,m_inputJetsKey,index,prevDecisionContainer,newDecisions ) );
  }

  ATH_MSG_DEBUG("   ** Added object links to output decision");

  return StatusCode::SUCCESS;
}
