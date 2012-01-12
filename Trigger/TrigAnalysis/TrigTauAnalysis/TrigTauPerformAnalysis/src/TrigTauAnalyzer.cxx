/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauAnalyzer.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual L1/L2/EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"

#include "TrigTauPerformNtuple/TrigTauEvent.h"

#include <iostream>
using namespace std;

ClassImp(TrigTauAnalyzer)  

//
// -----------------------------------------------------------------------
//

TrigTauAnalyzer::TrigTauAnalyzer() : _debug(false) {
}

TrigTauAnalyzer::~TrigTauAnalyzer() {
}

void
TrigTauAnalyzer::analyze(const TrigTauEvent* event, bool signal, 
			 TrigTauMCTauList::OFFLINESELECTION offline)  {
  truthList.setRecoSelection(offline);
  analyze(event,signal);
}

void
TrigTauAnalyzer::analyze(const TrigTauEvent* event, bool signal)  {

  if (event == 0) {
    cout << "TrigTauAnalyzer::analyze - event pointer is NULL!" << endl;
    return;
  }

  if (signal) {

    // Fill the MC truth list
    if (_debug) cout << "Fill truth pure list" << endl;
    truthList.fillPureList(&event->mcEvent);
    if (_debug) cout << "Fill truth good list" << endl;
    truthList.fillGoodList();
    if (_debug) cout << "Apply offline match" << endl;
    truthList.requireRecoMatch(*event);
  }

  // Fill trigger chains
  if (_debug) cout << "Fill chain list" << endl;
  chainList.fillChainList(event);
  if (_debug) cout << "Fill trigger list" << endl;
  chainList.fillTriggerList(trigMan);

}

void TrigTauAnalyzer::printSettings() const
{
  
  truthList.printSettings();
  trigMan.printSettings();
}
