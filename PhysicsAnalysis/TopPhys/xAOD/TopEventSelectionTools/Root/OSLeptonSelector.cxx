/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/OSLeptonSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

bool OSLeptonSelector::apply(const top::Event& event) const {
    unsigned int negative_counter = 0;
    unsigned int positive_counter = 0;

    for (const auto* const elPtr : event.m_electrons)
        if (elPtr->charge() > 0)
            ++positive_counter;
        else
            ++negative_counter;

    for (const auto* const muPtr : event.m_muons)
        if (muPtr->charge() > 0)
            ++positive_counter;
        else
            ++negative_counter;

    return positive_counter > 0 && negative_counter > 0;
}

bool OSLeptonSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
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

    return positive_counter > 0 && negative_counter > 0;
}

std::string  OSLeptonSelector::name() const {
    return "OS";
}

}
