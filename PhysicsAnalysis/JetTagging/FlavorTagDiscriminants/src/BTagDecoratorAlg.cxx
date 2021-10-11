/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagDecoratorAlg.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ReadDecorHandle.h"

#include <exception>

namespace FlavorTagDiscriminants {
  BTagDecoratorAlg::BTagDecoratorAlg(
    const std::string& name, ISvcLocator* svcloc):
    AthReentrantAlgorithm(name, svcloc)
  {
  }


  StatusCode BTagDecoratorAlg::initialize() {
    ATH_CHECK(m_btagContainerKey.initialize());
    ATH_CHECK(m_trackContainerKey.initialize());

    ATH_CHECK(m_decorator.retrieve());

    std::set<std::string> veto(
      m_undeclaredReadDecorKeys.begin(),
      m_undeclaredReadDecorKeys.end());

    // now we build data dependencies from the internal tools. We have
    // to reserve the vectors here to prevent a segfault since read /
    // write handles aren't movable once declaired as a property.
    m_btagAux.reserve(m_decorator->getAuxInputKeys().size());
    for (const std::string& key: m_decorator->getAuxInputKeys()) {
      const std::string full = m_btagContainerKey.key() + "." + key;
      if (veto.count(full)) {
        ATH_MSG_DEBUG("Not declaring accessor: " + full);
        continue;
      }
      ATH_MSG_DEBUG("Adding btag accessor: " + full);
      m_btagAux.emplace_back(this, key, full, "");
      ATH_CHECK(m_btagAux.back().initialize());
    }
    m_trkAux.reserve(m_decorator->getConstituentAuxInputKeys().size());
    for (const std::string& key: m_decorator->getConstituentAuxInputKeys()) {
      const std::string full = m_trackContainerKey.key() + "." + key;
      if (veto.count(full)) {
        ATH_MSG_DEBUG("Not declaring accessor: " + full);
        continue;
      }
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


  StatusCode BTagDecoratorAlg::execute(const EventContext& cxt ) const {
    SG::ReadHandle<xAOD::BTaggingContainer> btagContainer(
      m_btagContainerKey, cxt);
    if (!btagContainer.isValid()) {
      ATH_MSG_ERROR("no b-tagging container " << btagContainer.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG(
      "Decorating " + std::to_string(btagContainer->size()) + " btags");
    for (const auto* btag: *btagContainer) {
      m_decorator->decorate(*btag);
    }
    return StatusCode::SUCCESS;
  }


  StatusCode BTagDecoratorAlg::finalize() {
    return StatusCode::SUCCESS;
  }

}
