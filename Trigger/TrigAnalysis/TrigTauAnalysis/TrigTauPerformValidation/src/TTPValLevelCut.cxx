/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
using namespace std;
#include "TrigTauPerformValidation/TTPValLevelCut.h"


TTPValLevelCut::TTPValLevelCut ( TrigTauBugFix::POOLFILE poolfile, TrigTauBugFix::TTPVERSION version, bool debug1 ) 
  : bugfix(version, poolfile) {
  
  initialize(debug1);
  fileDefined = false;
  return;
}

TTPValLevelCut::TTPValLevelCut (TString filename1, TrigTauBugFix::POOLFILE poolfile,  TrigTauBugFix::TTPVERSION version,  bool debug1)
  : bugfix(version, poolfile)  {

  filename = filename1;
  chain = new TChain("eventTree");
  chain->Add(filename);
  initialize(debug1);
  fileDefined = true;
  return;
}

void TTPValLevelCut::initialize(bool debug1) {

  debug = debug1;
  signal = true;
  nprong = 0;
  maxVisEta = 2.5;
  minLeadingPt = 6.0;
  gStyle->SetPadGridX(true);
  gStyle->SetPadGridY(true);
  menu = TrigTauFlags::TAU16;
  //cutLevel = TrigTauSelectionManager::LOOSE;
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
   
  style[0] = 20;
  style[1] = 26;
  style[2] = 21;
  
}

void TTPValLevelCut::setFile(TString filename1) {

  filename = filename1;
  chain = new TChain("eventTree"); //potential memory leak
  chain->Add(filename);
  if (debug) cout << "file " << filename << " added. " << endl;
  fileDefined = true;
  return;

} 

void TTPValLevelCut::analyze() {

  if (!fileDefined) {
    cout << "File was not defined and variables not initialized!!" << endl;
    return; 
  }

  selmanager.setMenuVersion(menuVersion);
  //selmanager.setTauMenu(menu, cutLevel );
  selmanager.setL1MET(0.);  

  // MC truth
  analyzer.truthList.setMaxVisEta(maxVisEta);
  analyzer.truthList.setMinVisPt(menu); // in GeV
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
  
  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(chainType);

  gROOT->cd();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gROOT->cd();

  theCutLevel[0] = TrigTauSelectionManager::LOOSE;
  theCutLevel[1] = TrigTauSelectionManager::MEDIUM;
  theCutLevel[2] = TrigTauSelectionManager::TIGHT;
  
  test[0] = "LOOSE" ;
  test[1] = "MEDIUM" ;
  test[2] = "TIGHT" ;
  
  hMC_InVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  hMCVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  
  hntrak = new TH1F("","",20,0.,10.);
   
  hMC_InVisEta = new TH1F("", "", 31, -3.1, 3.1);
  hMCVisEta = new TH1F("", "", 31, -3.1, 3.1);
  
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {

    analyzer.trigMan.setTauMenu(menu, theCutLevel[iCutLevel]);

    nMatch[iCutLevel]=0;
    nL1passed[iCutLevel]=0;
    nL2passed[iCutLevel]=0;
    nEFpassed[iCutLevel]=0;
    n_OfflineMatchEvent[iCutLevel]=0;
    n_L1MatchEvent[iCutLevel]=0;
    n_L2MatchEvent[iCutLevel]=0;
    n_EFMatchEvent[iCutLevel]=0;
  
    //hEFnTracks[iCutLevel] = new TH1F(*hntrak);
    hL1VisPt[iCutLevel] = new TH1F(*hMCVisPt);
    hL2VisPt[iCutLevel] = new TH1F(*hMCVisPt);
    hEFVisPt[iCutLevel] = new TH1F(*hMCVisPt);
    hL1PtEff[iCutLevel] = new TH1F(*hMCVisPt);
    hL2PtEff[iCutLevel] = new TH1F(*hMCVisPt);
    hEFPtEff[iCutLevel] = new TH1F(*hMCVisPt);
    hL1PtEff[iCutLevel]->Sumw2();
    hL2PtEff[iCutLevel]->Sumw2();
    hEFPtEff[iCutLevel]->Sumw2();
  
    hL1VisEta[iCutLevel] = new TH1F(*hMCVisEta);
    hL2VisEta[iCutLevel] = new TH1F(*hMCVisEta);
    hEFVisEta[iCutLevel] = new TH1F(*hMCVisEta);
    hL1EtaEff[iCutLevel] = new TH1F(*hMCVisEta);
    hL2EtaEff[iCutLevel] = new TH1F(*hMCVisEta);
    hEFEtaEff[iCutLevel] = new TH1F(*hMCVisEta);
    hL1EtaEff[iCutLevel]->Sumw2();
    hL2EtaEff[iCutLevel]->Sumw2();
    hEFEtaEff[iCutLevel]->Sumw2();
  }

  TrigTauBugFix bugfix (TrigTauBugFix::TTP9, TrigTauBugFix::RDO);
  if (debug) cout << "Looping over all events" << endl;
  int nev = int(chain->GetEntries());
  //if ( nev > 100 ) nev = 100;
  for(int iev = 0; iev<nev; ++iev) {
    analyzeEvent( iev );
  }
}

