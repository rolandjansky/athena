/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MomentumPullPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

using namespace xAOD::P4Helpers;
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

    pt_cbme = Book2D("pt_cbme", "pt_cbme; pt_{cb} [GeV]; pt_{me} [GeV]", 100, 0.0, 100.0, 100, 0.0, 100.0);
    pt_cbid = Book2D("pt_cbid", "pt_cbid; pt_{cb} [GeV]; pt_{id} [GeV]", 100, 0.0, 100.0, 100, 0.0, 100.0);
    pt_meid = Book2D("pt_meid", "pt_meid; pt_{me} [GeV]; pt_{id} [GeV]", 100, 0.0, 100.0, 100, 0.0, 100.0);
}

  void MomentumPullPlots::fill(const xAOD::Muon& mu, float weight) {
  const xAOD::TrackParticle* cb = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
  const xAOD::TrackParticle* id = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

  ////////////////// sorting out the mess with the link to the extrapolated muon
  
  //check correct numbering in Muon.h -> OK!
  const xAOD::TrackParticle* me = mu.trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
  if (!me) me = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
    
  if (cb && me){
    dpt_cbme->Fill( (cb->pt() - me->pt())*0.001, weight);
    ddpt_cbme->Fill( (cb->pt() - me->pt())/cb->pt(), weight);
    dphi_cbme->Fill( deltaPhi(cb->phi(), me->phi()),weight);
    deta_cbme->Fill( cb->eta() - me->eta(), weight);
    pt_cbme->Fill( cb->pt()*0.001, me->pt()*0.001 , weight);
  }

  if (id && me){
    dpt_idme->Fill( (id->pt() - me->pt())*0.001, weight);
    ddpt_idme->Fill( (id->pt() - me->pt())/id->pt(), weight);
    dphi_idme->Fill( deltaPhi(id->phi() , me->phi()), weight);
    deta_idme->Fill( id->eta() - me->eta(), weight);
    pt_meid->Fill( me->pt()*0.001, id->pt()*0.001 , weight);
  }

  if(id && cb) 
    pt_cbid->Fill( cb->pt()*0.001, id->pt()*0.001, weight );
}
  
}
