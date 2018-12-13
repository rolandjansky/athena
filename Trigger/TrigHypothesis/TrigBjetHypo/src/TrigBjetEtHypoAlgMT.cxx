/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "AthViews/ViewHelper.h"

TrigBjetEtHypoAlgMT::TrigBjetEtHypoAlgMT( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetEtHypoAlgMT::~TrigBjetEtHypoAlgMT() {}

StatusCode TrigBjetEtHypoAlgMT::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"    );
  ATH_MSG_DEBUG(  "   " << m_readFromView           );
  ATH_MSG_DEBUG(  "   " << m_roiLink           );
  ATH_MSG_DEBUG(  "   " << m_jetLink           );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve()   );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_inputJetsKey.initialize()       );
  CHECK( m_inputRoIKey.initialize()        );

  if ( m_readFromView )
    renounce( m_inputJetsKey );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::execute_r( const EventContext& context ) const {  
  ATH_MSG_INFO ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Taken from Jet code
  // Read in previous Decisions made before running this Hypo Alg.
  // The container should have only one such Decision in case we are cutting on 'j' threshold (for L1)
  ATH_MSG_DEBUG( "Retrieving Previous Decision" );
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );

  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // Retrieve Jets
  const xAOD::JetContainer *jetCollection = nullptr;
  if ( not m_readFromView ) CHECK( retrieveJetsFromStoreGate( context,jetCollection ) );
  else CHECK( retrieveJetsFromEventView( context,jetCollection,prevDecisionHandle ) );

  ATH_MSG_DEBUG( "Found " << jetCollection->size()<< " jets."  );
  for ( const xAOD::Jet *jet : * jetCollection ) 
    ATH_MSG_INFO("   -- Jet pt=" << jet->p4().Et() <<" eta="<< jet->eta() << " phi="<< jet->phi() );

  // Retrieve RoI to be linked to the output decision
  const TrigRoiDescriptorCollection *roiContainer = nullptr;
  if ( not m_readFromView ) {
    ATH_MSG_DEBUG( "Retrieving input TrigRoiDescriptorCollection with key: " << m_inputRoIKey );
    SG::ReadHandle< TrigRoiDescriptorCollection > roiContainerHandle = SG::makeHandle( m_inputRoIKey,context );
    CHECK( roiContainerHandle.isValid() );
    
    roiContainer = roiContainerHandle.get();
    ATH_MSG_DEBUG( "Retrieved " << roiContainer->size() <<" input RoIs" );
    for ( const TrigRoiDescriptor *roi : *roiContainer )
      ATH_MSG_DEBUG( "   ** eta="<< roi->eta() << " phi=" << roi->phi() );
  }

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
  const unsigned int nDecisions = jetCollection->size();

  // Create output decisions
  ATH_MSG_DEBUG("Creating Output Decisions and Linking Stuff to it");
  std::vector< TrigCompositeUtils::Decision* > newDecisions;
  for ( unsigned int index(0); index<nDecisions; index++ ) 
    newDecisions.push_back( TrigCompositeUtils::newDecisionIn( outputDecisions ) );

  // Adding Links
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    // We want multiple output decision (one per RoI/Jet)    
    newDecisions.at( index )->setObjectLink( m_roiLink.value(),ElementLink< TrigRoiDescriptorCollection >( m_inputRoIKey.key(),index ) );
    newDecisions.at( index )->setObjectLink( m_jetLink.value(),ElementLink< xAOD::JetContainer >( m_inputJetsKey.key(),index ) );
  }
  ATH_MSG_DEBUG("   ** Added object links to output decision");

  ATH_MSG_DEBUG("Ready to Link Output Decision to Input Decision");
  // Link To previous decision
  // *** If we are NOT reading from event views it means we are in step1,
  // Thus, we may have one or more output decision, but only one input decision (1-to-many correspondance)
  // *** If we are reading from event views it means we are are in step2,
  // Thus, we have out output decision for each input decision (1-to-1 correspendance)
  for ( unsigned int index(0); index<nDecisions; index++ ) {
    if ( m_readFromView ) TrigCompositeUtils::linkToPrevious( newDecisions.at( index ),decisionInput().key(),index );
    else TrigCompositeUtils::linkToPrevious( newDecisions.at( index ),decisionInput().key(),0 );
  }

  ATH_MSG_DEBUG("   ** Done with Linking Output Decision to Input Decision");


  // Run on Trigger Chains
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) {
    const HLT::Identifier  decisionId = tool->getId();

    // At this point the JetCollection size is equal to the number of jets reconstructed in the entire event
    // independently of the stage (stage1 or stage2) of b-jet chains.
    // In case we are reading from Event Views, the jets are retrieved and merged together inside the
    // `retrieveJetsFromEventView` method. In this way I can require multeplicity requirements inside the 'decide method.'

    // Since we have one jet per decision, we run on nDecisions and compute the decide outcome
    for( unsigned int index(0); index<nDecisions; index++ ) {
      bool pass = false;
      CHECK( tool->decide( jetCollection->at(index),pass ) );
      if ( pass ) TrigCompositeUtils::addDecisionID( decisionId,newDecisions.at(index) );      
    }
  }
  
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::retrieveJetsFromStoreGate( const EventContext& context,
							   const xAOD::JetContainer*& jetCollection ) const {
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_inputJetsKey,context );
  ATH_MSG_DEBUG( "Retrieved jets from : " << m_inputJetsKey.key() );
  CHECK( jetContainerHandle.isValid() );
  jetCollection = jetContainerHandle.get();
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT::retrieveJetsFromEventView( const EventContext& context,
							   const xAOD::JetContainer*& jetCollection, 
							   SG::ReadHandle< TrigCompositeUtils::DecisionContainer >& prevDecisionHandle ) const {
  xAOD::JetContainer *output = new xAOD::JetContainer();
  std::unique_ptr< xAOD::JetAuxContainer > outputAux( new xAOD::JetAuxContainer() );
  output->setStore( outputAux.release() );

  for ( auto previousDecision: *prevDecisionHandle ) {
    //get RoI
    auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
    ATH_CHECK( roiEL.isValid() );
    const TrigRoiDescriptor* roi = *roiEL;
    ATH_MSG_DEBUG( "Retrieved RoI from previous decision " );
    ATH_MSG_DEBUG( "   ** eta=" << roi->eta() <<" phi="<< roi->phi() );

    // get View
    auto viewEL = previousDecision->objectLink< ViewContainer >( "view" );
    ATH_CHECK( viewEL.isValid() );
    ATH_MSG_DEBUG( "Retrieved View" );
    SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = ViewHelper::makeHandle( *viewEL, m_inputJetsKey, context);
    ATH_CHECK( jetContainerHandle.isValid() );
    ATH_MSG_DEBUG ( "jet container handle size: " << jetContainerHandle->size() << "..." );
    const xAOD::JetContainer *jetContainer = jetContainerHandle.get();
    for ( const xAOD::Jet *jet : *jetContainer ) {
      ATH_MSG_DEBUG( "   *** pt=" << jet->p4().Et() << " eta="<< jet->eta()<< " phi=" << jet->phi() );

      // Make a Copy
      xAOD::Jet *copyJet = new xAOD::Jet();
      output->push_back( copyJet );
      *copyJet = *jet;
    }
  }

  jetCollection = new xAOD::JetContainer( *output );
  return StatusCode::SUCCESS;
}

