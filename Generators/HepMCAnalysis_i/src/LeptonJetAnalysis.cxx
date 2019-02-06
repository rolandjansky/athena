/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------------

#include <iostream>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/IO_AsciiParticles.h"
#include "HepMC/SimpleVector.h"
#include "HepMC/WeightContainer.h"
#include "CLHEP/Vector/LorentzVector.h"


#include "TH1.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/SISConePlugin.hh"

#include "TruthUtils/HepMCHelpers.h" //MCUtils.h
#include "HEPUtils/FastJet.h"

#include "../HepMCAnalysis_i/LeptonJetAnalysis.h"

using namespace std;

// ----------------------------------------------------------------------
LeptonJetAnalysis::LeptonJetAnalysis()
{
  InitPlots();
}

// ----------------------------------------------------------------------
LeptonJetAnalysis::~LeptonJetAnalysis()
{}

// ----------------------------------------------------------------------
void LeptonJetAnalysis::InitPlots()
{

  m_event_weight = new TH1D("EventWeight",  "event weight",  10000, -5000000.0, 5000000.0);
  m_event_weight -> Sumw2();
  m_event_weight_zoom = new TH1D("EventWeight_Zoom",  "event weight",  41, -20.5, 20.5);
  m_event_weight_zoom -> Sumw2();
  //********** leptons ***********//
  m_leptonInclPt = new TH1D("PtIncllepton",  "lepton p_{T}",  50, 0.0, 200.0);
  m_leptonInclPt -> Sumw2();
  m_leptonNum = new TH1D("Numlepton",  "number of leptons",  10, 0.0, 10.0);
  m_leptonNum -> Sumw2();

  for(UInt_t i = 0; i < nleptons; ++i){
    m_histName.Form("lepton%d_pT", i+1);
    m_leptonPt[i] = new TH1D(m_histName, m_histName, 100, 0., 300);
    m_leptonPt[i] -> Sumw2();
    m_histName.Form("lepton%d_eta", i+1);
    m_leptonEta[i] =   new TH1D(m_histName, m_histName, 50, -4, 4);
    m_leptonEta[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < nleptons-1; ++i){
    for (UInt_t j=i+1;  j < nleptons;++j){
      m_histName.Form("lepton%d%d_deltaR", i+1,j+1);
      m_leptondR[i][j] =   new TH1D(m_histName, m_histName,  64, 0, 3.2);
      m_leptondR[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_deltaPhi", i+1,j+1);
      m_leptondPhi[i][j] =   new TH1D(m_histName, m_histName, 40, -3.14, 3.14);
      m_leptondPhi[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_Mass", i+1,j+1);
      m_leptonMass[i][j] =   new TH1D(m_histName, m_histName, 40, 40.0, 120.0);
      m_leptonMass[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_lowMass", i+1,j+1);
      m_leptonLowMass[i][j] =   new TH1D(m_histName, m_histName, 20, 0.0, 40.0);
      m_leptonLowMass[i][j] -> Sumw2();
    }
  }

  m_electronPt = new TH1D("Ptelectron",  "electron p_{T}",  50, 0.0, 200.0);
  m_electronPt -> Sumw2();
  m_electronNum = new TH1D("Numelectron",  "number of electrons",  10, 0.0, 10.0);
  m_electronNum -> Sumw2();
  m_electronCharge = new TH1D("Chargeelectron",  "electrons charge",  10, -5, 5);
  m_electronCharge -> Sumw2();

  m_muonPt = new TH1D("Ptmuon",  "lepton p_{T}",  50, 0.0, 200.0);
  m_muonPt -> Sumw2();
  m_muonNum = new TH1D("Nummuon",  "number of muons",  10, 0.0, 10.0);
  m_muonNum -> Sumw2();
  m_muonCharge = new TH1D("Chargemuon",  "muons charge",  10, -5, 5);
  m_muonCharge -> Sumw2();

  m_tauPt = new TH1D("Pttau",  "lepton p_{T}",  50, 0.0, 200.0);
  m_tauPt -> Sumw2();
  m_tauNum = new TH1D("Numtau",  "number of taus",  10, 0.0, 10.0);
  m_tauNum -> Sumw2();
  m_tauCharge = new TH1D("Chargetau",  "taus charge",  10, -5, 5);
  m_tauCharge -> Sumw2();



  //********** jets with pT cut ***********//

  //pT(jet)>20, eta(jet)<2.8

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT", i+1);
    m_jetPt[i] = new TH1D(m_histName, m_histName, 100, 0., 300);
    m_jetPt[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange", i+1);
    m_jetPtHighRange[i] = new TH1D(m_histName, m_histName, 300, 0., 1000);
    m_jetPtHighRange[i] -> Sumw2();
    m_histName.Form("jet%d_eta", i+1);
    m_jetEta[i] =   new TH1D(m_histName, m_histName, 50, -4, 4);
    m_jetEta[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt", i+1);
    m_jetMassPt[i] =   new TH1D(m_histName, m_histName, 100, 0.0, 0.1);
    m_jetMassPt[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR", i+1,j+1);
      m_jetdR[i][j] =   new TH1D(m_histName, m_histName,  64, 0, 3.2);
      m_jetdR[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi", i+1,j+1);
      m_jetdPhi[i][j] =   new TH1D(m_histName, m_histName, 40, -3.14, 3.14);
      m_jetdPhi[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass", i+1,j+1);
      m_jetMass[i][j] =   new TH1D(m_histName, m_histName, 50, 0.0, 50.0);
      m_jetMass[i][j] -> Sumw2();
    }
  }

  m_jetNum =    new TH1D("Numjet",     "number of jet",  10, 0.0, 10.0);
  m_jetNum -> Sumw2();

  //********** jets w/o pT cut ***********//

  //eta(jet)<2.8

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT_nocuts", i+1);
    m_jetPt_nocuts[i] = new TH1D(m_histName, m_histName, 100, 0., 300);
    m_jetPt_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange_nocuts", i+1);
    m_jetPtHighRange_nocuts[i] = new TH1D(m_histName, m_histName, 300, 0., 1000);
    m_jetPtHighRange_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_eta_nocuts", i+1);
    m_jetEta_nocuts[i] =   new TH1D(m_histName, m_histName, 50, -4, 4);
    m_jetEta_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt_nocuts", i+1);
    m_jetMassPt_nocuts[i] =   new TH1D(m_histName, m_histName, 100, 0.0, 0.1);
    m_jetMassPt_nocuts[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR_nocuts", i+1,j+1);
      m_jetdR_nocuts[i][j] =   new TH1D(m_histName, m_histName,  64, 0, 3.2);
      m_jetdR_nocuts[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi_nocuts", i+1,j+1);
      m_jetdPhi_nocuts[i][j] =   new TH1D(m_histName, m_histName, 40, -3.14, 3.14);
      m_jetdPhi_nocuts[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass_nocuts", i+1,j+1);
      m_jetMass_nocuts[i][j] =   new TH1D(m_histName, m_histName, 50, 0.0, 50.0);
      m_jetMass_nocuts[i][j] -> Sumw2();
    }
  }

  m_jetNum_nocuts  =    new TH1D("Numjet_nocuts",     "number of jet",  20, 0.0, 20.0);
  m_jetNum_nocuts -> Sumw2();

  //********** forward jets ***********//

  //eta(jet)>2.8, pT>20

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT_forward", i+1);
    m_jetPt_forward[i] = new TH1D(m_histName, m_histName, 100, 0., 300);
    m_jetPt_forward[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange_forward", i+1);
    m_jetPtHighRange_forward[i] = new TH1D(m_histName, m_histName, 300, 0., 1000);
    m_jetPtHighRange_forward[i] -> Sumw2();
    m_histName.Form("jet%d_eta_forward", i+1);
    m_jetEta_forward[i] =   new TH1D(m_histName, m_histName, 50, -4, 4);
    m_jetEta_forward[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt_forward", i+1);
    m_jetMassPt_forward[i] =   new TH1D(m_histName, m_histName, 100, 0.0, 0.1);
    m_jetMassPt_forward[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR_forward", i+1,j+1);
      m_jetdR_forward[i][j] =   new TH1D(m_histName, m_histName,  64, 0, 3.2);
      m_jetdR_forward[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi_forward", i+1,j+1);
      m_jetdPhi_forward[i][j] =   new TH1D(m_histName, m_histName, 40, -3.14, 3.14);
      m_jetdPhi_forward[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass_forward", i+1,j+1);
      m_jetMass_forward[i][j] =   new TH1D(m_histName, m_histName, 50, 0.0, 50.0);
      m_jetMass_forward[i][j] -> Sumw2();

    }
  }

  m_jetNum_forward  =    new TH1D("Numjet_forward",     "number of jet",  20, 0.0, 20.0);
  m_jetNum_forward -> Sumw2();

  //********** set with "tight" plots ***********//

  //pT(lepton)>20, eta(lepton)<2.5
  m_leptonPt_tight =    new TH1D("Ptlepton_tight",     "lepton p_{T}",  50, 0.0, 200.0);
  m_leptonPt_tight -> Sumw2();
  m_leptonEta_tight =   new TH1D("Etalepton_tight",    "lepton #eta",  50, -4, 4);
  m_leptonEta_tight -> Sumw2();

  //pT(jet)>20, eta(jet)<2.8

  m_jet1Pt_tight =    new TH1D("Ptjet1_tight",     "jet_{1} p_{T}",  50, 0.0, 200.0);
  m_jet1Pt_tight -> Sumw2();
  m_jet1PtHighRange_tight =    new TH1D("Ptjet1HighRange_tight",     "jet_{1} p_{T}",  300, 0.0, 1000.0);
  m_jet1PtHighRange_tight -> Sumw2();
  m_jet1Eta_tight =   new TH1D("Etajet1_tight",    "jet_{1} #eta",  50, -4, 4);
  m_jet1Eta_tight -> Sumw2();
  m_jet2Pt_tight =    new TH1D("Ptjet2_tight",     "jet_{2} p_{T}",  50, 0.0, 200.0);
  m_jet2Pt_tight -> Sumw2();
  m_jet2PtHighRange_tight =    new TH1D("Ptjet2HighRange_tight",     "jet_{2} p_{T}",  300, 0.0, 1000.0);
  m_jet2PtHighRange_tight -> Sumw2();
  m_jet2Eta_tight =   new TH1D("Etajet2_tight",    "jet_{2} #eta",  50, -4, 4);
  m_jet2Eta_tight -> Sumw2();

  m_dR_jet1_to_lepton_tight = new TH1D("DeltaRjet1_to_lepton_tight",   "#Delta R (jet_{1}-lepton)",  64, 0, 3.2);
  m_dR_jet1_to_lepton_tight -> Sumw2();
  m_dR_jet2_to_lepton_tight = new TH1D("DeltaRjet2_to_lepton_tight",   "#Delta R (jet_{2}-lepton)",  64, 0, 3.2);
  m_dR_jet2_to_lepton_tight -> Sumw2();
  m_dR_lepton_to_closest_jet_tight = new TH1D("DeltaRlepton_to_closest_jet_tight",   "#Delta R (lepton-jet)",  64, 0, 3.2);
  m_dR_lepton_to_closest_jet_tight -> Sumw2();

  m_dPhi_jet1_to_lepton_tight = new TH1D("DeltaPhijet1_to_lepton_tight",  "#Delta #phi (jet_{1}-lepton)",  40, -3.14, 3.14);
  m_dPhi_jet1_to_lepton_tight -> Sumw2();
  m_dPhi_jet2_to_lepton_tight = new TH1D("DeltaPhijet2_to_lepton_tight",  "#Delta #phi (jet_{2}-lepton)",  40, -3.14, 3.14);
  m_dPhi_jet2_to_lepton_tight -> Sumw2();
  m_dPhi_lepton_to_closest_jet_tight = new TH1D("DeltaPhilepton_to_closest_jet_tight",   "#Delta #phi (lepton-jet)",  40, -3.14, 3.14);
  m_dPhi_lepton_to_closest_jet_tight -> Sumw2();

  //vector boson plots                                                                                                                                        
  m_ZNum =  new TH1D("NumZ",   "number of Z", 10, 0.0, 10.0);
  m_ZNum -> Sumw2();
  m_Z1pt =  new TH1D("ptZ1",   "Z1 pT", 100, 0.0, 400.0);
  m_Z1pt -> Sumw2();
  m_Z1eta = new TH1D("etaZ1",   "Z1 eta", 50, -4, 4);
  m_Z1eta -> Sumw2();
  m_WNum =  new TH1D("NumW",   "number of W", 10, 0.0, 10.0);
  m_WNum -> Sumw2();
  m_W1pt =  new TH1D("ptW1",   "W1 pT", 100, 0.0, 400.0);
  m_W1pt -> Sumw2();
  m_W1eta =  new TH1D("etaW1",   "W1 eta", 50, -4, 4);
  m_W1eta -> Sumw2();
  m_gammaNum =  new TH1D("Numgamma",   "number of photons", 10, 0.0, 10.0);
  m_gammaNum -> Sumw2();
  m_gamma1pt =  new TH1D("ptgamma1",   "photon1 pt", 100, 0.0, 400.0);
  m_gamma1pt -> Sumw2();
  m_gamma1eta =  new TH1D("etagamma1",   "photon1 eta", 50, -4, 4);
  m_gamma1eta -> Sumw2();



}

// ----------------------------------------------------------------------
int LeptonJetAnalysis::Init(double maxEta, double minPt)
{
  m_max_eta = maxEta;
  m_min_pt = minPt;

  m_outputFileName = "LeptonJet.root";
  m_outputRootDir = "LeptonJet";

  m_event_weight = initHist("EventWeight",  "event weight", "weight", 10000, -5000000.0, 5000000.0);
  m_event_weight -> Sumw2();
  m_event_weight_zoom = initHist("EventWeight_Zoom",  "event weight", "weight", 41, -20.5, 20.5);
  m_event_weight_zoom -> Sumw2();

  //********** leptons ***********//

  m_leptonInclPt =   initHist("PtIncllepton", "lepton p_{T}", "p_{T} [GeV]", 50, 0.0, 200.0);
  m_leptonInclPt -> Sumw2();
  m_leptonNum =  initHist("Numlepton", "number of leptons", "number", 10, 0.0, 10.0);
  m_leptonNum -> Sumw2();

  for(UInt_t i = 0; i < nleptons; ++i){
    m_histName.Form("lepton%d_pT", i+1);
    m_leptonPt[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 100, 0., 300.);
    m_leptonPt[i] -> Sumw2();
    m_histName.Form("lepton%d_eta", i+1);
    m_leptonEta[i] = initHist(string( m_histName ), string( m_histName ), string( "#eta") ,   50, -4, 4);
    m_leptonEta[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < nleptons-1; ++i){
    for (UInt_t j=i+1;  j < nleptons;++j){
      m_histName.Form("lepton%d%d_deltaR", i+1,j+1);
      m_leptondR[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta R" ), 64, 0, 3.2);
      m_leptondR[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_deltaPhi", i+1,j+1);
      m_leptondPhi[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta #phi" ), 40, -3.14, 3.14);
      m_leptondPhi[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_Mass", i+1,j+1);
      m_leptonMass[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "mass [GeV]" ), 40, 40.0, 120.0);
      m_leptonMass[i][j] -> Sumw2();
      m_histName.Form("lepton%d%d_lowMass", i+1,j+1);
      m_leptonLowMass[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "mass [GeV]" ), 20, 0.0, 40.0);
      m_leptonLowMass[i][j] -> Sumw2();
    }
  }

  m_electronPt =   initHist("Ptelectron", "electron p_{T}", "p_{T} [GeV]", 50, 0.0, 200.0);
  m_electronPt -> Sumw2();
  m_electronNum =  initHist("Numelectron", "number of electrons", "number", 10, 0.0, 10.0);
  m_electronNum -> Sumw2();
  m_electronCharge =  initHist("Chargeelectron", "electrons charge", "number", 10, -5, 5);
  m_electronCharge -> Sumw2();



  m_muonPt =   initHist("Ptmuon", "muon p_{T}", "p_{T} [GeV]", 50, 0.0, 200.0);
  m_muonPt -> Sumw2();
  m_muonNum =  initHist("Nummuon", "number of muons", "number", 10, 0.0, 10.0);
  m_muonNum -> Sumw2();
  m_muonCharge =  initHist("Chargemuons", "muons charge", "number", 10, -5, 5);
  m_muonCharge -> Sumw2();

  m_tauPt =   initHist("Pttau", "tau p_{T}", "p_{T} [GeV]", 50, 0.0, 200.0);
  m_tauPt -> Sumw2();
  m_tauNum =  initHist("Numtau", "number of taus", "number", 10, 0.0, 10.0);
  m_tauNum -> Sumw2();
  m_tauCharge =  initHist("Chargetaus", "taus charge", "number", 10, -5, 5);
  m_tauCharge -> Sumw2();

  //********** jets with pT cut ***********//

  //pT(jet)>20, eta(jet)<2.8

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT", i+1);
    m_jetPt[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 100, 0., 300.);
    m_jetPt[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange", i+1);
    m_jetPtHighRange[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 300, 0., 1000.);
    m_jetPtHighRange[i] -> Sumw2();
    m_histName.Form("jet%d_eta", i+1);
    m_jetEta[i] = initHist(string( m_histName ), string( m_histName ), string( "#eta") ,   50, -4, 4);
    m_jetEta[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt", i+1);
    m_jetMassPt[i] = initHist(string( m_histName ), string( m_histName ), string( "mass^{2}/p_{T}^{2}") , 100, 0.0, 0.1);
    m_jetMassPt[i] -> Sumw2();

  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR", i+1,j+1);
      m_jetdR[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta R" ), 64, 0, 3.2);
      m_jetdR[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi", i+1,j+1);
      m_jetdPhi[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta #phi" ), 40, -3.14, 3.14);
      m_jetdPhi[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass", i+1,j+1);
      m_jetMass[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "mass [GeV]" ), 50, 0.0, 50.0);
      m_jetMass[i][j] -> Sumw2();
    }
  }

  m_jetNum =  initHist("Numjet",   "number of jet", "number", 10, 0.0, 10.0);
  m_jetNum -> Sumw2();

  //********** jets w/o pT cut ***********//

  //pT(jet)>20, eta(jet)<2.8

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT_nocuts", i+1);
    m_jetPt_nocuts[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 100, 0., 300.);
    m_jetPt_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange_nocuts", i+1);
    m_jetPtHighRange_nocuts[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 300, 0., 1000.);
    m_jetPtHighRange_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_eta_nocuts", i+1);
    m_jetEta_nocuts[i] = initHist(string( m_histName ), string( m_histName ), string( "#eta") ,   50, -4, 4);
    m_jetEta_nocuts[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt_nocuts", i+1);
    m_jetMassPt_nocuts[i] = initHist(string( m_histName ), string( m_histName ), string( "mass^{2}/p_{T}^{2}") , 100, 0.0, 0.1);
    m_jetMassPt_nocuts[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR_nocuts", i+1,j+1);
      m_jetdR_nocuts[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta R" ), 64, 0, 3.2);
      m_jetdR_nocuts[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi_nocuts", i+1,j+1);
      m_jetdPhi_nocuts[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta #phi" ), 40, -3.14, 3.14);
      m_jetdPhi_nocuts[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass_nocuts", i+1,j+1);
      m_jetMass_nocuts[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "mass [GeV]" ), 50, 0.0, 50.0);
      m_jetMass_nocuts[i][j] -> Sumw2();
    }
  }

  m_jetNum_nocuts =  initHist("Numjet_nocuts",   "number of jet", "number", 10, 0.0, 10.0);
  m_jetNum_nocuts -> Sumw2();

  //********** forward jets  ***********//

  //pT(jet)>20, eta(jet)>2.8

  for(UInt_t i = 0; i < njets; ++i){
    m_histName.Form("jet%d_pT_forward", i+1);
    m_jetPt_forward[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 100, 0., 300.);
    m_jetPt_forward[i] -> Sumw2();
    m_histName.Form("jet%d_pT_highrange_forward", i+1);
    m_jetPtHighRange_forward[i] = initHist(string( m_histName ), string( m_histName ), string( "p_{T} [GeV]") , 300, 0., 1000.);
    m_jetPtHighRange_forward[i] -> Sumw2();
    m_histName.Form("jet%d_eta_forward", i+1);
    m_jetEta_forward[i] = initHist(string( m_histName ), string( m_histName ), string( "#eta") ,   50, -4, 4);
    m_jetEta_forward[i] -> Sumw2();
    m_histName.Form("jet%d_MassPt_forward", i+1);
    m_jetMassPt_forward[i] = initHist(string( m_histName ), string( m_histName ), string( "mass^{2}/p_{T}^{2}") , 100, 0.0, 0.1);
    m_jetMassPt_forward[i] -> Sumw2();
  }

  for(UInt_t i = 0; i < njets-1; ++i){
    for (UInt_t j=i+1;  j < njets;++j){
      m_histName.Form("jet%d%d_deltaR_forward", i+1,j+1);
      m_jetdR_forward[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta R" ), 64, 0, 3.2);
      m_jetdR_forward[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_deltaPhi_forward", i+1,j+1);
      m_jetdPhi_forward[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "#Delta #phi" ), 40, -3.14, 3.14);
      m_jetdPhi_forward[i][j] -> Sumw2();
      m_histName.Form("jet%d%d_Mass_forward", i+1,j+1);
      m_jetMass_forward[i][j] =   initHist(string( m_histName ), string( m_histName ), string( "mass [GeV]" ), 50, 0.0, 50.0);
      m_jetMass_forward[i][j] -> Sumw2();
    }
  }

  m_jetNum_forward =  initHist("Numjet_forward",   "number of jet", "number", 10, 0.0, 10.0);
  m_jetNum_forward -> Sumw2();


  //********** set with "tight" plots ***********//

  //pT(lepton)>20, eta(lepton)<2.5
  m_leptonPt_tight =    initHist("Ptlepton_tight",     "lepton p_{T}",  "p_{T} [GeV]",  50, 0.0, 200.0);
  m_leptonPt_tight -> Sumw2();
  m_leptonEta_tight =   initHist("Etalepton_tight",    "lepton #eta",  "#eta",  50, -4, 4);
  m_leptonEta_tight -> Sumw2();

  //pT(jet)>20, eta(jet)<2.8

  m_jet1Pt_tight =    initHist("Ptjet1_tight",     "jet_{1} p_{T}",  "p_{T} [GeV]",   50, 0.0, 200.0);
  m_jet1Pt_tight -> Sumw2();
  m_jet1PtHighRange_tight =    initHist("Ptjet1HighRange_tight",     "jet_{1} p_{T}",  "p_{T} [GeV]",   300, 0.0, 1000.0);
  m_jet1PtHighRange_tight -> Sumw2();
  m_jet1Eta_tight =   initHist("Etajet1_tight",    "jet_{1} #eta",  "#eta", 50, -4, 4);
  m_jet1Eta_tight -> Sumw2();
  m_jet2Pt_tight =    initHist("Ptjet2_tight",     "jet_{2} p_{T}",  "p_{T} [GeV]",   50, 0.0, 200.0);
  m_jet2Pt_tight -> Sumw2();
  m_jet2PtHighRange_tight =    initHist("Ptjet2HighRange_tight",     "jet_{2} p_{T}",  "p_{T} [GeV]",   300, 0.0, 1000.0);
  m_jet2PtHighRange_tight -> Sumw2();
  m_jet2Eta_tight =   initHist("Etajet2_tight",    "jet_{2} #eta",  "#eta",  50, -4, 4);
  m_jet2Eta_tight -> Sumw2();

  m_dR_jet1_to_lepton_tight = initHist("DeltaRjet1_to_lepton_tight",   "#Delta R (jet_{1}-lepton)","#Delta R",  64, 0, 3.2);
  m_dR_jet1_to_lepton_tight -> Sumw2();
  m_dR_jet2_to_lepton_tight = initHist("DeltaRjet2_to_lepton_tight",   "#Delta R (jet_{2}-lepton)", "#Delta R", 64, 0, 3.2);
  m_dR_jet2_to_lepton_tight -> Sumw2();
  m_dR_lepton_to_closest_jet_tight = initHist("DeltaRlepton_to_closest_jet_tight",   "#Delta R (lepton-jet)", "#Delta R", 64, 0, 3.2);
  m_dR_lepton_to_closest_jet_tight -> Sumw2();

  m_dPhi_jet1_to_lepton_tight = initHist("DeltaPhijet1_to_lepton_tight",  "#Delta #phi (jet_{1}-lepton)",   "#Delta #phi", 40, -3.14, 3.14);
  m_dPhi_jet1_to_lepton_tight -> Sumw2();
  m_dPhi_jet2_to_lepton_tight = initHist("DeltaPhijet2_to_lepton_tight",  "#Delta #phi (jet_{2}-lepton)",   "#Delta #phi", 40, -3.14, 3.14);
  m_dPhi_jet2_to_lepton_tight -> Sumw2();
  m_dPhi_lepton_to_closest_jet_tight = initHist("DeltaPhilepton_to_closest_jet_tight",   "#Delta #phi (lepton-jet)",   "#Delta #phi", 40, -3.14, 3.14);
  m_dPhi_lepton_to_closest_jet_tight -> Sumw2();

  //vector boson plots
  m_ZNum =  initHist("NumZ",   "number of Z", "number", 10, 0.0, 10.0);
  m_ZNum -> Sumw2();
  m_Z1pt =  initHist("ptZ1",   "Z1 pT", "number", 100, 0.0, 400.0);
  m_Z1pt -> Sumw2();
  m_Z1eta =  initHist("etaZ1",   "Z1 eta", "number", 50, -4, 4);
  m_Z1eta -> Sumw2();
  m_WNum =  initHist("NumW",   "number of W", "number", 10, 0.0, 10.0);
  m_WNum -> Sumw2();
  m_W1pt =  initHist("ptW1",   "W1 pT", "number", 100, 0.0, 400.0);
  m_W1pt -> Sumw2();
  m_W1eta =  initHist("etaW1",   "W1 eta", "number", 50, -4, 4);
  m_W1eta -> Sumw2();
  m_gammaNum =  initHist("Numgamma",   "number of photons", "number", 10, 0.0, 10.0);
  m_gammaNum -> Sumw2();
  m_gamma1pt =  initHist("ptgamma1",   "photon1 pt", "number", 100, 0.0, 400.0);
  m_gamma1pt -> Sumw2();
  m_gamma1eta =  initHist("etagamma1",   "photon1 eta", "number", 50, -4, 4);
  m_gamma1eta -> Sumw2();

  return true;
}

// ----------------------------------------------------------------------
int LeptonJetAnalysis::Process(HepMC::GenEvent *event)
{
  double weight;

  HepMC::GenParticle *lepton = 0;

  std::vector<fastjet::PseudoJet> selected_jets;
  std::vector<fastjet::PseudoJet> forward_jets;
  std::vector<fastjet::PseudoJet> no_cuts_jets;
  std::vector<fastjet::PseudoJet> unsorted_leptons;
  std::vector<fastjet::PseudoJet> leptons;
  std::vector<fastjet::PseudoJet> unsorted_tight_leptons;
  std::vector<fastjet::PseudoJet> tight_leptons;
  double ptmin_l = 20;
  double etamax_l = 2.5;
  double ptmin_jet = 20;
  double etamax_jet = 2.8;
  double etamin_jetf = 2.8;

  Int_t NumLeptons = 0, NumElectrons = 0, NumMuons = 0, NumTaus = 0, NumZ = 0, NumW = 0, NumGamma = 0;

  if (event -> event_number() % 1000 == 0) {
    cout << "Processing event " << event -> event_number() << endl;
  }
  weight = MC::get_weight(event, 0 );
  m_event_weight -> Fill(weight);
  m_event_weight_zoom -> Fill(weight);

  for (HepMC::GenEvent::particle_const_iterator p = event -> particles_begin(); p != event -> particles_end(); ++p){


    if ( abs((*p) -> pdg_id()) == 23 && (MC::isLastReplica(*p))){
      NumZ++;
      m_ZNum->Fill(weight);
      m_Z1pt->Fill((*p) -> momentum().perp(), weight);
      m_Z1eta->Fill((*p) -> momentum().eta(), weight);

    }

    if ( abs((*p) -> pdg_id()) == 24 && (MC::isLastReplica(*p))){
      NumW++;
      m_WNum->Fill(weight);
      m_W1pt->Fill((*p) -> momentum().perp(), weight);
      m_W1eta->Fill((*p) -> momentum().eta(), weight);

    }

    if ( abs((*p) -> pdg_id()) == 22 && (MC::isLastReplica(*p))){
      NumGamma++;
      m_gammaNum->Fill(weight);
      m_gamma1pt->Fill((*p) -> momentum().perp(), weight);
      m_gamma1eta->Fill((*p) -> momentum().eta(), weight);
    }




    if ((MC::PID::isLepton((*p) -> pdg_id())) && (MC::isLastReplica(*p))) // last replica of charged lepton selected
      {
  	lepton = (*p);
  	if (!MC::ptGtr(lepton, ptmin_l)) continue;
  	if (!MC::isStable(lepton)) continue;
  	if (!MC::isCharged(lepton)) continue;

	int charge = ((*p) -> pdg_id() > 0) ? 1 : (((*p) -> pdg_id() < 0) ? -1 : 0);

  	m_leptonInclPt -> Fill(lepton -> momentum().perp(), weight);

  	//fill plots for electrons
  	if (abs(lepton -> pdg_id()) == 11) {
  	  m_electronPt -> Fill(lepton -> momentum().perp(), weight);
	  m_electronCharge -> Fill(charge, weight);
  	  NumElectrons++;
  	}
  	//fill plots for muons
  	if (abs(lepton -> pdg_id()) == 13) {
  	  m_muonPt -> Fill(lepton -> momentum().perp(), weight);
	  m_muonCharge -> Fill(charge, weight);
  	  NumMuons++;
  	}
  	//fill plots for taus
	//pdgid 17??? seems wrong  	if ((abs(lepton -> pdg_id()) == 15) || (abs(lepton -> pdg_id()) == 17)) {
  	if (abs(lepton -> pdg_id()) == 15) {
  	  m_tauPt -> Fill(lepton -> momentum().perp(), weight);
	  m_tauCharge -> Fill(charge, weight);
  	  NumTaus++;
  	}
  	//fill vector<fastjet::PseudoJet> with all leptons selected in event
//  	unsorted_leptons.push_back(MC::mk_pseudojet(lepton));
        const HepMC::FourVector& p4 = lepton->momentum();

        const HEPUtils::P4 leptonMomentum(p4.px(), p4.py(), p4.pz(), p4.e());

       unsorted_leptons.push_back(HEPUtils::mk_pseudojet(leptonMomentum));
  	if( abs(lepton -> momentum().eta()) < etamax_l){
//  	  unsorted_tight_leptons.push_back(MC::mk_pseudojet(lepton));
          unsorted_tight_leptons.push_back(HEPUtils::mk_pseudojet(leptonMomentum));
  	}
  	NumLeptons++;
      }
  }

  //fill number of electrons/muons/taus/leptons
  m_leptonNum -> Fill(NumLeptons, weight);
  m_electronNum -> Fill(NumElectrons, weight);
  m_muonNum -> Fill(NumMuons, weight);
  m_tauNum -> Fill(NumTaus, weight);

  //sort selected leptons by pT

  leptons = sorted_by_pt(unsorted_leptons);
  tight_leptons = sorted_by_pt(unsorted_tight_leptons);

  //fill histos for leptons

  for (UInt_t i = 0; i < leptons.size(); ++i) {
    if (i < nleptons){
      m_leptonPt[i] -> Fill(leptons[i].perp(), weight);
      m_leptonEta[i] -> Fill(leptons[i].eta(), weight);
    }
     for (UInt_t j = i + 1;  j < leptons.size() ;++j){
       if ((i < nleptons-1)&&(j < nleptons)){
	 m_leptondR[i][j]->Fill(sqrt(leptons[i].squared_distance(leptons[j])), weight);
	 m_leptondPhi[i][j]->Fill(leptons[i].delta_phi_to(leptons[j]), weight);
	 m_leptonMass[i][j]->Fill((leptons[i] + leptons[j]).m(), weight);
	 m_leptonLowMass[i][j]->Fill((leptons[i] + leptons[j]).m(), weight);
       }
     }
  }

  //loop over jets and fill vectors

  for (size_t i = 0; i < m_inclusive_jets.size(); ++i) {
    if ((abs(m_inclusive_jets[i].eta()) < etamax_jet) && (m_inclusive_jets[i].perp() > ptmin_jet)){
      selected_jets.push_back(m_inclusive_jets[i]);
    }
    if (abs(m_inclusive_jets[i].eta()) < etamax_jet){
      no_cuts_jets.push_back(m_inclusive_jets[i]);
    }
    if ( (abs(m_inclusive_jets[i].eta()) >= etamin_jetf) && (m_inclusive_jets[i].perp() > ptmin_jet)){
      forward_jets.push_back(m_inclusive_jets[i]);
    }
  }

  //fill histos for jets with pT > 20, eta < 2.8

  for (UInt_t i = 0; i < selected_jets.size(); ++i) {
    if (i < njets){
      m_jetPt[i] -> Fill(selected_jets[i].perp(), weight);
      m_jetPtHighRange[i] -> Fill(selected_jets[i].perp(), weight);
      m_jetEta[i] -> Fill(selected_jets[i].eta(), weight);
      m_jetMassPt[i] -> Fill((selected_jets[i].m()*selected_jets[i].m())/(selected_jets[i].perp()*selected_jets[i].perp()), weight);
    }
     for (UInt_t j = i + 1;  j < selected_jets.size() ;++j){
       if ((i < njets-1)&&(j < njets)){
	 m_jetdR[i][j]->Fill(sqrt(selected_jets[i].squared_distance(selected_jets[j])), weight);
	 m_jetMass[i][j] -> Fill(selected_jets[i].m() + selected_jets[j].m(), weight);
	 m_jetdPhi[i][j]->Fill(selected_jets[i].delta_phi_to(selected_jets[j]), weight);
       }
     }
  }

  //fill histos for jets with eta < 2.8

  for (UInt_t i = 0; i < no_cuts_jets.size(); ++i) {
    if (i < njets){
      m_jetPt_nocuts[i] -> Fill(no_cuts_jets[i].perp(), weight);
      m_jetPtHighRange_nocuts[i] -> Fill(no_cuts_jets[i].perp(), weight);
      m_jetEta_nocuts[i] -> Fill(no_cuts_jets[i].eta(), weight);
      m_jetMassPt_nocuts[i] -> Fill((no_cuts_jets[i].m()*no_cuts_jets[i].m())/(no_cuts_jets[i].perp()*no_cuts_jets[i].perp()), weight);
    }
     for (UInt_t j = i + 1;  j < no_cuts_jets.size() ;++j){
       if ((i < njets-1)&&(j < njets)){
	 m_jetdR_nocuts[i][j]->Fill(sqrt(no_cuts_jets[i].squared_distance(no_cuts_jets[j])), weight);
	 m_jetMass_nocuts[i][j] -> Fill(no_cuts_jets[i].m() + no_cuts_jets[j].m(), weight);
	 m_jetdPhi_nocuts[i][j]->Fill(no_cuts_jets[i].delta_phi_to(no_cuts_jets[j]), weight);
       }
     }
  }

  //fill histos for jets with pT > 20, eta > 2.8

  for (UInt_t i = 0; i < forward_jets.size(); ++i) {
    if (i < njets){
      m_jetPt_forward[i] -> Fill(forward_jets[i].perp());
      m_jetPtHighRange_forward[i] -> Fill(no_cuts_jets[i].perp(), weight);
      m_jetEta_forward[i] -> Fill(forward_jets[i].eta());
      m_jetMassPt_forward[i] -> Fill((forward_jets[i].m()*forward_jets[i].m())/(forward_jets[i].perp()*forward_jets[i].perp()), weight);
    }
     for (UInt_t j = i + 1;  j < forward_jets.size() ;++j){
       if ((i < njets-1)&&(j < njets)){
	 m_jetdR_forward[i][j]->Fill(sqrt(forward_jets[i].squared_distance(forward_jets[j])), weight);
  	m_jetMass_forward[i][j] -> Fill(forward_jets[i].m() + forward_jets[j].m(), weight);
  	m_jetdPhi_forward[i][j]->Fill(forward_jets[i].delta_phi_to(forward_jets[j]), weight);
       }
     }
  }

  m_jetNum_nocuts -> Fill(no_cuts_jets.size(), weight);
  m_jetNum -> Fill(selected_jets.size(), weight);
  m_jetNum_forward -> Fill(forward_jets.size(), weight);


  Double_t dR1 = 9999, dR2 = 9999, dRmin = 9999;
  Double_t dPhi1 = 9999, dPhi2 = 9999, dPhimin = 9999;

  if (tight_leptons.size() !=0 ){
    m_leptonPt_tight -> Fill(tight_leptons[0].perp(), weight);
    m_leptonEta_tight -> Fill(tight_leptons[0].eta(), weight);
    for (size_t i = 0; i < selected_jets.size(); ++i) {
      m_jet1Pt_tight -> Fill(selected_jets[0].perp(), weight);
      m_jet1PtHighRange_tight -> Fill(selected_jets[0].perp(), weight);
      m_jet1Eta_tight -> Fill(selected_jets[0].eta(), weight);
      m_dR_jet1_to_lepton_tight -> Fill(sqrt(selected_jets[0].squared_distance(tight_leptons[0])), weight);
      m_dPhi_jet1_to_lepton_tight -> Fill(selected_jets[0].delta_phi_to(tight_leptons[0]), weight);

      dR1 = selected_jets[0].squared_distance (tight_leptons[0]);
      dPhi1 = selected_jets[0].delta_phi_to(tight_leptons[0]);

      if  (i>=1){

  	m_jet2Pt_tight -> Fill(selected_jets[1].perp(), weight);
	m_jet2PtHighRange_tight -> Fill(selected_jets[1].perp(), weight);
  	m_jet2Eta_tight -> Fill(selected_jets[1].eta(), weight);
  	m_dR_jet2_to_lepton_tight -> Fill(sqrt(selected_jets[1].squared_distance(tight_leptons[0])), weight);
  	m_dPhi_jet2_to_lepton_tight -> Fill(selected_jets[1].delta_phi_to(tight_leptons[0]), weight);

  	dR2 = selected_jets[1].squared_distance(tight_leptons[0]);
  	dPhi2 = selected_jets[1].delta_phi_to(tight_leptons[0]);
      }

      dRmin = ( abs(dR2) > abs(dR1)) ? dR1 :dR2;
      dPhimin = (abs(dPhi2) > abs(dPhi1)) ? dPhi1 :dPhi2;
      m_dR_lepton_to_closest_jet_tight -> Fill(dRmin, weight);
      m_dPhi_lepton_to_closest_jet_tight -> Fill(dPhimin, weight);
    }
  }


  return true;
}
