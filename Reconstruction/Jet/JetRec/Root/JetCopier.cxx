/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetCopier.h"

#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"

using xAOD::JetContainer;

JetCopier::JetCopier(std::string name) : AsgTool(name) {
}

StatusCode JetCopier::initialize() {

  ATH_MSG_DEBUG("Initializing...");

  if(m_inputJets.empty()){
    ATH_MSG_ERROR("Jet finding requested with no inputs");

    return StatusCode::FAILURE;
  }
  else{
    ATH_CHECK(m_inputJets.initialize());
  }

  return StatusCode::SUCCESS;
}


xAOD::JetContainer* JetCopier::getJets() const {

  // retrieve input
  SG::ReadHandle<JetContainer> inputJetsHandle(m_inputJets);


  if(inputJetsHandle.isValid()) {
    ATH_MSG_DEBUG("Retrieval of JetContainer was OK");
  }
  else{
    ATH_MSG_ERROR("Retrieval of JetContainer failed");
    return nullptr;
  }

  const JetContainer* injets = inputJetsHandle.cptr();

  ATH_MSG_DEBUG("Shallow-copying jets");

  xAOD::JetContainer& outjets = *(xAOD::shallowCopyContainer(*injets).first);

  xAOD::setOriginalObjectLink(*injets, outjets);

  return &outjets;
}
