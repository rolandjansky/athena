/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValRateTable.h"



TTPValRateTable::TTPValRateTable(TrigTauBugFix::POOLFILE poolfile1, TrigTauBugFix::TTPVERSION version1, bool debug1)
  : bugfix(version1, poolfile1) {
  
  version = version1;
  poolfile = poolfile1;

  initialize(debug1);
  fileDefined = false;
  return;

}
  



TTPValRateTable::TTPValRateTable( TString filename1, TrigTauBugFix::POOLFILE poolfile1, TrigTauBugFix::TTPVERSION version1,  bool debug1)
  : bugfix (version1, poolfile1) {

  poolfile = poolfile1;
  version = version1;

  filename = filename1;  
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1);
  fileDefined = true;
  return;

}

void TTPValRateTable::setFile (TString filename1) {

  if (!fileDefined ) {
    filename = filename1;
    chain = new TChain("eventTree");
    chain->Add(filename);
  }
  else {
    cout << "WARNING: File already defined!! Overwriting old file settings...." << endl;
    filename = filename1;
    chain = new TChain("eventTree"); //memory leak
    chain->Add(filename);
  }
  fileDefined = true;


}

void TTPValRateTable::initialize ( bool debug1 ) {

  type = 1;
  debug = debug1;
  nprong = 0; 
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  met = 0.;
  nprong = 0;
  signalSet = false;
  checkOffline = true;
  doDetailedPrinting = false;
  checkL2Cal = true; printChain=false;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU16;
  cutLevel = TrigTauSelectionManager::LOOSE;
  offlineType =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK;  
}


void TTPValRateTable::checkAllMenus(unsigned int nMenu, unsigned int nLevel, double lum, double crosssec) {

  if (!signalSet) {

    cout << "ERROR: Boolean SIGNAL is not set!" << endl;
    return;

  }

  
  TrigTauFlags::TRIGGERMENU names[] = {TrigTauFlags::TAU12, TrigTauFlags::TAU16, TrigTauFlags::TAU20, TrigTauFlags::TAU29, TrigTauFlags::TAU38, TrigTauFlags::TAU50, TrigTauFlags::TAU84, TrigTauFlags::TAU100, TrigTauFlags::TAU125};
  TrigTauSelectionManager::CUTLEVEL levels[] = {TrigTauSelectionManager::LOOSE, TrigTauSelectionManager::MEDIUM, TrigTauSelectionManager::TIGHT};
  TTPValRateTable * tables[9][3];
  /*
  int j=0;
  int i=1;
  cout << "Checking number " << j*nMenu+i << " of " << nLevel*nMenu << endl;
  tables[i][j] = new TTPValRateTable(filename, poolfile, version, debug);
  tables[i][j]->setMenu(names[i]);
  tables[i][j]->setCutLevel(levels[j]);
  tables[i][j]->analyze();

  tables[i][j]->print(lum, crosssec);
  
  i=6;
  cout << "Checking number " << j*nMenu+i << " of " << nLevel*nMenu << endl;
  tables[i][j] = new TTPValRateTable(filename, poolfile, version, debug);
  tables[i][j]->setMenu(names[i]);
  tables[i][j]->setCutLevel(levels[j]);
  tables[i][j]->analyze();

  cout << "\n Displaying results for ";
    switch (j) {
    case 0: cout << "cutLevel: LOOSE" << endl; break;
    case 1: cout << "cutLevel: MEDIUM" << endl; break;
    case 2: cout << "cutLevel: TIGHT" << endl; break;
    }
  i=1;
  tables[i][j]->print(lum, crosssec);
  i=6;
  tables[i][j]->print(lum, crosssec);
  */


  for (unsigned int j =0; j!=nLevel; ++j) {
    for (unsigned int i = 0; i!=nMenu; ++i) {
      cout << "Checking number " << j*nMenu+i << " of " << nLevel*nMenu << endl;
      tables[i][j] = new TTPValRateTable(filename, poolfile, version, debug);
      tables[i][j]->setMenu(names[i]);
      tables[i][j]->setCutLevel(levels[j]);
      tables[i][j]->setSignal(signal);
      tables[i][j]->setChainType(chainType);
      tables[i][j]->analyze();
    }
  }
  for (unsigned int j=0; j!=nLevel; ++j) {
    outfile << "\n Displaying results for ";
    switch (j) {
    case 0: outfile << "cutLevel: LOOSE" << endl; break;
    case 1: outfile << "cutLevel: MEDIUM" << endl; break;
    case 2: outfile << "cutLevel: TIGHT" << endl; break;
    }
    for (unsigned int i = 0; i!=nMenu; ++i) {
      if (lum == 0 || crosssec == 0) tables[i][j]->print(false);
      else tables[i][j]->print(lum, crosssec, false);
    }
  }

}




