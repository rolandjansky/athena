/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetGroomer.h"

#include "JetRec/PseudoJetContainer.h"

using xAOD::JetContainer;

StatusCode JetGroomer::initialize() {

  ATH_MSG_DEBUG("Initializing...");

 if(m_inputJetsKey.empty()){
    ATH_MSG_ERROR("Jet grooming requested with no input ungroomed jets");
    return StatusCode::FAILURE;
  } else if(m_inputPseudoJetsKey.empty()){
    ATH_MSG_ERROR("Jet grooming requested with no input pseudojets");
    return StatusCode::FAILURE;
  }
  else{
    ATH_CHECK(m_inputJetsKey.initialize());
    ATH_CHECK(m_inputPseudoJetsKey.initialize());
    ATH_CHECK(m_groomer.retrieve());
  }

  return StatusCode::SUCCESS;
}


std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetGroomer::getJets() const {

  // retrieve input
  SG::ReadHandle<JetContainer> inputJets(m_inputJetsKey);

  if(inputJets.isValid()) {
    ATH_MSG_DEBUG("Retrieval of ungroomed jets succeeded");
  } else {
    ATH_MSG_ERROR("Retrieval of ungroomed jets failed");
    return std::make_pair(std::unique_ptr<xAOD::JetContainer>(nullptr),std::unique_ptr<SG::IAuxStore>(nullptr));
  }

  ATH_MSG_DEBUG("Grooming jets");
  // Create the output containers
  // Make sure that memory is managed safely
  auto outjets = std::make_unique<xAOD::JetContainer>();
  auto outaux  = std::make_unique<xAOD::JetAuxContainer>();
  outjets->setStore(outaux.get());

  SG::ReadHandle<PseudoJetContainer> inputPseudoJets(m_inputPseudoJetsKey);

  for (const xAOD::Jet* jet : *inputJets){
    m_groomer->groom(*jet, *inputPseudoJets, *outjets);
  }

  return std::make_pair(std::move(outjets),std::move(outaux));
}
