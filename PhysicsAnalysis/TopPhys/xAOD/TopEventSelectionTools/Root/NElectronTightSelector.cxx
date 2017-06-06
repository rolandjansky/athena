/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NElectronTightSelector.h"

#include <algorithm>

namespace top {

NElectronTightSelector::NElectronTightSelector(const std::string& params) :
            SignValueSelector("EL_N_TIGHT", params, true) {
    checkMultiplicityIsInteger();
}

bool NElectronTightSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Electron* elPtr){return elPtr->pt() > value() && elPtr->auxdataConst<char>("passPreORSelection");};
    auto count = std::count_if(event.m_electrons.begin(), event.m_electrons.end(), func);
    return checkInt(count, multiplicity());
}

bool NElectronTightSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons ){
	return false;
    }

    auto func = [&](const xAOD::TruthParticle* truElPtr){return truElPtr->pt() > value();};
    auto count = std::count_if(event.m_electrons->begin(), event.m_electrons->end(), func);
    return checkInt(count, multiplicity());
}

}
