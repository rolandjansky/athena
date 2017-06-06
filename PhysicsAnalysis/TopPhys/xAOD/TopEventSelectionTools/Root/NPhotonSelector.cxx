/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NPhotonSelector.h"

#include <algorithm>

namespace top {

NPhotonSelector::NPhotonSelector(const std::string& params) :
            SignValueSelector("PH_N", params, true) {
    checkMultiplicityIsInteger();
}

bool NPhotonSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Photon* phPtr){return phPtr->pt() > value();};
    auto count = std::count_if(event.m_photons.begin(), event.m_photons.end(), func);
    return checkInt(count, multiplicity());
}

bool NPhotonSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
   auto func = [&](const xAOD::TruthParticle* truPhPtr){return truPhPtr->pt() > value();};
   auto count = std::count_if(event.m_photons->begin(), event.m_photons->end(), func);
   return checkInt(count, multiplicity());
}

}
