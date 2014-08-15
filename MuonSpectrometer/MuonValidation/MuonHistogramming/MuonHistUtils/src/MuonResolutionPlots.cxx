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
  float ptBins[]={5,10,20,30,40,50,75,100,150,200};
  float etaBins[]={-2.7,-2.5,-2.25,-2.0,-1.7,-1.5,-1.15,-0.8,-0.4,0,0.4,0.8,1.15,1.5,1.7,2.0,2.25,2.5,2.7};
  float phiBins[] = {-3.2,-2.8,-2.4,-2.0,-1.6,-1.2,-0.8,-0.4,0,0.4,0.8,1.2,1.6,2.0,2.4,2.8,3.2};

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
  Res_pT_vs_pT = BookTProfile("Res_pT_vs_pT", "Res_pT_vs_pT;pT;(pT-pTtruth)/pTtruth", nptBins,ptBins);
  Res_pT_vs_eta = BookTProfile("Res_pT_vs_eta", "Res_pT_vs_eta;eta;(pT-pTtruth)/pTtruth", netaBins,etaBins);
  Res_pT_vs_phi = BookTProfile("Res_pT_vs_phi", "Res_pT_vs_phi;phi;(pT-pTtruth)/pTtruth", nphiBins,phiBins);
  
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


} // closing namespace Muon
