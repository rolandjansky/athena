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

  // Ugly check but no other good way of testing at initialisation
  auto add_if_ghost = [](unsigned int sum_ghosts, const SG::ReadHandleKey<PseudoJetContainer>& pjckey) {
      return sum_ghosts + (pjckey.key().find("Ghost")!=std::string::npos);
  };
  unsigned int N_ghosts = std::accumulate(m_inputPJC.begin(), m_inputPJC.end(), 0, add_if_ghost);
  if(m_inputPJC.size()-N_ghosts!=1) {
    ATH_MSG_ERROR("List of input pseudojet containers to be merged must have exactly one non-ghost collection!");
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
    allPseudoJets->append( pjcHandle.get() );
  }

  SG::WriteHandle<PseudoJetContainer> outHandle(m_outcoll, ctx);
  ATH_MSG_DEBUG("Merged PseudoJetContainer \"" << m_outcoll.key() << "\" has size " << allPseudoJets->size());
  ATH_CHECK( outHandle.record(std::move(allPseudoJets)) );
  
  return StatusCode::SUCCESS;
}

