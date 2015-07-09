/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonResolutionPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonResolutionPlots::MuonResolutionPlots(PlotBase* pParent, std::string sDir, std::string sType, bool doBinnedResolutionPlots):
  PlotBase(pParent, sDir),
  m_oResolutionPlots(this, "", sType),
  Res_pT_vs_lowpT(NULL),
  Res_pT_vs_highpT(NULL),
  Res_pT_vs_pT(NULL),
  Res_pT_vs_eta(NULL),
  Res_pT_vs_phi(NULL),
  m_sType(sType),
  m_doBinnedResolutionPlots(doBinnedResolutionPlots)
{
  //inline bin definitions
  Double_t resolution_ptBins[9] = {5, 10, 20, 30, 40, 50, 60, 100, 200};
  Double_t resolution_etaBins[10] = {-2.7, -2.5, -1.7, -1.05, -0.1, 0.1, 1.05, 1.7, 2.5, 2.7};
  Double_t resolution_phiBins[9] = {-3.15, -2.4, -1.6, -0.8, 0, 0.8, 1.6, 2.4, 3.15};
    
  std::pair<Int_t,Double_t*> ptBins(6,resolution_ptBins);
  std::pair<Int_t,Double_t*> etaBins(9,resolution_etaBins);
  std::pair<Int_t,Double_t*> phiBins(8,resolution_phiBins);

  //book 2D hists
  if (m_doBinnedResolutionPlots) {
    Res_pT_vs_lowpT  = Book2D("Res"+m_sType+"_pT_vs_lowpT","Res"+m_sType+"_pT vs low pT;pT [GeV];(pT-pTtruth)/pTtruth", 16,2.,10.,  100,-0.5,0.5);
    Res_pT_vs_highpT  = Book2D("Res"+m_sType+"_pT_vs_highpT","Res"+m_sType+"_pT vs high pT;pT [GeV];(pT-pTtruth)/pTtruth",10,100.,1000., 100,-0.5,0.5);

    Res_pT_vs_pT  = Book2D("Res"+m_sType+"_pT_vs_pT","Res"+m_sType+"_pT vs pT;pT [GeV];(pT-pTtruth)/pTtruth",
			   ptBins.first, ptBins.second,  100,-0.5,0.5);
    Res_pT_vs_eta = Book2D("Res"+m_sType+"_pT_vs_eta","Res"+m_sType+"_pT vs eta;eta,eta-etatruth",
			   etaBins.first, etaBins.second, 100,-0.5,0.5);
    Res_pT_vs_phi = Book2D("Res"+m_sType+"_pT_vs_phi","Res"+m_sType+"_pT vs phi;phi,phi-phitruth",
			   phiBins.first, phiBins.second, 100,-0.5,0.5);					
  }

}

void MuonResolutionPlots::fill(const xAOD::TrackParticle& muPrimaryTrk, const xAOD::TruthParticle& truthMu) {
  //fill plots Res_pt,eta,phi
  m_oResolutionPlots.fill(muPrimaryTrk, truthMu);

  //fill plots Res_pt_vs_pt,eta,phi
  float pt = 0.001*truthMu.pt();
  float eta = truthMu.eta();
  float phi = truthMu.phi();
  float respt = (muPrimaryTrk.pt() - truthMu.pt())/truthMu.pt();

  if (m_doBinnedResolutionPlots) {
    if ( pt < Res_pT_vs_lowpT->GetXaxis()->GetXmax() ) Res_pT_vs_lowpT->Fill(pt,respt);
    if ( pt > Res_pT_vs_highpT->GetXaxis()->GetXmin() ) Res_pT_vs_highpT->Fill(pt,respt);
      
    Res_pT_vs_pT->Fill(pt,respt);
    Res_pT_vs_eta->Fill(eta,respt);
    Res_pT_vs_phi->Fill(phi,respt);
  }
}

} // closing namespace Muon
