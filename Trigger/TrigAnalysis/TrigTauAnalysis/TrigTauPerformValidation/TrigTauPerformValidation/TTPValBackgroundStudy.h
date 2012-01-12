/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALBACKGROUNDSTUDY_H_
#define _TTPVALBACKGROUNDSTUDY_H_

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


class TTPValBackgroundStudy {

private:
  TString filename;
  bool fileDefined;
  TChain * chain;
  int type;
  bool debug, invertL2;
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
  vector<TString> hnames;
  unsigned int nL1Cuts;
  unsigned int nL2CaloCuts;
  unsigned int nL2TrkCuts;
  unsigned int nEFCuts;
  unsigned int nHistos;
  unsigned int nNames;
  TH1F * hTot[3];
  static const unsigned int nHistograms =96;
  TH1F * histograms[nHistograms];
  TH1F * effHistograms[nHistograms];
  unsigned int passedCounter[nHistograms];
  TH2F * testHisto[2];
  unsigned long l1Word[100];
  unsigned long l2CaloWord[100];
  unsigned long l2TrkWord[100];
  unsigned long l2Word[100];
  unsigned long efWord[100];
  TrigTauBugFix bugfix;
  TrigTauEvent* event;
  int nev, testing;
  bool fillHisto[nHistograms];
  int totalPassed;

  void analyzeEvent(int eventNumber);
  TH1F * getEffHist(TH1F * hTot1, TH1F * hCut1, TString name);
  void createHistograms(TH1F ** histos);
  void initialize(bool debug1 = false, bool invertL21 = false);
  void createEffHistograms();
  void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1);
  TString getName(TrigTauMCTauList::OFFLINESELECTION offlineType1);

public:

  TTPValBackgroundStudy(bool invertL21 = false, TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);
  TTPValBackgroundStudy (TString filename1 , bool invertL21 = false, TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);

  void analyze();
  void print(float lum = 0, float xsection = 0);                             //prints under ntuple.eps
  void print(TString name, float lum = 0, float xsectioni = 0);
  void save();
  void save(TString name);             //saves under trigtau.ntuple.root
  void compare(TString reference, TString saveName);
  void compare(TString reference1, TString reference2, TString saveName);
  void compare(TString reference);     //saves under ntuple.eps 
  void update(TString name);           //updates a root file with all histograms
  void reset();
  
  void setFile( TString filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setType ( int newValue) { type = newValue; };
  void setNprong (int newValue) { nprong = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta (float  newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setSignal (bool newValue) {signal = newValue; };
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };
  void setInvertL2 (bool newValue) {invertL2 = newValue;};  
  
  bool getDebug () { return debug; };
  int getType () { return type; };
  int getNprong () { return nprong; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getSignal () {return signal; };
  bool getInvertL2 () {return invertL2;};
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};

  

};

#endif