void TTPValRateTable::analyze (int nEvents) {

  if (fileDefined == false) {
    cout << "ERROR: Filename is not set. Please set filename with setFile(char*)" << endl;
    return;
  }

  if (!signalSet) {

    cout << "ERROR: Boolean SIGNAL is not set!" << endl;
    return;

  }

 
  TrigTauAnalyzer analyzer;

  //===================================================
  // MC truth
  analyzer.truthList.setMaxVisEta(maxVisEta);
  if (debug) cout << "Analyzer: maxVisEta set." << endl;
  analyzer.truthList.setMinVisPt(menu);
  if (debug) cout << "Analyzer: minVisPt set." << endl;
  if( nprong == 1 )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  analyzer.truthList.setRecoMatchdR(0.2);
  if (debug) cout << "Analyzer: RecoMatch set." << endl;

   reset();
   gROOT->cd();


  TTree* tree = chain;   
  if (debug) cout << "Tree created..." << endl;
 
  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  if (debug) cout << "EventBranch connected..." << endl;
  tree->SetBranchStatus("*", 1);
  //tree->SetBranchStatus("mcEvent*", 0);
  if (debug) cout << "Tree configured.." << endl;

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(met);
  analyzer.trigMan.setMenuVersion(menuVersion);
  analyzer.trigMan.setTauMenu(menu, cutLevel);

  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(chainType);
  if (debug) cout << "Analyzer configured.." << endl;

  // Loop over all events
  nev = int(tree->GetEntries());
  cout << "Checking a total of " << nev << " events..." << endl;
  if (nev > nEvents) nev = nEvents;
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    analyzer.analyze(event, signal, offlineType);

    // ============ Select type of events (e.g. signal) ================
    if( signal &&  analyzer.truthList.nMCTauGood() ==0 ) continue;
    //==================================================================
    nEvent++;

    bool decL1 = event->decision.passedL1(analyzer.trigMan.usedTauMenu());
    bool decL2 = event->decision.passedL2(analyzer.trigMan.usedTauMenu());
    bool decEF = event->decision.passedEF(analyzer.trigMan.usedTauMenu());
    
    nDecL1 += int(decL1);
    nDecL2 += int(decL2);
    nDecEF += int(decEF);

    bool ntL1 = false;
    bool ntL2 = false;
    bool ntEF = false;

    if( analyzer.chainList.nPassL1Chains() > 0 ) {	
	nL1Sel++; 
	ntL1 = true;
    }

    if( checkL2Cal )
      if( analyzer.chainList.nPassL2CalChains() > 0 ) 
	nL2CalSel++; 

    if( analyzer.chainList.nPassL2Chains() > 0 ) {
      nL2Sel++; 
      ntL2 = true;
    }

    if( analyzer.chainList.nPassEFChains() > 0 ) {
      nEFSel++;
      ntEF=true;
    }
    
    printChain = false;
    if(doDetailedPrinting && decL1 != ntL1 ) {
      cout << "\n============ Event " << event->eventNumber << " L1 decision does not match: NTUPLE " 
	   << ntL1 << "\n and HLTcode " << decL1 << endl;
      printChain = true;
      nPrintL1++;
      
    }
    if( doDetailedPrinting && decL2 != ntL2 ) {
      cout << "\n============ Event " << event->eventNumber << " L2 decision does not match: NTUPLE " 
	   << ntL2 << "\n and HLTcode " << decL2 << endl;
      printChain = true;
      nPrintL2++;
    }
    if( doDetailedPrinting && decEF != ntEF ) {
      cout << "\n============ Event " << event->eventNumber << " EF decision does not match: NTUPLE " 
	   << ntEF << "\n and HLTcode " << decEF << endl;
      printChain = true;
      nPrintEF++;
    }
    if( doDetailedPrinting && printChain ) 
      {
	analyzer.chainList.printChainList();
	cout << endl;
	analyzer.trigMan.setDebug(true);
	unsigned int numCh = analyzer.chainList.nTrigChains();
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
    // end of doDetailedPrinting
      
      
    }  // end loop over events

  delete event;
  delete tree;
  //delete file;
  
    
  if (doDetailedPrinting) cout << "Out of "<< nEvent << " events: Mistmatch found in L1 " <<  nPrintL1 << " , L2 " << nPrintL2 << ", EF " << nPrintEF << endl;
 return;
}

