/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValMultiPlots.h"


TTPValMultiPlots::TTPValMultiPlots ( TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1 ) 
  : bugfix(version, poolfile) {
  
  initialize(debug1);
  fileDefined = false;
  return;
}

TTPValMultiPlots::TTPValMultiPlots (TString filename1, TrigTauBugFix::POOLFILE poolfile,  TrigTauBugFix::TTPVERSION version,  bool debug1)
  : bugfix(version, poolfile)  {

  filename = filename1;
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1);
  fileDefined = true;
  return;
}

void TTPValMultiPlots::initialize(bool debug1) {

  debug = debug1;
  signal = true;
  nprong = 0;
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU16;
  cutLevel = TrigTauSelectionManager::LOOSE;
  offlineType =  TrigTauMCTauList::TAUCUTSAFEMEDIUM;
  menuVersion = TrigTauSelectionManager::MENU1560;
  chainType = TrigTauFlags::SITRK;

  nEvent = 0;
  nTrue = 0;
  nGood = 0;
  nMatched = 0;

  npt =50;
  minpt = 0;
  maxpt = 120;

  color[0] = 51;
  color[1] = 8;
  color[2] = 60;
  color[3] = 96;
  color[4] = 66;
  color[5] = 100;
  color[6] = 84;
 
  style[0] = 20;
  style[1] = 26;
  style[2] = 21;
  style[3] = 24;
  style[4] = 23;
  style[5] = 25;
  style[6] = 22;


}

void TTPValMultiPlots::setFile(TString filename1) {
  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename);
  if (debug) cout << "file " << filename << " added. " << endl;
  fileDefined = true;
  return;
} 

