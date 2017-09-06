/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NMuonSelector.h"

#include <algorithm>

namespace top {

NMuonSelector::NMuonSelector(const std::string& params) :
            SignValueSelector("MU_N", params, true) {
    checkMultiplicityIsInteger();
}

bool NMuonSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Muon* muPtr){return muPtr->pt() > value();};
    auto count = std::count_if(event.m_muons.begin(), event.m_muons.end(), func);
    return checkInt(count, multiplicity());
}

bool NMuonSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_muons  ){
	return false;
    }

    auto func = [&](const xAOD::TruthParticle* truMuPtr){return truMuPtr->pt() > value();};
    auto count = std::count_if(event.m_muons->begin(), event.m_muons->end(), func);
    return checkInt(count, multiplicity());
}

}