void TTPValRateTable::print(double lum, double crosssec, bool file) {

  print(lum, crosssec, "rateTable.txt", file);

}

void TTPValRateTable::print(bool file) {

  print("rateTable.txt", file);

}


void TTPValRateTable::print(double lum, double crosssec,TString target, bool file) {

  if (debug) cout << "luminosity is set to " << lum << " and x-sec is set to " << crosssec << endl;
  nL1SelWt = lum*crosssec*double(nL1Sel)/double(nEvent);
  nL2CalSelWt = lum*crosssec*double(nL2CalSel)/double(nEvent);
  nL2SelWt = lum*crosssec*double(nL2Sel)/double(nEvent);
  nEFSelWt = lum*crosssec*double(nEFSel)/double(nEvent);
  print(target, file);

}


void TTPValRateTable::print(TString target, bool file) {

  outfile.open(target, ios::app);

  if (file==true) {

    TrigTauFlags menu1;
    outfile << "For sample: " << filename << endl;
    outfile << " Use menu " << menu1.menuName(menu) << endl;
    
    outfile << "Number of Events in ntuple: " << nev << endl;
    
    // Print out matching statistics
    outfile << "Events: " << nEvent << endl;
    
    outfile << "L1Sel  L2CalSel  L2TrkSel  EFSel" << endl;
  
    outfile << "Decision : "
	    << nDecL1 << " " 
	    << nDecL2 << " " 
	    << nDecEF << endl;


    outfile << "Number:    " 
	    << nL1Sel << " " 
	    << nL2CalSel << " " 
	    << nL2Sel << " " 
	    << nEFSel << endl;
    
    outfile << "Rate: " 
	    << nL1SelWt << " " 
	    << nL2CalSelWt << " "
            << nL2SelWt << " " 
	    << nEFSelWt << endl;
    
    outfile << "Cum Eff: "
	    << 100*float(nL1Sel)/nEvent << " " 
	    << 100*float(nL2CalSel)/nEvent << " " 
	    << 100*float(nL2Sel)/nEvent << " " 
	    << 100*float(nEFSel)/nEvent << endl;

    outfile << endl;
    outfile << endl;

    
    //   outfile << "Dif Eff: "
    //        << 100*float(nL1Sel)/nEvent << " " 
    //        << (nL1Sel > 0 ? 100*float(nL2CalSel)/nL1Sel : 0) << " " 
    //        << (nL2CalSel > 0 ? 100*float(nL2Sel)/nL2CalSel : 0) 
    //        << (nL2Sel > 0 ? 100*float(nEFSel)/nL2Sel : 0) << endl;
    
    //   outfile << "Cum Rej: "
    //        << (nL1Sel > 0 ? nEvent/float(nL1Sel) : 0) << " " 
    //        << (nL2CalSel > 0 ? nEvent/float(nL2CalSel) : 0) << " " 
    //        << (nL2Sel > 0 ? nEvent/float(nL2Sel) : 0) << " " 
    //        << (nEFSel > 0 ? nEvent/float(nEFSel) : 0) << endl;
    
    //   outfile << "Dif Rej: "
    //        << (nL1Sel > 0 ? nEvent/float(nL1Sel) : 0) << " " 
    //        << (nL2CalSel > 0 ? nL1Sel/float(nL2CalSel) : 0) << " " 
    //        << (nL2Sel > 0 ? nL2CalSel/float(nL2Sel) : 0) << " " 
    //        << (nEFSel > 0 ? nL2Sel/float(nEFSel) : 0) << endl;
  }
  else {

        TrigTauFlags menu1;
    cout << " Use menu " << menu1.menuName(menu) << endl;
    
    cout << "Number of Events in ntuple: " << nev << endl;
    
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
    
    cout << "Rate: " 
	    << nL1SelWt << " " 
	    << nL2SelWt << " " 
	    << nEFSelWt << endl;
    
    cout << "Cum Eff: "
	    << 100*float(nL1Sel)/nEvent << " " 
	    << 100*float(nL2CalSel)/nEvent << " " 
	    << 100*float(nL2Sel)/nEvent << " " 
	    << 100*float(nEFSel)/nEvent << endl;

  }
    


}

