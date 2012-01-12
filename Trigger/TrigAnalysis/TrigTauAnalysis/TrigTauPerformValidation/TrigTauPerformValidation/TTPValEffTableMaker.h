/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALEFFTABLEMAKER_H_
#define _TTPVALEFFTABLEMAKER_H_

#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TPostScript.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"


class TTPValEffTableMaker {

private:
 
  TString filename;
  bool filenameSet;
  TrigTauSelectionManager selmanager;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;
  TrigTauSelectionManager::CUTLEVEL cutLevel;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  bool signal, debug;
  TChain * tree;
  float maxVisEta, minLeadingPt;
  int nev;
  TrigTauBugFix bugfix;
  
  // TAU1P3P, TAUREC, EITHERTAU, BOTHTAU //, NONE
  TrigTauMCTauList::OFFLINESELECTION offlineType[11];

  static const unsigned int nMatching = 11;  // nMatching: TAU1P3P, TAUREC, EITHERTAU, BOTHTAU, NONE
  static const unsigned int nProngType = 3; // nProngType: 1Pr, 3Pr, all
  
//counter with index: [matching][prong-type][passed l1, l2Calo, L2trk, EF ,tot]
  unsigned int counter[nMatching][nProngType][5];
  TrigTauAnalyzer analyzer[nMatching][nProngType];
 
  void initialize (bool debug1 = false);
  bool analyzeEvent (TrigTauEvent * event, TrigTauMCTauList::OFFLINESELECTION offlineType, int i, int j);

public:

  TTPValEffTableMaker( TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);
  TTPValEffTableMaker( TString filename1, TrigTauBugFix::POOLFILE poolfile = TrigTauBugFix::RDO, TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);
 
  void analyze();
  void print( int which = 1 );  //0 prints absolute efficiencies, 1 prints relative efficiencies
  void printCSC (TPostScript & ps, bool relEff = true);
  void update ( TString filename, bool relEff = true);

  void setFile ( TString filename1 );
  void setChain ( TChain * tree1);
  void setSignal ( bool signal1 ) { signal = signal1; };
  void setDebug ( bool debug1 ) { debug = debug1; };
  void setMaxVisEta (float eta ) { maxVisEta = eta; }
  void setMinLeadingPt (float pt ) { minLeadingPt = pt; };
  void setMenu ( TrigTauFlags::TRIGGERMENU newValue ) {menu = newValue; };
  void setCutLevel (TrigTauSelectionManager::CUTLEVEL newValue ) {cutLevel = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };

  TString getFile() { return filename; };
  TChain * getChain() { return tree; };
  bool getSignal() { return signal; };
  bool getDebug() { return debug; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauFlags::TRIGGERMENU getMenu() { return menu; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel() { return cutLevel; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};
};


#endif
