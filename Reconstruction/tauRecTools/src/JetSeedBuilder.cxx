/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//#include "EventKernel/SignalStateHelper.h"
//#include "FourMomUtils/P4Helpers.h"

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
  TauRecToolBase(name),
  m_jetCollectionName("AntiKt4LCTopoJets"),
  m_maxJetdist(0.1),
  m_minJetPt(10000.0),
  m_switch_jets_em_scale(false) {
  declareProperty("JetCollection", m_jetCollectionName);
  declareProperty("maxDist", m_maxJetdist);
  declareProperty("minPt", m_minJetPt);
  declareProperty("SwitchJetsEmScale", m_switch_jets_em_scale);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

JetSeedBuilder::~JetSeedBuilder() {
}


void JetSeedBuilder::print() const {
  
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::initialize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::finalize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Event Finalize
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::eventFinalize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode JetSeedBuilder::execute(xAOD::TauJet& pTau) {

	StatusCode sc;

	ATH_MSG_DEBUG("Starting execute");

	bool inTrigger = tauEventData()->inTrigger();

	if(inTrigger)
		ATH_MSG_DEBUG("inTrigger read properly");

	const xAOD::JetContainer *pJetColl;

	if (sc.isSuccess() && inTrigger) {
		// called by Trigger
		// retrieve JetCollection for trigger
		ATH_MSG_DEBUG("Try to retrieve object from DataContainer");
		//sc = tauEventData()->getObject("JetCollection", pJetColl);
		// Try a different approach: grab it directly
		sc = true;
		pJetColl = tauEventData()->seedContainer;
		if (sc.isFailure() || !pJetColl) {
			ATH_MSG_DEBUG("no JetCollection for trigger available");
			ATH_MSG_DEBUG("retrieve standard JetCollection <" << m_jetCollectionName << ">");
			// retrieve standard jet collection
			sc = evtStore()->retrieve(pJetColl, m_jetCollectionName);
			if (sc.isFailure()) {
				ATH_MSG_WARNING("no JetCollection retrieved");
				return StatusCode::FAILURE;
			}
		}
	} else {
		// called by offline tauRec
		// retrieve standard jet collection
		sc = evtStore()->retrieve(pJetColl, m_jetCollectionName);
		if (sc.isFailure()) {
			ATH_MSG_WARNING("no JetCollection retrieved");
			return StatusCode::FAILURE;
		}
	}

	ATH_MSG_DEBUG("Pulling out the seed");

	const xAOD::Jet* pJetSeed = 0;
	if (pTau.jetLink().isValid()) pJetSeed = * pTau.jetLink();
	if (!pJetSeed) {
		ATH_MSG_DEBUG("seed is not a jet -> tau will not be reconstructed");
		return StatusCode::FAILURE;
	}

	ATH_MSG_DEBUG("Seed extracted");
	ATH_MSG_DEBUG("seed is Jet with"
			<< " pt=" << pJetSeed->pt()
			<< " eta=" << pJetSeed->eta()
			<< " phi=" << pJetSeed->phi()
	);

	xAOD::TauJetContainer* pContainer = tauEventData()->xAODTauContainer;
	if(pContainer==0){
	  pContainer = static_cast<xAOD::TauJetContainer*> (pTau.container());
	}
	if(pContainer==0) {
	  ATH_MSG_FATAL("Can't find tau Container");
	  return StatusCode::FAILURE;
	}
	
	xAOD::TauJetContainer::iterator itTau = pContainer->begin();
	xAOD::TauJetContainer::iterator itTauE = pContainer->end();

	for (; itTau != itTauE; ++itTau) {
	  if( (*itTau) == &pTau ) continue;
		if ( (*itTau)->jetLink().isValid() ) {
			if ( pJetSeed == ( * (*itTau)->jetLink() ) ) {
				ATH_MSG_DEBUG("seed already used");
				return StatusCode::FAILURE;
			}
		}
	}

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

	//
	// ATTENTION: direction will be overwritten later by TauAxis and TauEnergyCalibration
	//
	if (inTrigger && pJetSeed->e() < 0) {
		// SL/SX trigger mode with negative jet_seed - do not set TauJet eta and phi in JetSeedBuilder
		ATH_MSG_DEBUG("TauJet eta/phi will be set in Level2 Trigger for negative energy jet");

		// pTau.setDetail(xAOD::TauJetParameters::seedCalo_eta , static_cast<float>( pTau.eta() ) );
		// pTau.setDetail(xAOD::TauJetParameters::seedCalo_phi , static_cast<float>( pTau.phi() ) );

		pTau.setP4(pJetSeed->pt(),pTau.eta(),pTau.phi(),0.0);

	} else {
		if (m_switch_jets_em_scale) {
			ATH_MSG_INFO("trying to set seed jet signal state to EMSCALE, but this code has not been migrated to xAOD::Jet yet");
			//XXX still need to look up how signal states are handled for the xAOD jets
			// SignalStateHelper sigstateH(P4SignalState::JETEMSCALE);
			// sigstateH.controlObject(pJetSeed);
		}

		// pTau.setDetail(xAOD::TauJetParameters::seedCalo_eta , static_cast<float>( pJetSeed->eta() ) );
		// pTau.setDetail(xAOD::TauJetParameters::seedCalo_phi , static_cast<float>( pJetSeed->phi() ) );
		if ( pJetSeed->pt() > 1e-7)
			pTau.setP4(static_cast<float>( pJetSeed->pt() ) ,static_cast<float>( pJetSeed->eta() ) ,static_cast<float>( pJetSeed->phi() ) ,0.0 );
		else
			pTau.setP4(static_cast<float>( 1e-7 ) ,static_cast<float>( pJetSeed->eta() ) ,static_cast<float>( pJetSeed->phi() ) , 0.0 );

		//store 4-vector of seed
		pTau.setP4( xAOD::TauJetParameters::JetSeed, pJetSeed->pt(), pJetSeed->eta(), pJetSeed->phi(), pJetSeed->m() );
	}

	// set now the link to the jet seed
	// if not already set
	if(!pTau.jetLink().isValid() || (*pTau.jetLink())!=pJetSeed ) pTau.setJet(pJetColl, pJetSeed);

	if ( pTau.jetLink().isValid() ) {
		ATH_MSG_DEBUG("seed associated with tau is Jet with"
				<< " pt=" << (*pTau.jetLink())->pt()
				<< " eta=" << (*pTau.jetLink())->eta()
				<< " phi=" << (*pTau.jetLink())->phi()
		);
	}

	return StatusCode::SUCCESS;
}


#endif