void TTPValRateTable::update( TString filename, double lum, double crosssec ) {

  TFile f(filename, "update");

  f.cd();
  f.mkdir("RateTable");
  f.cd("RateTable");

  TrigTauFlags menu1;
  TH1F *EventNumbers = new TH1F("TTPValRateTable_EventNumbers_"+menu1.menuName(menu), "EventCounter", 20, 0, 20);
  EventNumbers->SetBinContent(1, nev);
  EventNumbers->GetXaxis()->SetBinLabel(1, "Total Number of Events");

  EventNumbers->SetBinContent(2, nEvent);
  EventNumbers->GetXaxis()->SetBinLabel(2, "# of good Events");

  EventNumbers->SetBinContent(3, nDecL1);
  EventNumbers->GetXaxis()->SetBinLabel(3, "Decision L1");

  EventNumbers->SetBinContent(4, nL1Sel);
  EventNumbers->GetXaxis()->SetBinLabel(4, "Number L1");

  EventNumbers->SetBinContent(5, nDecL2);
  EventNumbers->GetXaxis()->SetBinLabel(5, "Decision L2");

  EventNumbers->SetBinContent(6, nL2CalSel);
  EventNumbers->GetXaxis()->SetBinLabel(6, "Number L2Calo");

  EventNumbers->SetBinContent(7, nL2Sel);
  EventNumbers->GetXaxis()->SetBinLabel(7, "Number L2Trk");

  EventNumbers->SetBinContent(8, nDecEF);
  EventNumbers->GetXaxis()->SetBinLabel(8, "Decision EF");

  EventNumbers->SetBinContent(9, nEFSel);
  EventNumbers->GetXaxis()->SetBinLabel(9, "Number EF");

  EventNumbers->LabelsOption("u","X");

  nL1SelWt = lum*crosssec*double(nL1Sel)/double(nEvent);
  nL2CalSelWt = lum*crosssec*double(nL2CalSel)/double(nEvent);
  nL2SelWt = lum*crosssec*double(nL2Sel)/double(nEvent);
  nEFSelWt = lum*crosssec*double(nEFSel)/double(nEvent);

  TH1F *Rates = new TH1F("TTPValRateTable_Rates_"+menu1.menuName(menu), "Rates", 10, 0, 10);

  Rates->SetBinContent(1, nL1SelWt);
  Rates->GetXaxis()->SetBinLabel(1, "L1 Rate");
  
  Rates->SetBinContent(2, nL2SelWt);
  Rates->GetXaxis()->SetBinLabel(2, "L2 Rate");
  
  Rates->SetBinContent(3, nEFSelWt);
  Rates->GetXaxis()->SetBinLabel(3, "EF Rate");

  Rates->LabelsOption("u","X");

  TH1F *Efficiencies = new TH1F("TTPValRateTable_Efficiencies_"+ menu1.menuName(menu), "Cumulated Efficiencies", 10, 0, 10);

  if (nEvent != 0 ) {
    Efficiencies->SetBinContent(1, (float)nL1Sel/nEvent);
    Efficiencies->GetXaxis()->SetBinLabel(1, "L1 Efficiency");
    
    Efficiencies->SetBinContent(2, (float)nL2CalSel/nEvent);
    Efficiencies->GetXaxis()->SetBinLabel(2, "L2Calo Efficiency");
    
    Efficiencies->SetBinContent(3, (float)nL2Sel/nEvent);
    Efficiencies->GetXaxis()->SetBinLabel(3, "L2Trk Efficiency");
    
    Efficiencies->SetBinContent(4, (float)nEFSel/nEvent);
    Efficiencies->GetXaxis()->SetBinLabel(4, "EF Efficiency");
    
  }
  else cout << "Due to lack of statistics, all efficiencies set to zero." << endl;
  Efficiencies->LabelsOption("u", "X");

  EventNumbers->Write();
  Rates->Write();
  Efficiencies->Write();
  f.Close();
}



