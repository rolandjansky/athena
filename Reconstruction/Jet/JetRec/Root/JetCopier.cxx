/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetCopier.h"

#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"

using xAOD::JetContainer;

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


std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetCopier::getJets() const {

  // retrieve input
  SG::ReadHandle<JetContainer> inputJetsHandle(m_inputJets);

  if(inputJetsHandle.isValid()) {
    ATH_MSG_DEBUG("Retrieval of JetContainer was OK");
  } else {
    ATH_MSG_ERROR("Retrieval of JetContainer failed");
    return std::make_pair(std::unique_ptr<xAOD::JetContainer>(nullptr),std::unique_ptr<SG::IAuxStore>(nullptr));
  }

  ATH_MSG_DEBUG("Shallow-copying jets");

  std::pair<xAOD::JetContainer*,xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*inputJetsHandle);
  // Make sure that memory is managed safely
  std::unique_ptr<xAOD::JetContainer> outjets(shallowcopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> shallowaux(shallowcopy.second);

  // Connect the copied jets to their originals
  xAOD::setOriginalObjectLink(*inputJetsHandle, *outjets);

  return std::make_pair(std::move(outjets),std::move(shallowaux));
}
