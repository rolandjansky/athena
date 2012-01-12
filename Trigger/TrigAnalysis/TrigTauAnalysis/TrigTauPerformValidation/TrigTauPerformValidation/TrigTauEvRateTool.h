/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUEVRATETOOL_H
#define _TRIGTAUEVRATETOOL_H

//
// More simplistic version of TrigTauRateTool
// This applies no selection, but just checks the
// event-level decision bits as produced when the 
// TTP ntuple was generated.
//

#include <map>
#include <list>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformValidation/TrigTauCounter.h"

class TrigTauEvRateTool {

 public:
  TrigTauEvRateTool();
  virtual ~TrigTauEvRateTool();

  // Functions to define selectors
  void defineSelector(TrigTauFlags::TRIGGERMENU menu);
  void defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu);

  // Utility to do all samples
  void defineAllSelectors();

  // Functions to define and fill data samples
  // Samples now defined automatically when filled
  void defineSample(TString tag);

  // One stop shopping
  void fillSample(TString tag, TString filename, double weight);

  // Lower level routines
  void setCurrentSample(TString tag) {_currentSample = tag;}
  void setCurrentWeight(double weight) {_currentWeight = weight;}
  void fillSample(TTree* tree);

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

 private:
  bool _debug;

  TString _currentSample;
  double  _currentWeight;

  // TString in both cases indexes the selector type
  std::map<TString, TrigTauFlags::TRIGGERMENU> _selectors;
  std::map<TString, TrigTauCounter*> _counters;
  std::list<TString> _samples;

  // Utility to set up new counter
  TrigTauCounter* makeNewCounter();

  // Shortcut for printing one line of rate table
  void printRateLine(TrigTauCounter* c, TString& label, bool latexFormat);
};

#endif
