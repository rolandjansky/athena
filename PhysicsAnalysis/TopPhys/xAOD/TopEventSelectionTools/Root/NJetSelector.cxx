/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NJetSelector.h"

namespace top {

NJetSelector::NJetSelector(const std::string& params) :
            SignValueSelector("JET_N", params, true) {
    checkMultiplicityIsInteger();
}

bool NJetSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr){return jetPtr->pt() > value();};
    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
}

bool NJetSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_jets  ){
	return false;
    }

    auto func = [&](const xAOD::Jet* truJetPtr){return truJetPtr->pt() > value();};
    auto count = std::count_if(event.m_jets->begin(), event.m_jets->end(), func);
    return checkInt(count, multiplicity());
}

}
