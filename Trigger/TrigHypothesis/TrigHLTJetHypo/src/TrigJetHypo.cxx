/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Property.h"
#include "TrigJetHypo.h"

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::addDecisionID;

TrigJetHypo::TrigJetHypo( const std::string& name, 
                          ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ),
  m_hypoTools(this){
  declareProperty("HypoTools", m_hypoTools);
}

TrigJetHypo::~TrigJetHypo(){
}

StatusCode TrigJetHypo::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK(m_hypoTools.retrieve());
  CHECK(m_jetsKey.initialize());
  CHECK(m_decisionsKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypo::finalize() {
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypo::execute_r( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto jetsHandle = SG::makeHandle(m_jetsKey, context);
  
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());
  
  // prepare decision storage
  auto decision = newDecisionIn(decisions.get());
  constexpr std::size_t roiCounter = 0;  // other signatures have >1 ROIs
  decision->setObjectLink( "feature", 
                           ElementLink<xAOD::JetContainer>(m_jetsKey.key(), 
                                                           roiCounter));

  auto jets = jetsHandle.cptr();

  // loop over hypo tools (one per chain) and report acceptances.
  for (auto tool : m_hypoTools) {
    bool accept{false};
    ATH_CHECK(tool->decide(jets, accept));

    if (accept) {   
      addDecisionID(tool->decisionId(), decision);
      ATH_MSG_DEBUG( " + " << tool->name() );
    } else {
      ATH_MSG_DEBUG( " - " << tool->name() );
    }
  }
  
  
  {
    auto handle =  SG::makeHandle(m_decisionsKey, context);
    CHECK(handle.record(std::move(decisions), std::move(aux)));
  }

  return StatusCode::SUCCESS;
}


