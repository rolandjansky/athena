/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void effTable(const char* filename="ntuples/trigtau.AOD.root", 
	      TString mode="all") {

  gROOT->cd();

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
  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  // We will use our own trigger manager (although we could just use
  // the one in TrigTauAnalyzer).
  TrigTauSelectionManager trigMan;

  // Pick a menu set (12.0.6 by default)
  //  trigMan.setMenuVersion(TrigTauSelectionManager::MENUJUNE06);

  // Also set an L1 MET cut if desired
  //   trigMan.setL1MET(20000.);

  // Set fiducial cuts
  analyzer.truthList.setMinVisPt(0.);
  analyzer.truthList.setMaxVisEta(2.5);

  // Select a particular tau decay mode (if desired)
  // Could also add pi, rho, etc.
  if (mode == "all") {
  }
  else if (mode == "1pr") {
    analyzer.truthList.select1Prong();
  }
  else if (mode == "3pr") {
    analyzer.truthList.select3Prong();
  }
  else {
    cout << "Unkown decay mode: " << mode << endl;
    delete event; delete tree; delete file;
    return;
  }

  // Set up the different menus to plot
  const int nMenus = 4;
  TrigTauFlags::TRIGGERMENU theMenu[nMenus];
  theMenu[0] = TrigTauFlags::TAU16I;
  theMenu[1] = TrigTauFlags::TAU20I;
  theMenu[2] = TrigTauFlags::TAU29I;
  theMenu[3] = TrigTauFlags::TAU50;

  TString menuName[nMenus];
  menuName[0] = "tau16i";
  menuName[1] = "tau20i";
  menuName[2] = "tau29i";
  menuName[3] = "tau50";

  // Create counters to hold triggered object counts 
  long nEvent = 0;
  long nTrue = 0;
  long nGood = 0;
  long nL1Sel[nMenus];
  long nL2Sel[nMenus];

  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    nL1Sel[iMenu] = 0;
    nL2Sel[iMenu] = 0;
  }

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // Analyze this event
    analyzer.analyze(event);

    nEvent++;
    nTrue += analyzer.truthList.nMCTauPure();
    nGood += analyzer.truthList.nMCTauGood();

    // Loop over all good truth taus in this event
    unsigned int nTau = analyzer.truthList.nMCTauGood();
    for(unsigned int itau = 0; itau < nTau; ++itau ) {
      TrigTauMCTau* tau =  &analyzer.truthList.mcTauGoodEntry(itau);

      // Now, loop through defined menus, apply selections, and look for
      // a matching trigger object within the allowed dR distance.
      // Note:  The object passing L2 doesn't necessarily need to come
      // from the ROI passing L1, but I think that is OK.
      for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {

	// Select the new menu
	trigMan.setTauMenu(theMenu[iMenu]);

	// Refill the selected chain list based on the new menu
	analyzer.chainList.fillTriggerList(trigMan);

	// Find a matching L1 object
	TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
	if (l1Tau != 0) nL1Sel[iMenu]++;

	// Because the dR range might be different between L1 and L2
	// it is possible (if unlikely) to find a L2 match without a L1.
	// In reality, this probably never happens...

	// Find a matching L2 object
	TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
	if (l2Tau != 0) nL2Sel[iMenu]++;

      }  // End of loop over menus
    }  // End of loop over good taus in the event    
  }  // end loop over events

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << " -> " 
       << float(nGood)/nEvent << " Good/Ev" << endl;

  cout << "Trig Menus:";
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++)
    cout << menuName[iMenu] << "    ";
  cout << endl;

  cout << "L1 Passed: " ;
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++)
    cout << nL1Sel[iMenu] << " (" << 100*float(nL1Sel[iMenu])/nGood << "%) ";
  cout << endl;

  cout << "L2 Passed: " ;
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++)
    cout << nL2Sel[iMenu] << " (" << 100*float(nL2Sel[iMenu])/nGood << "%) ";
  cout << endl;

  delete event;
  delete tree;
  delete file;

  return;
}

