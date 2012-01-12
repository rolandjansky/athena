/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"
#include "TrigTauPerformValidation/TrigTauEffTool.h"

TrigTauEffTool::TrigTauEffTool() {
  _debug = true;
  _recoMatch = true;

  _currentSample = "";

  _defVersion = TrigTauSelectionManager::MENU13030pre;

  _selectors.clear();
  _counters.clear();
  _cuts.clear();

  _samples.clear();

}

TrigTauEffTool::~TrigTauEffTool() {
  for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
       imap != _selectors.end(); imap++) {
    delete imap->second;
  }

  for (map<TString, TrigTauCounter*>::iterator imap = _counters.begin();
       imap != _counters.end(); imap++) {
    delete imap->second;
  }

  for (map<TString, TrigTauEffCuts*>::iterator imap = _cuts.begin();
       imap != _cuts.end(); imap++) {
    delete imap->second;
  }
}

void 
TrigTauEffTool::setDefaultVersion(TrigTauSelectionManager::MENUVERSION version) {
  _defVersion = version;
}

void
TrigTauEffTool::defineSelector(TrigTauFlags::TRIGGERMENU menu) {
  TrigTauFlags f;
  defineSelector(f.menuName(menu), menu, _defVersion);
}

void
TrigTauEffTool::defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu) {
  defineSelector(tag, menu, _defVersion);
}

void
TrigTauEffTool::defineSelector(TString tag, TrigTauFlags::TRIGGERMENU menu,
			    TrigTauSelectionManager::MENUVERSION version) {

  if (_selectors[tag] != NULL) {
    cerr << "TrigTauEffTool::defineSelector - Selector with tag " << tag <<
      " already defined!" << endl;
    return;
  }

  TrigTauSelectionManager* sel = new TrigTauSelectionManager();
  sel->setMenuVersion(version);
  sel->setTauMenu(menu);
  // Enable EF debug (temporary)
  //  sel->efSel.setDebug();

  _selectors[tag] = sel;

  // Also define a cuts object for this selector
  TrigTauEffCuts* cut = new TrigTauEffCuts();
  cut->minVisPt = TrigTauMCTauList::nominalMinVisPt(menu);

  _cuts[tag] = cut;

  return;
}

void
TrigTauEffTool::setPtCut(TString tag, double visPt) {
  TrigTauEffCuts* cut = getCuts(tag);
  if (cut != 0) cut->minVisPt = visPt;
}

TrigTauSelectionManager*
TrigTauEffTool::getSelector(TString tag) {
  if (_selectors[tag] == NULL)
    cerr << "TrigTauEffTool::getSelector - No selector with tag " << tag << endl;
  return _selectors[tag];
}

TrigTauEffCuts*
TrigTauEffTool::getCuts(TString tag) {
  if (_cuts[tag] == NULL)
    cerr << "TrigTauEffTool::getCuts - No cuts with tag " << tag << endl;
  return _cuts[tag];
}

TrigTauCounter*
TrigTauEffTool::getCounter(TString selectorTag, TString sampleTag) {
  TString tag = selectorTag + TString("_")+sampleTag;
  if (_counters[tag] == NULL) {
    if (_debug)
      cout << "TrigTauEffTool::getCounter - No counter with tag " << tag 
	   << " making new" << endl;
    _counters[tag] = makeNewCounter();

    // Check if the sample tag exists, if not add it here
    if (find(_samples.begin(), _samples.end(), sampleTag) == _samples.end())
      defineSample(sampleTag);

  }

  return _counters[tag];
}

void
TrigTauEffTool::defineSample(TString tag) {
  _samples.push_back(tag);
}

void
TrigTauEffTool::fillSample(TString tag, TString filename) {

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

  _fix.setTTPVersion(filename);
  setCurrentSample(tag);
  fillSample(tree);

  delete tree;
  delete file;
}

