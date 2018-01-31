/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// author: cpollard@cern.ch

#include "xAODCore/ShallowCopy.h"
#include "ParticleJetTools/JetAssocConstAlg.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"

using namespace std;
using namespace xAOD;

JetAssocConstAlg::JetAssocConstAlg(
    string name
    , ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator) {

    declareProperty("Associators", m_assocs);
    declareProperty("JetCollections", m_jetcollections);
  }

StatusCode JetAssocConstAlg::initialize() {
  if ( m_assocs.retrieve().isFailure() ) {
    ATH_MSG_FATAL(name() + " jet-particle association tool retrieval failed.");
    return StatusCode::FAILURE;
  }

  for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
    if ( jpa->initialize().isFailure() ) {
      ATH_MSG_FATAL(jpa->name() + " tool initialization failed.");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetAssocConstAlg::execute() {
  for (const string& jetContainerName : m_jetcollections) {
    SG::ReadHandle<JetContainer> jetsHandle(jetContainerName);

    if ( !jetsHandle.isValid()) {
      ATH_MSG_FATAL(name() + 
          " failed to retrieve jet collection \"" +
          jetContainerName + "\"");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO(name() + 
          " retrieved jet collection \"" +
          jetContainerName + "\"");
    }

    const JetContainer& jets = *jetsHandle;

    pair<JetContainer*,ShallowAuxContainer*> shallowcopy
      = shallowCopyContainer(jets);

    // Make unique_ptrs so as to avoid needing to delete
    unique_ptr<JetContainer> jets_copy(shallowcopy.first);
    unique_ptr<ShallowAuxContainer> jets_copyaux(shallowcopy.second);


    for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
      SG::AuxElement::ConstAccessor<vector<ElementLink<IParticleContainer> > > acc(jpa->outputCollectionName());
      SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > > dec(jpa->outputCollectionName());

      ATH_MSG_INFO(jpa->name() + " branch access: " + jpa->outputCollectionName());

      // modify the shallow copy of the jets
      if ( jpa->modify(*jets_copy) ) {
        ATH_MSG_FATAL(jpa->name() + " tool execution failed.");
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_INFO(jpa->name() + " tool execution success.");
      }

      // directly copy over the decoration to the original jet
      // collections
      for (size_t iJet = 0; iJet < jets.size(); iJet++) {
        dec(*jets.at(iJet)) = acc(*jets_copy->at(iJet));
        ATH_MSG_INFO(jpa->name() + " number of associated particles to copy: " + to_string(acc(*jets_copy->at(iJet)).size()));
        ATH_MSG_INFO(jpa->name() + " number of associated particles to jet: " + to_string(acc(*jets.at(iJet)).size()));
      }

    }
  }

  return StatusCode::SUCCESS;
}


StatusCode JetAssocConstAlg::finalize() {
  for (const ToolHandle<JetParticleAssociation>& jpa : m_assocs) {
    if ( jpa->finalize().isFailure() ) {
      ATH_MSG_FATAL(jpa->name() + " tool finalization failed.");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
