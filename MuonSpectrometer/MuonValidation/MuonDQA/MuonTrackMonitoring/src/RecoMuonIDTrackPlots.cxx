/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonIDTrackPlots.h"

RecoMuonIDTrackPlots::RecoMuonIDTrackPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oAllPlots(this, "/", "Reco Muon"),
m_oImpactPlots(this, "/"),
m_oTrkRecoInfoPlots(this, "/"),
m_oIDHitPlots(this,"/")
{}

void RecoMuonIDTrackPlots::initializePlots(){
  //booking histograms
}

//when the plot function called with a Muon Container
//loop through each muon, get the corresponding link and fill it

//when the plot function called with a Muon
//get's the corresponding link and fill it
void RecoMuonIDTrackPlots::fill(const xAOD::Muon& mu, int component){
  if (component == 2 ){
    ElementLink<xAOD::TrackParticleContainer> Mu_idtrack = mu.inDetTrackParticleLink();
    if(Mu_idtrack.isValid()){
      //std::cout<<"Plot base test ID track"<<std::endl;
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

}


