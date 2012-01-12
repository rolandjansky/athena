/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dump out trigger bits
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "TFile.h"

#include "TrigTauPerformValidation/TTPValTrigDump.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"

using namespace std;

TTPValTrigDump::TTPValTrigDump() : _maxNEvents(0) {
}

TTPValTrigDump::~TTPValTrigDump() {
}

// Add event numbers to list
void
TTPValTrigDump::selectEvent(long evNumber) {
  _eventList.push_back(evNumber);
}
 
// More user friendly way to process file
void
TTPValTrigDump::dump(const TString& filename) {

  // Open file
  TFile* file = new TFile(filename);
  if (!file->IsOpen()) {
    std::cerr << "Can't open file " << filename << std::endl;
    return;
  }

  // Initialize tree
  TTree* tree = (TTree*)file->Get("eventTree");
  if (tree == 0) {
    std::cerr << "Can't find eventTree in " << filename << std::endl;
    return;
  }

  dump(tree);

  delete tree;
  delete file;
}

// Process file
void
TTPValTrigDump::dump(TTree* tree) {

  if (tree == 0) {
    std::cerr << "TTPValTrigDump::fill - TTree is NULL!" << std::endl;
    return;
  }

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 0);

  // Loop over all events
  int nev = int(tree->GetEntries());
  long ndumped = 0;

  for(int iev = 0; iev<nev; ++iev) {

    if (_maxNEvents > 0 && ndumped >= _maxNEvents) break;

    event->reset();
    tree->GetEvent(iev);

    // Check for specific event
    if (_eventList.size() > 0 && std::find(_eventList.begin(), _eventList.end(), event->eventNumber) == _eventList.end()) continue;

    ndumped++;

    // Analyze this event
    cout << endl << "--------------------- Run: " << event->runNumber 
	 << " Event: " << event->eventNumber 
	 << " ---------------------" << endl;

    cout << " Time Stamp: " << event->time_stamp
	 << " Lumi Block: " << event->lumi_block
	 << " BX: " << event->bunch_crossing << endl;

    cout << " --- Triggers Passed ---" << endl;

    event->decision.print();

    for (unsigned int word=0; word<4; word++) {
      for (unsigned int ibit=0; ibit<32; ibit++) {

	unsigned int imenu = 100*word+ibit;
	TrigTauFlags::TRIGGERMENU 
	  menu = (TrigTauFlags::TRIGGERMENU) imenu;

	if (event->decision.passedL1(menu)) {
	  cout << "[" << setw(3) << imenu << "] " 
	       << TrigTauFlags::menuName(menu) << ": L1";
	  if (event->decision.passedL2(menu)) cout << " L2";
	  if (event->decision.passedEF(menu)) cout << " EF";
	  cout << endl;
	}
      }
    } 
    
    cout << " --- L1 ROIs found --- " << endl;
    for (unsigned long i=0; i<event->nTauL1(); i++) {
      TrigTauL1Object* tau = &event->tauL1Entry(i);
      cout << " Et: " << tau->m_TauEnergy
	   << " eta/phi: " << tau->Eta() << "/" << tau->Phi()
	   << " EMIso: " << tau->m_EmIsol
	   << " roiWord: " << std::hex << tau->m_roiWord << std::dec << endl;
    }

    cout << " --- L2 ROIs found --- " << endl;
    for (unsigned long i=0; i<event->nTauL2(); i++) {
      TrigTauL2Object* tau = &event->tauL2Entry(i);
      cout << " Et: " << tau->etNarrow()
	   << " eta/phi: " << tau->Eta() << "/" << tau->Phi()
	   << " (calo: " << tau->m_caloEta << "/" << tau->m_caloPhi << ")"
	   << " nTrks: " << tau->nTracks()
	   << " roiWord: " << std::hex << tau->m_roiWord << std::dec;

      cout << " type: ";
      switch (tau->m_seedType) {
      case TrigTauTauObject::UNDEFINED:
	cout << "undefined";
	break;
      case TrigTauTauObject::CALOOBJECT:
	cout << "calo";
	break;
      case TrigTauTauObject::TRACK:
	cout << "track";
	break;
      case TrigTauTauObject::BOTH:
	cout << "both";
	break;
      default:
	cout << "unknown!";
      }
      cout << endl;

      cout << "     Triggers: ";
      for (unsigned int word=0; word<4; word++) {
	for (unsigned int ibit=0; ibit<32; ibit++) {
	  
	  unsigned int imenu = 100*word+ibit;
	  TrigTauFlags::TRIGGERMENU 
	    menu = (TrigTauFlags::TRIGGERMENU) imenu;
	  if (tau->m_tags.passedL2(menu)) 
	    cout << "[" << imenu << "] " 
		 << TrigTauFlags::menuName(menu) << " ";
	}
      }
      cout << endl;
      //      tau->m_tags.print();
    }

    cout << " --- EF ROIs found --- " << endl;
    for (unsigned long i=0; i<event->nTauEF(); i++) {
      TrigTauRecEFObject* tau = &event->tauEFEntry(i);
      cout << " Et: " << tau->Et()
	   << " eta/phi: " << tau->Eta() << "/" << tau->Phi()
	   << " (calo: " << tau->m_caloEta << "/" << tau->m_caloPhi << ")"
	   << " nTrks: " << tau->nTracks()
	   << " roiWord: " << std::hex << tau->m_roiWord << std::dec
	   << " type: ";
      switch (tau->m_seedType) {
      case TrigTauTauObject::UNDEFINED:
	cout << "undefined";
	break;
      case TrigTauTauObject::CALOOBJECT:
	cout << "calo";
	break;
      case TrigTauTauObject::TRACK:
	cout << "track";
	break;
      case TrigTauTauObject::BOTH:
	cout << "both";
	break;
      default:
	cout << "unknown!";
      }
      cout << endl;

      cout << "     Triggers:";
      for (unsigned int word=0; word<4; word++) {
	for (unsigned int ibit=0; ibit<32; ibit++) {
	  
	  unsigned int imenu = 100*word+ibit;
	  TrigTauFlags::TRIGGERMENU 
	    menu = (TrigTauFlags::TRIGGERMENU) imenu;
	 
	  if (tau->m_tags.passedEF(menu)) 
	    cout << "[" << imenu << "] " 
		 << TrigTauFlags::menuName(menu) << " ";
	}
      }
      cout << endl;
      //      tau->m_tags.print();
 
    }




  } // End event loop

  delete event;
}


