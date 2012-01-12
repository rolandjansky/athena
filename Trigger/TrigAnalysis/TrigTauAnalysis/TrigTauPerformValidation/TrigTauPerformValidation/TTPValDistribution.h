/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALDISTRIBUTION_H_
#define _TTPVALDISTRIBUTION_H_

#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
//#include "TrigTauPerformAnalysis/TrigTauL2TrkFEX.h"
#include <string>
using namespace std;


class TTPValDistribution {

private:
  string filename;
  bool fileDefined;
  bool selected;
  bool logScale;
  bool plotAllMatchedEvents;
  bool ignoreMatching;
  TChain * chain;
  bool debug;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauSelectionManager::CUTLEVEL cutLevel;
  TrigTauMCTauList::OFFLINESELECTION offlineType;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  TrigTauFlags::HLTAUTHOR chainType;
  bool signal;
  float maxVisEta, minLeadingPt;
  TrigTauAnalyzer analyzer[3];
  TrigTauSelectionManager selmanager;
  vector<TString> hnames;
  static const unsigned int nHistograms =96;
  TH1F * histograms[nHistograms];
  TrigTauBugFix bugfix;
  TrigTauEvent* event;
  unsigned int nL1Cuts;
  unsigned int nL2CaloCuts;
  unsigned int nL2TrkCuts;
  unsigned int nEFCuts;
  unsigned int nHistos;
  unsigned int nNames;

  void analyzeEvent(int eventNumber);
  void createHistograms(TH1F ** histos);
  void initialize(bool debug1 = false);

public:

  TTPValDistribution(TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);
  TTPValDistribution ( const char * filename1 , TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);

  void analyze(int number = 10000000);
  void print();
  void save(const char * name = "histo.root");
  void compare(const char * reference, const char * output = "ntuple.eps");
  void compare(const char * reference1, const char* reference2, const char * output);
  void update(const char * name);
  
  void setFile( const char * filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta ( float newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setSelection ( bool newValue ) { selected = newValue; };          //selected turns on cuts for histograms to not plot extrem values
  void setLogScale ( bool newValue) { logScale = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setPlotAllMatchedEvents (bool newValue) { plotAllMatchedEvents = newValue; };
  void setPlotAllEvents (bool newValue);
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };

  bool getDebug () { return debug; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  bool getSelection() { return selected; };
  bool getLogScale() { return logScale; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getPlotAllMatchedEvents () { return plotAllMatchedEvents; };
  bool getIgnoreMatching () { return ignoreMatching; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};

};

#endif
