/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/BFMonPlots.h"
#include "MuonTrkPhysMonitoring/MuonTrkPhysMonUtils.h"

using namespace MuonTrkPhysMonUtils;

void BFMonPlots::initializePlots(){	
  Book1D(m_LSag, "m_LSag",m_sTrackCollectionName+": p_{T}/R ;[GeV]/m", 100,0,0.75);
  Book1D(m_LSag_weights, "m_LSag_weights",m_sTrackCollectionName+": p_{T}/R weights ;;[GeV]", 8,0.,8.);
  Book1D(m_LSag_entries, "m_LSag_entries",m_sTrackCollectionName+": p_{T}/R entries", 8,0.,8.);
  Book1D(m_LSag_mean, "m_LSag_mean",m_sTrackCollectionName+": <p_{T}/R> ;;[GeV]/m", 8,0.,8.);
  Book2D(m_LSag_eta_phi_entries, "m_LSag_eta_phi_entries",m_sTrackCollectionName+": N_{#mu} Entries  : #eta v #phi;#eta (per 0.1)",55,-2.75,2.75,32,1.,17.); //Going to have to find the best binning for this
  Book2D(m_LSag_eta_phi_weights, "m_LSag_eta_phi_weights",m_sTrackCollectionName+": p_{T}/R Weights  : #eta v #phi;#eta (per 0.1)", 55,-2.75,2.75,32,1.,17.); //Going to have to find the best binning for this
  Book2D(m_LSag_eta_phi_mean, "m_LSag_eta_phi_mean",m_sTrackCollectionName+":  <p_{T}/R>  : #eta v #phi;#eta (per 0.1)", 55,-2.75,2.75,32,1.,17.); //Going to have to find the best binning for this
}

void BFMonPlots::fill(const xAOD::Muon *Muon, float fSagitta ) {
  m_LSag->Fill(fSagitta);
  m_LSag_entries->Fill(EtaRegion(Muon->eta()));
  m_LSag_weights->Fill(EtaRegion(Muon->eta()),fSagitta);
    
  m_LSag_eta_phi_weights->Fill(Muon->eta(),PhiSector(Muon->phi()),fSagitta);
  m_LSag_eta_phi_entries->Fill(Muon->eta(),PhiSector(Muon->phi()));

}

void BFMonPlots::finalizePlots(){
  
  LabelLargeEtaRegions(m_LSag_weights->GetXaxis());
  LabelLargeEtaRegions(m_LSag_entries->GetXaxis());
  LabelLargeEtaRegions(m_LSag_mean->GetXaxis());
      
  m_LSag_mean->Divide(m_LSag_weights,m_LSag_entries);
  m_LSag_eta_phi_mean->Divide(m_LSag_eta_phi_weights,m_LSag_eta_phi_entries);
      
  LabelSectorAxis(m_LSag_eta_phi_mean->GetYaxis());
  LabelSectorAxis(m_LSag_eta_phi_weights->GetYaxis());
  LabelSectorAxis(m_LSag_eta_phi_entries->GetYaxis());
      
  for (int j = 1; j < m_LSag_mean->GetNbinsX()+1; j++) {
    m_LSag_mean->SetBinError(j,m_LSag_entries->GetBinContent(j) ? m_LSag->GetRMS(1)/sqrt(m_LSag_entries->GetBinContent(j)) : 0.0);
  }
  
  
}