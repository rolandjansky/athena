/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
/********************************************************************
NAME:     JetSeedBuilder.cxx
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  N. Meyer
CREATED:  Nov 27 2007

17/03/2010: AK: change to P4Helpers
16/05/2011: FF: fix rare case if no jet_seed found at all (coverity 21744)
  Dec 2011: FF: changes for tauRec4
 ********************************************************************/

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "JetSeedBuilder.h"

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

JetSeedBuilder::JetSeedBuilder(const std::string& name) :
  TauRecToolBase(name) {
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

JetSeedBuilder::~JetSeedBuilder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::initialize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// finalize 
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::finalize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::execute(xAOD::TauJet& pTau) const {

	ATH_MSG_DEBUG("Starting execute");

	const xAOD::Jet* pJetSeed = nullptr;
	if (pTau.jetLink().isValid()) pJetSeed = * pTau.jetLink();
    else { 
		ATH_MSG_ERROR("seed is not a jet -> tau will not be reconstructed");
		return StatusCode::FAILURE;
	}

	ATH_MSG_DEBUG("seed is Jet with"
			<< " pt=" << pJetSeed->pt()
			<< " eta=" << pJetSeed->eta()
			<< " phi=" << pJetSeed->phi()
	);


	///XXX need to decide whether to remove this, because there's no author flag in xAOD::TauJet

	//***********************************************************************
	// set author to both-seeded because we don't want to break any analysis
	// most of the analyses are using author==3 or author==1
	// so setting author=3 should be safe for everybody
	// calo seeded tau
	// pTau.setAuthor(TauJetParameters::tauRec);
	// // track seeded tau
	// pTau.setAuthor(TauJetParameters::tau1P3P);
	//***********************************************************************
	
    if (inTrigger() && pJetSeed->e() < 0) {
		// SL/SX trigger mode with negative jet_seed - do not set TauJet eta and phi in JetSeedBuilder
		ATH_MSG_DEBUG("TauJet eta/phi will be set in Level2 Trigger for negative energy jet");
		pTau.setP4(pJetSeed->pt(),pTau.eta(),pTau.phi(),0.0);

	} else {

		if ( pJetSeed->pt() > 1e-7)
			pTau.setP4(static_cast<float>( pJetSeed->pt() ) ,static_cast<float>( pJetSeed->eta() ) ,static_cast<float>( pJetSeed->phi() ) ,0.0 );
		else
			pTau.setP4(static_cast<float>( 1e-7 ) ,static_cast<float>( pJetSeed->eta() ) ,static_cast<float>( pJetSeed->phi() ) , 0.0 );

		//store 4-vector of seed
		pTau.setP4( xAOD::TauJetParameters::JetSeed, pJetSeed->pt(), pJetSeed->eta(), pJetSeed->phi(), pJetSeed->m() );
	}

	return StatusCode::SUCCESS;
}


#endif
