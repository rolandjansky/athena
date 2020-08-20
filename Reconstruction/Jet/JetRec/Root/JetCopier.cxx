/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetCopier.h"

#include "AsgDataHandles/ReadHandle.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
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
    if(m_shallowIO && !m_shallowCopy){
      ATH_MSG_ERROR("ShallowIO set to True while ShallowCopy is False");

      return StatusCode::FAILURE;
    }
    else{
      ATH_CHECK(m_inputJets.initialize());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetCopier::getAndRecordJets(SG::WriteHandle<xAOD::JetContainer>& jetHandle) const {
  std::unique_ptr<xAOD::JetContainer> jets(nullptr);
  std::unique_ptr<SG::IAuxStore> auxCont(nullptr);
  std::tie(jets, auxCont) = getJets();

  if(jets.get() == nullptr || auxCont.get() == nullptr) {return StatusCode::FAILURE;}

  if(m_shallowCopy){
    std::unique_ptr<xAOD::ShallowAuxContainer> auxCont_derived(static_cast<xAOD::ShallowAuxContainer*>(auxCont.release()));
    return jetHandle.record(std::move(jets), std::move(auxCont_derived));
  }
  else{
    std::unique_ptr<xAOD::JetAuxContainer> auxCont_derived(static_cast<xAOD::JetAuxContainer*>(auxCont.release()));
    return jetHandle.record(std::move(jets), std::move(auxCont_derived));
  }
}

std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetCopier::getJets() const {
  if(m_shallowCopy){
    return ShallowCopyJets();
  }
  else{
    return DeepCopyJets();
  }
}

std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetCopier::ShallowCopyJets() const {

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

  shallowcopy.second->setShallowIO(m_shallowIO);

  // Make sure that memory is managed safely
  std::unique_ptr<xAOD::JetContainer> outjets(shallowcopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> shallowaux(shallowcopy.second);

  // Connect the copied jets to their originals
  xAOD::setOriginalObjectLink(*inputJetsHandle, *outjets);

  return std::make_pair(std::move(outjets),std::move(shallowaux));
}

std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetCopier::DeepCopyJets() const {

  // retrieve input
  SG::ReadHandle<JetContainer> inputJetsHandle(m_inputJets);

  if(inputJetsHandle.isValid()) {
    ATH_MSG_DEBUG("Retrieval of JetContainer was OK");
  } else {
    ATH_MSG_ERROR("Retrieval of JetContainer failed");
    return std::make_pair(std::unique_ptr<xAOD::JetContainer>(nullptr),std::unique_ptr<SG::IAuxStore>(nullptr));
  }

  ATH_MSG_DEBUG("Deep-copying jets");

  std::unique_ptr<xAOD::JetContainer> goodJets = std::make_unique<xAOD::JetContainer>();
  std::unique_ptr<xAOD::JetAuxContainer> goodJetsAux = std::make_unique<xAOD::JetAuxContainer>();

  goodJets->setStore (goodJetsAux.get());

  for(const xAOD::Jet* jet : *inputJetsHandle){
    xAOD::Jet* goodJet = new xAOD::Jet();
    goodJets->push_back (goodJet);
    *goodJet = *jet;
  }

  return std::make_pair(std::move(goodJets),std::move(goodJetsAux));
}
