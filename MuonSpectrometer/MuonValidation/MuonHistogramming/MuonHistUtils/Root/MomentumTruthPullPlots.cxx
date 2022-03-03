/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonHistUtils/MomentumTruthPullPlots.h"
#ifndef XAOD_ANALYSIS
#include "xAODTracking/TrackingPrimitives.h"
#endif // not XAOD_ANALYSIS

namespace Muon{


MomentumTruthPullPlots::MomentumTruthPullPlots(PlotBase* pParent, const std::string& sDir, std::string sType):
    PlotBase(pParent, sDir),
    m_sType(std::move(sType))
{}
  
MomentumTruthPullPlots::~MomentumTruthPullPlots(){}

void MomentumTruthPullPlots::initializePlots()
{
  dp_CB_truthIP = Book1D("dp_CB_truthIP_"+m_sType,m_sType+" pCB-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP = Book1D("dp_ME_truthIP_"+m_sType,m_sType+" pME-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP_MS_truthMS = Book1D("dp_ME_truthIP_MS_truthMS_"+m_sType,m_sType+" (pME-ptruthIP) - (pMS-ptruthMS);#Delta p;Entries",200,-10,10);
  dp_eloss = Book1D("dp_eloss_"+m_sType,m_sType+" dp_eloss;#Delta p;Entries",200,-10,10);

  Double_t ptBins[] = {5, 10, 25, 60, 100, 200, 1000};
  Double_t etaBins[] = {-2.7, -2.5, -1.7, -1.05, -0.1, 0.1, 1.05, 1.7, 2.5, 2.7};
  //Double_t phiBins[] = {-3.15, -2.4, -1.6, -0.8, 0, 0.8, 1.6, 2.4, 3.15};

  const unsigned int nptBins = sizeof(ptBins)/sizeof(ptBins[0])-1;
  const unsigned int netaBins = sizeof(etaBins)/sizeof(etaBins[0])-1;
  
  dp_CB_truthIP_vs_pt = Book2D("dp_CB_truthIP_vs_pt_"+m_sType,m_sType+" pCB-ptruthIP vs pt;pt;#Delta p",nptBins,ptBins,100,-10.,10.);
  dp_ME_truthIP_vs_pt = Book2D("dp_ME_truthIP_vs_pt_"+m_sType,m_sType+" pME-ptruthIP vs pt;pt;#Delta p",nptBins,ptBins,100,-10.,10.);
  dp_ME_truthIP_MS_truthMS_vs_pt = Book2D("dp_ME_truthIP_MS_truthMS_vs_pt_"+m_sType,m_sType+" (pME-ptruthIP) - (pMS-ptruthMS) vs pt;pt;#Delta p",nptBins,ptBins,200,-10.,10.);
  dp_eloss_vs_pt = Book2D("dp_eloss_vs_pt_"+m_sType,m_sType+" dp_eloss vs pt;pt [GeV]; #Delta p",nptBins,ptBins,100,-10.,10.);

  dp_CB_truthIP_vs_eta = Book2D("dp_CB_truthIP_vs_eta_"+m_sType,m_sType+" pCB-ptruthIP vs eta;eta;#Delta p",netaBins,etaBins,100,-10.,10.);
  dp_ME_truthIP_vs_eta = Book2D("dp_ME_truthIP_vs_eta_"+m_sType,m_sType+" pME-ptruthIP vs eta;eta;#Delta p",netaBins,etaBins,100,-10.,10.);
  dp_ME_truthIP_MS_truthMS_vs_eta = Book2D("dp_ME_truthIP_MS_truthMS_vs_eta_"+m_sType,m_sType+" (pME-ptruthIP) - (pMS-ptruthMS) vs eta;eta;#Delta p",netaBins,etaBins,200,-10.,10.);
  dp_eloss_vs_eta = Book2D("dp_eloss_vs_eta_"+m_sType,m_sType+" dp_eloss vs eta;eta; #Delta p",netaBins,etaBins,100,-10.,10.);

}
  
  void MomentumTruthPullPlots::fill(const xAOD::Muon& muon, const xAOD::TrackParticle* msTrk, const xAOD::TruthParticle& truthMu, float weight)
{
  if (!(muon.muonType()==xAOD::Muon::Combined || muon.muonType()==xAOD::Muon::SegmentTagged || muon.muonType()==xAOD::Muon::MuonStandAlone)) return;

  const xAOD::TrackParticle* muPrimaryTrk = muon.trackParticle(xAOD::Muon::Primary);
  if (!muPrimaryTrk) return;

  const xAOD::TrackParticle* msExtrapTrk = muon.trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
  if (!msExtrapTrk) msExtrapTrk = muon.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);

  float eta = truthMu.eta();
  float pt = 0.001*truthMu.pt();
  
  float pTruth = truthMu.p4().P();
  float pCB = muPrimaryTrk->p4().P();
  float pME = 0;
  if (msExtrapTrk) pME = msExtrapTrk->p4().P(); //muon extrapolated
  float pMS = 0;
  if (msTrk) pMS = msTrk->p4().P(); //at muon spectrometer entry//@@@
  
  float eloss = 0;
  if (muon.parameter(eloss,xAOD::Muon::MeasEnergyLoss)) {;}
  float pTruthMS = 0; //p truth at MS entry
  if (truthMu.isAvailable<float>("MuonEntryLayer_px") &&
      truthMu.isAvailable<float>("MuonEntryLayer_py") &&
      truthMu.isAvailable<float>("MuonEntryLayer_pz") ) {
    Amg::Vector3D pvecTruthMS{truthMu.auxdata<float>("MuonEntryLayer_px"),
			 truthMu.auxdata<float>("MuonEntryLayer_py"),
			 truthMu.auxdata<float>("MuonEntryLayer_pz")};
    pTruthMS = pvecTruthMS.mag();
  }

  if (muon.muonType()!=xAOD::Muon::SegmentTagged) {
    if (pTruth) {
      float dpCB = 0.001*(pCB-pTruth);
      dp_CB_truthIP->Fill(dpCB,weight);
      dp_CB_truthIP_vs_pt->Fill(pt,dpCB,weight);
      dp_CB_truthIP_vs_eta->Fill(eta,dpCB,weight);
      if (pME) {
	float dpME = 0.001*(pME-pTruth);
	dp_ME_truthIP->Fill(dpME,weight);
	dp_ME_truthIP_vs_pt->Fill(pt,dpME,weight);
	dp_ME_truthIP_vs_eta->Fill(eta,dpME,weight);
	if (pMS && pTruthMS) {
	  float dpMS = 0.001*(pME - pMS - pTruth + pTruthMS);
	  dp_ME_truthIP_MS_truthMS->Fill(dpMS,weight);
	  dp_ME_truthIP_MS_truthMS_vs_pt->Fill(pt,dpMS,weight);
	  dp_ME_truthIP_MS_truthMS_vs_eta->Fill(eta,dpMS,weight);
	}
      }
    }
  }

  //if (muon.muonType()==xAOD::Muon::Combined || muon.muonType()==xAOD::Muon::SegmentTagged || muon.muonType()==xAOD::Muon::MuonStandAlone) {
  if (eloss>2000 && pTruthMS && pTruth) {
    float dpEloss = 0.001*(pTruth - pTruthMS - eloss);
    dp_eloss->Fill(dpEloss,weight);
    dp_eloss_vs_pt->Fill(pt,dpEloss,weight);
    dp_eloss_vs_eta->Fill(eta,dpEloss,weight);
  }
  
}

  

} // closing namespace Muon
