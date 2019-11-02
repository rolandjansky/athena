/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NFwdElectronSelector.h"
#include <algorithm>

namespace top {
  NFwdElectronSelector::NFwdElectronSelector(const std::string& params) :
    SignValueSelector("FWDEL_N", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NFwdElectronSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Electron* elPtr) {
                  return elPtr->pt() > value();
                };
    auto count = std::count_if(event.m_fwdElectrons.begin(), event.m_fwdElectrons.end(), func);

    return checkInt(count, multiplicity());
  }

  bool NFwdElectronSelector::applyParticleLevel(const top::ParticleLevelEvent& /*event*/) const {
    return true;
  }
}
