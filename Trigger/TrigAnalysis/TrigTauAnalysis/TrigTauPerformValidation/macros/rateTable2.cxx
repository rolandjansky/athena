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


void rateTable2(const char* fileName)
{
  bool signal = false;
  cout << " +++++++++++++++++++++++++ LOOSE ++++++++++++++++++++++ \n";
  TrigTauSelectionManager::CUTLEVEL cutLevel =  TrigTauSelectionManager::LOOSE;
  rateTable1(fileName, TrigTauFlags::TAU16I, cutLevel, signal);

  cout << " +++++++++++++++++++++++++ MEDIUM ++++++++++++++++++++++ \n";
  TrigTauSelectionManager::CUTLEVEL cutLevel =  TrigTauSelectionManager::MEDIUM;
  //rateTable1(fileName, TrigTauFlags::TAU15_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU20I_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU25I_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU35I_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU45_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU60_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU100_MEDIUM, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU150_MEDIUM, cutLevel, signal);
  
  cout << " +++++++++++++++++++++++++ TIGHT ++++++++++++++++++++++ \n";
  TrigTauSelectionManager::CUTLEVEL cutLevel =  TrigTauSelectionManager::TIGHT;
  //rateTable1(fileName, TrigTauFlags::TAU15_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU20I_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU25I_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU35I_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU45_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU60_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU100_TIGHT, cutLevel, signal);
  //rateTable1(fileName, TrigTauFlags::TAU150_TIGHT, cutLevel, signal);



}

