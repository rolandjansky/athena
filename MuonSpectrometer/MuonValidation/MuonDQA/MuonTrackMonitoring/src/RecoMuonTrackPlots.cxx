/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonTrackPlots.h"

RecoMuonTrackPlots::RecoMuonTrackPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oAllPlots(this, "/", "Reco Muon"),
m_oImpactPlots(this, "/"),
m_oTrkRecoInfoPlots(this, "/"),
// m_oMSHitPlots(this,"/"),
// m_oResidualPlots_Global(this, "/Global/", ""),
// m_oResidualPlots_MDT(this, "/MDT/", ""),
// m_oResidualPlots_RPC_eta(this, "/RPC/", "eta"),
// m_oResidualPlots_RPC_phi(this, "/RPC/", "phi"),
// m_oResidualPlots_TGC_wire(this, "/TGC/", "wire"),
// m_oResidualPlots_TGC_strip(this, "/TGC/", "strip"),
// m_oResidualPlots_CSC_eta(this, "/CSC/", "eta"),
// m_oResidualPlots_CSC_phi(this, "/CSC/", "phi"),
m_pt_broad(NULL),
m_eta_phi_broad(NULL)
//m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
{}

void RecoMuonTrackPlots::initializePlots(){
  //booking histograms
  m_pt_broad = Book1D("_pt_broad", "High p_{T} Distribution", 70, 100, 1500);
  m_eta_phi_broad = Book2D("_eta_phi_broad", "High p_{T} Muon #eta #phi Distribution;#eta;#phi", 52, -2.6, 2.6, 64, -3.2, 3.2);
}

//when the plot function called with a Muon Container
//loop through each muon, get the corresponding link and fill it

//when the plot function called with a Muon
//get's the corresponding link and fill it
void RecoMuonTrackPlots::fill(const xAOD::Muon& mu, int component){

  if (component == 0 ) {
    ElementLink<xAOD::TrackParticleContainer> Mu_MStrack = mu.muonSpectrometerTrackParticleLink();
    if(Mu_MStrack.isValid()){
      //std::cout<<"Plot base test MS track"<<std::endl;
      const xAOD::TrackParticle* trk = *Mu_MStrack;
      fill(*trk);
    }
  }

  if (component == 1 ){

    ElementLink<xAOD::TrackParticleContainer> Mu_metrack = mu.trackParticleLink(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
    //ElementLink<xAOD::TrackParticleContainer> Mu_metrack = mu.extrapolatedMuonSpectrometerTrackParticleLink();
    if(Mu_metrack.isValid()){
      //std::cout<<"Plot base test ME track"<<std::endl;
      const xAOD::TrackParticle* trk = *Mu_metrack;
      fill(*trk);
    }
  }
}


void RecoMuonTrackPlots::fill(const xAOD::TrackParticle& muTP){
  m_oAllPlots.fill(muTP);
  m_oImpactPlots.fill(muTP);
  m_oTrkRecoInfoPlots.fill(muTP);
  //m_oMSHitPlots.fill(muTP);
  
  if (muTP.pt()/1000.0 > 100) {//ony for high pt muons
    m_pt_broad->Fill(muTP.pt()/1000.0);
    m_eta_phi_broad->Fill(muTP.eta(), muTP.phi());
  }
  
  // const Trk::Track* track = muTP.track(); //@@@ NUMEROUS FPE WARNINGS!!!
  // if (!track) return;

  // const DataVector< const Trk::TrackStateOnSurface>* trackSoS=track->trackStateOnSurfaces();
  // if (!trackSoS) return;
 
  // for (const auto stateOnSurface: *trackSoS) {
     
  //   if (!stateOnSurface) continue;

  //   if (stateOnSurface->type(Trk::TrackStateOnSurface::Outlier)) continue;

  //   const Trk::MeasurementBase* meas = stateOnSurface->measurementOnTrack();
  //   if (!meas) continue;

  //   Identifier id = m_helperTool->getIdentifier(*meas);
  //   if (!id.is_valid()) continue;

  //   const Trk::TrackParameters* trackParameters = stateOnSurface->trackParameters();
  //   if (!trackParameters) continue;

  //   const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(meas, trackParameters, Trk::ResidualPull::Unbiased);
  //   if (!resPull) continue;

  //   const Muon::MuonStationIndex::TechnologyIndex techid = m_idHelperTool->technologyIndex(id);
  //   const int stationPhi = m_idHelperTool->sector(id); //1-16, odd=large, even=small
  //   const bool measuresPhi = m_idHelperTool->measuresPhi(id);
    
  //   m_oResidualPlots_Global.fill(*resPull,stationPhi);
    
  //   if (techid==Muon::MuonStationIndex::MDT) {
  //     m_oResidualPlots_MDT.fill(*resPull,stationPhi);
  //   }
  //   else if (techid==Muon::MuonStationIndex::RPC){
  //     if (measuresPhi)
  //       m_oResidualPlots_RPC_phi.fill(*resPull,stationPhi);
  //     else
  //       m_oResidualPlots_RPC_eta.fill(*resPull,stationPhi);
  //   }
  //   else if (techid==Muon::MuonStationIndex::TGC){
  //     if (measuresPhi)
  //       m_oResidualPlots_TGC_strip.fill(*resPull,stationPhi);
  //     else
  //       m_oResidualPlots_TGC_wire.fill(*resPull,stationPhi);
  //   }
  //   else if (techid==Muon::MuonStationIndex::CSCI){ //it's CSCI, not CSC!!!
  //     if (measuresPhi)
  //       m_oResidualPlots_CSC_phi.fill(*resPull,stationPhi);
  //     else
  //       m_oResidualPlots_CSC_eta.fill(*resPull,stationPhi);
  //   }
  //   delete resPull;
  // }
}


