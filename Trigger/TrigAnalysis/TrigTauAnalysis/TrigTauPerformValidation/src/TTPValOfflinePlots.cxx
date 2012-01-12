/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValOfflinePlots.h"


TTPValOfflinePlots::TTPValOfflinePlots ( TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1 ) 
  : bugfix(version, poolfile) {
  
  initialize(debug1);
  fileDefined = false;
  return;
}

TTPValOfflinePlots::TTPValOfflinePlots (TString filename1, TrigTauBugFix::POOLFILE poolfile,  TrigTauBugFix::TTPVERSION version,  bool debug1)
  : bugfix(version, poolfile)  {

  filename = filename1;
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1);
  fileDefined = true;
  if (debug) cout << "Finishing constructor..." << endl;
  return;
}

void TTPValOfflinePlots::initialize(bool debug1) {

  debug = debug1;
  if (debug) cout << "Initializing variables..." << endl;
  signal = true;
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU16;
  cutLevel = TrigTauSelectionManager::LOOSE;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK;
  threshold = 5;
  nEvent = 0;

  int i = 0;  

  if (debug) cout << "Setting counters to zero..." << endl;

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
      nTrue[iOffline][iNprong] = 0;
      nGood[iOffline][iNprong] = 0;
      nMatched[iOffline][iNprong] = 0;
      ++i;
      color[iOffline][iNprong] = i;
      style[iOffline][iNprong] = 19+i;
    }
  
  if (debug) cout << "Finishing initialization..." << endl;

  npt =40;
  minpt = 0;
  maxpt = 80;

  //color[0][0] = 1;
  //color[0][1] = 2;
  //color[1][0] = 60;
  //color[1][1] = 96;
  //color[2][0] = 66;
  //color[2][1] = 100;
  //color[6] = 84;
 
  //style[0][0] = 20;
  //style[0][1] = 26;
  //style[1][0] = 21;
  //style[1][1] = 24;
  //style[2][0] = 23;
  //style[2][1] = 25;
  //style[6] = 22;

  if (debug) cout << "Initialization finished... " << endl;
}

void TTPValOfflinePlots::setFile(TString filename1) {
  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename);
  if (debug) cout << "file " << filename << " added. " << endl;
  fileDefined = true;
  return;
} 

