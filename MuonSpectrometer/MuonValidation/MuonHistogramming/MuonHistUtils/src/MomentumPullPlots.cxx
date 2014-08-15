/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MomentumPullPlots.h"
#include "xAODTracking/TrackParticle.h"
//#include "xAODTracking/TrackParticleContainer.h"

namespace Muon{

void MomentumPullPlots::initializePlots(){

    dpt_idme   = Book1D("dpt_idme",  "idme_dpt;pt_{id} - pt_{me} [GeV];Entries / 0.2 GeV", 100,-100.,100);
    ddpt_idme  = Book1D("ddpt_idme", "idme_ddpt;(pt_{id} - pt_{me})/pt_{id};Entries", 300,-0.5,0.5);
    dphi_idme  = Book1D("dphi_idme", "idme_dphi;#phi_{id} - #phi_{me};Entries", 100,-0.1,0.1);
    deta_idme  = Book1D("deta_idme", "idme_deta;#eta_{id} - #eta_{me};Entries", 100,-0.1,0.1);

    dpt_cbme   = Book1D("dpt_cbme",  "cbme_dpt;pt_{cb} - pt_{me} [GeV];Entries / 0.2 GeV", 100,-100.,100);
    ddpt_cbme  = Book1D("ddpt_cbme", "cbme_ddpt;(pt_{cb} - pt_{me})/pt_{cb};Entries", 300,-0.5,0.5);
    dphi_cbme  = Book1D("dphi_cbme", "cbme_dphi;#phi_{cb} - #phi_{me};Entries", 100,-0.1,0.1);
    deta_cbme  = Book1D("deta_cbme", "cbme_deta;#eta_{cb} - #eta_{me};Entries", 100,-0.1,0.1);

}

void MomentumPullPlots::fill(const xAOD::Muon& mu) {
  const xAOD::TrackParticle* cb = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
  const xAOD::TrackParticle* id = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* me = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  
  if (cb && me){
    dpt_cbme->Fill( (cb->pt() - me->pt())*0.001);
    ddpt_cbme->Fill( (cb->pt() - me->pt())/cb->pt());
    dphi_cbme->Fill( cb->phi() - me->phi());
    deta_cbme->Fill( cb->eta() - me->eta());
  }

  if (id && me){
    dpt_idme->Fill( (id->pt() - me->pt())*0.001);
    ddpt_idme->Fill( (id->pt() - me->pt())/id->pt());
    dphi_idme->Fill( id->phi() - me->phi());
    deta_idme->Fill( id->eta() - me->eta());
  }
}

}