void TTPValMultiPlots::analyze() {

  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return; 
  }

  selmanager.setMenuVersion(menuVersion);
  //selmanager.setTauMenu(menu, cutLevel );
  selmanager.setL1MET(0.);  

  // MC truth
  analyzer.truthList.setMaxVisEta(maxVisEta);
  //analyzer.truthList.setMinVisPt(menu); // in GeV
  //analyzer.truthList.setMinLeadingPt(minLeadingPt); // pt of the leading pt of track is
  if( nprong == 1       )  analyzer.truthList.select1Prong();
  else  if( nprong == 3 )  analyzer.truthList.select3Prong();
  //analyzer.truthList.setRecoMatchdR(0.2);

  event  = new TrigTauEvent();
  chain->SetBranchAddress("EventBranch", &event);
  chain->SetBranchStatus("*", 1);

  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(0.);
  analyzer.trigMan.setMenuVersion(menuVersion);
  analyzer.trigMan.setTauMenu(menu, cutLevel);
  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(chainType);

  gROOT->cd();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gROOT->cd();

  theMenu[0] = TrigTauFlags::TAU12;
  theMenu[1] = TrigTauFlags::TAU16;
  theMenu[2] = TrigTauFlags::TAU20;
  theMenu[3] = TrigTauFlags::TAU29;
  theMenu[4] = TrigTauFlags::TAU38;
  theMenu[5] = TrigTauFlags::TAU50;
  theMenu[6] = TrigTauFlags::TAU84;
  //theMenu[3] = TrigTauFlags::TAU125;

  for(unsigned int im = 0; im < nMenus; ++im )
    test[im] = TrigTauFlags::menuName(theMenu[im]);

  
  
  hMC_InVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  hMCVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  
  hntrak = new TH1F("","",20,0.,10.);
  
  hMC_InVisEta = new TH1F("", "", 31, -3.1, 3.1);
  hMCVisEta = new TH1F("", "", 31, -3.1, 3.1);
  
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    nMatch[iMenu]=0;
    nL1passed[iMenu]=0;
    nL2passed[iMenu]=0;
    nEFpassed[iMenu]=0;
    n_OfflineMatchEvent[iMenu]=0;
    n_L1MatchEvent[iMenu]=0;
    n_L2MatchEvent[iMenu]=0;
    n_EFMatchEvent[iMenu]=0;
  
    hEFnTracks[iMenu] = new TH1F(*hntrak);
    hL1VisPt[iMenu] = new TH1F(*hMCVisPt);
    hL2VisPt[iMenu] = new TH1F(*hMCVisPt);
    hEFVisPt[iMenu] = new TH1F(*hMCVisPt);
    hL1PtEff[iMenu] = new TH1F(*hMCVisPt);
    hL2PtEff[iMenu] = new TH1F(*hMCVisPt);
    hEFPtEff[iMenu] = new TH1F(*hMCVisPt);
    hL21PtEff[iMenu] = new TH1F(*hMCVisPt);
    hEFL2PtEff[iMenu] = new TH1F(*hMCVisPt);
    hL1PtEff[iMenu]->Sumw2();
    hL2PtEff[iMenu]->Sumw2();
    hEFPtEff[iMenu]->Sumw2();
    hL21PtEff[iMenu]->Sumw2();
    hEFL2PtEff[iMenu]->Sumw2();
  
    hL1VisEta[iMenu] = new TH1F(*hMCVisEta);
    hL2VisEta[iMenu] = new TH1F(*hMCVisEta);
    hEFVisEta[iMenu] = new TH1F(*hMCVisEta);
    hL1EtaEff[iMenu] = new TH1F(*hMCVisEta);
    hL2EtaEff[iMenu] = new TH1F(*hMCVisEta);
    hEFEtaEff[iMenu] = new TH1F(*hMCVisEta);
    hL21EtaEff[iMenu] = new TH1F(*hMCVisEta);
    hEFL2EtaEff[iMenu] = new TH1F(*hMCVisEta);
    hL1EtaEff[iMenu]->Sumw2();
    hL2EtaEff[iMenu]->Sumw2();
    hEFEtaEff[iMenu]->Sumw2();
    hL21EtaEff[iMenu]->Sumw2();
    hEFL2EtaEff[iMenu]->Sumw2();
  }

  TrigTauBugFix bugfix (TrigTauBugFix::TTP9, TrigTauBugFix::RDO);
  if (debug) cout << "Looping over all events" << endl;
  int nev = int(chain->GetEntries());
  //if ( nev > 100 ) nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    analyzeEvent( iev );
  }
}

