/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUANALYZER_H
#define _TRIGTAUANALYZER_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauAnalyzer.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Utility class to collect all the analysis functions into one
///           place.  Makes the ROOT interface easier.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauEvent;
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"

class TrigTauAnalyzer : public TObject {

 public:
  TrigTauAnalyzer();
  virtual ~TrigTauAnalyzer();

  void analyze(const TrigTauEvent* event, bool signal=true) ;
  void analyze(const TrigTauEvent* event, bool signal, 
	       TrigTauMCTauList::OFFLINESELECTION offline) ;

  TrigTauMCTauList        truthList;
  TrigTauTrigChainList    chainList;
  TrigTauSelectionManager trigMan;

  void setDebug(bool debug=true) {_debug = debug;}
  void printSettings() const;

private:

  bool _debug;
  ClassDef(TrigTauAnalyzer, 1) ;

};

#endif
