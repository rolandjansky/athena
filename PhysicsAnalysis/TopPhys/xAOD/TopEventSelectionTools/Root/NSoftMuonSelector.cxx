/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NSoftMuonSelector.h"

#include <algorithm>

namespace top {
  NSoftMuonSelector::NSoftMuonSelector(const std::string& params) :
    SignValueSelector("SOFTMU_N", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NSoftMuonSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Muon* muPtr) {
                  return muPtr->pt() > value();
                };
    auto count = std::count_if(event.m_softmuons.begin(), event.m_softmuons.end(), func);

    return checkInt(count, multiplicity());
  }

  bool NSoftMuonSelector::applyParticleLevel(const top::ParticleLevelEvent& /*event*/) const {
    // the soft muon selector at particle level is just dummy for the time being
    return true;
  }
}
