/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NRCJetSelector.h"

namespace top{

  NRCJetSelector::NRCJetSelector(const std::string& params) :
    SignValueSelector("RCJET_N", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NRCJetSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr){return jetPtr->pt() > value();};
    auto count = std::count_if(event.m_largeJets.begin(), event.m_largeJets.end(), func);
    return checkInt(count, multiplicity());
  }

  bool NRCJetSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    auto func = [&](const xAOD::Jet* jetPtr){return jetPtr->pt() > value();};
    auto count = std::count_if(event.m_largeRJets->begin(), event.m_largeRJets->end(), func);
    return checkInt(count, multiplicity());
  }

}
