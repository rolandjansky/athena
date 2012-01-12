/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALOFFLINEPLOTS_H_
#define _TTPVALOFFLINEPLOTS_H_

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


class TTPValOfflinePlots {

private:
  TString filename;
  bool fileDefined;
  TChain * chain;
  bool debug;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;  
  TrigTauSelectionManager::CUTLEVEL cutLevel;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  bool signal;
  float maxVisEta, minLeadingPt;
  TrigTauSelectionManager selmanager;
  TrigTauBugFix bugfix;
  TrigTauEvent* event;
  unsigned int nTau;
  int npt;
  float minpt;
  float maxpt;
  static const int nOffline = 1;
  static const int nNprong =2;
  int color[nOffline][nNprong];
  int style[nOffline][nNprong];
  TrigTauMCTauList::OFFLINESELECTION offlineType[nOffline];
  int nprong[nNprong];
  TrigTauAnalyzer analyzer[nOffline][nNprong];
  float threshold;
  TString test[nOffline][nNprong];
  TH1F* hMC_InVisPt[nOffline][nNprong];
  TH1F* hMCVisPt[nOffline][nNprong];
  TH1F* hL1VisPt[nOffline][nNprong];   // L1 selected (numerator)
  TH1F* hL2VisPt[nOffline][nNprong];   // L2 selected (numerator)
  TH1F* hEFVisPt[nOffline][nNprong];   // EF selected (numerator)
  TH1F* hL1PtEff[nOffline][nNprong];
  TH1F* hL2PtEff[nOffline][nNprong];
  TH1F* hEFPtEff[nOffline][nNprong];
  TH1F* hntrak;
  TH1F* hEFnTracks[nOffline][nNprong];

  TH1F* hL21PtEff[nOffline][nNprong];
  TH1F* hEFL2PtEff[nOffline][nNprong];

  TH1F* hMC_InVisEta[nOffline][nNprong];
  TH1F* hMCVisEta[nOffline][nNprong];
  TH1F* hL1VisEta[nOffline][nNprong];
  TH1F* hL2VisEta[nOffline][nNprong];
  TH1F* hEFVisEta[nOffline][nNprong];
  TH1F* hL1EtaEff[nOffline][nNprong];
  TH1F* hL2EtaEff[nOffline][nNprong];
  TH1F* hEFEtaEff[nOffline][nNprong];

  TH1F* hL21EtaEff[nOffline][nNprong];
  TH1F* hEFL2EtaEff[nOffline][nNprong];

  int nMatch[nOffline][nNprong];
  int nL1passed[nOffline][nNprong];
  int nL2passed[nOffline][nNprong];
  int nEFpassed[nOffline][nNprong];
  int n_OfflineMatchEvent[nOffline][nNprong];
  int n_L1MatchEvent[nOffline][nNprong];
  int n_L2MatchEvent[nOffline][nNprong];
  int n_EFMatchEvent[nOffline][nNprong];
  
  int nEvent;
  int nTrue[nOffline][nNprong];
  int nGood[nOffline][nNprong];
  int nMatched[nOffline][nNprong];
  TString getName(TrigTauMCTauList::OFFLINESELECTION selection);
  TString getName(int selection);
  void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1);
  void initialize(bool debug1 = false);
  void analyzeEvent(int eventNumber);
  

public:
  
  TTPValOfflinePlots (TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);

  TTPValOfflinePlots ( TString filename1 , TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);
  void analyze();
  void print();
  void update(TString name);
  void printCSC(TPostScript & ps);

  void setFile( TString filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta (float  newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setSignal (bool newValue) {signal = newValue; };
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };


  bool getDebug () { return debug; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getSignal () {return signal; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};



};
#endif
