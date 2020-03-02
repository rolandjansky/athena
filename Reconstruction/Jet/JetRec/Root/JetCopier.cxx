/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetCopier.h"

#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "JetEDM/PseudoJetVector.h"
#include "JetRec/PseudoJetTranslator.h"

using xAOD::JetContainer;

JetCopier::JetCopier(std::string name) : AsgTool(name) {
}

StatusCode JetCopier::initialize() {

  ATH_MSG_DEBUG("Initializing...");

  ATH_CHECK(m_inputJets.initialize());

  return StatusCode::SUCCESS;
}


xAOD::JetContainer* JetCopier::getJets() const {

  // retrieve input
  SG::ReadHandle<JetContainer> pjCopierHandle(m_inputJets);

  const JetContainer* pjetsin = 0;
  pjetsin = pjCopierHandle.cptr();

  ATH_MSG_DEBUG("Shallow-copying jets");

  xAOD::JetContainer& outjets = *(xAOD::shallowCopyContainer(*pjetsin).first);

  xAOD::setOriginalObjectLink(*pjetsin, outjets);

  return &outjets;
}
