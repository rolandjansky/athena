/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONRESPLOTS_H
#define MUONHISTUTILS_MUONRESPLOTS_H

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkValHistUtils/PlotBase.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

class MuonResonancePlots: public PlotBase {
 public:
 MuonResonancePlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){;}

  TString selitool;
  const double M_pdg[3] = {91187.6, 3096.6, 9460.3};
  const double Pi = TMath::Pi();

  void setSuffix(std::string suff){suffix=suff;};
  void setPrefix(std::string pref){prefix=pref;};
  void setBinning(std::map< std::string,std::vector<double> >);
  void BookPlots();
  void fill(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd, int trk);
  const float M0(); 
  float p_star(TLorentzVector v1, TLorentzVector v2);
  float deltaPt(TLorentzVector v1, TLorentzVector v2);
  float effWeight(const xAOD::Muon& mu);
  const xAOD::TruthParticle* findTruthMuon(const xAOD::Muon& mu);
  float getChiSquared(const xAOD::Muon& mu, int type);
  TLorentzVector getTrackTLV(const xAOD::Muon& mu, int type);
  std::pair<TLorentzVector, TLorentzVector> sortedPair(TLorentzVector v1, TLorentzVector v2);

  std::map<std::string,std::vector<double> > m_binning;
  void PrintVectors(TLorentzVector l1, TLorentzVector l2, TLorentzVector V,
		    TLorentzVector l1_truth, TLorentzVector l2_truth, TLorentzVector V_truth, int type, float weight);
  

  //Histograms
  TH1* mu_1stAuthor;
  TH1* mu_2ndAuthor;

  TH1* mu_1stPt;
  TH1* mu_2ndPt;
  TH1* mu_avPt;
  TH1* mu_1stEta;
  TH1* mu_2ndEta;
  TH1* mu_1stPhi;
  TH1* mu_2ndPhi;

  TH1* Mmumu;
  TH1* Z_pt;
  TH1* Z_phi;

  //correlation plots
  TH2* h_Zpt_mu_1stPt;
  TH2* h_mu_1stPt_1stPhi;
  TH2* h_mu_2ndPt_2ndPhi;  
  TH2* h_Zpt_mu_avPt;

  TH2* h_Zm_1stPhi;
  TH2* h_Zm_2ndPhi;
  TH2* h_Zm_1stEta;
  TH2* h_Zm_2ndEta;
  TH2* h_Zm_1stPt;
  TH2* h_Zm_2ndPt;
  TH2* h_Zm_mu_avPt;  
  TH2* h_Zm_Pexp;

  TH2* h_Zm_1stPhi_truth;
  TH2* h_Zm_2ndPhi_truth;
  TH2* h_Zm_1stEta_truth;
  TH2* h_Zm_2ndEta_truth;
  TH2* h_Zm_1stPt_truth;
  TH2* h_Zm_2ndPt_truth;
  TH2* h_Zm_mu_avPt_truth;
  TH2* h_Zm_Pexp_truth;

  // resolution                                                    
  TH2* Res_mu_1stPt_2D;
  TH2* Res_mu_2ndPt_2D;
  TH2* Res_mu_1stPhi_2D;
  TH2* Res_mu_2ndPhi_2D;
  TH2* Res_mu_1stEta_2D;
  TH2* Res_mu_2ndEta_2D;

  TH2* Res_Zm_2D;
  TH2* Res_Zm_Eta_2D;
  TH2* Res_Zm_Phi_2D;
  TH2* Res_Zm_Pt_2D;
  TH2* Res_Zm_Pexp_2D;
  TH2 *Res_Zpt_2D;
  TH2 *Res_Zphi_2D;

  //chi2overDoF      
  TH1* ChiSquared;
 
  //momentum imbalance
  TH1* p_pTRUE;
  TH1* pID_pME;
  
  //eta/phi maps 
  TH2* Z_m_etaphi;
  TH2* DeltaZ_m_etaphi;
  TH2* DeltaZ_m_q_etaphi;
  TH2* p_pTRUE_etapt;
  TH2* p_pTRUE_etaphi;


 private:

  void initializePlots();
  std::string suffix;
  std::string prefix;
  void Binning1D(TH1*& histo,std::string hname);
  void Binning2D(TH2*& histo,std::string hname);
  void Fill1D(TH1*& histo, float x, float w);
  void Fill2D(TH2*& histo, float x, float y, float w);
  void FillTwice(TH1*& histo, float x1, float x2, float w);

};

#endif
