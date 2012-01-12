/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPValRateTable_H_
#define _TTPValRateTable_H_

#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
#include <iostream>
#include <fstream>

using namespace std;

class TTPValRateTable  {
  
private:
  long nEvent;
  long nev;
  long nL1Sel;
  long nL2Sel;
  long nL2CalSel;
  long nEFSel;

  int nDecL1;
  int nDecL2;
  int nDecEF;
  
  double nEventWt;
  double nL1SelWt;
  double nL2SelWt;
  double nL2CalSelWt;
  double nEFSelWt;

  int nPrintL1;
  int nPrintL2;
  int nPrintEF;
  
  bool signal, signalSet;
  bool fileDefined;
  bool checkOffline;
  bool doDetailedPrinting;
  bool checkL2Cal, printChain;
  TString filename;
  TChain * chain;
  int type;
  bool debug;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;
  TrigTauSelectionManager::CUTLEVEL cutLevel;
  TrigTauMCTauList::OFFLINESELECTION offlineType;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  TrigTauBugFix::TTPVERSION version;
  TrigTauBugFix::POOLFILE poolfile;
  int nprong;
  float maxVisEta, minLeadingPt;
  float met;
  TrigTauBugFix bugfix;
  

  void initialize (bool debug1 = false);

public:

  TTPValRateTable(TrigTauBugFix::POOLFILE poolfile1 = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version1 = TrigTauBugFix::TTP15, bool debug1 = false);
  TTPValRateTable( TString filename1 , TrigTauBugFix::POOLFILE poolfile1 = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version1 = TrigTauBugFix::TTP15,  bool debug1 = false);

  void analyze(int nEvents = 10000000);
  void print(bool file = true);
  void print(TString target, bool file = true);                                //prints under ntuple.eps
  void checkAllMenus(unsigned int nMenu = 9, unsigned int nLevels = 3, double lum = 0, double crosssec = 0);
  void print (double lum, double crosssec, TString target, bool file = true);
  void print (double lum, double crosssec, bool file = true);
  void reset();         
  void update( TString filename, double lum = 0.0, double crosssec = 0.0 );
  void printCSC(TPostScript & ps, double lum, double crossec, vector<double> &counter );
  //void print(const char * name);
  //void save();
  //void save(const char * name);             //saves under trigtau.ntuple.eps
  //void compare(const char * reference, const char * saveName);
  //void compare(const char * reference);     //saves under ntuple.eps
  //void reset();
  
  void setFile (TString filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setType ( int newValue) { type = newValue; };
  void setSignal (bool newValue) { signal = newValue; signalSet = true; };
  void setNprong (int newValue) { nprong = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta (float  newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setDetailedPrinting( bool newValue) { doDetailedPrinting = newValue;};
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };

  bool getDebug () { return debug; };
  int getType () { return type; };
  bool getSignal () {return signal; };
  int getNprong () { return nprong; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getDetailedPrinting() { return doDetailedPrinting; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;}
  ofstream outfile;

};

#endif
