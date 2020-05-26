/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LeptonJetAnalysis_H
#define LeptonJetAnalysis_H

#include "baseAnalysis.h"

class TH1D;

class LeptonJetAnalysis: public baseAnalysis
{
 public:

  LeptonJetAnalysis();
  virtual ~LeptonJetAnalysis();

  virtual int Init(double tr_max_eta, double tr_min_pt);
  virtual int Process(HepMC::GenEvent* hepmcevt);

  static const int GeV=1000;

  static const UInt_t njets = 4;
  static const UInt_t nleptons = 4;
  
 private:

  void InitPlots();

  TString m_histName;

  TH1D *m_event_weight;
  TH1D *m_event_weight_zoom;

  //********** leptons ***********//

  TH1D *m_leptonInclPt;
  TH1D *m_leptonNum;

  //pT(lepton)>20
  
  TH1D *m_leptonPt[nleptons];
  TH1D *m_leptonEta[nleptons];
  TH1D *m_leptondR[nleptons][nleptons];
  TH1D *m_leptondPhi[nleptons][nleptons];
  TH1D *m_leptonMass[nleptons][nleptons];
  TH1D *m_leptonLowMass[nleptons][nleptons];


  TH1D *m_electronPt;
  TH1D *m_electronNum;
  TH1D *m_electronCharge;

  TH1D *m_muonPt;
  TH1D *m_muonNum;
  TH1D *m_muonCharge;

  TH1D *m_tauPt;
  TH1D *m_tauNum;
  TH1D *m_tauCharge;

  //********** jets with pT cut ***********//

  //pT(jet)>20, eta(jet)<2.8

  TH1D *m_jetNum;

  TH1D *m_jetPt[njets];
  TH1D *m_jetPtHighRange[njets];
  TH1D *m_jetEta[njets];
  TH1D *m_jetMassPt[njets];
  TH1D *m_jetdR[njets][njets];
  TH1D *m_jetdPhi[njets][njets];
  TH1D *m_jetMass[njets][njets];

  //********** jets w/o pT cut ***********//

  //eta(jet)<2.8

  TH1D *m_jetNum_nocuts;

  TH1D *m_jetPt_nocuts[njets];
  TH1D *m_jetPtHighRange_nocuts[njets];
  TH1D *m_jetEta_nocuts[njets];
  TH1D *m_jetMassPt_nocuts[njets];
  TH1D *m_jetdR_nocuts[njets][njets];
  TH1D *m_jetdPhi_nocuts[njets][njets];
  TH1D *m_jetMass_nocuts[njets][njets];

  //********** forward jets ***********//

  //pT(jet)>20,eta(jet)>2.8

  TH1D *m_jetNum_forward;

  TH1D *m_jetPt_forward[njets];
  TH1D *m_jetPtHighRange_forward[njets];
  TH1D *m_jetEta_forward[njets];
  TH1D *m_jetMassPt_forward[njets];
  TH1D *m_jetdR_forward[njets][njets];
  TH1D *m_jetdPhi_forward[njets][njets];
  TH1D *m_jetMass_forward[njets][njets];


  //********** set with "tight" cuts ***********//

  //pT(lepton)>20, eta(lepton)<2.5
  TH1D *m_leptonPt_tight;
  TH1D *m_leptonEta_tight;

  //pT(jet)>20, eta(jet)<2.8

  TH1D *m_jet1Pt_tight;
  TH1D *m_jet1PtHighRange_tight;
  TH1D *m_jet1Eta_tight;
  TH1D *m_jet2Pt_tight;
  TH1D *m_jet2PtHighRange_tight;
  TH1D *m_jet2Eta_tight;

  TH1D *m_dR_jet1_to_lepton_tight;
  TH1D *m_dR_jet2_to_lepton_tight;
  TH1D *m_dR_lepton_to_closest_jet_tight;

  TH1D *m_dPhi_jet1_to_lepton_tight;
  TH1D *m_dPhi_jet2_to_lepton_tight;
  TH1D *m_dPhi_lepton_to_closest_jet_tight;

  //*********** vector boson plots **********//
  TH1D *m_ZNum;
  TH1D *m_Z1pt;
  TH1D *m_Z1eta;
  TH1D *m_WNum;
  TH1D *m_W1pt;
  TH1D *m_W1eta;
  TH1D *m_gammaNum;
  TH1D *m_gamma1pt;
  TH1D *m_gamma1eta;
 
};

#endif
