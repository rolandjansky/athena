/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL1BugFix.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual L1/L1/EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauL1BugFix.h"

#include "TrigTauPerformNtuple/TrigTauEvent.h"

#include <iostream>
using namespace std;

ClassImp(TrigTauL1BugFix)  

//
// -----------------------------------------------------------------------
//

TrigTauL1BugFix::TrigTauL1BugFix() {
}

TrigTauL1BugFix::~TrigTauL1BugFix() {
}

void
TrigTauL1BugFix::fix(TrigTauEvent* event) {

  if (event == 0) {
    cout << "TrigTauL1BugFix::analyze - event pointer is NULL!" << endl;
    return;
  }

  // Loop over the L1 objects and replace eta/phi with calo eta/phi
  for(std::vector<TrigTauL1Object*>::iterator
	l2 = event->tauL1.begin(); l2 != event->tauL1.end(); ++l2) {
    
    if( (*l2)->m_TauEnergy > 3000. ) continue; // this was corrected
    (*l2)->m_TauEnergy *= 1000.;
    (*l2)->m_EmIsol *= 1000.;
    (*l2)->m_HdIsol *= 1000.;
    (*l2)->m_EmCore *= 1000.;
    (*l2)->m_HdCore *= 1000.;

  }

}
