/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestHypoAlg.h"



namespace HLTTest {
  
  TestHypoAlg::TestHypoAlg( const std::string& name, 
			    ISvcLocator* pSvcLocator )    :
    ::AthReentrantAlgorithm( name, pSvcLocator ) {}


  TestHypoAlg::~TestHypoAlg() {}

  
  StatusCode TestHypoAlg::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    CHECK( m_output.initialize() );
    CHECK( m_recoInput.initialize() );
    CHECK( m_previousDecisions.initialize() );
    renounce(m_previousDecisions);
    ATH_MSG_DEBUG("Will consume implicit previous decisions "<< m_previousDecisions.key());
    //    for (auto& input: m_previousDecisions){  
    //ATH_MSG_DEBUG(" "<<input.key());
    //}
    CHECK( m_tools.retrieve() );  
    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoAlg::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }


  StatusCode TestHypoAlg::execute_r( const EventContext& context ) const {  
    //  StatusCode TestHypoAlg::execute() {  
    ATH_MSG_DEBUG( "Executing " << name() << "..." );

    auto previousDecisionsHandle = SG::makeHandle( m_previousDecisions, context );
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" implicit ReadHandles for previous decisions");

    auto recoInput = SG::makeHandle(m_recoInput, context);
    ATH_MSG_DEBUG( "and with "<< recoInput->size() <<" reco inputs");
    
    // new output decisions
    auto decisions = std::make_unique<DecisionContainer>();
    auto aux = std::make_unique<DecisionAuxContainer>();
    decisions->setStore( aux.get() );

    
    // loop over previous decisions
    size_t counter = 0;
    for ( auto previousDecision: *previousDecisionsHandle ) {
      auto roiEL = previousDecision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
      CHECK( roiEL.isValid() );
      // const TrigRoiDescriptor* roi = *roiEL;      
      auto d = newDecisionIn(decisions.get());
      d->setObjectLink( "feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput.key(), counter) );// feature used by the Tool
      d->setObjectLink( "initialRoI", roiEL );// this is used by the InputMaker
      d->setObjectLink( "previousDecisions", ElementLink<DecisionContainer>(m_previousDecisions.key(), counter) );// link to previous decision object
      counter++;
    }

    if (decisions->size()>0){
      for ( auto tool: m_tools ) {
	CHECK( tool->decide( decisions.get() ) );
      }
    }

    auto outputHandle = SG::makeHandle(m_output, context);
    CHECK( outputHandle.record(std::move(decisions), std::move(aux) ) );
  
    ATH_MSG_DEBUG( "Exiting with "<< outputHandle->size() <<" decisions");
    //debug
    for (auto outh: *outputHandle){
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( outh, objDecisions );

      ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size() );

      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
       	ATH_MSG_DEBUG( " --- found decision " << HLT::Identifier( id ) );
      }  

    }


    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