void
TrigTauEffTool::fillSample(TTree *tree) {

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // Bugfix
    _fix.fix(event);

    // Loop over defined selectors
    for (map<TString, TrigTauSelectionManager*>::iterator imap = _selectors.begin();
	 imap != _selectors.end(); imap++) {

      TString seltag = imap->first;

      // Must apply new fiducial cuts here for different selector tags
      // Get fiducial cuts here
      TrigTauEffCuts* cuts = getCuts(seltag);
      TrigTauMCTauList truthList;
      cuts->setFiducialCuts(truthList);

      truthList.fillPureList(&event->mcEvent);
      truthList.fillGoodList();
      if (_recoMatch)
	truthList.requireRecoMatch(*event);

      // Apply the trigger selection and make chains
      TrigTauTrigChainList chainList;
      chainList.setRoiWordMatching();
      chainList.fillChainList(event);

      TrigTauSelectionManager* selman = imap->second;
      
      // Fill trigger chain list for this selection manager
      chainList.fillTriggerList(*selman);

      // Get counters
      TrigTauCounter* eff = getCounter(seltag, _currentSample);

      // Loop over all found good taus
      unsigned int nTau = truthList.nMCTauGood();
      for (unsigned int itau=0; itau<nTau; itau++) {

	const TrigTauMCTau* tau = &truthList.mcTauGoodEntry(itau);

  // All events (denominator)
  eff->fill(0);
  
  const TrigTauTrigChain* chain;
  
  // Need a generic, non-selected ROI chain...
  chain = chainList.getChain(*tau);
  
  if (chain != 0) {

    // Sequential L1 cuts
    unsigned long l1Word = selman->l1Sel.cutWord(*(chain->L1));
    
    eff->fill(10);
    if ((l1Word & L1CUT_TAUCLUS) == 0) {
      eff->fill(11);
      if ((l1Word & L1CUT_EMISOL) == 0) eff->fill(12);
    }
  }

  // Find a matching L1 chain
  chain = chainList.getL1Chain(*tau);
  if (chain != 0) {
    eff->fill(1);
    
    // Sequential L2Cal cuts
    if (chain->L2 != 0) {
      eff->fill(20);
      
      unsigned long l2Word = selman->l2CalSel.cutWord(*(chain->L2));
      
      if ((l2Word & L2CUT_STRWID) == 0) {
	eff->fill(21);
	if ((l2Word & L2CUT_ISOFRAC) == 0) {
	  eff->fill(22);
	  if ((l2Word & L2CUT_EMRAD) == 0) {
	    eff->fill(23);
	    if ((l2Word & L2CUT_ETCALIB) == 0) {
	      eff->fill(24);
	      if ((l2Word & L2CUT_ETNAR) == 0) {
		eff->fill(25);
	      }
	    }
	  }
	}
      }
    }
    
  }

  // Find a matching L2Cal chain
  chain = chainList.getL2CalChain(*tau);
  if (chain != 0) {
    eff->fill(2);
    
    // Sequential L2Trk cuts
    unsigned long l2Word = selman->l2TrkSel.cutWord(*(chain->L2));
    if ((l2Word & L2CUT_LEADPT) == 0) {
      eff->fill(31);
      if ((l2Word & L2CUT_NSLOW) == 0) {
	eff->fill(32);
	if ((l2Word & L2CUT_RISO) == 0) {
	  eff->fill(33);
	  if ((l2Word & L2CUT_CHARGE) == 0) {
	    eff->fill(34);
	    if ((l2Word & L2CUT_NTRK) == 0) {
	      eff->fill(35);
	      if ((l2Word & L2CUT_ETTRKEM) == 0) {
		eff->fill(36);
		if ((l2Word & L2CUT_NMATCHED) == 0) {
		  eff->fill(37);
		}
	      }
	    }
	  }
	}
      }
    }
    
  }
  
  // Find a matching L2Cal + L2Trk chain
  chain = chainList.getL2Chain(*tau);
  if (chain != 0) {
    eff->fill(3);
    
    // Sequential EF cuts
    if (chain->EF != 0) {
      eff->fill(40);
      
      unsigned long efWord = selman->efSel.cutWord(*(chain->EF));
      
      if ((efWord & EFCUT_EMRAD) == 0) {
	eff->fill(41);
	if ((efWord & EFCUT_ISOFRAC) == 0) {
	  eff->fill(42);
	  if ((efWord & EFCUT_NTRKMIN) == 0 && (efWord & EFCUT_NTRKMAX) == 0) {
	    eff->fill(43);
	    if ((efWord & EFCUT_LEADPT) == 0) {
	      eff->fill(44);
	      if ((efWord & EFCUT_MASS) == 0) {
		eff->fill(45);
		if ((efWord & EFCUT_ETCORR) == 0) {
		  eff->fill(46); 
		  if ((efWord & EFCUT_EMET) == 0) {
		    eff->fill(47);
		  }
		}
	      }
	    }
	  }
	}
      }
    }    
  }
  
  // Find a matching EF chain
  chain = chainList.getEFChain(*tau);
  if (chain != 0) eff->fill(4);

      }  // end loop over taus

    }  // end loop over selectors
  }  // end loop over events

  delete event;
}

