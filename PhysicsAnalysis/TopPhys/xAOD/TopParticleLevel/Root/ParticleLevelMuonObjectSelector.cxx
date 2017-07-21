/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelMuonObjectSelector.cxx
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 15:18:45 2015

#include "TopParticleLevel/ParticleLevelMuonObjectSelector.h"

#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "TopParticleLevel/TruthTools.h"

namespace top {

    ParticleLevelMuonObjectSelector::ParticleLevelMuonObjectSelector( Options opt /* = Options() */ )
	: m_opt( opt ){ /* Deliberately Empty */ }

    /* virtual */ bool
    ParticleLevelMuonObjectSelector::apply( const xAOD::TruthParticle & truthParticle ) {

	// Obtain the particle type and particle origin from the decoration
	// created by the MCTruthClassifier. For legacy reasons, try both
	// decoration names.
	unsigned int type;
	if ( truthParticle.isAvailable<unsigned int>( "particleType" ) ){
	    type = truthParticle.auxdata<unsigned int>( "particleType" );
	} else if ( truthParticle.isAvailable<unsigned int>( "classifierParticleType" ) ){
	    type = truthParticle.auxdata<unsigned int>( "classifierParticleType" );
	} else {
	    std::cerr << "Could not obtain MCTruthClassifier result decoration." << std::endl;
	    std::exit( 1 );
	}

	// --------------------------------------------------
	// Accept muons iff they do not come from a hadron
	using namespace MCTruthPartClassifier;
	if ( m_opt.not_from_hadron &&
	     type != ParticleType::IsoMuon ){
	    return false;
	}

	// If it is requested that tauons are hadrons, perform a check of the
	// ancestry: It shall not contain a single tauon.
	if ( m_opt.tau_is_hadron ){
	    auto truthProxy = truthParticle.auxdata<ElementLink<xAOD::TruthParticleContainer> >( "originalTruthParticle" );
	    if ( not truthProxy.isValid() ){
		std::cerr << "Could not obtain 'originalTruthParticle' reference." << std::endl;
		std::exit( 1 );
	    }
	    if ( truth::isLeptonFromTau( * truthProxy ) ){
		return false;
	    }
	}


	// --------------------------------------------------
	// Apply kinematic cut on the dressed pT:
	//     must exceed 25 GeV
	if ( truthParticle.auxdata<float>( "pt_dressed" ) < m_opt.pt_min ){ return false; }

	// --------------------------------------------------
	// Apply kinematic cut on the dressed eta:
	//     must be less than or equal to 2.5
	if ( std::abs( truthParticle.auxdata<float>( "eta_dressed" ) ) > m_opt.eta_max ){ return false; }


	// --------------------------------------------------
	// Everything that reaches this point has passed the selection
	return true;
    }

}
