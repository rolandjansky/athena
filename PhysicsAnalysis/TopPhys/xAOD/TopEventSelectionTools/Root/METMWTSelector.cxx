/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/METMWTSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

METMWTSelector::METMWTSelector(const std::string& params) :
            SignValueSelector("MET+MWT", params) {
}

bool METMWTSelector::apply(const top::Event& event) const {
    const xAOD::IParticle* lepton = 0;
    if (!event.m_electrons.empty())
        lepton = event.m_electrons.front();
    else if (!event.m_muons.empty())
        lepton = event.m_muons.front();
    else {
        std::cout << "MET+MWT Tool: Not got a charged lepton" << std::endl;
        exit(1);
    }

    const double metmwt = event.m_met->met() + top::mwt(*lepton, *event.m_met);
    return checkFloat(metmwt, value());
}

bool METMWTSelector::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons
	 or not event.m_met ){
	return false;
    }

    const xAOD::IParticle* lepton = 0;
    if (!event.m_electrons->empty())
	lepton = event.m_electrons->front();
    else if (!event.m_muons->empty())
	lepton = event.m_muons->front();
    else {
	std::cout << "MET+MWT Tool: Not got a charged lepton" << std::endl;
	exit(1);
    }

    const double metmwt = event.m_met->met() + top::mwt(*lepton, *event.m_met);
    return checkFloat(metmwt, value());
}


}