void TTPValOfflinePlots::analyze() {

  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return; 
  }


  selmanager.setMenuVersion(menuVersion);
  //selmanager.setTauMenu(menu, cutLevel );
  selmanager.setL1MET(0.);  

  event  = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);

  gROOT->cd();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gROOT->cd();

  offlineType[0] =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  //offlineType[1] =  TrigTauMCTauList::TAULLHMEDIUM;
  //offlineType[2] =  TrigTauMCTauList::EITHERTAU;

  nprong[0] = 1;
  nprong[1] = 3;  

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
  
      // MC truth
      analyzer[iOffline][iNprong].truthList.setMaxVisEta(maxVisEta);
      //analyzer[iOffline][iNprong].truthList.setMinVisPt(menu); // in GeV
      analyzer[iOffline][iNprong].truthList.setMinLeadingPt(minLeadingPt); // pt of the leading pt of track is
     //analyzer.truthList.setRecoMatchdR(0.2);
 
      // Also set an L1 MET cut if desired
      analyzer[iOffline][iNprong].trigMan.setL1MET(0.);
      analyzer[iOffline][iNprong].trigMan.setMenuVersion(menuVersion);
      analyzer[iOffline][iNprong].trigMan.setTauMenu(menu, cutLevel);
      analyzer[iOffline][iNprong].chainList.setRoiWordMatching(false);
      analyzer[iOffline][iNprong].chainList.setHLTAuthor(chainType);
    
      hMC_InVisPt[iOffline][iNprong] = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
      hMCVisPt[iOffline][iNprong] = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)

      hMC_InVisEta[iOffline][iNprong] = new TH1F("", "", 31, -3.1, 3.1);
      hMCVisEta[iOffline][iNprong] = new TH1F("", "", 31, -3.1, 3.1);

      nTrue[iOffline][iNprong]=0;
      nGood[iOffline][iNprong]=0;
      nMatch[iOffline][iNprong]=0;
      nL1passed[iOffline][iNprong]=0;
      nL2passed[iOffline][iNprong]=0;
      nEFpassed[iOffline][iNprong]=0;
      n_OfflineMatchEvent[iOffline][iNprong]=0;
      n_L1MatchEvent[iOffline][iNprong]=0;
      n_L2MatchEvent[iOffline][iNprong]=0;
      n_EFMatchEvent[iOffline][iNprong]=0;
  
     // hEFnTracks[iOffline][iNprong] = new TH1F(*hntrak[iOffline][iNprong]);
      hL1VisPt[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hL2VisPt[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hEFVisPt[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hL1PtEff[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hL2PtEff[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hEFPtEff[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hL21PtEff[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hEFL2PtEff[iOffline][iNprong] = new TH1F(*hMCVisPt[iOffline][iNprong]);
      hL1PtEff[iOffline][iNprong]->Sumw2();
      hL2PtEff[iOffline][iNprong]->Sumw2();
      hEFPtEff[iOffline][iNprong]->Sumw2();
      hL21PtEff[iOffline][iNprong]->Sumw2();
      hEFL2PtEff[iOffline][iNprong]->Sumw2();
  
      hL1VisEta[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hL2VisEta[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hEFVisEta[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hL1EtaEff[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hL2EtaEff[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hEFEtaEff[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hL21EtaEff[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hEFL2EtaEff[iOffline][iNprong] = new TH1F(*hMCVisEta[iOffline][iNprong]);
      hL1EtaEff[iOffline][iNprong]->Sumw2();
      hL2EtaEff[iOffline][iNprong]->Sumw2();
      hEFEtaEff[iOffline][iNprong]->Sumw2();
      hL21EtaEff[iOffline][iNprong]->Sumw2();
      hEFL2EtaEff[iOffline][iNprong]->Sumw2();
    }

  TrigTauBugFix bugfix (TrigTauBugFix::TTP14, TrigTauBugFix::RDO);
  if (debug) cout << "Looping over all events" << endl;
  int nev = int(chain->GetEntries());
  //if ( nev > 100 ) nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    if (debug) cout << "Analyzing event number " << iev << endl;
    analyzeEvent( iev );
  }
}

void TTPValOfflinePlots::analyzeEvent(int eventNumber) {

  if (debug) cout << "Starting 'analyzeEvent' " << endl; 
  event->reset();
  chain->GetEvent(eventNumber);
  if (debug) cout << "Chain loaded" << endl;
  bugfix.fix(event);
  if (debug) cout << "Bugs are fixed.." << endl; 
  nEvent++;

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {

      if (debug) cout << "Starting with combination "<< iOffline << " and " << iNprong << endl; 
      analyzer[iOffline][iNprong].analyze(event, signal, offlineType[iOffline]);
      if( nprong[iNprong] == 1) analyzer[iOffline][iNprong].truthList.select1Prong();
      else if( nprong[iNprong] == 3 ) analyzer[iOffline][iNprong].truthList.select3Prong();
      
      nTrue[iOffline][iNprong] += analyzer[iOffline][iNprong].truthList.nMCTauPure();
      nGood[iOffline][iNprong] += analyzer[iOffline][iNprong].truthList.nMCTauGood();

      nTau = analyzer[iOffline][iNprong].truthList.nMCTauGood();
  
      nMatch[iOffline][iNprong]=0;
      nL1passed[iOffline][iNprong]=0;
      nL2passed[iOffline][iNprong]=0;
      nEFpassed[iOffline][iNprong]=0;
   
      for(unsigned int itau = 0; itau < nTau; ++itau ) {
      
        const TrigTauMCTau* tau =  &analyzer[iOffline][iNprong].truthList.mcTauGoodEntry(itau);

        float visPt = tau->visible().Pt()/1000.;
        float visEta = tau->visible().Eta();

        // Fill denominator with all good taus
        hMC_InVisPt[iOffline][iNprong]->Fill(visPt);
        hMC_InVisEta[iOffline][iNprong]->Fill(visEta);
        if (debug) cout << "hMC_InVisPt filled" << endl;
        nMatched[iOffline][iNprong]++;
        if (debug) cout << "Now filling hMcVisPt..." << endl;
        hMCVisPt[iOffline][iNprong]->Fill(visPt);
        hMCVisEta[iOffline][iNprong]->Fill(visEta);
  
        if(visPt> threshold) nMatch[iOffline][iNprong]++;
	
        // Find a matching L1 object
        const TrigTauL1Object* l1Tau = analyzer[iOffline][iNprong].chainList.getL1Tau(*tau);
        if (l1Tau != 0) {
          // Found a L1 trigger within dR range, fill numerator histograms
          // with MC truth Pt and Eta
          hL1VisPt[iOffline][iNprong]->Fill(visPt);
          hL1VisEta[iOffline][iNprong]->Fill(visEta);
          if(visPt> threshold) nL1passed[iOffline][iNprong]++;
        }
	const TrigTauL2Object* l2Tau = analyzer[iOffline][iNprong].chainList.getL2Tau(*tau);
        if (l2Tau != 0) {
          hL2VisPt[iOffline][iNprong]->Fill(visPt);
          hL2VisEta[iOffline][iNprong]->Fill(visEta);
          if(visPt> threshold) nL2passed[iOffline][iNprong]++;
        }

        // Find a matching EF object
        const TrigTauRecEFObject* efTau = analyzer[iOffline][iNprong].chainList.getEFTau(*tau);
        if (efTau != 0) {
          hEFVisPt[iOffline][iNprong]->Fill(visPt);
          hEFVisEta[iOffline][iNprong]->Fill(visEta);
          if(visPt> threshold) nEFpassed[iOffline][iNprong]++;
          //if(visPt> threshold) hEFnTracks[iOffline][iNprong]->Fill(efTau->nTracks());
        }
	
      }  // End of loop over menus
  }  // End of loop over good taus in the event
  if (debug) cout << "End loop over tood taus" << endl;
 
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) { 
      if(nMatch[iOffline][iNprong]>0) n_OfflineMatchEvent[iOffline][iNprong]++;
      if(nL1passed[iOffline][iNprong]>0) n_L1MatchEvent[iOffline][iNprong]++;
      if(nL2passed[iOffline][iNprong]>0) n_L2MatchEvent[iOffline][iNprong]++;
      if(nEFpassed[iOffline][iNprong]>0) n_EFMatchEvent[iOffline][iNprong]++; 
  }
}
  
void TTPValOfflinePlots::print() {

  gROOT->cd();
  
  // Make ratios
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
   
    hL1PtEff[iOffline][iNprong]->  Divide(hL1VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
    hL2PtEff[iOffline][iNprong]->  Divide(hL2VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
    hEFPtEff[iOffline][iNprong]->  Divide(hEFVisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");

    hL21PtEff[iOffline][iNprong]-> Divide(hL2VisPt[iOffline][iNprong],  hL1VisPt[iOffline][iNprong],  1., 1., "B");
    hEFL2PtEff[iOffline][iNprong]-> Divide(hEFVisPt[iOffline][iNprong],  hL2VisPt[iOffline][iNprong],  1., 1.,"B");
  
    hL1EtaEff[iOffline][iNprong]-> Divide(hL1VisEta[iOffline][iNprong], hMCVisEta[iOffline][iNprong], 1., 1., "B");
    hL2EtaEff[iOffline][iNprong]-> Divide(hL2VisEta[iOffline][iNprong], hMCVisEta[iOffline][iNprong], 1., 1., "B");
    hEFEtaEff[iOffline][iNprong]-> Divide(hEFVisEta[iOffline][iNprong], hMCVisEta[iOffline][iNprong], 1., 1., "B");
  
    hL21EtaEff[iOffline][iNprong]->Divide(hL2VisEta[iOffline][iNprong], hL1VisEta[iOffline][iNprong], 1., 1., "B");
    hEFL2EtaEff[iOffline][iNprong]->Divide(hEFVisEta[iOffline][iNprong], hL2VisEta[iOffline][iNprong], 1., 1., "B");
  }
  TString plotfile = "effMultiPlots";
  //plotfile += append;
  
  // Titles
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
    hL1PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[iOffline][iNprong]->GetYaxis()->SetTitle("Efficiency");
    hL21PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL21PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2/L1 efficiency");
    hEFL2PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFL2PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("EF/L2 efficiency");

    hL1EtaEff[iOffline][iNprong]->GetXaxis()->SetTitle("True Visible Eta");
    hL1EtaEff[iOffline][iNprong]->GetYaxis()->SetTitle("L1 Efficiency");
    hL2EtaEff[iOffline][iNprong]->GetXaxis()->SetTitle("True Visible Eta");
   hL2EtaEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2 Efficiency");
    hEFEtaEff[iOffline][iNprong]->GetXaxis()->SetTitle("True Visible Eta");
    hEFEtaEff[iOffline][iNprong]->GetYaxis()->SetTitle("Efficiency");

    hL21EtaEff[iOffline][iNprong]->GetXaxis()->SetTitle("True Visible Eta");
    hL21EtaEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2/L1 Efficiency");
    hEFL2EtaEff[iOffline][iNprong]->GetXaxis()->SetTitle("True Visible Eta");
    hEFL2EtaEff[iOffline][iNprong]->GetYaxis()->SetTitle("EF/L2 Efficiency");

  }


  //gStyle->SetHistLineWidth(2);
  gStyle->SetCanvasColor(0);

  TCanvas * c4 = (TCanvas *) gROOT->Get("c4");
  if( c4 ) delete c4;
  c4 = new TCanvas("c4","EF Selected",10,10,800,620);
  c4->cd();
  gStyle->SetHistLineColor(1);
  TLegend *l = new TLegend(0.75,0.1,0.95,0.5);
  l->SetFillStyle(0);
  //l->SetTextSize(0.9);

  //commented out, since it's done later for all histos
  // int id = 2;
//   gStyle->SetHistLineColor(color[id]);
//   gStyle->SetMarkerStyle(style[id]);
//   gStyle->SetMarkerColor(color[id]);
//   gStyle->SetLineColor(color[id]);
//         gStyle->SetFillColor(0);
//   hEFPtEff[id]->SetLineColor(color[id]);
//   hEFPtEff[id]->UseCurrentStyle();
//   hEFPtEff[id]->SetMaximum(1.05);
//   hEFPtEff[id]->Draw("P");



  //l->AddEntry(hEFPtEff[id],test[id],"p");

  //  TLegend *l = new TLegend(0.13,0.65,0.25,0.85);
  //l->SetFillStyle(0);
  //hEFPtEff[2]->UseCurrentStyle();
  //hEFPtEff[2]->Draw("P");
  //l->AddEntry(hEFPtEff[2],"tau20i","p");
 
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for ( int iNprong = 0; iNprong<nNprong; ++iNprong) {
    gStyle->SetHistLineColor(color[iOffline][iNprong]);
    //if([iOffline][iNprong]==3 || [iOffline][iNprong]==5) continue;
    //if ([iOffline][iNprong]==2) continue;

    gStyle->SetMarkerColor(color[iOffline][iNprong]);
    gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    gStyle->SetLineColor(color[iOffline][iNprong]);
    hEFPtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hEFPtEff[iOffline][iNprong]->UseCurrentStyle();
    hEFPtEff[iOffline][iNprong]->SetMaximum(1.05);
    if (iOffline==0 && iNprong ==0) hEFPtEff[iOffline][iNprong]->Draw("P");
    else hEFPtEff[iOffline][iNprong]->Draw("PSAME");
   
    l->AddEntry(hEFPtEff[iOffline][iNprong],getName(offlineType[iOffline]) + ", " + getName(nprong[iNprong]),"p");

    //gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    //gStyle->SetMarkerColor(color[iOffline][iNprong]);
    //hEFPtEff[iOffline][iNprong]->UseCurrentStyle();
    //hEFPtEff[iOffline][iNprong]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iOffline][iNprong],test[iOffline][iNprong],"p");
  }
  l->Draw("SAME");
  //ATLAS_LABEL(0.15,0.85);
  
  for( int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {

    cout<< "n_L1MatchEvent[iOffline][iNprong] "<<n_L1MatchEvent[iOffline][iNprong]<<endl;
    cout<< "n_OfflineMatchEvent[iOffline][iNprong] "<<n_OfflineMatchEvent[iOffline][iNprong]<<endl;

    cout << " L1 efficiency menu "<< menu <<" is "<< ((float)n_L1MatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;
    cout << " L2 efficiency menu "<< menu <<" is "<< ((float)n_L2MatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;
    cout << " EF efficiency menu "<< menu <<" is "<< ((float)n_EFMatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;
  
  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue[iOffline][iNprong] << endl;
  cout << "Good Taus:    " << nGood[iOffline][iNprong] << endl;
  cout << "Matched Taus:    " << nMatched[iOffline][iNprong] << " -> "
       << float(nMatched[iOffline][iNprong])/nEvent << " Matched/Ev" << endl;
  }
}

void TTPValOfflinePlots::printCSC(TPostScript & ps) {
  
   gROOT->cd();
  
  // Make ratios
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
 
    hL1PtEff[iOffline][iNprong]->  Divide(hL1VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
    hL2PtEff[iOffline][iNprong]->  Divide(hL2VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
    hEFPtEff[iOffline][iNprong]->  Divide(hEFVisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");

    hL21PtEff[iOffline][iNprong]-> Divide(hL2VisPt[iOffline][iNprong],  hL1VisPt[iOffline][iNprong],  1., 1., "B");
    hEFL2PtEff[iOffline][iNprong]-> Divide(hEFVisPt[iOffline][iNprong],  hL2VisPt[iOffline][iNprong],  1., 1.,"B");
  }
  TString plotfile = "effMultiPlots";
  
  // Titles
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
    hL1PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[iOffline][iNprong]->GetYaxis()->SetTitle("EF Efficiency");
  }

  //gStyle->SetHistLineWidth(2);
  gStyle->SetCanvasColor(0);

  TCanvas * c1 = (TCanvas *) gROOT->Get("c4");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","EF Selected",10,10,800,620);
  c1->cd();
  gStyle->SetHistLineColor(1);
  TLegend *l = new TLegend(0.75,0.2,0.9,0.5);
  l->SetFillStyle(0);

  ps.On();

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
    gStyle->SetHistLineColor(color[iOffline][iNprong]);
    gStyle->SetMarkerColor(color[iOffline][iNprong]);
    gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    gStyle->SetLineColor(color[iOffline][iNprong]);
    hL1PtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hL1PtEff[iOffline][iNprong]->UseCurrentStyle();
    hL1PtEff[iOffline][iNprong]->SetMaximum(1.05);
    if (iOffline==0 && iNprong==0) hL1PtEff[iOffline][iNprong]->Draw("P");
    else hL1PtEff[iOffline][iNprong]->Draw("PSAME");
    l->AddEntry(hL1PtEff[iOffline][iNprong],test[iOffline][iNprong],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();
  
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
  
    gStyle->SetHistLineColor(color[iOffline][iNprong]);
    gStyle->SetMarkerColor(color[iOffline][iNprong]);
    gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    gStyle->SetLineColor(color[iOffline][iNprong]);
    hL2PtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hL2PtEff[iOffline][iNprong]->UseCurrentStyle();
    hL2PtEff[iOffline][iNprong]->SetMaximum(1.05);
    if (iOffline == 0 && iNprong == 0) hL2PtEff[iOffline][iNprong]->Draw("P");
    else hL2PtEff[iOffline][iNprong]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iOffline][iNprong],test[iOffline][iNprong],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
    gStyle->SetHistLineColor(color[iOffline][iNprong]);
    gStyle->SetMarkerColor(color[iOffline][iNprong]);
    gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    gStyle->SetLineColor(color[iOffline][iNprong]);
    hEFPtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hEFPtEff[iOffline][iNprong]->UseCurrentStyle();
    hEFPtEff[iOffline][iNprong]->SetMaximum(1.05);
    if (iOffline == 0 && iNprong ==0) hEFPtEff[iOffline][iNprong]->Draw("P");
    else hEFPtEff[iOffline][iNprong]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iOffline][iNprong],test[iOffline][iNprong],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {

    cout<< "n_L1MatchEvent[iOffline][iNprong] "<<n_L1MatchEvent[iOffline][iNprong]<<endl;
    cout<< "n_OfflineMatchEvent[iOffline][iNprong] "<<n_OfflineMatchEvent[iOffline][iNprong]<<endl;

    cout << " L1 efficiency menu "<< menu <<" is "<< ((float)n_L1MatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;
    cout << " L2 efficiency menu "<< menu <<" is "<< ((float)n_L2MatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;
    cout << " EF efficiency menu "<< menu <<" is "<< ((float)n_EFMatchEvent[iOffline][iNprong])/((float)n_OfflineMatchEvent[iOffline][iNprong]) <<endl;

   ps.Off();

  

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue[iOffline][iNprong] << endl;
  cout << "Good Taus:    " << nGood[iOffline][iNprong] << endl;
  cout << "Matched Taus:    " << nMatched[iOffline][iNprong] << " -> "
       << float(nMatched[iOffline][iNprong])/nEvent << " Matched/Ev" << endl;
  } 
}


void TTPValOfflinePlots::update(TString name) {

  TFile f(name, "update");

  f.cd();
  f.mkdir("MultiPlots");
  f.cd("MultiPlots");

  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for ( int iNprong = 0; iNprong<nNprong; ++iNprong) {
     hL1PtEff[iOffline][iNprong]->  Divide(hL1VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
     hL2PtEff[iOffline][iNprong]->  Divide(hL2VisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
     hEFPtEff[iOffline][iNprong]->  Divide(hEFVisPt[iOffline][iNprong],  hMCVisPt[iOffline][iNprong],  1., 1., "B");
 }

  // Titles
  for(int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
    hL1PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[iOffline][iNprong]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[iOffline][iNprong]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[iOffline][iNprong]->GetYaxis()->SetTitle("Efficiency");
  }
  
  for( int iOffline = 0; iOffline<nOffline; ++iOffline)
    for (int iNprong = 0; iNprong<nNprong; ++iNprong) {
 
    gStyle->SetHistLineColor(color[iOffline][iNprong]);
    gStyle->SetMarkerColor(color[iOffline][iNprong]);
    gStyle->SetMarkerStyle(style[iOffline][iNprong]);
    gStyle->SetLineColor(color[iOffline][iNprong]);
    hL1PtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hL1PtEff[iOffline][iNprong]->UseCurrentStyle();
    hL1PtEff[iOffline][iNprong]->SetMaximum(1.05);
    //l->AddEntry(hEFPtEff[iOffline][iNprong],test[iOffline][iNprong],"p");

    hL2PtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hL2PtEff[iOffline][iNprong]->UseCurrentStyle();
    hL2PtEff[iOffline][iNprong]->SetMaximum(1.05);

    hEFPtEff[iOffline][iNprong]->SetLineColor(color[iOffline][iNprong]);
    hEFPtEff[iOffline][iNprong]->UseCurrentStyle();
    hEFPtEff[iOffline][iNprong]->SetMaximum(1.05);


    TrigTauFlags test;
    hL1PtEff[iOffline][iNprong]->SetName("TTPVAlMultiPlots_" + TString(iOffline) + "_" + TString(iNprong)+"_L1");
    hL1PtEff[iOffline][iNprong]->Write();
    hL2PtEff[iOffline][iNprong]->SetName("TTPVAlMultiPlots_" + TString(iOffline) + "_" + TString(iNprong)+"_L2");
    hL2PtEff[iOffline][iNprong]->Write();
    hEFPtEff[iOffline][iNprong]->SetName("TTPVAlMultiPlots_" + TString(iOffline) + "_" + TString(iNprong) +"_EF");
    hEFPtEff[iOffline][iNprong]->Write();    
  }
  f.Close();
}


void TTPValOfflinePlots::ATLAS_LABEL(Double_t x,Double_t y,Color_t color) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS Preliminary");
}

TString TTPValOfflinePlots::getName(TrigTauMCTauList::OFFLINESELECTION selection) {

  if (selection == TrigTauMCTauList::TAUCUTSAFEMEDIUM) return "TauCutSafeMedium";
  else if (selection == TrigTauMCTauList::TAULLHMEDIUM) return "TauLlhMedium";
  else return "Offline selection not known to printout routine";

}

TString TTPValOfflinePlots::getName(int selection) {

  if (selection == 1) return "1Prong";
  else if (selection == 3) return "3Prong";
  else return "allDecays";

}

