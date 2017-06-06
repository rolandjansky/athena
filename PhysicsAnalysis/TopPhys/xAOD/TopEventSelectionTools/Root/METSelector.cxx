/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/METSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

METSelector::METSelector(const std::string& params) :
            SignValueSelector("MET", params) {
}

bool METSelector::apply(const top::Event& event) const {
    return checkFloat(event.m_met->met(), value());
}

bool METSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_met  ){
	return false;
    }

    return checkFloat(event.m_met->met(), value());
}

}
