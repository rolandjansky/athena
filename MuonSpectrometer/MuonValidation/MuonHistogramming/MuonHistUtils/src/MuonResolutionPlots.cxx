/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonResolutionPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonResolutionPlots::MuonResolutionPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots):
  PlotBase(pParent, sDir), 

  m_oResolutionPlots(this, ""),
  m_oResolutionPlots_ptBins(0),
  m_oResolutionPlots_etaBins(0),
  m_oResolutionPlots_phiBins(0),
  resolution_ptBins(0),
  resolution_etaBins(0),
  resolution_phiBins(0)
{  
  std::string dirName("");

  //inline bin definitions
  Double_t ptBins[] = {5, 10, 25, 60, 100, 200, 1000};
  Double_t etaBins[] = {-2.7, -2.5, -1.7, -1.05, -0.1, 0.1, 1.05, 1.7, 2.5, 2.7};
  Double_t phiBins[] = {-3.15, -2.4, -1.6, -0.8, 0, 0.8, 1.6, 2.4, 3.15};

  const unsigned int nptBins = sizeof(ptBins)/sizeof(ptBins[0])-1;
  const unsigned int netaBins = sizeof(etaBins)/sizeof(etaBins[0])-1;
  const unsigned int nphiBins = sizeof(phiBins)/sizeof(phiBins[0])-1;
  
  //passing binning to class
  for (unsigned int i=0; i<=nptBins; i++) resolution_ptBins.push_back(ptBins[i]);
  for (unsigned int i=0; i<=netaBins; i++) resolution_etaBins.push_back(etaBins[i]);
  for (unsigned int i=0; i<=nphiBins; i++) resolution_phiBins.push_back(phiBins[i]);

  if (doBinnedResolutionPlots) {
    //creating resolution plots for each bin
    for (unsigned int i=0; i<nptBins; i++) {
      dirName = Form("/Resolution_vs_pt/bin_%.2f_%.2f/",ptBins[i],ptBins[i+1]);
      m_oResolutionPlots_ptBins.push_back(new Trk::ResolutionPlots(this,dirName));
    }

    for (unsigned int i=0; i<netaBins; i++) {
      dirName = Form("/Resolution_vs_eta/bin_%.2f_%.2f/",etaBins[i],etaBins[i+1]);
      std::replace(dirName.begin(),dirName.end(),'-','m');
      m_oResolutionPlots_etaBins.push_back(new Trk::ResolutionPlots(this,dirName));
    }

    for (unsigned int i=0; i<nphiBins; i++) {
      dirName = Form("/Resolution_vs_phi/bin_%.2f_%.2f/",phiBins[i],phiBins[i+1]);
      std::replace(dirName.begin(),dirName.end(),'-','m');
      m_oResolutionPlots_phiBins.push_back(new Trk::ResolutionPlots(this,dirName));
    }
  }

  //booking profile histograms
  Res_pT_vs_pT = BookTProfileRangeY("Res_pT_vs_pT", "Res_pT_vs_pT;pT;(pT-pTtruth)/pTtruth", nptBins,ptBins,-0.2,0.2,true);
  Res_pT_vs_eta = BookTProfileRangeY("Res_pT_vs_eta", "Res_pT_vs_eta;eta;(pT-pTtruth)/pTtruth", netaBins,etaBins,-0.2,0.2,true);
  Res_pT_vs_phi = BookTProfileRangeY("Res_pT_vs_phi", "Res_pT_vs_phi;phi;(pT-pTtruth)/pTtruth", nphiBins,phiBins,-0.2,0.2,true);

}	

MuonResolutionPlots::~MuonResolutionPlots() {
  for (unsigned int i=0; i<m_oResolutionPlots_ptBins.size(); i++) {
    Trk::ResolutionPlots *resPlots = m_oResolutionPlots_ptBins[i];
    delete resPlots;
    resPlots=0;
  }
  for (unsigned int i=0; i<m_oResolutionPlots_etaBins.size(); i++) {
    Trk::ResolutionPlots *resPlots = m_oResolutionPlots_etaBins[i];
    delete resPlots;
    resPlots=0;
  }
  for (unsigned int i=0; i<m_oResolutionPlots_phiBins.size(); i++) {
    Trk::ResolutionPlots *resPlots = m_oResolutionPlots_phiBins[i];
    delete resPlots;
    resPlots=0;
  }
}

void MuonResolutionPlots::fill(const xAOD::TrackParticle& muPrimaryTrk, const xAOD::TruthParticle& truthMu) {
  //fill plots Res_pt,eta,phi
  m_oResolutionPlots.fill(muPrimaryTrk, truthMu);

  if (m_oResolutionPlots_ptBins.size()) {
    //fill plots Res_pt,eta,phi for each pt/eta/phi bin
    int iPtBin = findBin(truthMu.pt()/1000.,resolution_ptBins);
    if (iPtBin>=0) m_oResolutionPlots_ptBins[iPtBin]->fill(muPrimaryTrk, truthMu);

    int iEtaBin = findBin(truthMu.eta(),resolution_etaBins);
    if (iEtaBin>=0) m_oResolutionPlots_etaBins[iEtaBin]->fill(muPrimaryTrk, truthMu);

    int iPhiBin = findBin(truthMu.phi(),resolution_phiBins);
    if (iPhiBin>=0) m_oResolutionPlots_phiBins[iPhiBin]->fill(muPrimaryTrk, truthMu);
  }

  //fill profile plots Res_pt_vs_pt,eta,phi
  double respt = (muPrimaryTrk.pt() - truthMu.pt())/truthMu.pt();
  Res_pT_vs_pT->Fill(truthMu.pt()/1000,respt);
  Res_pT_vs_eta->Fill(truthMu.eta(),respt);
  Res_pT_vs_phi->Fill(truthMu.phi(),respt);
}