// Menu2 should either be E10, Mu10, or tau
void rateTable1(const char* filename, 
		TrigTauFlags::TRIGGERMENU menu1, 
		TrigTauSelectionManager::CUTLEVEL cutLevel,
		bool signal )
{

  TrigTauSelectionManager::MENUVERSION version = TrigTauSelectionManager::MENU1400;
  //  TrigTauSelectionManager::MENUVERSION version = TrigTauSelectionManager::MENU13030pre;
  //TrigTauSelectionManager::MENUVERSION version = TrigTauSelectionManager::MENU1206;
 
  float met = 0.;

  //bool signal       = true;
  int nprong        = 0;
  bool checkOffline = true;
  // TAU1P3P, TAUREC, EITHERTAU, BOTHTAU
  TrigTauMCTauList::OFFLINESELECTION offlineType =  TrigTauMCTauList::NONE;

  bool doDetailedPrinting=true;

  //TrigTauBugFix bugfix (TrigTauBugFix::TTP4, TrigTauBugFix::RDO);
  TrigTauBugFix bugfix(TrigTauBugFix::TTP7, TrigTauBugFix::RDO);
  //bugfix.setTTPVersion(filename);

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  //===================================================
  // MC truth
  analyzer.truthList.setMaxVisEta(2.5);
  analyzer.truthList.setMinVisPt(menu1);
  if( nprong == 1 )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);

   rateTable2EllClear();
   gROOT->cd();

  // Open file
  
  //commented out; use to use file declared when calling function
   
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
  
  

  //TChain * tree = new TChain("eventTree");
  //tree->Add("ntuples/14.2.0/5107/tmp.*");
  //tree->Add("ntuples/TTP09prelim/tmp.*");
  //tree->Add("~/temp/temp/ChangeEDM.trigtau.5200.root");

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  //tree->SetBranchStatus("mcEvent*", 0);

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(met);
  analyzer.trigMan.setMenuVersion(version);
  analyzer.trigMan.setTauMenu(menu1, cutLevel);

  //TrigTauHistogram::setHistLimits(TrigTauHistogram::PTHIST,30,0,90000);
  //analyzer.trigMan.l2CalSel.setHistDebug();


  //analyzer.trigMan.setL2CalOnly(false);
  bool checkL2Cal = true;

  int nPrintL1 = 0;
  int nPrintL2 = 0;
  int nPrintEF = 0;

  analyzer.chainList.setRoiWordMatching(false);

  // Loop over all events
  int nev = int(tree->GetEntries());
  cout << "Checking a total of " << nev << " events..." << endl;
  //if( nev > 15000 ) nev = 15000;
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    analyzer.analyze(event, signal, offlineType);

    // ============ Select type of events (e.g. signal) ================
    if( signal &&  analyzer.truthList.nMCTauGood() ==0 ) continue;
    //==================================================================
    nEvent++;

    bool decL1 = event->decision.passedL1(menu1);
    bool decL2 = event->decision.passedL2(menu1);
    bool decEF = event->decision.passedEF(menu1);
    
    nDecL1 += int(decL1);
    nDecL2 += int(decL2);
    nDecEF += int(decEF);

    bool ntL1 = false;
    bool ntL2 = false;
    bool ntEF = false;

    if( analyzer.chainList.nPassL1Chains() > 0 ) 
      {
	nL1Sel++; 
	ntL1 = true;
      }

    if( checkL2Cal ){
      if( analyzer.chainList.nPassL2CalChains() > 0 ) 
	nL2CalSel++; 
    }

    if( analyzer.chainList.nPassL2Chains() > 0 ) {
      nL2Sel++; 
      ntL2 = true;
    }


    if( analyzer.chainList.nPassEFChains() > 0 ) {
      nEFSel++;
      ntEF=true;
    }
    
    bool printChain=false;
    if(doDetailedPrinting && decL1 != ntL1 ) {
      cout << "\n============ Event " << event->eventNumber << " L1 decision does not match: NTUPLE " 
	   << ntL1 << "
 and HLTcode " << decL1 << endl;
      printChain = true;
      nPrintL1++;
      
    }
    if( doDetailedPrinting &&decL2 != ntL2 ) {
      cout << "\n============ Event " << event->eventNumber << " L2 decision does not match: NTUPLE " 
	   << ntL2 << "
 and HLTcode " << decL2 << endl;
      printChain = true;
      nPrintL2++;
    }
    if( doDetailedPrinting &&decEF != ntEF ) {
      cout << "\n============ Event " << event->eventNumber << " EF decision does not match: NTUPLE " 
	   << ntEF << "
 and HLTcode " << decEF << endl;
      printChain = true;
      nPrintEF++;
    }
    if( doDetailedPrinting && printChain ) 
      {
	analyzer.chainList.printChainList();
	cout << endl;
	analyzer.trigMan.setDebug(true);
	int numCh = analyzer.chainList.nTrigChains();
	for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
	  cout << " ==================================\n";
	  TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
	  if (!analyzer.trigMan.passedL1(ch1)) continue;
	  if (!analyzer.trigMan.passedL2(ch1)) continue;
	  if (!analyzer.trigMan.passedEF(ch1)) continue;
	}
	analyzer.trigMan.setDebug(false);
	cout << endl;
      }
    //}// end of doDetailedPrinting
      
      
    }  // end loop over events

  //  analyzer.trigMan.print();
  //  trigMan2.print();
  //analyzer.trigMan.l2CalSel.histManager()->plotAll();
  // analyzer.trigMan.l2CalSel.histManager()->plotAllEfficiencies();

  delete event;
  delete tree;
  //delete file;

  cout << " Use menu " << menu1.menuName(menu1) << endl;
  //analyzer.trigMan.print();
  rateTable2EllPrint() ;

 cout << "Mistmatch found in L1 " <<  nPrintL1 << " , L2 " << nPrintL2 << ", EF " << nPrintEF << endl;
/*
 cout << "Com Eff " << menu1.menuName(menu1) << " \& "<< nEvent << " & " //<< std::setprecision(3)
      <<  float(nL1Sel)/float(nEvent)<< " & " //<< std::setprecision(3)
      <<  float(nL2CalSel)/float(nEvent)<< " & "//<< std::setprecision(3)
      <<  float(nL2Sel)/float(nEvent) << " & "//<< std::setprecision(3)
       <<  float(nEFSel)/float(nEvent)<< " \\ \n";
 cout << "Lev Eff " << menu1.menuName(menu1) << " \& "<< nEvent << " & " //<< std::setprecision(3)
      <<  float(nL1Sel)/float(nEvent)<< " & " //<< std::setprecision(3)
      <<  float(nL2CalSel)/float(nL1Sel)<< " & "//<< std::setprecision(3)
      <<  float(nL2Sel)/float(nL2CalSel) << " & "//<< std::setprecision(3)
       <<  float(nEFSel)/float(nL2Sel)<< " \\ \n";
*/

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
   nL2CalSel = 0;
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

