/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagJetDecoratorAlg.h"
#include "StoreGate/ReadDecorHandle.h"

#include <exception>

namespace FlavorTagDiscriminants {
  BTagJetDecoratorAlg::BTagJetDecoratorAlg(
    const std::string& name, ISvcLocator* svcloc):
    AthReentrantAlgorithm(name, svcloc)
  {
  }


  StatusCode BTagJetDecoratorAlg::initialize() {
    ATH_CHECK(m_btagContainerKey.initialize());
    ATH_CHECK(m_trackContainerKey.initialize());

    m_jetAcc = m_btagContainerKey.key() + ".jetLink";
    ATH_CHECK(m_jetAcc.initialize());

    ATH_CHECK(m_decorator.retrieve());

    // now we build data dependencies from the internal tools. We have
    // to reserve the vectors here to prevent a segfault since read /
    // write handles aren't movable once declaired as a property.
    m_btagAux.reserve(m_decorator->getAuxInputKeys().size());
    for (const std::string& key: m_decorator->getAuxInputKeys()) {
      const std::string full = m_btagContainerKey.key() + "." + key;
      ATH_MSG_DEBUG("Adding btag accessor: " + full);
      m_btagAux.emplace_back(this, key, full, "");
      ATH_CHECK(m_btagAux.back().initialize());
    }
    m_trkAux.reserve(m_decorator->getConstituentAuxInputKeys().size());
    for (const std::string& key: m_decorator->getConstituentAuxInputKeys()) {
      const std::string full = m_trackContainerKey.key() + "." + key;
      ATH_MSG_DEBUG("Adding track accessor: " + full);
      m_trkAux.emplace_back(this, key, full, "");
      ATH_CHECK(m_trkAux.back().initialize());
    }
    m_btagDecor.reserve(m_decorator->getDecoratorKeys().size());
    for (const std::string& key: m_decorator->getDecoratorKeys()) {
      const std::string full = m_btagContainerKey.key() + "." + key;
      ATH_MSG_DEBUG("Adding btag decorator: " + full);
      m_btagDecor.emplace_back(this, key, full, "");
      ATH_CHECK(m_btagDecor.back().initialize());
    }
    ATH_MSG_DEBUG("Finished setting up");
    return StatusCode::SUCCESS;
  }


  StatusCode BTagJetDecoratorAlg::execute(const EventContext& cxt ) const {
    SG::ReadHandle<xAOD::BTaggingContainer> btagContainer(
      m_btagContainerKey, cxt);
    if (!btagContainer.isValid()) {
      ATH_MSG_ERROR("no b-tagging container " << btagContainer.key());
      return StatusCode::FAILURE;
    }
    // We need to navigate back to the jet for now. In the future we
    // shuld rewrite the jet decorator class to work on the b-tagging
    // object instead, but there is still some code that expects the
    // release 21 EDM where no btag -> jet link exists.
    SG::ReadDecorHandle<
      xAOD::BTaggingContainer,ElementLink<xAOD::JetContainer>
      > jetAcc(m_jetAcc, cxt);

    if (!jetAcc.isAvailable()) {
      ATH_MSG_ERROR("no jet link " << jetAcc.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG(
      "Decorating " + std::to_string(btagContainer->size()) + " btags");
    for (const auto* btag: *btagContainer) {
      const auto& jet_link = jetAcc(*btag);
      if (!jet_link.isValid()) {
        ATH_MSG_ERROR("invalid jet link!");
        return StatusCode::FAILURE;
      }
      const xAOD::Jet* jet = *jet_link;
      if (!jet) {
        ATH_MSG_ERROR("null jet pointer!");
        return StatusCode::FAILURE;
      }
      m_decorator->decorate(*jet);
    }
    return StatusCode::SUCCESS;
  }


  StatusCode BTagJetDecoratorAlg::finalize() {
    return StatusCode::SUCCESS;
  }

}
