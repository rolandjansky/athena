/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/MLLWindowSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

MLLWindow::MLLWindow(std::string params) :
            m_cutvalue0(0.),
            m_cutvalue1(0.) {
    std::istringstream(params) >> m_cutvalue0 >> m_cutvalue1;
}

bool MLLWindow::apply(const top::Event& event) const {
    const xAOD::IParticle* lepton0 = 0;
    const xAOD::IParticle* lepton1 = 0;

    if (event.m_electrons.size() == 2) {
        lepton0 = event.m_electrons[0];
        lepton1 = event.m_electrons[1];
    } else if (event.m_muons.size() == 2) {
        lepton0 = event.m_muons[0];
        lepton1 = event.m_muons[1];
    } else {
        std::cout << "need two charged leptons of the same flavour" << std::endl;
        exit(1);
    }

    const double mll = top::invariantMass(*lepton0, *lepton1);
    return mll < m_cutvalue0 || mll > m_cutvalue1;
}


bool MLLWindow::applyParticleLevel(const top::ParticleLevelEvent& event) const {
    // If any of the required collections is a nullptr (i.e. has not been
    // loaded) return false.
    if ( not event.m_electrons
	 or not event.m_muons ){
	return false;
    }

    const xAOD::IParticle* lepton0 = 0;
    const xAOD::IParticle* lepton1 = 0;

    if (event.m_electrons->size() == 2) {
        lepton0 = (*event.m_electrons)[0];
        lepton1 = (*event.m_electrons)[1];
    } else if (event.m_muons->size() == 2) {
        lepton0 = (*event.m_muons)[0];
        lepton1 = (*event.m_muons)[1];
    } else {
        std::cout << "need two charged leptons of the same flavour" << std::endl;
        exit(1);
    }

    const double mll = top::invariantMass(*lepton0, *lepton1);
    return mll < m_cutvalue0 || mll > m_cutvalue1;
}

std::string MLLWindow::name() const {
    std::stringstream ss;
    ss << "MLLWIN " << m_cutvalue0 << " " << m_cutvalue1;
    return ss.str();
}

}