TrigTauCounter*
TrigTauEffTool::makeNewCounter() {
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
TrigTauEffTool::print() {

  for (map<TString, TrigTauCounter*>::iterator imap = _counters.begin();
       imap != _counters.end(); imap++) {
    TrigTauCounter* c = imap->second;
    cout << imap->first << endl;
    c->print();
  }

}

void 
TrigTauEffTool::printSelector(TString seltag, bool latex) {

  if (latex) cout << "\\hline" << endl;
  if (latex) {
    cout << "\\multicolumn{4}{c}{{\\tt " << seltag << " }} \\\\" << endl;
  } else {
    cout << seltag << endl;
  }
  if (latex) cout << "\\hline" << endl;

  TrigTauCounter* c;

  // Sort sample list (to put Sum at the end)
  //  _samples.sort();

  // Loop over defined samples and print out what we have
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

// Print samples (columns) vs. selector (rows)
void 
TrigTauEffTool::printSummary() {

  TrigTauCounter* c;

  cout << "\\hline\\hline" << endl;

  // Print header
  list<TString>::iterator ilist;
  for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
    cout << " & " << *ilist;
  }
  cout << "\\\\" << endl;
  cout << "\\hline" << endl;

  // Loop over all selectors
  map<TString, TrigTauSelectionManager*>::iterator isel;
  for (isel = _selectors.begin(); isel != _selectors.end(); isel++) {

    TString seltag = isel->first;
    cout << "\\sig{" << seltag << "} ";

    for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
      c = getCounter(seltag, *ilist);

      // Make sure we got something
      if (c == 0) {
	cout << "Counter for selector " << seltag << " and sample " << *ilist
	     << " is NULL!" << endl;
	continue;
      }

      cout << setprecision(1);
      cout << " & " << 100*c->getWt(4)/c->getWt(0);
    }
    cout << " \\\\" << endl;
  }
  cout << "\\hline\\hline" << endl;
}

// Print samples (columns) vs. selector (rows)
void 
TrigTauEffTool::printL1Summary() {

  TrigTauCounter* c;

  cout << "\\hline\\hline" << endl;

  // Print header
  list<TString>::iterator ilist;
  for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
    cout << " & " << *ilist;
  }
  cout << "\\\\" << endl;
  cout << "\\hline" << endl;

  // Loop over all selectors
  map<TString, TrigTauSelectionManager*>::iterator isel;
  for (isel = _selectors.begin(); isel != _selectors.end(); isel++) {

    TString seltag = isel->first;
    cout << "\\sig{" << seltag << "} ";

    for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
      c = getCounter(seltag, *ilist);

      // Make sure we got something
      if (c == 0) {
	cout << "Counter for selector " << seltag << " and sample " << *ilist
	     << " is NULL!" << endl;
	continue;
      }

      cout << setprecision(1);
      cout << " & " << 100*c->getWt(1)/c->getWt(0);
    }
    cout << " \\\\" << endl;
  }
  cout << "\\hline\\hline" << endl;
}