void TTPValMultiPlots::analyzeEvent(int eventNumber) {

  event->reset();
  chain->GetEvent(eventNumber);
  bugfix.fix(event);

  analyzer.analyze(event, signal, offlineType);

  nEvent++;
  nTrue += analyzer.truthList.nMCTauPure();
  nGood += analyzer.truthList.nMCTauGood();

  nTau = analyzer.truthList.nMCTauGood();
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    nMatch[iMenu]=0;
    nL1passed[iMenu]=0;
    nL2passed[iMenu]=0;
    nEFpassed[iMenu]=0;
  }

  for(unsigned int itau = 0; itau < nTau; ++itau ) {
      
    const TrigTauMCTau* tau =  &analyzer.truthList.mcTauGoodEntry(itau);

      float visPt = tau->visible().Pt()/1000.;
      float visEta = tau->visible().Eta();
      if (debug) cout << "True visible Pt and Eta found" << endl;

      // Fill denominator with all good taus
      hMC_InVisPt->Fill(visPt);
      hMC_InVisEta->Fill(visEta);
      if (debug) cout << "hMC_InVisPt filled" << endl;
      nMatched++;
      if (debug) cout << "Now filling hMcVisPt..." << endl;
      hMCVisPt->Fill(visPt);
      hMCVisEta->Fill(visEta);
  
      if (debug) cout << "Looping over all Menus...." << endl;
      for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {

        if(visPt> Threshold[iMenu]) nMatch[iMenu]++;
	
        // Select the new menu
        selmanager.setTauMenu(theMenu[iMenu], cutLevel);
	
        // Refill the selected chain list based on the new menu
        analyzer.chainList.fillTriggerList(selmanager);

        // Find a matching L1 object
        const TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
        if (l1Tau != 0) {
          // Found a L1 trigger within dR range, fill numerator histograms
          // with MC truth Pt and Eta
          hL1VisPt[iMenu]->Fill(visPt);
          hL1VisEta[iMenu]->Fill(visEta);
          if(visPt> Threshold[iMenu]) nL1passed[iMenu]++;
        }
	const TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
        if (l2Tau != 0) {
          hL2VisPt[iMenu]->Fill(visPt);
          hL2VisEta[iMenu]->Fill(visEta);
          if(visPt> Threshold[iMenu]) nL2passed[iMenu]++;
        }

        // Find a matching EF object
        const TrigTauRecEFObject* efTau = analyzer.chainList.getEFTau(*tau);
        if (efTau != 0) {
          hEFVisPt[iMenu]->Fill(visPt);
          hEFVisEta[iMenu]->Fill(visEta);
          if(visPt> Threshold[iMenu]) nEFpassed[iMenu]++;
          if(visPt> Threshold[iMenu]) hEFnTracks[iMenu]->Fill(efTau->nTracks());
        }
	
      }  // End of loop over menus
  }  // End of loop over good taus in the event
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    
    if(nMatch[iMenu]>0) n_OfflineMatchEvent[iMenu]++;
    if(nL1passed[iMenu]>0) n_L1MatchEvent[iMenu]++;
    if(nL2passed[iMenu]>0) n_L2MatchEvent[iMenu]++;
    if(nEFpassed[iMenu]>0) n_EFMatchEvent[iMenu]++; 
  }
}
  
