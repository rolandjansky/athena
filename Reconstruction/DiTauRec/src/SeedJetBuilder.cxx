/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/SeedJetBuilder.h"
#include "DiTauRec/DiTauToolBase.h"

#include "DiTauRec/DiTauCandidateData.h"
//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

SeedJetBuilder::SeedJetBuilder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent),
    m_jetContainerName("AntiKt10LCTopoJets")
{
	declareInterface<DiTauToolBase > (this);
	declareProperty("JetCollection", m_jetContainerName);
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

SeedJetBuilder::~SeedJetBuilder() {
}

//-------------------------------------------------------------------------
// initialize
//-------------------------------------------------------------------------

StatusCode SeedJetBuilder::initialize() {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Event Finalize
//-------------------------------------------------------------------------

StatusCode SeedJetBuilder::eventFinalize(DiTauCandidateData *) {
	return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode SeedJetBuilder::execute(DiTauCandidateData * data) {

	ATH_MSG_DEBUG("jet seed finder executing...");

	xAOD::DiTauJet* pDiTau = data->xAODDiTau;

	if (pDiTau == NULL) {
		ATH_MSG_ERROR("no di-tau candidate given");
		return StatusCode::FAILURE;
	}


	// TODO: trigger stuff...

	const xAOD::Jet* pSeed = data->seed;
	if (!pSeed) {
		ATH_MSG_WARNING("no seed given. Di-Tau can not be reconstructed.");
		return StatusCode::FAILURE;
	}

	// TODO: check if seed is already used?

	ATH_MSG_DEBUG("ditau candidate: pt: "<<pSeed->pt()<< " eta: "<<pSeed->eta()<< " phi: "<<pSeed->phi()<< " m: "<<pSeed->m());
	pDiTau->setP4(pSeed->pt(), pSeed->eta(), pSeed->phi(), pSeed->m());


	// retrieve jet container
	StatusCode sc;
	const xAOD::JetContainer* pJetCont = nullptr;
	sc = evtStore()->retrieve(pJetCont, m_jetContainerName);
	

	pDiTau->setJet(pJetCont, pSeed);

	if (pDiTau->jetLink().isValid()) {
		ATH_MSG_DEBUG("assciated seed jet with"
			<<" pt:" << (*pDiTau->jetLink())->pt()
			<<" eta:" << (*pDiTau->jetLink())->eta()
			<<" phi:" << (*pDiTau->jetLink())->phi() );
	}
	else
		ATH_MSG_WARNING("jet link could not be registered");

	if (data->Rjet == 0 || data->Rsubjet == 0 || data->Rcore == 0 || 
	    data->Rjet < data->Rsubjet || data->Rsubjet < data->Rcore ) {
	    ATH_MSG_WARNING("ditau radius parameters are not set correctly." <<
	    	" R_jet: " << data->Rjet <<
	    	" R_subjet: " << data->Rsubjet <<
	    	" R_core: " << data->Rcore);
	}

	pDiTau->setDetail(xAOD::DiTauJetParameters::R_jet, static_cast<float>(data->Rjet));
	pDiTau->setDetail(xAOD::DiTauJetParameters::R_subjet, static_cast<float>(data->Rsubjet));
	pDiTau->setDetail(xAOD::DiTauJetParameters::R_core, static_cast<float>(data->Rcore));




 	return StatusCode::SUCCESS;
}