// Print samples (columns) vs. individual cuts (rows)
void 
TrigTauEffTool::printCutFlow(TString seltag) {

  cout << "\\hline\\hline" << endl;

  // Print header, listing samples
  list<TString>::iterator ilist;
  for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {
    cout << " & \\multicolumn{3}{c}{" << *ilist << "}";
  }
  cout << "\\\\" << endl;
  cout << "\\hline" << endl;

  // Write out selector tag
  cout << "\\multicolumn{" << 3 * _samples.size()+1 
       << "}{c}{\\sig{ " << seltag << " }} \\\\" << endl;
  cout << "\\hline" << endl;

  switch(_defVersion) {

  case TrigTauSelectionManager::MENU13030pre:
    cout << "L1 ROI     "; printCutFlowLine(seltag, 10,  0, 0);
    cout << "TauCluster "; printCutFlowLine(seltag, 11, 10, 0);
    cout << "EM Isol    "; printCutFlowLine(seltag, 12, 11, 0);
    cout << "\\hline" << endl;
    
    cout << "L2 ROI     "; printCutFlowLine(seltag, 20, 12, 0);
    cout << "L2 Str Wid "; printCutFlowLine(seltag, 21, 20, 0);
    cout << "L2 Iso Frac"; printCutFlowLine(seltag, 22, 21, 0);
    cout << "L2 EM Rad  "; printCutFlowLine(seltag, 23, 22, 0);
    cout << "L2 Et      "; printCutFlowLine(seltag, 24, 23, 0);
    cout << "\\hline" << endl;
    
    cout << "L2 LeadPt  "; printCutFlowLine(seltag, 31, 24, 0);
    cout << "L2 NSlow   "; printCutFlowLine(seltag, 32, 31, 0);
    cout << "L2 RIso    "; printCutFlowLine(seltag, 33, 32, 0);
    cout << "L2 Charge  "; printCutFlowLine(seltag, 34, 33, 0);
    cout << "L2 Ntrk    "; printCutFlowLine(seltag, 35, 34, 0);
    cout << "\\hline" << endl;
    
    cout << "EF ROI     "; printCutFlowLine(seltag, 40, 35, 0);
    cout << "EF EM Rad  "; printCutFlowLine(seltag, 41, 40, 0);
    cout << "EF Iso Frac"; printCutFlowLine(seltag, 42, 41, 0);
    cout << "EF Ntrk    "; printCutFlowLine(seltag, 43, 42, 0);
    cout << "EF LeadPt  "; printCutFlowLine(seltag, 44, 43, 0);
    cout << "EF EtCorr  "; printCutFlowLine(seltag, 46, 44, 0);
    cout << "\\hline\\hline" << endl;
    break;

  case TrigTauSelectionManager::MENU1400:
    cout << "L1 ROI     "; printCutFlowLine(seltag, 10,  0, 0);
    cout << "TauCluster "; printCutFlowLine(seltag, 11, 10, 0);
    cout << "EM Isol    "; printCutFlowLine(seltag, 12, 11, 0);
    cout << "\\hline" << endl;
    
    cout << "L2 ROI     "; printCutFlowLine(seltag, 20, 12, 0);
    cout << "L2 EM Rad  "; printCutFlowLine(seltag, 23, 20, 0);
    cout << "L2 EtNar   "; printCutFlowLine(seltag, 25, 23, 0);
    cout << "\\hline" << endl;
    
    cout << "L2 RIso    "; printCutFlowLine(seltag, 33, 25, 0);
    cout << "L2 Ntrk    "; printCutFlowLine(seltag, 35, 33, 0);
    cout << "L2 EtTrkEM "; printCutFlowLine(seltag, 36, 35, 0);
    cout << "L2 NMatch  "; printCutFlowLine(seltag, 37, 36, 0);
    cout << "\\hline" << endl;
    
    cout << "EF ROI     "; printCutFlowLine(seltag, 40, 37, 0);
    cout << "EF EM Rad  "; printCutFlowLine(seltag, 41, 40, 0);
    cout << "EF Ntrk    "; printCutFlowLine(seltag, 43, 41, 0);
    cout << "EF LeadPt  "; printCutFlowLine(seltag, 44, 43, 0);
    cout << "EF Mass    "; printCutFlowLine(seltag, 45, 44, 0);
    cout << "EF EtCorr  "; printCutFlowLine(seltag, 46, 45, 0);
    cout << "\\hline\\hline" << endl;
    break;

  default:
    cout << "TrigTauEffTool - Selected menu version not supported!" << endl;
  }
}

