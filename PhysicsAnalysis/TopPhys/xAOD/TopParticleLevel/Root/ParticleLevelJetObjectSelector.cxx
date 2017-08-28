/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelJetObjectSelector.cxx
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 15:51:27 2015

#include "TopParticleLevel/ParticleLevelJetObjectSelector.h"

namespace top {

    ParticleLevelJetObjectSelector::ParticleLevelJetObjectSelector( Options opt /* = Options() */)
	: m_opt( opt ){ /* Deliberately Empty */ }

    /* virtual */ bool
    ParticleLevelJetObjectSelector::apply( const xAOD::Jet & jet ) {
	// --------------------------------------------------
	// Apply kinematic cut on the pT:
	//     must exceed 25 GeV
	if ( jet.pt() < m_opt.pt_min ){ return false; }

	// --------------------------------------------------
	// Apply kinematic cut on the eta:
	//     must be less than or equal to 2.5
	if ( std::abs( jet.eta() ) > m_opt.eta_max ){ return false; }

	// --------------------------------------------------
	// Everything that reaches this point has passed the selection
	return true;
    }

}
