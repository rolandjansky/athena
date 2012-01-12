/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"

#include "TrigTauPerformValidation/TrigTauRateTool.h"

TrigTauRateTool::TrigTauRateTool() {
  _debug = false;

  _currentSample = "";
  _currentWeight = 0.;

  _defVersion = TrigTauSelectionManager::MENU13030pre;

  _selectors.clear();
  _counters.clear();

  _samples.clear();

}

TrigTauRateTool::~TrigTauRateTool() {
  for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
       imap != _selectors.end(); imap++) {
    delete imap->second;
  }

  for (map<TString, TrigTauCounter*>::iterator imap = _counters.begin();
       imap != _counters.end(); imap++) {
    delete imap->second;
  }
}

void 
TrigTauRateTool::setDefaultVersion(TrigTauSelectionManager::MENUVERSION version) {
  _defVersion = version;
}

void
TrigTauRateTool::defineSelector(TrigTauFlags::TRIGGERMENU menu) {
  TrigTauFlags f;
  defineSelector(f.menuName(menu), menu, _defVersion);
}

void
TrigTauRateTool::defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu) {
  defineSelector(tag, menu, _defVersion);
}

void
TrigTauRateTool::defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu,
			    TrigTauSelectionManager::MENUVERSION version) {

  if (_selectors[tag] != NULL) {
    cerr << "TrigTauRateTool::defineSelector - Selector with tag " << tag <<
      " already defined!" << endl;
    return;
  }

  TrigTauSelectionManager* sel = new TrigTauSelectionManager();
  sel->setMenuVersion(version);
  sel->setTauMenu(menu);
  _selectors[tag] = sel;

  return;
}

TrigTauSelectionManager*
TrigTauRateTool::getSelector(TString tag) {
  if (_selectors[tag] == NULL)
    cerr << "TrigTauRateTool::getSelector - No selector with tag " << tag << endl;
  return _selectors[tag];
}

TrigTauCounter*
TrigTauRateTool::getCounter(TString selectorTag, TString sampleTag) {
  TString tag = selectorTag + TString("_")+sampleTag;
  if (_counters[tag] == NULL) {
    if (_debug)
      cout << "TrigTauRateTool::getCounter - No counter with tag " << tag << endl;
    _counters[tag] = makeNewCounter();

    // Check if the sample tag exists, if not add it here
    if (find(_samples.begin(), _samples.end(), sampleTag) == _samples.end())
      defineSample(sampleTag);

  }

  return _counters[tag];
}

void
TrigTauRateTool::clearSumCounter(TString sumTag) {

  // Loop over all defined selectors as we need a summary counter for each
  for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
       imap != _selectors.end(); imap++) {

    TString selectorTag = imap->first;

    // Asking for counter will automatically create everything we need
    TrigTauCounter* sum = getSumCounter(selectorTag, sumTag);

    // Check we have something
    sum->print();
  }
}

// Add counts from given sampleTag to the summary counter
void
TrigTauRateTool::addSumSample(TString sampleTag, TString sumTag) {

  // Loop over all defined selectors as we need a summary counter for each
  for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
       imap != _selectors.end(); imap++) {

    TString selectorTag = imap->first;

    // Asking for counter will automatically create everything we need
    // in case it doesn't already exist
    TrigTauCounter* sum = getSumCounter(selectorTag, sumTag);
    if (_debug) {
      cout << "Retrieved sum counter " << selectorTag << "_" << sumTag<< endl;
      sum->print();
    }

    // Get the sample counter
    TrigTauCounter* samp = getCounter(selectorTag, sampleTag);
    if (_debug) {
      cout << "Retrieved samp counter " << selectorTag << "_" << sampleTag << endl;
      samp->print();
    }

    // Add contents
    sum->add(*samp);
  }
}

TrigTauCounter*
TrigTauRateTool::getSumCounter(TString selectorTag, TString sumTag) {
  return getCounter(selectorTag, sumTag);
}

void
TrigTauRateTool::defineSample(TString tag) {
  _samples.push_back(tag);
}

void
TrigTauRateTool::fillSample(TString tag, TString filename, double wt) {

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

  fix.setTTPVersion(filename);

  setCurrentSample(tag);
  setCurrentWeight(wt);
  fillSample(tree);

  delete tree;
  delete file;
}