void
TrigTauEffTool::printCutFlowLine(TString seltag, unsigned int c2, 
			     unsigned int c1, unsigned int c0) {

  // Loop over all samples
  list<TString>::iterator ilist;
  for (ilist = _samples.begin(); ilist != _samples.end(); ilist++) {

    TrigTauCounter* c = getCounter(seltag, *ilist);

    // Make sure we got something
    if (c == 0) {
      cout << "Counter for selector " << seltag << " and sample " << *ilist
	   << " is NULL!" << endl;
      continue;
    }
    
    cout << setprecision(0);
    cout << " & " << c->getWt(c2);   // Raw count
    cout << setprecision(1);
    cout << " & " << 100.*c->getWt(c2)/c->getWt(c0);
    cout << " & " << 100.*c->getWt(c2)/c->getWt(c1);
  }
  cout << " \\\\" << endl;
}

void
TrigTauEffTool::printRateLine(TrigTauCounter* c, TString& label, bool latex) {

  // Make sure we got something
  if (c == 0) {
    cout << "TrigTauEffTool::printRateLine - TrigTauCounter pointer is NULL!" << endl;
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
  
  // Total events
  
  cout << setprecision(0);
  cout << c->getWt(1) << space;
  //    cout << setprecision(1);
  cout << c->getWt(2) << space;
  cout << c->getWt(3) << space;
  //    cout << setprecision(2);
  cout << c->getWt(4) << space;
  
  if (latex) cout << " \\\\";
  cout << endl;
  
  // Total efficiency
  
  cout << setprecision(1);
  cout << space;
  cout << 100.*c->getWt(1)/c->getWt(0) << space; 
  cout << 100.*c->getWt(2)/c->getWt(0) << space; 
  cout << 100.*c->getWt(3)/c->getWt(0) << space; 
  cout << 100.*c->getWt(4)/c->getWt(0) << space; 
  /*
    if (latex) {
    cout << "$" << c->getWt(4) << "\\pm " << c->getDWt(4) << "$";
    } else {
    cout << c->getWt(4) << " (" << c->getDWt(4) << ") ";
    }
  */
  if (latex) cout << " \\\\";
  cout << endl;
  
  // Differential efficiency
  cout << space << space;
  cout << 100*c->getWt(2)/c->getWt(1) << space; 
  cout << 100*c->getWt(3)/c->getWt(2) << space; 
  cout << 100*c->getWt(4)/c->getWt(3) << space; 
  /*
    if (latex) {
    cout << "$" << c->getWt(4) << "\\pm " << c->getDWt(4) << "$";
    } else {
    cout << c->getWt(4) << " (" << c->getDWt(4) << ") ";
    }
  */
  if (latex) cout << " \\\\";
  cout << endl;
}

//-------------------------------------------------------------------

TrigTauEffCuts::TrigTauEffCuts() : 
  maxVisEta(2.5), 
  minVisPt(0.), 
  decayMode(TrigTauMCTau::NONE) {
}

TrigTauEffCuts::~TrigTauEffCuts() {
}

void 
TrigTauEffCuts::setFiducialCuts(TrigTauMCTauList& truthList) {

  truthList.setMaxVisEta(maxVisEta);
  truthList.setMinVisPt(minVisPt);
  truthList.selectDecayMode(decayMode);

}

