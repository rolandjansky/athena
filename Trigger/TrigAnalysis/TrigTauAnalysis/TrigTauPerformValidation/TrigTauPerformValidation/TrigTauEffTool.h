/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUEFFTOOL_H
#define _TRIGTAUEFFTOOL_H

#include <map>
#include <list>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
#include "TrigTauPerformValidation/TrigTauCounter.h"

// Defined below
class TrigTauEffCuts;

class TrigTauEffTool {

public:
  TrigTauEffTool();
  virtual ~TrigTauEffTool();

  // Functions to define selectors
  void setDefaultVersion(TrigTauSelectionManager::MENUVERSION version);
  void defineSelector(TrigTauFlags::TRIGGERMENU menu);
  void defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu);
  void defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu,
		      TrigTauSelectionManager::MENUVERSION version);

  // Return pointer to selector by tag (useful for making changes)
  TrigTauSelectionManager* getSelector(TString tag);

  // Return pointer to fiducial cuts by tag
  TrigTauEffCuts* getCuts(TString tag);
  void setPtCut(TString tag, double visPt); // Pt in GeV

  // Functions to define and fill data samples
  // Samples now defined automatically when filled
  void defineSample(TString tag);

  // One stop shopping
  void fillSample(TString tag, TString filename);

  // Lower level routines
  void setCurrentSample(TString tag) {_currentSample = tag;}
  void fillSample(TTree* tree);

  // Counter access
  TrigTauCounter* getCounter(TString selectorTag, TString sampleTag);

  // Dump all counters
  void print();

  // Print total rate vs. selector table
  void printSelector(TString selectorTag, bool latexFormat=false);
  void printCutFlow(TString selectorTag);
  void printSummary();
  void printL1Summary();

  // Define offline reco match or not (set true by default)
  void setRecoMatch(bool match=true) {_recoMatch = match;}

 private:
  bool _debug;
  bool _recoMatch;

  TString _currentSample;

  TrigTauSelectionManager::MENUVERSION _defVersion;   // Default version

  TrigTauBugFix _fix;

  // TString in both cases indexes the selector type
  map<TString, TrigTauSelectionManager*> _selectors;
  map<TString, TrigTauCounter*> _counters;
  map<TString, TrigTauEffCuts*> _cuts;

  list<TString> _samples;

  // Utility to set up new counter
  TrigTauCounter* makeNewCounter();

  // Shortcut for printing one line of rate table
  void printRateLine(TrigTauCounter* c, TString& label, bool latexFormat);
  void printCutFlowLine(TString seltag, unsigned int c2, 
			unsigned int c1, unsigned int c0);

};

class TrigTauEffCuts {

public:
  TrigTauEffCuts();
  virtual ~TrigTauEffCuts();

  void setFiducialCuts(TrigTauMCTauList& truthList);
  double maxVisEta;
  double minVisPt;
  TrigTauMCTau::DECAY decayMode;
};

#endif