void TTPValLevelCut::analyzeEvent(int eventNumber) {

  event->reset();
  chain->GetEvent(eventNumber);
  bugfix.fix(event);

  analyzer.analyze(event, signal, offlineType);

  nEvent++;
  nTrue += analyzer.truthList.nMCTauPure();
  nGood += analyzer.truthList.nMCTauGood();

  nTau = analyzer.truthList.nMCTauGood();
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    nMatch[iCutLevel]=0;
    nL1passed[iCutLevel]=0;
    nL2passed[iCutLevel]=0;
    nEFpassed[iCutLevel]=0;
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
  
      if (debug) cout << "Looping over all CutLevels...." << endl;
      for(unsigned int iCutLevel = 0; iCutLevel<nCutLevels; iCutLevel++) {

        // Select the new menu
        selmanager.setTauMenu(menu, theCutLevel[iCutLevel]);
	
        // Refill the selected chain list based on the new menu
        analyzer.chainList.fillTriggerList(selmanager);

        // Find a matching L1 object
        const TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
        if (l1Tau != 0) {
          // Found a L1 trigger within dR range, fill numerator histograms
          // with MC truth Pt and Eta
          hL1VisPt[iCutLevel]->Fill(visPt);
          hL1VisEta[iCutLevel]->Fill(visEta);
        }
	const TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
        if (l2Tau != 0) {
          hL2VisPt[iCutLevel]->Fill(visPt);
          hL2VisEta[iCutLevel]->Fill(visEta);
        }

        // Find a matching EF object
        const TrigTauRecEFObject* efTau = analyzer.chainList.getEFTau(*tau);
        if (efTau != 0) {
          hEFVisPt[iCutLevel]->Fill(visPt);
          hEFVisEta[iCutLevel]->Fill(visEta);
        }
	
      }  // End of loop over menus
  }  // End of loop over good taus in the event
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    
    if(nMatch[iCutLevel]>0) n_OfflineMatchEvent[iCutLevel]++;
    if(nL1passed[iCutLevel]>0) n_L1MatchEvent[iCutLevel]++;
    if(nL2passed[iCutLevel]>0) n_L2MatchEvent[iCutLevel]++;
    if(nEFpassed[iCutLevel]>0) n_EFMatchEvent[iCutLevel]++; 
  }
}
  
