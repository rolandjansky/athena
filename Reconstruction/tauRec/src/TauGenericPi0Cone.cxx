/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauGenericPi0Cone.cxx
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-04
//
//
//-----------------------------------------------------------------------------

#include <GaudiKernel/IToolSvc.h>
#include <GaudiKernel/ListItem.h>

#include "FourMomUtils/P4Helpers.h"
#include "FourMom/P4EEtaPhiM.h"
#include "Particle/TrackParticle.h"

#include "tauRec/TauCandidateData.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauJetParameters.h"

#include "tauRec/TauGenericPi0Cone.h"
#include "tauRec/TauTrackFilterUtils.h"

#include "TLorentzVector.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauGenericPi0Cone::TauGenericPi0Cone(const std::string &type,
    const std::string &name,
    const IInterface *parent) :
TauToolBase(type, name, parent) {
    declareInterface<TauToolBase > (this);
}


//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauGenericPi0Cone::~TauGenericPi0Cone() {
}


//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauGenericPi0Cone::initialize() {
    ATH_MSG_VERBOSE("TauGenericPi0Cone Initialising");

    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauGenericPi0Cone::finalize() {
    ATH_MSG_VERBOSE("TauGenericPi0Cone Finalizing");

    return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauGenericPi0Cone::execute(TauCandidateData *data) {
    ATH_MSG_VERBOSE("TauGenericPi0Cone Executing");

    xAOD::TauJet *pTau = data->xAODTau;

    TLorentzVector tau;
    tau.SetPtEtaPhiE(pTau->pt(),
                     pTau->eta(),
                     pTau->phi(),
                     pTau->e());

    int nProng = pTau->trackFilterProngs();
    int flag = pTau->trackFilterQuality();
    float pi0angle = 0.; //TODO sane default value

    // Get pi0 cone
    if(flag != 0){
        int recProng = 0;
        if((nProng == 3)||(nProng == 2)) recProng = 3;
        if(nProng == 1) recProng = 1;
        pi0angle = TauTrackFilterUtils::ComputePi0Cone(recProng,tau);
     }

    // Convert to dR.
    m_pi0conedr = std::min(pi0angle*cosh(tau.Eta()), 0.2);

    // Store result
    pTau->setPi0ConeDR(m_pi0conedr);

    return StatusCode::SUCCESS;
}

