/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAURATETOOL_H
#define _TRIGTAURATETOOL_H

#include <map>
#include <list>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"

#include "TrigTauPerformValidation/TrigTauCounter.h"

class TrigTauRateTool {

 public:
  TrigTauRateTool();
  virtual ~TrigTauRateTool();

  // Functions to define selectors
  void setDefaultVersion(TrigTauSelectionManager::MENUVERSION version);
  void defineSelector(TrigTauFlags::TRIGGERMENU menu);
  void defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu);
  void defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu,
		      TrigTauSelectionManager::MENUVERSION version);

  // Return pointer to selector by tag (useful for making changes)
  TrigTauSelectionManager* getSelector(TString tag);

  // Functions to define and fill data samples
  // Samples now defined automatically when filled
  void defineSample(TString tag);

  // One stop shopping
  void fillSample(TString tag, TString filename, double weight);

  // Lower level routines
  void setCurrentSample(TString tag) {_currentSample = tag;}
  void setCurrentWeight(double weight) {_currentWeight = weight;}
  void fillSample(TTree* tree);

  //  void fillStdBgd();   // Shortcut to do this using JetManager.h

  // Counter access
  TrigTauCounter* getCounter(TString selectorTag, TString sampleTag);

  // Function to define summary counters (one will be made for each selector)
  // These should be done after filling
  void clearSumCounter(TString sumTag);  // Also creates if needed
  void addSumSample(TString sampleTag, TString sumTag);
  TrigTauCounter* getSumCounter(TString selectorTag, TString sumTag);

  // Dump all counters
  void print();

  // Print total rate vs. selector table
  void printRateSum(TString sumTag, bool latexFormat=false);
  void printSelector(TString selectorTag, bool latexFormat=false);

  // Used to deal with problems (most notably RDO L1 values)
  // Must set this up before use
  TrigTauBugFix fix;

 private:
  bool _debug;

  TString _currentSample;
  double  _currentWeight;

  TrigTauSelectionManager::MENUVERSION _defVersion;   // Default version

  // TString in both cases indexes the selector type
  std::map<TString, TrigTauSelectionManager*> _selectors;
  std::map<TString, TrigTauCounter*> _counters;
  std::list<TString> _samples;

  // Utility to set up new counter
  TrigTauCounter* makeNewCounter();

  // Shortcut for printing one line of rate table
  void printRateLine(TrigTauCounter* c, TString& label, bool latexFormat);
};

#endif
