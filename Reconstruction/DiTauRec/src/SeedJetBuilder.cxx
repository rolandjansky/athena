/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DiTauRec/SeedJetBuilder.h"
#include "DiTauRec/DiTauToolBase.h"
#include "DiTauRec/DiTauCandidateData.h"
#include "StoreGate/ReadHandle.h"

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

SeedJetBuilder::SeedJetBuilder(const std::string& type,
    const std::string& name,
    const IInterface * parent) :
    DiTauToolBase(type, name, parent)
{
	declareInterface<DiTauToolBase > (this);
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
  ATH_CHECK( m_jetContainerName.initialize() );
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// execute
//-------------------------------------------------------------------------

StatusCode SeedJetBuilder::execute(DiTauCandidateData * data,
                                   const EventContext& ctx) const {

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
        SG::ReadHandle<xAOD::JetContainer> pJetCont (m_jetContainerName, ctx);
	

	pDiTau->setJet(pJetCont.get(), pSeed);

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
