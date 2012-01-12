/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUDOUBLETRIGGER_H
#define _TRIGTAUDOUBLETRIGGER_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauDoubleTrigger.h
/// Package : TrigTauPerformAnalysis
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2007
///
/// DESCRIPTION:
///           Utility class to run double trigger, e.g. tau+lept
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauEvent;
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"

class TrigTauDoubleTrigger : public TObject {

 public:
  TrigTauDoubleTrigger(TrigTauFlags::TRIGGERMENU item1, TrigTauFlags::TRIGGERMENU item2);
  virtual ~TrigTauDoubleTrigger();

  void pass(const TrigTauEvent* event,
	    bool & passL1, bool &  passL2, bool & passEF);

private:
  void passNoTauSignature(const TrigTauEvent* event, 
			  bool & passL1, bool &  passL2, bool & passEF);


  bool                      menuWithTau;
  bool                      bothMenuWithTau;
  bool                      menuWithMET;
  TrigTauFlags::TRIGGERMENU menu1;
  TrigTauFlags::TRIGGERMENU menu2;
  
  TrigTauAnalyzer analyzer;
  TrigTauSelectionManager trigMan1;
  TrigTauSelectionManager trigMan2;

  ClassDef(TrigTauDoubleTrigger, 1) ;

};

#endif
