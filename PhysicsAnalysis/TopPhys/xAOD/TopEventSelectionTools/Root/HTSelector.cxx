/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/HTSelector.h"

#include "TopEvent/EventTools.h"

namespace top {

HTSelector::HTSelector(const std::string& params) :
        SignValueSelector("HT", params) {
}

bool HTSelector::apply(const top::Event& event) const {
    const double ht = top::ht(event);
    return checkFloat(ht, value());
}

bool HTSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons
	 or not event.m_jets  ){
	return false;
    }

    double sumHt = 0.0;

    for (const auto el : * event.m_electrons)
        sumHt += el->pt();

    for (const auto mu : * event.m_muons)
        sumHt += mu->pt();

    for (const auto jet : * event.m_jets)
        sumHt += jet->pt();

    return checkFloat(sumHt, value());
}

}
