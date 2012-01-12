/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2BugFix.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual L1/L2/EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauL2BugFix.h"

#include "TrigTauPerformNtuple/TrigTauEvent.h"

#include <iostream>
using namespace std;

ClassImp(TrigTauL2BugFix) 

//
// -----------------------------------------------------------------------
//

TrigTauL2BugFix::TrigTauL2BugFix() {
}

TrigTauL2BugFix::~TrigTauL2BugFix() {
}

void
TrigTauL2BugFix::fix(TrigTauEvent* event) {

  if (event == 0) {
    cout << "TrigTauL2BugFix::analyze - event pointer is NULL!" << endl;
    return;
  }

  // Loop over the L2 objects and replace eta/phi with calo eta/phi
  for(std::vector<TrigTauL2Object*>::iterator
	l2 = event->tauL2.begin(); l2 != event->tauL2.end(); ++l2) {

    (*l2)->SetPtEtaPhiM((*l2)->Pt(), (*l2)->m_caloEta, (*l2)->m_caloPhi, (*l2)->M());

  }

}