void
TrigTauRateTool::fillSample(TTree *tree) {

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 0);

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // Bug fix
    fix.fix(event);

    TrigTauTrigChainList chainList;
    chainList.setRoiWordMatching();
    chainList.fillChainList(event);

    // Loop over defined selectors
    for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
	 imap != _selectors.end(); imap++) {
      TString seltag = imap->first;
      TrigTauSelectionManager* selman = imap->second;
      
      // Fill trigger chain list for this selection manager
      chainList.fillTriggerList(*selman);

      // Get counters
      TrigTauCounter* bgd = getCounter(seltag, _currentSample);

      bgd->fill(0, _currentWeight);

      unsigned int numL1 = chainList.nPassL1Chains();
      if (numL1 == 0) continue;
      bgd->fill(1, _currentWeight);
      
      unsigned int numL2Cal = chainList.nPassL2CalChains();
      if (numL2Cal == 0) continue;
      bgd->fill(2, _currentWeight);
      
      unsigned int numL2 = chainList.nPassL2Chains();
      if (numL2 == 0) continue;
      bgd->fill(3, _currentWeight);

      unsigned int numEF = chainList.nPassEFChains();
      if (numEF == 0) continue;
      bgd->fill(4, _currentWeight);

    }  // end loop over selectors
  }  // end loop over events

  delete event;
}

TrigTauCounter*
TrigTauRateTool::makeNewCounter() {
  TrigTauCounter* count = new TrigTauCounter();
  count->reset();
  count->name(0, string("L1 In "));
  count->name(1, string("L1 Out"));
  count->name(2, string("L2Cal Out"));
  count->name(3, string("L2Trk Out"));
  count->name(4, string("EF Out"));
  return count;
}

void
TrigTauRateTool::print() {

  for (map<TString, TrigTauCounter*>::iterator imap = _counters.begin();
       imap != _counters.end(); imap++) {
    TrigTauCounter* c = imap->second;
    cout << imap->first << endl;
    c->print();
  }

}

void
TrigTauRateTool::printRateSum(TString sumTag, bool latex) {

  if (latex) cout << "\\hline" << endl;

  // Loop over defined selectors
  map<TString, TrigTauSelectionManager*>::iterator imap;
  for (imap = _selectors.begin(); imap != _selectors.end(); imap++) {
    TString seltag = imap->first;
    TrigTauCounter* c = getSumCounter(seltag, sumTag);
    // Make sure we got something
    if (c == 0) {
      cout << "Sum counter for selector " << seltag << " is NULL!" << endl;
      continue;
    }

    // Now print
    printRateLine(c, seltag, latex);

  }

  if (latex) cout << "\\hline" << endl;
}

void 
TrigTauRateTool::printSelector(TString seltag, bool latex) {

  if (latex) cout << "\\hline" << endl;
  if (latex) {
    cout << "\\multicolumn{4}{c}{{\\tt " << seltag << " }} \\\\" << endl;
  } else {
    cout << seltag << endl;
  }
  if (latex) cout << "\\hline" << endl;

  TrigTauCounter* c;

  // Sort sample list (to put Sum at the end)
  // No longer necessary...
  //  _samples.sort();

  // Loop over defined samples (includes summary) and print out what we have
  list<TString>::iterator ilist;
  for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
    c = getCounter(seltag, *ilist);

    // Make sure we got something
    if (c == 0) {
      cout << "Counter for selector " << seltag << " and sample " << *ilist 
	   << " is NULL!" << endl;
      continue;
    }

    // Now print
    printRateLine(c, *ilist, latex);
  }

  if (latex) cout << "\\hline" << endl;

}

void
TrigTauRateTool::printRateLine(TrigTauCounter* c, TString& label, bool latex) {

   // Make sure we got something
    if (c == 0) {
      cout << "TrigTauRateTool::printRateLine - TrigTauCounter pointer is NULL!" << endl;
      return;
    }

    // Now print
    TString space;
    if (latex) space = " & ";
    else space = " ";

    cout << fixed;
    if (latex) {
      cout << "{\\tt " << label << " }" << space;
    } else {
      cout << label << space;
    }

    cout << setprecision(0);
    cout << c->getWt(1) << space;
    cout << setprecision(1);
    cout << c->getWt(2) << space;
    cout << c->getWt(3) << space;
    cout << setprecision(2);
    if (latex) {
      cout << "$" << c->getWt(4) << "\\pm " << c->getDWt(4) << "$";
    } else {
      cout << c->getWt(4) << " (" << c->getDWt(4) << ") ";
    }

    if (latex) cout << " \\\\";
    cout << endl;
 
}
