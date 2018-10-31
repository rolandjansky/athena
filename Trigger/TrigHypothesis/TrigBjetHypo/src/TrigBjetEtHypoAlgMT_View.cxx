/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigBjetEtHypoAlgMT_View.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "AthViews/ViewHelper.h"

TrigBjetEtHypoAlgMT_View::TrigBjetEtHypoAlgMT_View( const std::string& name, 
						    ISvcLocator* pSvcLocator ) : 
  ::HypoBase( name, pSvcLocator ) {}

TrigBjetEtHypoAlgMT_View::~TrigBjetEtHypoAlgMT_View()
{}

StatusCode TrigBjetEtHypoAlgMT_View::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_roiLink          );
  ATH_MSG_DEBUG(  "   " << m_jetLink          );

  ATH_MSG_DEBUG( "Initializing Tools" );
  ATH_CHECK( m_hypoTools.retrieve()   );

  ATH_MSG_DEBUG( "Initializing HandleKeys" );
  CHECK( m_inputJetsKey.initialize()       );
  CHECK( m_inputRoIKey.initialize()        ); // To understand what to do with it

  // What lives in the View
  renounce( m_inputJetsKey );

  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT_View::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetEtHypoAlgMT_View::execute_r( const EventContext& context ) const {  
  ATH_MSG_INFO ( "Executing " << name() << "..." );

  // ========================================================================================================================== 
  //    ** Retrieve Ingredients
  // ========================================================================================================================== 

  // Read in previous Decisions made before running this Hypo Alg.
  // The container should have only one such Decision in case we are cutting on 'j' threshold (for L1)
  ATH_MSG_DEBUG( "Retrieving Previous Decision" );
  SG::ReadHandle< TrigCompositeUtils::DecisionContainer > prevDecisionHandle = SG::makeHandle( decisionInput(),context );
  CHECK( prevDecisionHandle.isValid() );

  ATH_MSG_DEBUG( "Running with "<< prevDecisionHandle->size() <<" implicit ReadHandles for previous decisions");
  const TrigCompositeUtils::DecisionContainer *prevDecisionContainer = prevDecisionHandle.get();
  ATH_MSG_DEBUG( "Running with "<< prevDecisionContainer->size() <<" previous decisions");

  // ========================================================================================================================== 
  //    ** Prepare Outputs
  // ========================================================================================================================== 

  // Decisions
  std::unique_ptr< TrigCompositeUtils::DecisionContainer > decisions = std::make_unique< TrigCompositeUtils::DecisionContainer >();
  std::unique_ptr< TrigCompositeUtils::DecisionAuxContainer > aux = std::make_unique< TrigCompositeUtils::DecisionAuxContainer >();
  decisions->setStore( aux.get() );

  // Jets
  std::unique_ptr< xAOD::JetContainer > outputJetContainer( new xAOD::JetContainer() );
  std::unique_ptr< xAOD::JetAuxContainer > outputJetAuxContainer( new xAOD::JetAuxContainer() );
  outputJetContainer->setStore( outputJetAuxContainer.get() );

  // ========================================================================================================================== 
  //    ** Retrieve View and perform computations
  // ========================================================================================================================== 

  size_t counter=0;
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
    const xAOD::JetContainer *jetCollection = jetContainerHandle.get();
    for ( const xAOD::Jet *jet : *jetCollection ) {
      ATH_MSG_DEBUG( "   *** pt=" << jet->p4().Et() << " eta="<< jet->eta()<< " phi=" << jet->phi() );
      
      xAOD::Jet *copyJet = new xAOD::Jet();
      outputJetContainer->push_back( copyJet );
      *copyJet = *jet;
    }
  }


  // ==========================================================================================================================
  //    ** Compute Decisions
  // ==========================================================================================================================

  // Taken from Jet Code here
  const TrigCompositeUtils::Decision *prevDecision = prevDecisionContainer->at(0);
  TrigCompositeUtils::Decision *newDecision = TrigCompositeUtils::newDecisionIn( decisions.get() );

  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs {
    TrigCompositeUtils::decisionIDs( prevDecision ).begin(),
      TrigCompositeUtils::decisionIDs( prevDecision ).end()
      };

  // Decide (Hypo Tool)
  for ( const ToolHandle< TrigBjetEtHypoTool >& tool : m_hypoTools ) {
    const HLT::Identifier  decisionId = tool->getId();
    // Check previous decision is 'passed'
    if ( not TrigCompositeUtils::passed( decisionId.numeric() , previousDecisionIDs ) )
      continue;
    bool pass = false;
    CHECK( tool->decide( outputJetContainer.get(),pass ) );
    if ( pass ) TrigCompositeUtils::addDecisionID( decisionId,newDecision );
  }


  // ==========================================================================================================================
  //    ** Store Output
  // ==========================================================================================================================

  // Save Output Decisions
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > handle =  SG::makeHandle( decisionOutput(), context );
  CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );
  ATH_MSG_DEBUG( "Exiting with " << handle->size() << " decisions" );

  // ==========================================================================================================================  
  //    ** Linking objects to decision 
  // ==========================================================================================================================  
  /*
  newDecision->setObjectLink( m_roiLink.value(),ElementLink< TrigRoiDescriptorCollection >( m_inputRoIKey.key(),0 ) );
  ATH_MSG_DEBUG( "Linking RoIs `" << m_roiLink.value() << "` to output decision." );

  newDecision->setObjectLink( m_jetLink.value(),ElementLink< xAOD::JetContainer >( m_inputJetsKey.key(),0 ) );
  ATH_MSG_DEBUG( "Linking Jets `" << m_jetLink.value() << "` to output decision." );
  */
  return StatusCode::SUCCESS;
}

