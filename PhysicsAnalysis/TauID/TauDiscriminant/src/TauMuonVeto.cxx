/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauDiscriminant/TauMuonVeto.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
// #include "tauEvent/TauPID.h"
#include "xAODTau/TauDefs.h"
#include "TauDiscriminant/TauDetails.h"
#include <PathResolver/PathResolver.h>

using namespace xAOD;
//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauMuonVeto::prepare(const TauDetailsManager& manager)
{
    this->detailsManager = &manager;
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauMuonVeto::finalize()
{
    return StatusCode::SUCCESS;
}

bool tauMuonVeto(int ntracks, float EMfrac, float ptEt, float eta) {

    if(ntracks != 1) return false;
    if( (eta > -0.1 && eta < 0.1) || (eta > 1.15 && eta < 1.3) ){
      if(EMfrac < 0.15 && ptEt > 0.9) return true;
      if(EMfrac > 0.80 && ptEt > 1.0) return true;
    }
    else {
      if(EMfrac < 0.18 && ptEt > 1.9) return true;
      if(EMfrac > 0.82 && ptEt < 0.12) return true;
    }
    return false;

}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauMuonVeto::execute(xAOD::TauJet* tau, FakeTauBits* /*bits*/, FakeTauScores* /*scores*/)
{
    if (!detailsManager)
    {
        return StatusCode::FAILURE;
    }

    float ptEt = detailsManager->getFloatDetailValue(Details::PTLEADTRKOVERET);
    float EMfrac = detailsManager->getFloatDetailValue(Details::EMFRACTIONATEMSCALE);

    int ntracks = tau->nTracks();
    float eta = tau->eta();

    bool muVeto = tauMuonVeto(ntracks, EMfrac, ptEt, eta);

//     Analysis::TauPID *p_tauid = tau->tauID();

    tau->setIsTau(TauJetParameters::MuonVeto, muVeto);

    return StatusCode::SUCCESS;
}
