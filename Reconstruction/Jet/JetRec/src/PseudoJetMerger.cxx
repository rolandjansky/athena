/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PseudoJetMerger.h"

StatusCode PseudoJetMerger::initialize() {
  ATH_MSG_DEBUG("Initializing...");

  if(m_inputPJC.empty() || m_outcoll.key().empty()) {
    ATH_MSG_ERROR("Either input or output collection is empty!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_inputPJC.initialize() );
  ATH_CHECK( m_outcoll.initialize() );
  
  return StatusCode::SUCCESS;
}


StatusCode PseudoJetMerger::execute(const EventContext& ctx) const {

  auto allPseudoJets = std::make_unique<PseudoJetContainer>();

  for (const auto& pjcKey : m_inputPJC) {
    SG::ReadHandle<PseudoJetContainer> pjcHandle( pjcKey, ctx );
    if(!pjcHandle.isValid()){
      ATH_MSG_ERROR("Can't retrieve PseudoJetContainer "<< pjcKey.key() ); return StatusCode::FAILURE;
    }
    allPseudoJets->append(pjcHandle.get() );
  }

  SG::WriteHandle<PseudoJetContainer> outHandle(m_outcoll, ctx);
  ATH_CHECK( outHandle.record(std::move(allPseudoJets)) );
  
  return StatusCode::SUCCESS;
}

