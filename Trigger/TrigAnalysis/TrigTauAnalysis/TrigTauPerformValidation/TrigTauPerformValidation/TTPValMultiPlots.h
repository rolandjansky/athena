/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALMULTIPLOTS_H_
#define _TTPVALMULTIPLOTS_H_

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


class TTPValMultiPlots {

private:
  TString filename;
  bool fileDefined;
  TChain * chain;
  bool debug;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;  
  TrigTauSelectionManager::CUTLEVEL cutLevel;
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
  static const unsigned int nMenus = 7;
  int color[nMenus];
  int style[nMenus];
  TrigTauFlags::TRIGGERMENU theMenu[nMenus];
  float Threshold[nMenus];
  TString test[nMenus];
  TH1F* hMC_InVisPt;
  TH1F* hMCVisPt;
  TH1F* hL1VisPt[nMenus];   // L1 selected (numerator)
  TH1F* hL2VisPt[nMenus];   // L2 selected (numerator)
  TH1F* hEFVisPt[nMenus];   // EF selected (numerator)
  TH1F* hL1PtEff[nMenus];
  TH1F* hL2PtEff[nMenus];
  TH1F* hEFPtEff[nMenus];
  TH1F* hntrak;
  TH1F* hEFnTracks[nMenus];

  TH1F* hL21PtEff[nMenus];
  TH1F* hEFL2PtEff[nMenus];

  TH1F* hMC_InVisEta;
  TH1F* hMCVisEta;
  TH1F* hL1VisEta[nMenus];
  TH1F* hL2VisEta[nMenus];
  TH1F* hEFVisEta[nMenus];
  TH1F* hL1EtaEff[nMenus];
  TH1F* hL2EtaEff[nMenus];
  TH1F* hEFEtaEff[nMenus];

  TH1F* hL21EtaEff[nMenus];
  TH1F* hEFL2EtaEff[nMenus];

  int nMatch[nMenus];
  int nL1passed[nMenus];
  int nL2passed[nMenus];
  int nEFpassed[nMenus];
  int n_OfflineMatchEvent[nMenus];
  int n_L1MatchEvent[nMenus];
  int n_L2MatchEvent[nMenus];
  int n_EFMatchEvent[nMenus];
  
  int nEvent;
  int nTrue;
  int nGood;
  int nMatched;

  void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1);
  void initialize(bool debug1 = false);
  void analyzeEvent(int eventNumber);
  

public:
  
  TTPValMultiPlots (TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);

  TTPValMultiPlots ( TString filename1 , TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);
  void analyze();
  void print();
  void update(TString name);
  void printCSC(TPostScript & ps);

  void setFile( TString filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setNprong (int newValue) { nprong = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta (float  newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setSignal (bool newValue) {signal = newValue; };
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };


  bool getDebug () { return debug; };
  int getNprong () { return nprong; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getSignal () {return signal; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};



};
#endif
