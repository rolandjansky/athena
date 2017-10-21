/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestHypoAlg.h"



namespace HLTTest {
  
  TestHypoAlg::TestHypoAlg( const std::string& name, 
			    ISvcLocator* pSvcLocator )    
    : AthAlgorithm( name, pSvcLocator ),
      m_tools(this) {
    declareProperty( "Input", m_recoInput );
    declareProperty( "Output", m_output );
    declareProperty( "HypoTools", m_tools );
  }

  TestHypoAlg::~TestHypoAlg() {}

  StatusCode TestHypoAlg::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    CHECK( m_output.initialize() );
    CHECK( m_recoInput.initialize() );
    CHECK( m_tools.retrieve() );  
    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoAlg::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoAlg::execute() {  
    ATH_MSG_DEBUG( "Executing " << name() << "..." );
    auto input = SG::makeHandle(m_recoInput);

    auto decisions = std::make_unique<DecisionContainer>();
    auto aux = std::make_unique<DecisionAuxContainer>();
    decisions->setStore( aux.get() );
    size_t counter = 0;
    for ( auto objIter =   input->begin();  objIter != input->end(); ++objIter, ++counter ) {
      auto d = newDecisionIn(decisions.get());
      d->setObjectLink("feature", ElementLink<xAOD::TrigCompositeContainer>(m_recoInput.key(), counter) );
    }

    if (decisions->size()>0){
      for ( auto tool: m_tools ) {
	CHECK( tool->decide( decisions.get() ) );
      }
    }

    auto outputHandle = SG::makeHandle(m_output);
    CHECK( outputHandle.record(std::move(decisions), std::move(aux) ) );
  
  
    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest
