/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/MWTSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

MWTSelector::MWTSelector(const std::string& params) :
            SignValueSelector("MWT", params) {
}

bool MWTSelector::apply(const top::Event& event) const {
    const xAOD::IParticle* lepton = 0;
    if (!event.m_electrons.empty())
        lepton = event.m_electrons.front();
    else if (!event.m_muons.empty())
        lepton = event.m_muons.front();
    else {
        std::cout << "MWTSelector: Not got a charged lepton" << std::endl;
        exit(1);
    }

    const double mwt = top::mwt(*lepton, *event.m_met);
    return checkFloat(mwt, value());
}

bool MWTSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons
	 or not event.m_met  ){
	return false;
    }

    const xAOD::IParticle* lepton = 0;
    if (!event.m_electrons->empty())
	lepton = event.m_electrons->front();
    else if (!event.m_muons->empty())
	lepton = event.m_muons->front();
    else {
	std::cout << "MWTSelector: Not got a charged lepton" << std::endl;
	exit(1);
    }

    const double mwt = top::mwt(*lepton, *event.m_met);
    return checkFloat(mwt, value());
}


}