void TTPValLevelCut::print() {

  gROOT->cd();
  
  // Make ratios
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    hL1PtEff[iCutLevel]->  Divide(hL1VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
    hL2PtEff[iCutLevel]->  Divide(hL2VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
    hEFPtEff[iCutLevel]->  Divide(hEFVisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
  
    hL1EtaEff[iCutLevel]-> Divide(hL1VisEta[iCutLevel], hMCVisEta, 1., 1., "B");
    hL2EtaEff[iCutLevel]-> Divide(hL2VisEta[iCutLevel], hMCVisEta, 1., 1., "B");
    hEFEtaEff[iCutLevel]-> Divide(hEFVisEta[iCutLevel], hMCVisEta, 1., 1., "B");
  
 }
  TString plotfile = "effMultiPlots";
  //plotfile += append;
  
  // Titles
  for(unsigned int i=0; i<nCutLevels; i++){
    TrigTauFlags test;
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiency_"+ test.menuName(menu));
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency_"+ test.menuName(menu));
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("EF Efficiency_+ test.menuName(menu)");
  }

  hL1EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hL1EtaEff[0]->GetYaxis()->SetTitle("L1 Efficiency");
  hL2EtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hL2EtaEff[0]->GetYaxis()->SetTitle("L2 Efficiency");
  hEFEtaEff[0]->GetXaxis()->SetTitle("True Visible Eta");
  hEFEtaEff[0]->GetYaxis()->SetTitle("Efficiency");

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

  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    gStyle->SetHistLineColor(color[iCutLevel]);
    //if(iCutLevel==3 || iCutLevel==5) continue;
    //if (iCutLevel==2) continue;

    gStyle->SetMarkerColor(color[iCutLevel]);
    gStyle->SetMarkerStyle(style[iCutLevel]);
    gStyle->SetLineColor(color[iCutLevel]);
    hEFPtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hEFPtEff[iCutLevel]->UseCurrentStyle();
    hEFPtEff[iCutLevel]->SetMaximum(1.05);
    if (iCutLevel==0) hEFPtEff[iCutLevel]->Draw("P");
    else hEFPtEff[iCutLevel]->Draw("PSAME");
   
    l->AddEntry(hEFPtEff[iCutLevel],test[iCutLevel],"p");

    //gStyle->SetMarkerStyle(style[iCutLevel]);
    //gStyle->SetMarkerColor(color[iCutLevel]);
    //hEFPtEff[iCutLevel]->UseCurrentStyle();
    //hEFPtEff[iCutLevel]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iCutLevel],test[iCutLevel],"p");
  }
  l->Draw("SAME");
  ATLAS_LABEL(0.15,0.85);





  for(unsigned int iCutLevel = 0; iCutLevel<nCutLevels; iCutLevel++) {
    // Select the new menu
    cout << endl << "Menu selection " << iCutLevel << endl << endl;
    selmanager.setTauMenu(menu, theCutLevel[iCutLevel]);
    selmanager.print();
  }

  for(unsigned int iCutLevel = 0; iCutLevel<nCutLevels; iCutLevel++) {

    cout<< "n_L1MatchEvent[iCutLevel] "<<n_L1MatchEvent[iCutLevel]<<endl;
    cout<< "n_OfflineMatchEvent[iCutLevel] "<<n_OfflineMatchEvent[iCutLevel]<<endl;

    cout << " L1 efficiency menu "<<iCutLevel<<" is "<< ((float)n_L1MatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;
    cout << " L2 efficiency menu "<<iCutLevel<<" is "<< ((float)n_L2MatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;
    cout << " EF efficiency menu "<<iCutLevel<<" is "<< ((float)n_EFMatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;
  }

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << endl;
  cout << "Matched Taus:    " << nMatched << " -> "
       << float(nMatched)/nEvent << " Matched/Ev" << endl;

}

void TTPValLevelCut::printCSC(TPostScript & ps) {
  
   gROOT->cd();
  
  // Make ratios
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    hL1PtEff[iCutLevel]->  Divide(hL1VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
    hL2PtEff[iCutLevel]->  Divide(hL2VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
    hEFPtEff[iCutLevel]->  Divide(hEFVisPt[iCutLevel],  hMCVisPt,  1., 1., "B");

   }
  TString plotfile = "effMultiPlots";
  
  // Titles
  for(unsigned int i=0; i<nCutLevels; i++){
    TrigTauFlags test;
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiency_"+ test.menuName(menu));
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency_"+ test.menuName(menu));
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("EF Efficiency_"+ test.menuName(menu));
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

  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    gStyle->SetHistLineColor(color[iCutLevel]);
    gStyle->SetMarkerColor(color[iCutLevel]);
    gStyle->SetMarkerStyle(style[iCutLevel]);
    gStyle->SetLineColor(color[iCutLevel]);
    hL1PtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hL1PtEff[iCutLevel]->UseCurrentStyle();
    hL1PtEff[iCutLevel]->SetMaximum(1.05);
    if (iCutLevel==0) hL1PtEff[iCutLevel]->Draw("P");
    else hL1PtEff[iCutLevel]->Draw("PSAME");
    l->AddEntry(hL1PtEff[iCutLevel],test[iCutLevel],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();
  
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    gStyle->SetHistLineColor(color[iCutLevel]);
    gStyle->SetMarkerColor(color[iCutLevel]);
    gStyle->SetMarkerStyle(style[iCutLevel]);
    gStyle->SetLineColor(color[iCutLevel]);
    hL2PtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hL2PtEff[iCutLevel]->UseCurrentStyle();
    hL2PtEff[iCutLevel]->SetMaximum(1.05);
    if (iCutLevel==0) hL2PtEff[iCutLevel]->Draw("P");
    else hL2PtEff[iCutLevel]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iCutLevel],test[iCutLevel],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
    gStyle->SetHistLineColor(color[iCutLevel]);
    gStyle->SetMarkerColor(color[iCutLevel]);
    gStyle->SetMarkerStyle(style[iCutLevel]);
    gStyle->SetLineColor(color[iCutLevel]);
    hEFPtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hEFPtEff[iCutLevel]->UseCurrentStyle();
    hEFPtEff[iCutLevel]->SetMaximum(1.05);
    if (iCutLevel==0) hEFPtEff[iCutLevel]->Draw("P");
    else hEFPtEff[iCutLevel]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iCutLevel],test[iCutLevel],"p");
  }
  l->Draw();
  ATLAS_LABEL(0.15,0.85);
  c1->Update();

  for(unsigned int iCutLevel = 0; iCutLevel<nCutLevels; iCutLevel++) {
    // Select the new menu
    cout << endl << "Menu selection " << iCutLevel << endl << endl;
    selmanager.setTauMenu(menu, theCutLevel[iCutLevel]);
    selmanager.print();
  }

  for(unsigned int iCutLevel = 0; iCutLevel<nCutLevels; iCutLevel++) {

    cout<< "n_L1MatchEvent[iCutLevel] "<<n_L1MatchEvent[iCutLevel]<<endl;
    cout<< "n_OfflineMatchEvent[iCutLevel] "<<n_OfflineMatchEvent[iCutLevel]<<endl;

    cout << " L1 efficiency menu "<<iCutLevel<<" is "<< ((float)n_L1MatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;
    cout << " L2 efficiency menu "<<iCutLevel<<" is "<< ((float)n_L2MatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;
    cout << " EF efficiency menu "<<iCutLevel<<" is "<< ((float)n_EFMatchEvent[iCutLevel])/((float)n_OfflineMatchEvent[iCutLevel]) <<endl;

   ps.Off();

  }

  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << endl;
  cout << "Matched Taus:    " << nMatched << " -> "
       << float(nMatched)/nEvent << " Matched/Ev" << endl;

}


void TTPValLevelCut::update(TString name) {

  TFile f(name, "update");

  f.cd();
  f.mkdir("MultiPlots");
  f.cd("MultiPlots");


  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {
   hL1PtEff[iCutLevel]->  Divide(hL1VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
   hL2PtEff[iCutLevel]->  Divide(hL2VisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
   hEFPtEff[iCutLevel]->  Divide(hEFVisPt[iCutLevel],  hMCVisPt,  1., 1., "B");
 }

  // Titles
  for(unsigned int i=0; i<nCutLevels; i++){
    TrigTauFlags test;
    hL1PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL1PtEff[i]->GetYaxis()->SetTitle("L1 efficiencyI_"+ test.menuName(menu));
    hL2PtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hL2PtEff[i]->GetYaxis()->SetTitle("L2 efficiency_"+ test.menuName(menu));
    hEFPtEff[i]->GetXaxis()->SetTitle("True visible E_{T} (GeV)");
    hEFPtEff[i]->GetYaxis()->SetTitle("EF Efficiency_"+ test.menuName(menu));
  }
  
  for (unsigned int iCutLevel=0; iCutLevel<nCutLevels; iCutLevel++) {

    gStyle->SetHistLineColor(color[iCutLevel]);
    gStyle->SetMarkerColor(color[iCutLevel]);
    gStyle->SetMarkerStyle(style[iCutLevel]);
    gStyle->SetLineColor(color[iCutLevel]);
    hL1PtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hL1PtEff[iCutLevel]->UseCurrentStyle();
    hL1PtEff[iCutLevel]->SetMaximum(1.05);
    //l->AddEntry(hEFPtEff[iCutLevel],test[iCutLevel],"p");

    hL2PtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hL2PtEff[iCutLevel]->UseCurrentStyle();
    hL2PtEff[iCutLevel]->SetMaximum(1.05);

    hEFPtEff[iCutLevel]->SetLineColor(color[iCutLevel]);
    hEFPtEff[iCutLevel]->UseCurrentStyle();
    hEFPtEff[iCutLevel]->SetMaximum(1.05);


    TrigTauFlags test;
    hL1PtEff[iCutLevel]->SetName("TTPVAlMultiPlots_" + test.menuName(menu)+"_L1"); //eventually replace menu with cutLevel
    hL1PtEff[iCutLevel]->Write();
    hL2PtEff[iCutLevel]->SetName("TTPVAlMultiPlots_" + test.menuName(menu)+"_L2");
    hL2PtEff[iCutLevel]->Write();
    hEFPtEff[iCutLevel]->SetName("TTPVAlMultiPlots_" + test.menuName(menu)+"_EF");
    hEFPtEff[iCutLevel]->Write();    
  }
  f.Close();
}


void TTPValLevelCut::ATLAS_LABEL(Double_t x,Double_t y,Color_t color) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
}
