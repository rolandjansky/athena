/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/SSLeptonTightSelector.h"
#include "TopEvent/EventTools.h"

// Note: initial version committed on Dec/18/2014
//   Can't yet fully reproduce the selection used by Run 1 Top/Exotics SS+bjets analysis
//   One thing needed to reproduce that is loose leptons
//   Need to support selection of leptons with different quality/isolation at once
//   Need to support applying SF to leptons with different quality/isolation at once

namespace top {

bool SSLeptonTightSelector::apply(const top::Event& event) const {
    unsigned int negative_counter = 0;
    unsigned int positive_counter = 0;

    for (const auto* const elPtr : event.m_electrons)
        if (elPtr->charge() > 0 && elPtr->auxdataConst<char>("passPreORSelection"))
            ++positive_counter;
        else if (elPtr->charge() < 0 && elPtr->auxdataConst<char>("passPreORSelection"))
            ++negative_counter;

    for (const auto* const muPtr : event.m_muons)
        if (muPtr->charge() > 0 && muPtr->auxdataConst<char>("passPreORSelection"))
            ++positive_counter;
        else if (muPtr->charge() < 0 && muPtr->auxdataConst<char>("passPreORSelection"))
            ++negative_counter;

    return positive_counter > 1 || negative_counter > 1;
}

bool SSLeptonTightSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons ){
	return false;
    }

    unsigned int negative_counter = 0;
    unsigned int positive_counter = 0;

    for (const auto elPtr : * event.m_electrons)
        if (elPtr->charge() > 0)
            ++positive_counter;
        else
            ++negative_counter;

    for (const auto muPtr : * event.m_muons)
        if (muPtr->charge() > 0)
            ++positive_counter;
        else
            ++negative_counter;

    return positive_counter > 1 || negative_counter > 1;
}


std::string  SSLeptonTightSelector::name() const {
    return "SS_TIGHT";
}

}
