/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

#include "LockTauContainers.h"

//**********************************
// Constructor
//**********************************

LockTauContainers::LockTauContainers(
        const std::string& name) :
  TauRecToolBase(name) {
}


//************************************
// initialize method
//************************************

StatusCode LockTauContainers::initialize() {
  return StatusCode::SUCCESS;
}

//************************************
// finalize method
//************************************

StatusCode LockTauContainers::finalize() {
  return StatusCode::SUCCESS;
}

//************************************
// event finalize method
//************************************

StatusCode LockTauContainers::eventFinalize() {
    ATH_MSG_VERBOSE("LockTauContainers::eventFinialize");
    
    //-------------------------------------------------------------------------
    // Lock Containers
    //-------------------------------------------------------------------------
    
    xAOD::TauJetContainer* pContainer = tauEventData()->xAODTauContainer;
    xAOD::TauJetAuxContainer *pAuxContainer = tauEventData()->tauAuxContainer;

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
#endif
