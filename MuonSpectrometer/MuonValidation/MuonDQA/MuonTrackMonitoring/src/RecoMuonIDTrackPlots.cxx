/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonIDTrackPlots.h"

RecoMuonIDTrackPlots::RecoMuonIDTrackPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oAllPlots(this, "/", "Reco Muon"),
m_oImpactPlots(this, "/"),
m_oTrkRecoInfoPlots(this, "/"),
m_oIDHitPlots(this,"/"),
m_pt_broad(nullptr),
m_eta_phi_broad(nullptr)
{}

void RecoMuonIDTrackPlots::initializePlots(){
  //booking histograms
  m_pt_broad = Book1D("_pt_broad", "High p_{T} Distribution", 70, 100, 1500);
  m_eta_phi_broad = Book2D("_eta_phi_broad", "High p_{T} Muon #eta #phi Distribution;;#eta;#phi", 52, -2.6, 2.6, 64, -3.2, 3.2);
}

//when the plot function called with a Muon Container
//loop through each muon, get the corresponding link and fill it

//when the plot function called with a Muon
//get's the corresponding link and fill it
void RecoMuonIDTrackPlots::fill(const xAOD::Muon& mu, int component){
  if (component == 2 ){
    ElementLink<xAOD::TrackParticleContainer> Mu_idtrack = mu.inDetTrackParticleLink();
    if(Mu_idtrack.isValid()){
      const xAOD::TrackParticle* trk = *Mu_idtrack;
      fill(*trk);
    }
  } 
}


void RecoMuonIDTrackPlots::fill(const xAOD::TrackParticle& muTP){
  m_oAllPlots.fill(muTP);
  m_oImpactPlots.fill(muTP);
  m_oTrkRecoInfoPlots.fill(muTP);
  m_oIDHitPlots.fill(muTP);
  if (muTP.pt()/1000.0 > 100) {//ony for high pt muons
    m_pt_broad->Fill(muTP.pt()/1000.0);
    m_eta_phi_broad->Fill(muTP.eta(), muTP.phi());
  }

}