void TTPValRateTable::printCSC(TPostScript & ps, double lum, double crosssec, vector<double> &counter) {

  TrigTauFlags menu1;
  TH1F * Rates = new TH1F("TTPValRateTable_Rates_"+menu1.menuName(menu), "Rates", 10, 0, 10);

  if (debug) cout << "Numbers for calculation of L1 are " << nL1Sel << ", " << nEvent << endl;

  if (nEvent != 0 ) {
    nL1SelWt = lum*crosssec*double(nL1Sel)/double(nEvent);
    nL2CalSelWt = lum*crosssec*double(nL2CalSel)/double(nEvent);
    nL2SelWt = lum*crosssec*double(nL2Sel)/double(nEvent);
    nEFSelWt = lum*crosssec*double(nEFSel)/double(nEvent);
  }
  else {
    nL1SelWt=-999999;
    nL2CalSelWt=-999999;
    nL2SelWt=-999999;
    nEFSelWt=-999999;
  }

  if (debug) cout << "Numbers to add are " << nEFSelWt << ", " << nL2CalSelWt << ", " << nL2SelWt << ", " << nEFSelWt << endl;

  if (debug) cout << "Before adding, counter has value " << counter[0] << ", " << counter[1] << ", " << counter[2] << ", " << counter[3] << endl;

//  vector<double> counter;
  //counter.push_back(counterOld[0] + nL1SelWt);
   //counter.push_back(counterOld[1] + nL2CalSelWt);
  //counter.push_back(counterOld[2] + nL2SelWt);
  //counter.push_back(counterOld[3] + nEFSelWt);

  counter[0] = counter[0] + nL1SelWt;
  counter[1] = counter[1] + nL2CalSelWt;
  counter[2] += nL2SelWt;
  counter[3] += nEFSelWt;
 
  if (debug) cout << "After adding, counter has value " << counter[0] << ", " << counter[1] << ", " << counter[2] << ", " << counter[3] << endl;
 
  Rates->SetBinContent(1, nL1SelWt);
  Rates->GetXaxis()->SetBinLabel(1, "L1 Rate");

  Rates->SetBinContent(2, nL2CalSelWt);
  Rates->GetXaxis()->SetBinLabel(2, "L2Cal Rate");
  
  Rates->SetBinContent(3, nL2SelWt);
  Rates->GetXaxis()->SetBinLabel(3, "L2Trk Rate");
  
  Rates->SetBinContent(4, nEFSelWt);
  Rates->GetXaxis()->SetBinLabel(4, "EF Rate");
  
  Rates->LabelsOption("u","X");
  Rates->GetYaxis()->SetTitle("Rates for "+filename);

 
  TCanvas * c1 = (TCanvas *)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,800,500);
  c1->cd();
  ps.On();

 
  
  Rates->Draw();
  c1->Update();
  ps.Off();
  return;
}

void TTPValRateTable::reset() {
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

  nPrintL1 = 0;
  nPrintL2 = 0;
  nPrintEF = 0;

}

