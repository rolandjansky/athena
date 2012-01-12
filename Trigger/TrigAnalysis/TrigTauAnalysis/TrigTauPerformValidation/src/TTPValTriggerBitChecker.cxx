/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigTauPerformValidation/TTPValTriggerBitChecker.h"

using namespace std;

TTPValTriggerBitChecker::TTPValTriggerBitChecker() {
}

void
TTPValTriggerBitChecker::setMenuVersion(TrigTauSelectionManager::MENUVERSION version) {
  m_analyzer.trigMan.setMenuVersion(version);
}

void
TTPValTriggerBitChecker::setTauMenu(TrigTauFlags::TRIGGERMENU menu) {
  m_menu = menu;
  m_analyzer.trigMan.setTauMenu(menu);
}

void
TTPValTriggerBitChecker::initialize(TrigTauFlags::TRIGGERMENU menu) {

  setTauMenu(menu);

  m_nevents = 0;

  m_L1Cnt.reset();
  m_L2Cnt.reset();
  m_EFCnt.reset();

  m_L1Cnt.name(1, "L1 match");
  m_L1Cnt.name(2, "L1 select");
  m_L1Cnt.name(3, "L1 bits");

  m_L2Cnt.name(1, "L2 match");
  m_L2Cnt.name(2, "L2 select");
  m_L2Cnt.name(3, "L2 bits");

  m_EFCnt.name(1, "EF match");
  m_EFCnt.name(2, "EF select");
  m_EFCnt.name(3, "EF bits");

}

void
TTPValTriggerBitChecker::execute(const TrigTauEvent* event) {

  m_nevents++;

  // Analyze this event (false to not fill MC info)
  m_analyzer.analyze(event, false);

  long nL1 = m_analyzer.chainList.nPassL1Chains();
  long nL2 = m_analyzer.chainList.nPassL2Chains();
  long nEF = m_analyzer.chainList.nPassEFChains();

  if ( (nL1 > 0) && event->decision.passedL1(m_menu) ) {
    m_L1Cnt.fill(1);
  } else if (nL1 > 0) {
    m_L1Cnt.fill(2);
  } else if (event->decision.passedL1(m_menu)) {
    m_L1Cnt.fill(3);
  }

  if ( (nL2 > 0) && event->decision.passedL2(m_menu) ) {
    m_L2Cnt.fill(1);
  } else if (nL2 > 0) {
    m_L2Cnt.fill(2);
  } else if (event->decision.passedL2(m_menu)) {
    m_L2Cnt.fill(3);
  }

  if ( (nEF > 0) && event->decision.passedEF(m_menu) ) {
    m_EFCnt.fill(1);
  } else if (nEF > 0) {
    m_EFCnt.fill(2);
  } else if (event->decision.passedEF(m_menu)) {
    m_EFCnt.fill(3);
  }

}

void
TTPValTriggerBitChecker::finalize() {

  cout << "TTPValTriggerBitChecker - Checking menu " 
       << TrigTauFlags::menuName(m_menu) << endl;

  cout << "   Events seen: " << m_nevents << endl;

  cout << "   L1Sel match: " << long(m_L1Cnt.getN(1))
       << " TTP only: " << long(m_L1Cnt.getN(2))
       << " EvBits only: " << long(m_L1Cnt.getN(3)) << endl;

  cout << "   L2Sel match: " << long(m_L2Cnt.getN(1) )
       << " TTP only: " << long(m_L2Cnt.getN(2))
       << " EvBits only: " << long(m_L2Cnt.getN(3)) << endl;

  cout << "   EFSel match: " << long(m_EFCnt.getN(1) )
       << " TTP only: " << long(m_EFCnt.getN(2))
       << " EvBits only: " << long(m_EFCnt.getN(3)) << endl;
}

void
TTPValTriggerBitChecker::processFile(const char* filename, 
				     TrigTauFlags::TRIGGERMENU menu) {

  // Open file
  TFile* file = new TFile(filename);
  if (!file->IsOpen()) {
    cerr << "Can't open file " << filename << endl;
    return;
  }

  // Initialize tree
  TTree* tree = (TTree*)file->Get("eventTree");
  if (tree == 0) {
    cerr << "Can't find eventTree in " << filename << endl;
    return;
  }

  processFile(tree, menu);

  delete tree;
  delete file;
}

void
TTPValTriggerBitChecker::processFile(TTree* tree, 
				     TrigTauFlags::TRIGGERMENU menu) {

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 0);

  initialize(menu);
  long nev = tree->GetEntries();
  for (long iev = 0; iev < nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    execute(event);
  }

  finalize();

  delete event;
}

