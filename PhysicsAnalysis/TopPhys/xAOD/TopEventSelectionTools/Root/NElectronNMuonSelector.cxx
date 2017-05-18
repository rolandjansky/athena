/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NElectronNMuonSelector.h"

namespace top {

NElectronNMuonSelector::NElectronNMuonSelector(const std::string& params) :
            SignValueSelector("EL_N_OR_MU_N", params, true) {
    checkMultiplicityIsInteger();
}

bool NElectronNMuonSelector::apply(const top::Event& event) const {
    auto elFunc = [&](const xAOD::Electron* elPtr){return elPtr->pt() > value();};
    auto elCount = std::count_if(event.m_electrons.begin(), event.m_electrons.end(), elFunc);

    auto muFunc = [&](const xAOD::Muon* muPtr){return muPtr->pt() > value();};
    auto muCount = std::count_if(event.m_muons.begin(), event.m_muons.end(), muFunc);

    return checkInt(elCount, multiplicity()) || checkInt(muCount, multiplicity());
}

bool NElectronNMuonSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons ){
	return false;
    }

    auto elFunc = [&](const xAOD::TruthParticle* truElPtr){return truElPtr->pt() > value();};
    auto elCount = std::count_if(event.m_electrons->begin(), event.m_electrons->end(), elFunc);

    auto muFunc = [&](const xAOD::TruthParticle* truMuPtr){return truMuPtr->pt() > value();};
    auto muCount = std::count_if(event.m_muons->begin(), event.m_muons->end(), muFunc);

    return checkInt(elCount, multiplicity()) || checkInt(muCount, multiplicity());
}

}
