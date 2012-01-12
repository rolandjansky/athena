/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALTRIGGERBITCHECKER_H_
#define _TTPVALTRIGGERBITCHECKER_H_

// STL headers
#include <map>
#include <list>
#include <string>

// Root headers
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

// TTP headers
#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformValidation/TrigTauCounter.h"

class TTPValTriggerBitChecker {

 public:

  TTPValTriggerBitChecker();
  virtual ~TTPValTriggerBitChecker() {}

  // Functions to define selectors
  void setMenuVersion(TrigTauSelectionManager::MENUVERSION version);
  void setTauMenu(TrigTauFlags::TRIGGERMENU menu);

  // Access functions for Athena use
  void initialize(TrigTauFlags::TRIGGERMENU menu);
  void execute(const TrigTauEvent* event);
  void finalize();

  // Run function for standalone use
  void processFile(const char* filename, TrigTauFlags::TRIGGERMENU menu);
  void processFile(TTree* tree,          TrigTauFlags::TRIGGERMENU menu);


 private:
  long m_nevents;

  TrigTauFlags::TRIGGERMENU m_menu;

  TrigTauAnalyzer m_analyzer;

  TrigTauCounter m_L1Cnt;
  TrigTauCounter m_L2Cnt;
  TrigTauCounter m_EFCnt;

};

#endif
