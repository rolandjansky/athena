/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

#include "tauRec/LockTauContainers.h"

//**********************************
// Constructor
//**********************************

LockTauContainers::LockTauContainers(
        const std::string& type,
        const std::string& name,
        const IInterface* parent) :
TauToolBase(type, name, parent) {
    declareInterface<TauToolBase > (this);
}


//************************************
// event finalize method
//************************************

StatusCode LockTauContainers::eventFinalize(TauCandidateData *data) {
    ATH_MSG_VERBOSE("LockTauContainers::eventFinialize");
    
    //-------------------------------------------------------------------------
    // Lock Containers
    //-------------------------------------------------------------------------
    
    xAOD::TauJetContainer* pContainer = data->xAODTauContainer;
    xAOD::TauJetAuxContainer *pAuxContainer = data->tauAuxContainer;

    StatusCode sc;

    if (pContainer)
        sc = evtStore()->setConst(pContainer);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Cannot set tau container to const");
        return StatusCode::FAILURE;
    }

    if (pAuxContainer)
        sc = evtStore()->setConst(pAuxContainer);
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Cannot set tau aux container to const");
        return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("all tau containers are now locked");
    return StatusCode::SUCCESS;
}
