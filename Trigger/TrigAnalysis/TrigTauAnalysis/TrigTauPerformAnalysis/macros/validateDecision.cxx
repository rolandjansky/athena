/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

  // Create counters to hold triggered object counts 
static long nEvent;
static long nL1Sel;
static long nL2Sel;
static long nL2CalSel;
static long nEFSel;

static int nDecL1;
static int nDecL2;
static int nDecEF;

static double nEventWt;
static double nL1SelWt;
static double nL2SelWt;
static double nL2SelCalWt;
static double nEFSelWt;

void rateTable2EllClear();
void rateTable2EllPrint();

// Menu2 should either be E10, Mu10, or tau
void validateDecision(const char* filename="ntuples/trigtau.5200.root", 
	       TrigTauFlags::TRIGGERMENU menu1 = TrigTauFlags::TAU16I, float met = 0.)
{
   rateTable2EllClear();
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
  tree->SetBranchStatus("mcEvent*", 0);

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(met);
  TrigTauSelectionManager trigMan1;
  trigMan1.setMenuVersion(TrigTauSelectionManager::MENU13030pre);
  trigMan1.setTauMenu(menu1);

  //trigMan1.setL2CalOnly(false);
  bool checkL2Cal = true;

  int nPrintL1 = 0;
  int nPrintL2 = 0;
  int nPrintEF = 0;

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    bool decL1 = event->decision.passedL1(menu1);
    bool decL2 = event->decision.passedL2(menu1);
    bool decEF = event->decision.passedEF(menu1);
    
    nDecL1 += int(decL1);
    nDecL2 += int(decL2);
    nDecEF += int(decEF);

    bool ntL1 = false;
    bool ntL2 = false;
    bool ntEF = false;

    // Analyze this event
    analyzer.chainList.fillChainList(event);

    nEvent++;

    bool pass = false;

    // Look for L1 pass
    unsigned int numCh = analyzer.chainList.nTrigChains();
    if( event->eventNumber == 1195 ) cout << "1195 number of chains " << numCh << endl;
    for (unsigned int iCh1=0; iCh1 < numCh; ++iCh1) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

      if (!trigMan1.passedL1(ch1)){
	  continue;
      }
      pass = true;
      if (pass) break;
    }

//     if( pass != decL1 ) continue;
//        nDecL1 += int(decL1);
//       nDecL2 += int(decL2);
//       nDecEF += int(decEF);

    if (pass) {
      nL1Sel++; 
      ntL1 = true;
    }


    // Look for L2Calo pass
    if( checkL2Cal ){
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

      if (!trigMan1.passedL2Cal(ch1)) continue;

      pass = true;
      if (pass) break;
    }

    if (pass) {
      nL2CalSel++;
    }
    }

    // Look for L2 pass
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
      
      if (!trigMan1.passedL2(ch1)) continue;

      pass = true;
      if (pass) break;
    }

    if (pass) {
      nL2Sel++;
      ntL2 = true;
    }


    // Look for EF pass
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

      if (!trigMan1.passedEF(ch1)) continue;
      pass = true;
      if (pass) break;
    }

    if (pass) {
      nEFSel++;
      ntEF=true;
    }

    bool printChain=false;
    if( decL1 != ntL1 ) {
      cout << "Event " << event->eventNumber << " L1 decision does not match: NTUPLE " << ntL1 << "
 and HLTcode " << decL1 << endl;
      printChain = true;
      nPrintL1++;

    }
    if( decL2 != ntL2 ) {
      cout << "Event " << event->eventNumber << " L2 decision does not match: NTUPLE " << ntL2 << "
 and HLTcode " << decL2 << endl;
      printChain = true;
      nPrintL2++;
    }
    if( decEF != ntEF ) {
      cout << "Event " << event->eventNumber << " EF decision does not match: NTUPLE " << ntEF << "
 and HLTcode " << decEF << endl;
      printChain = true;
      nPrintEF++;
    }
    if( printChain ) 
      {
	analyzer.chainList.printChainList();
	cout << endl;
	trigMan1.setDebug(true);
	for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
	  cout << " ==================================\n";
	  TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
	  if (!trigMan1.passedL1(ch1)) continue;
	  if (!trigMan1.passedL2(ch1)) continue;
	  if (!trigMan1.passedEF(ch1)) continue;
	}
	trigMan1.setDebug(false);
	cout << endl;
      }


  }  // end loop over events

  //  trigMan1.print();
  //  trigMan2.print();

  delete event;
  delete tree;
  delete file;

  cout << " Use menu " << menu1.menuName(menu1) << endl;
  //trigMan1.print();
rateTable2EllPrint() ;

 cout << "Mistmatch found in L1 " <<  nPrintL1 << " , L2 " << nPrintL2 << ", EF " << nPrintEF << endl;


  return;
}

void rateTable2EllPrint() {


  // Print out matching statistics
  cout << "Events: " << nEvent << endl;

  cout << "L1Sel  L2CalSel  L2TrkSel  EFSel" << endl;

  cout << "Decision : "
       << nDecL1 << " " 
       << nDecL2 << " " 
       << nDecEF << endl;


  cout << "Number:    " 
       << nL1Sel << " " 
       << nL2CalSel << " " 
       << nL2Sel << " " 
       << nEFSel << endl;

  //cout << "Rate: " 
  //    << nL1SelWt << " " 
  //   << nL2SelWt << " " 
  //   << nEFSelWt << endl;

  cout << "Cum Eff: "
       << 100*float(nL1Sel)/nEvent << " " 
       << 100*float(nL2CalSel)/nEvent << " " 
       << 100*float(nL2Sel)/nEvent << " " 
       << 100*float(nEFSel)/nEvent << endl;

//   cout << "Dif Eff: "
//        << 100*float(nL1Sel)/nEvent << " " 
//        << (nL1Sel > 0 ? 100*float(nL2CalSel)/nL1Sel : 0) << " " 
//        << (nL2CalSel > 0 ? 100*float(nL2Sel)/nL2CalSel : 0) 
//        << (nL2Sel > 0 ? 100*float(nEFSel)/nL2Sel : 0) << endl;

//   cout << "Cum Rej: "
//        << (nL1Sel > 0 ? nEvent/float(nL1Sel) : 0) << " " 
//        << (nL2CalSel > 0 ? nEvent/float(nL2CalSel) : 0) << " " 
//        << (nL2Sel > 0 ? nEvent/float(nL2Sel) : 0) << " " 
//        << (nEFSel > 0 ? nEvent/float(nEFSel) : 0) << endl;

//   cout << "Dif Rej: "
//        << (nL1Sel > 0 ? nEvent/float(nL1Sel) : 0) << " " 
//        << (nL2CalSel > 0 ? nL1Sel/float(nL2CalSel) : 0) << " " 
//        << (nL2Sel > 0 ? nL2CalSel/float(nL2Sel) : 0) << " " 
//        << (nEFSel > 0 ? nL2Sel/float(nEFSel) : 0) << endl;

}

void rateTable2EllClear() {
  // Create counters to hold triggered object counts 
   nEvent = 0;
   nL1Sel = 0;
   nL2Sel = 0;
   nEFSel = 0;
   nEventWt = 0.;
   nL1SelWt = 0.;
   nL2SelWt = 0.;
   nEFSelWt = 0.;

nDecL1= 0;
nDecL2= 0;
nDecEF= 0;

}