int MuonResolutionPlots::findBin(float x,std::vector<float> bins) {
  for (unsigned int i=0; i<bins.size()-1; i++) {
    float low = bins[i];
    float upp = bins[i+1];
    if (x>low && x<upp) {
      return i;
    }
  }
  return -1; //value out of range
}

MomentumDiffPlots::MomentumDiffPlots(PlotBase* pParent, std::string sDir):
  PlotBase(pParent, sDir), dp_CB_truthIP(NULL), dp_ME_truthIP(NULL), dp_ME_truthIP_MS_truthMS(NULL), dp_eloss(NULL) {}
MomentumDiffPlots::~MomentumDiffPlots(){}

void MomentumDiffPlots::initializePlots()
{
  dp_CB_truthIP = Book1D("dp_CB_truthIP","pCB-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP = Book1D("dp_ME_truthIP","pME-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP_MS_truthMS = Book1D("dp_ME_truthIP_MS_truthMS","(pME-ptruthIP) - (pMS-ptruthMS);#Delta p;Entries",200,-10,10);

  dp_CB_truthIP_Tail = Book1D("dp_CB_truthIP_Tail","TAIL pCB-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP_Tail = Book1D("dp_ME_truthIP_Tail","TAIL pME-ptruthIP;#Delta p;Entries",200,-10,10);
  dp_ME_truthIP_MS_truthMS_Tail = Book1D("dp_ME_truthIP_MS_truthMS_Tail","TAIL (pME-ptruthIP) - (pMS-ptruthMS);#Delta p;Entries",200,-10,10);

  dp_eloss = Book1D("dp_eloss","dp_eloss;#Delta p;Entries",200,-10,10);
}
  
void MomentumDiffPlots::fill(const xAOD::Muon& muon, const xAOD::TrackParticle* muonTrackIP, const xAOD::TruthParticle& truthMu)
{
  const xAOD::TrackParticle* muPrimaryTrk = muon.trackParticle(xAOD::Muon::Primary);
  if (!muPrimaryTrk) return;
  const xAOD::TrackParticle* msTrk = muon.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);//muon extrapolated to IP

  float pTruth = truthMu.p4().P();
  float pCB = muPrimaryTrk->p4().P();
  float pME = 0;
  if (msTrk) pME = msTrk->p4().P(); //muon extrapolated
  float pMS = 0;
  if (muonTrackIP) pMS = muonTrackIP->p4().P(); //at muon spectrometer entry//@@@
  
  float eloss = 0;
  muon.parameter(eloss,xAOD::Muon::MeasEnergyLoss);

  float pTruthMS = 0; //p truth at MS entry
  if (truthMu.isAvailable<float>("MuonEntryLayer_px") &&
      truthMu.isAvailable<float>("MuonEntryLayer_py") &&
      truthMu.isAvailable<float>("MuonEntryLayer_pz") ) {
    TVector3 pvecTruthMS(truthMu.auxdata<float>("MuonEntryLayer_px"),
			 truthMu.auxdata<float>("MuonEntryLayer_py"),
			 truthMu.auxdata<float>("MuonEntryLayer_pz"));
    pTruthMS = pvecTruthMS.Mag();
  }

  if (muon.muonType()==xAOD::Muon::Combined || muon.muonType()==xAOD::Muon::MuonStandAlone) {
    if ( muon.energyLossType()!=xAOD::Muon::Tail ) { //to test PARAMETRIZED energy loss
      if (pTruth) {
	dp_CB_truthIP->Fill(0.001*(pCB-pTruth));
	if (pME) {
	  dp_ME_truthIP->Fill(0.001*(pME-pTruth));
	  if (pMS && pTruthMS) {
	    dp_ME_truthIP_MS_truthMS->Fill(0.001*(pME - pMS - pTruth + pTruthMS));
	  }
	}
      }
    }
    else {//muons with eloss=TAIL to test MEASURED energy loss
      if (pTruth) {
	dp_CB_truthIP_Tail->Fill(0.001*(pCB-pTruth));
	if (pME) {
	  dp_ME_truthIP_Tail->Fill(0.001*(pME-pTruth));
	  if (pMS && pTruthMS) {
	    dp_ME_truthIP_MS_truthMS_Tail->Fill(0.001*(pME - pMS - pTruth + pTruthMS));
	  }
	}
      }
    }
  }

  if (eloss>2000 && pTruthMS && pTruth) {
    if (muon.muonType()==xAOD::Muon::Combined || muon.muonType()==xAOD::Muon::SegmentTagged || muon.muonType()==xAOD::Muon::MuonStandAlone) {
      dp_eloss->Fill(0.001*(pTruth - pTruthMS - eloss));
    }
  }
  
}

  

} // closing namespace Muon
