/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NTauSelector.h"

namespace top {
  NTauSelector::NTauSelector(const std::string& params) :
    SignValueSelector("TAU_N", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NTauSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::TauJet* tauPtr) {
                  return tauPtr->pt() > value();
                };
    auto count = std::count_if(event.m_tauJets.begin(), event.m_tauJets.end(), func);

    return checkInt(count, multiplicity());
  }

  bool NTauSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if (!event.m_taus) {
      return false;
    }

    auto func = [&](const xAOD::TruthParticle* truTauPtr) {
                  return truTauPtr->pt() > value();
                };
    auto count = std::count_if(event.m_taus->begin(), event.m_taus->end(), func);
    return checkInt(count, multiplicity());
  }
}
