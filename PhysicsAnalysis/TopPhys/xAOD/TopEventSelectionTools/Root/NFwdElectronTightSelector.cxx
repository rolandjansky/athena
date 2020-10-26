/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/NFwdElectronTightSelector.h"
#include <algorithm>

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  NFwdElectronTightSelector::NFwdElectronTightSelector(const std::string& params) :
    SignValueSelector("FWDEL_N_TIGHT", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NFwdElectronTightSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Electron* elPtr) {
                  return elPtr->pt() > value() && elPtr->auxdataConst<char>("passPreORSelection");
                };
    auto count = std::count_if(event.m_fwdElectrons.begin(), event.m_fwdElectrons.end(), func);

    return checkInt(count, multiplicity());
  }

  bool NFwdElectronTightSelector::applyParticleLevel(const top::ParticleLevelEvent& /*event*/) const {
      ATH_MSG_INFO("NFwdElectronTightSelector::applyParticleLevel: "
          << "no separate fwdElectrons container currently defined at particleLevel"
          << "FWDEL_N_TIGHT always returning true at particle level.");
    return true;
  }
}
