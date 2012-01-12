/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALLEVELCUT_H_
#define _TTPVALLEVELCUT_H_

#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPostScript.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
using namespace std;


class TTPValLevelCut {

private:
  static const unsigned int nCutLevels = 3;
  TString filename;
  bool fileDefined;
  TChain * chain;
  bool debug;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;  
  TrigTauSelectionManager::CUTLEVEL theCutLevel[nCutLevels];
  TrigTauMCTauList::OFFLINESELECTION offlineType;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  bool signal;
  int nprong;
  float maxVisEta, minLeadingPt;
  TrigTauAnalyzer analyzer;
  TrigTauSelectionManager selmanager;
  TrigTauBugFix bugfix;
  TrigTauEvent* event;
  unsigned int nTau;
  int npt;
  float minpt;
  float maxpt;
  int color[nCutLevels];
  int style[nCutLevels];
  float Threshold;
  TString test[nCutLevels];
  TH1F* hMC_InVisPt;            
  TH1F* hMCVisPt;               //MC Pt dist (denominator)
  TH1F* hL1VisPt[nCutLevels];   // L1 selected (numerator)
  TH1F* hL2VisPt[nCutLevels];   // L2 selected (numerator)
  TH1F* hEFVisPt[nCutLevels];   // EF selected (numerator)
  TH1F* hL1PtEff[nCutLevels];   // L1 efficiency (result)
  TH1F* hL2PtEff[nCutLevels];   // L2 efficiency (result)
  TH1F* hEFPtEff[nCutLevels];   // EF efficiency (result)
  TH1F* hntrak;
  
  TH1F* hMC_InVisEta;
  TH1F* hMCVisEta;              //MC Pt dist (denominator)
  TH1F* hL1VisEta[nCutLevels];  // L1 selected (numerator)
  TH1F* hL2VisEta[nCutLevels];  // L2 selected (numerator)
  TH1F* hEFVisEta[nCutLevels];  // EF selected (numerator)
  TH1F* hL1EtaEff[nCutLevels];  // L1 efficiency (result)
  TH1F* hL2EtaEff[nCutLevels];  // L2 efficiency (result)
  TH1F* hEFEtaEff[nCutLevels];  // EF efficiency (result)

  int nMatch[nCutLevels];
  int nL1passed[nCutLevels];
  int nL2passed[nCutLevels];
  int nEFpassed[nCutLevels];
  int n_OfflineMatchEvent[nCutLevels];
  int n_L1MatchEvent[nCutLevels];
  int n_L2MatchEvent[nCutLevels];
  int n_EFMatchEvent[nCutLevels];
  
  int nEvent;
  int nTrue;
  int nGood;
  int nMatched;

  void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1);
  void initialize(bool debug1 = false);
  void analyzeEvent(int eventNumber);
  

public:
  
  TTPValLevelCut(TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);

  TTPValLevelCut( TString filename1 , TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);
  void analyze();
  void print();
  void update(TString name);
  void printCSC(TPostScript & ps);

  void setFile( TString filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setNprong (int newValue) { nprong = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  //void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta (float  newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setSignal (bool newValue) {signal = newValue; };
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };


  bool getDebug () { return debug; };
  int getNprong () { return nprong; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  //TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getSignal () {return signal; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};



};
#endif