void TTPValMultiPlots::print() {

  gROOT->cd();
  
  // Make ratios
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    hL1PtEff[iMenu]->  Divide(hL1VisPt[iMenu],  hMCVisPt,  1., 1., "B");
    hL2PtEff[iMenu]->  Divide(hL2VisPt[iMenu],  hMCVisPt,  1., 1., "B");
    hEFPtEff[iMenu]->  Divide(hEFVisPt[iMenu],  hMCVisPt,  1., 1., "B");

    hL21PtEff[iMenu]-> Divide(hL2VisPt[iMenu],  hL1VisPt[iMenu],  1., 1., "B");
    hEFL2PtEff[iMenu]-> Divide(hEFVisPt[iMenu],  hL2VisPt[iMenu],  1., 1.,"B");
  
    hL1EtaEff[iMenu]-> Divide(hL1VisEta[iMenu], hMCVisEta, 1., 1., "B");
    hL2EtaEff[iMenu]-> Divide(hL2VisEta[iMenu], hMCVisEta, 1., 1., "B");
    hEFEtaEff[iMenu]-> Divide(hEFVisEta[iMenu], hMCVisEta, 1., 1., "B");
  
    hL21EtaEff[iMenu]->Divide(hL2VisEta[iMenu], hL1VisEta[iMenu], 1., 1., "B");
    hEFL2EtaEff[iMenu]->Divide(hEFVisEta[iMenu], hL2VisEta[iMenu], 1., 1., "B");
  }
  TString plotfile = "effMultiPlots";
  //plotfile += append;
  
  // Titles
  for(unsigned int i=0; i<nMenus; i++){
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("Efficiency");
  }

  hL21PtEff[0]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
  hL21PtEff[0]->GetYaxis()->SetTitle("L2/L1 efficiency");
  hEFL2PtEff[0]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
  hEFL2PtEff[0]->GetYaxis()->SetTitle("EF/L2 efficiency");

  hL1EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hL1EtaEff[0]->GetYaxis()->SetTitle("L1 Efficiency");
  hL2EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hL2EtaEff[0]->GetYaxis()->SetTitle("L2 Efficiency");
  hEFEtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hEFEtaEff[0]->GetYaxis()->SetTitle("Efficiency");

  hL21EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hL21EtaEff[0]->GetYaxis()->SetTitle("L2/L1 Efficiency");
  hEFL2EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hEFL2EtaEff[0]->GetYaxis()->SetTitle("EF/L2 Efficiency");

  


  //gStyle->SetHistLineWidth(2);
  gStyle->SetCanvasColor(0);

  TCanvas * c4 = (TCanvas *) gROOT->Get("c4");
  if( c4 ) delete c4;
  c4 = new TCanvas("c4","EF Selected",10,10,800,620);
  c4->cd();
  gStyle->SetHistLineColor(1);
  TLegend *l = new TLegend(0.75,0.2,0.9,0.5);
  l->SetFillStyle(0);

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

  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    gStyle->SetHistLineColor(color[iMenu]);
    //if(iMenu==3 || iMenu==5) continue;
    //if (iMenu==2) continue;

    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->UseCurrentStyle();
    hEFPtEff[iMenu]->SetMaximum(1.05);
    if (iMenu==0) hEFPtEff[iMenu]->Draw("P");
    else hEFPtEff[iMenu]->Draw("PSAME");
   
    l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");

    //gStyle->SetMarkerStyle(style[iMenu]);
    //gStyle->SetMarkerColor(color[iMenu]);
    //hEFPtEff[iMenu]->UseCurrentStyle();
    //hEFPtEff[iMenu]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");
  }
  l->Draw("SAME");
  ATLAS_LABEL(0.15,0.85);





  for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {
    // Select the new menu
    cout << endl << "Menu selection " << iMenu << endl << endl;
    selmanager.setTauMenu(theMenu[iMenu], cutLevel);
    selmanager.print();
  }

  for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {

    cout<< "n_L1MatchEvent[iMenu] "<<n_L1MatchEvent[iMenu]<<endl;
    cout<< "n_OfflineMatchEvent[iMenu] "<<n_OfflineMatchEvent[iMenu]<<endl;

    cout << " L1 efficiency menu "<<iMenu<<" is "<< ((float)n_L1MatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;
    cout << " L2 efficiency menu "<<iMenu<<" is "<< ((float)n_L2MatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;
    cout << " EF efficiency menu "<<iMenu<<" is "<< ((float)n_EFMatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;
  }

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << endl;
  cout << "Matched Taus:    " << nMatched << " -> "
       << float(nMatched)/nEvent << " Matched/Ev" << endl;

}

void TTPValMultiPlots::printCSC(TPostScript & ps) {
  
   gROOT->cd();
  
  // Make ratios
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    hL1PtEff[iMenu]->  Divide(hL1VisPt[iMenu],  hMCVisPt,  1., 1., "B");
    hL2PtEff[iMenu]->  Divide(hL2VisPt[iMenu],  hMCVisPt,  1., 1., "B");
    hEFPtEff[iMenu]->  Divide(hEFVisPt[iMenu],  hMCVisPt,  1., 1., "B");

    hL21PtEff[iMenu]-> Divide(hL2VisPt[iMenu],  hL1VisPt[iMenu],  1., 1., "B");
    hEFL2PtEff[iMenu]-> Divide(hEFVisPt[iMenu],  hL2VisPt[iMenu],  1., 1.,"B");
  }
  TString plotfile = "effMultiPlots";
  
  // Titles
  for(unsigned int i=0; i<nMenus; i++){
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("EF Efficiency");
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

  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    gStyle->SetHistLineColor(color[iMenu]);
    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hL1PtEff[iMenu]->SetLineColor(color[iMenu]);
    hL1PtEff[iMenu]->UseCurrentStyle();
    hL1PtEff[iMenu]->SetMaximum(1.05);
    if (iMenu==0) hL1PtEff[iMenu]->Draw("P");
    else hL1PtEff[iMenu]->Draw("PSAME");
    l->AddEntry(hL1PtEff[iMenu],test[iMenu],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();
  
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    gStyle->SetHistLineColor(color[iMenu]);
    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hL2PtEff[iMenu]->SetLineColor(color[iMenu]);
    hL2PtEff[iMenu]->UseCurrentStyle();
    hL2PtEff[iMenu]->SetMaximum(1.05);
    if (iMenu==0) hL2PtEff[iMenu]->Draw("P");
    else hL2PtEff[iMenu]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    gStyle->SetHistLineColor(color[iMenu]);
    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->UseCurrentStyle();
    hEFPtEff[iMenu]->SetMaximum(1.05);
    if (iMenu==0) hEFPtEff[iMenu]->Draw("P");
    else hEFPtEff[iMenu]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {
    // Select the new menu
    cout << endl << "Menu selection " << iMenu << endl << endl;
    selmanager.setTauMenu(theMenu[iMenu], cutLevel);
    selmanager.print();
  }

  for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {

    cout<< "n_L1MatchEvent[iMenu] "<<n_L1MatchEvent[iMenu]<<endl;
    cout<< "n_OfflineMatchEvent[iMenu] "<<n_OfflineMatchEvent[iMenu]<<endl;

    cout << " L1 efficiency menu "<<iMenu<<" is "<< ((float)n_L1MatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;
    cout << " L2 efficiency menu "<<iMenu<<" is "<< ((float)n_L2MatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;
    cout << " EF efficiency menu "<<iMenu<<" is "<< ((float)n_EFMatchEvent[iMenu])/((float)n_OfflineMatchEvent[iMenu]) <<endl;

   ps.Off();

  }

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << endl;
  cout << "Matched Taus:    " << nMatched << " -> "
       << float(nMatched)/nEvent << " Matched/Ev" << endl;

}


void TTPValMultiPlots::update(TString name) {

  TFile f(name, "update");

  f.cd();
  f.mkdir("MultiPlots");
  f.cd("MultiPlots");


  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
   hL1PtEff[iMenu]->  Divide(hL1VisPt[iMenu],  hMCVisPt,  1., 1., "B");
   hL2PtEff[iMenu]->  Divide(hL2VisPt[iMenu],  hMCVisPt,  1., 1., "B");
   hEFPtEff[iMenu]->  Divide(hEFVisPt[iMenu],  hMCVisPt,  1., 1., "B");
 }

  // Titles
  for(unsigned int i=0; i<nMenus; i++){
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiency");
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency");
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("Efficiency");
  }
  
  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {

    gStyle->SetHistLineColor(color[iMenu]);
    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hL1PtEff[iMenu]->SetLineColor(color[iMenu]);
    hL1PtEff[iMenu]->UseCurrentStyle();
    hL1PtEff[iMenu]->SetMaximum(1.05);
    //l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");

    hL2PtEff[iMenu]->SetLineColor(color[iMenu]);
    hL2PtEff[iMenu]->UseCurrentStyle();
    hL2PtEff[iMenu]->SetMaximum(1.05);

    hEFPtEff[iMenu]->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->UseCurrentStyle();
    hEFPtEff[iMenu]->SetMaximum(1.05);


    TrigTauFlags test;
    hL1PtEff[iMenu]->SetName("TTPVAlMultiPlots_" + test.menuName(theMenu[iMenu])+"_L1");
    hL1PtEff[iMenu]->Write();
    hL2PtEff[iMenu]->SetName("TTPVAlMultiPlots_" + test.menuName(theMenu[iMenu])+"_L2");
    hL2PtEff[iMenu]->Write();
    hEFPtEff[iMenu]->SetName("TTPVAlMultiPlots_" + test.menuName(theMenu[iMenu])+"_EF");
    hEFPtEff[iMenu]->Write();    
  }
  f.Close();
}


void TTPValMultiPlots::ATLAS_LABEL(Double_t x,Double_t y,Color_t color) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS Preliminary");
}
