/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonResolutionPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "FourMomUtils/xAODP4Helpers.h"
using namespace xAOD::P4Helpers;
namespace Muon{

MuonResolutionPlots::MuonResolutionPlots(PlotBase* pParent, const std::string& sDir, const std::string& sType, bool doBinnedResolutionPlots):
  PlotBase(pParent, sDir),
  m_oResolutionPlots(this, "", sType), 
  m_sType(sType),
  m_doBinnedResolutionPlots(doBinnedResolutionPlots)
{
  const int sizeOf_ptBins=7;
  Double_t ptBins[sizeOf_ptBins] = {5, 15, 30, 45, 60, 100, 200};
  const int sizeOf_etaBins=10;
  Double_t etaBins[sizeOf_etaBins] = {-2.7, -2.5, -1.7, -1.05, -0.1, 0.1, 1.05, 1.7, 2.5, 2.7};
  const int sizeOf_phiBins=9;
  Double_t phiBins[sizeOf_phiBins] = {-3.15, -2.4, -1.6, -0.8, 0, 0.8, 1.6, 2.4, 3.15};

  //book 2D hists
  if (m_doBinnedResolutionPlots) {
    Res_pT_vs_lowpT  = Book2D("Res"+m_sType+"_pT_vs_lowpT","Res"+m_sType+"_pT vs low pT;pT [GeV];(pT-pTtruth)/pTtruth", 16,2.,10.,  100,-0.5,0.5);
    Res_pT_vs_highpT  = Book2D("Res"+m_sType+"_pT_vs_highpT","Res"+m_sType+"_pT vs high pT;pT [GeV];(pT-pTtruth)/pTtruth",10,100.,1000., 100,-0.5,0.5);

    Res_pT_vs_pT  = Book2D("Res"+m_sType+"_pT_vs_pT","Res"+m_sType+"_pT vs pT;pT [GeV];(pT-pTtruth)/pTtruth",
			   sizeOf_ptBins-1, &ptBins[0],  100,-0.5,0.5);
    Res_pT_vs_eta = Book2D("Res"+m_sType+"_pT_vs_eta","Res"+m_sType+"_pT vs eta;eta,eta-etatruth",
			   sizeOf_etaBins-1, &etaBins[0], 100,-0.5,0.5);
    Res_pT_vs_phi = Book2D("Res"+m_sType+"_pT_vs_phi","Res"+m_sType+"_pT vs phi;phi,phi-phitruth",
			   sizeOf_phiBins-1, &phiBins[0], 100,-0.5,0.5);

    Res_eta_vs_pT = Book2D("Res"+m_sType+"_eta_vs_pT","Res"+m_sType+"_eta vs pT;pT [GeV];eta-etatruth",
			   sizeOf_ptBins-1, &ptBins[0],  100,-0.5,0.5);
    Res_phi_vs_pT = Book2D("Res"+m_sType+"_phi_vs_pT","Res"+m_sType+"_phi vs pT;pT [GeV];phi-phitruth",
			   sizeOf_ptBins-1, &ptBins[0],  100,-0.5,0.5);
  }

}

  void MuonResolutionPlots::fill(const xAOD::TrackParticle& muPrimaryTrk, const xAOD::TruthParticle& truthMu, float weight) {
  //fill plots Res_pt,eta,phi
    m_oResolutionPlots.fill(muPrimaryTrk, truthMu, weight);

  //fill plots Res_pt_vs_pt,eta,phi
  float pt = 0.001*truthMu.pt();
  float eta = truthMu.eta();
  float phi = truthMu.phi();
  float respt = (muPrimaryTrk.pt() - truthMu.pt())/truthMu.pt();

  if (m_doBinnedResolutionPlots) {
    if ( pt < Res_pT_vs_lowpT->GetXaxis()->GetXmax() ) Res_pT_vs_lowpT->Fill(pt,respt,weight);
    if ( pt > Res_pT_vs_highpT->GetXaxis()->GetXmin() ) Res_pT_vs_highpT->Fill(pt,respt,weight);
      
    Res_pT_vs_pT->Fill(pt,respt,weight);
    Res_pT_vs_eta->Fill(eta,respt,weight);
    Res_pT_vs_phi->Fill(phi,respt,weight);

    Res_eta_vs_pT->Fill(pt, muPrimaryTrk.eta()-eta,weight);
    Res_phi_vs_pT->Fill(pt, deltaPhi(muPrimaryTrk.phi(),phi), weight);
  }
}

} // closing namespace Muon
