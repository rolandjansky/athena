/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauDiscriminant/TauMuonVeto.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauDefs.h"

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
StatusCode TauMuonVeto::execute(xAOD::TauJet& tau)
{
    float etpt = tau.auxdata<float>("etOverPtLeadTrk");
    float ptEt = etpt > 0 ? 1./ etpt : -1111.;

    float etEMScale = tau.auxdata<float>("etEMAtEMScale") + tau.auxdata<float>("etHadAtEMScale");
    float EMfrac = (etEMScale != 0) ? tau.auxdata<float>("etEMAtEMScale") / etEMScale : -1111.;

    int ntracks = tau.nTracks();
    float eta = tau.eta();

    bool muVeto = tauMuonVeto(ntracks, EMfrac, ptEt, eta);

    tau.setIsTau(xAOD::TauJetParameters::MuonVeto, muVeto);

    return StatusCode::SUCCESS;
}
