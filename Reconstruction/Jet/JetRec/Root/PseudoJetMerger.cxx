/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "JetRec/PseudoJetMerger.h"


PseudoJetMerger::PseudoJetMerger(const std::string& name) : AsgTool(name)  {
  
}

StatusCode PseudoJetMerger::initialize() {
  ATH_MSG_DEBUG("Initializing...");

  ATH_CHECK( m_outcoll.initialize() );

  
  return StatusCode::SUCCESS;
}


StatusCode PseudoJetMerger::createAndRecord() const {

  auto allPseudoJets = std::make_unique<PseudoJetContainer>();

  for (const auto& pjcName : m_inputPJC) {
    SG::ReadHandle<PseudoJetContainer> pjcHandle( pjcName );
    if(!pjcHandle.isValid()){
      ATH_MSG_ERROR("Can't retrieve PseudoJetContainer "<< pjcName ); return StatusCode::FAILURE;
    }
    allPseudoJets->append(pjcHandle.get() );
  }

  SG::WriteHandle<PseudoJetContainer> outHandle(m_outcoll);
  ATH_CHECK( outHandle.record(std::move(allPseudoJets)) );
  
  return StatusCode::SUCCESS;

}


const PseudoJetVector* PseudoJetMerger::get() const {

  return nullptr; // not supposed to be used anymore (?)
}
