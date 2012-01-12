/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//void effMultiPlots(const char* filename="ntuples/trigtau.AOD.root", 
//		   TString mode="all") {
void effMultiPlots(TTree * tree,
		   TString mode="all") {

  gROOT->cd();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle(); 

  /*
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
  */

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  // We will use our own trigger manager (although we could just use
  // the one in TrigTauAnalyzer).
  TrigTauSelectionManager trigMan;

  // Pick a menu set (12.0.6 by default)
  //  trigMan.setMenuVersion(TrigTauSelectionManager:: MENU1206);
  //  trigMan.setMenuVersion(TrigTauSelectionManager::MENU13020);
  //trigMan.setMenuVersion(TrigTauSelectionManager::MENU13030pre);
  trigMan.setMenuVersion(TrigTauSelectionManager::MENU1400);
  // trigMan.setMenuVersion(TrigTauSelectionManager::MENUJUNE07);

  // Also set an L1 MET cut if desired
  trigMan.setL1MET(0.);

  // Set fiducial cuts
  analyzer.truthList.setMinVisPt(12.);
  analyzer.truthList.setMaxVisEta(2.5);

  // Select a particular tau decay mode (if desired)
  // Could also add pi, rho, etc.
  TString append;
  if (mode == "all") {
  }
  else if (mode == "1pr") {
    append += "_1pr";
    analyzer.truthList.select1Prong();
  }
  else if (mode == "3pr") {
    append += "_3pr";
    analyzer.truthList.select3Prong();
  }
  else {
    cout << "Unkown decay mode: " << mode << endl;
    delete event; delete tree; delete file;
    return;
  }

  // Formatting commands
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gROOT->cd();

  // Set up the different menus to plot
  const int nMenus = 7;
  TrigTauSelectionManager::CUTLEVEL cutLevel = TrigTauSelectionManager::LOOSE;
  TrigTauFlags::TRIGGERMENU theMenu[nMenus];
  theMenu[0] = TrigTauFlags::TAU12;
  theMenu[1] = TrigTauFlags::TAU16I;
  theMenu[2] = TrigTauFlags::TAU20I;
  theMenu[3] = TrigTauFlags::TAU29I;
  theMenu[4] = TrigTauFlags::TAU38I;
  theMenu[5] = TrigTauFlags::TAU50;
  theMenu[6] = TrigTauFlags::TAU84;

  float Threshold[nMenus]={15.,20.,25.,35.,45.,60., 100.};

  TrigTauMCTauList::OFFLINESELECTION offlineType =  TrigTauMCTauList::EITHERTAU;

  TString test[nMenus];
  for(unsigned int im = 0; im < nMenus; ++im )
    test[im] = TrigTauFlags::menuName(theMenu[im]);


  int npt =50;
  float minpt = 0;
  float maxpt = 120;

  // Create histograms to hold triggered object counts by pt and eta
  TH1F* hMC_InVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  TH1F* hMCVisPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  TH1F* hL1VisPt[nMenus];   // L1 selected (numerator)
  TH1F* hL2VisPt[nMenus];   // L2 selected (numerator)
  TH1F* hEFVisPt[nMenus];   // EF selected (numerator)
  TH1F* hL1PtEff[nMenus];
  TH1F* hL2PtEff[nMenus];
  TH1F* hEFPtEff[nMenus];

  TH1F* hntrak = new TH1F("","",20,0.,10.);

  TH1F* hEFnTracks[nMenus];

  TH1F* hL21PtEff[nMenus];
  TH1F* hEFL2PtEff[nMenus];

  TH1F* hMC_InVisEta = new TH1F("", "", 31, -3.1, 3.1);
  TH1F* hMCVisEta = new TH1F("", "", 31, -3.1, 3.1);
  TH1F* hL1VisEta[nMenus];
  TH1F* hL2VisEta[nMenus];
  TH1F* hEFVisEta[nMenus];
  TH1F* hL1EtaEff[nMenus];
  TH1F* hL2EtaEff[nMenus];
  TH1F* hEFEtaEff[nMenus];

  TH1F* hL21EtaEff[nMenus];
  TH1F* hEFL2EtaEff[nMenus];

  int nMatch[nMenus];
  int nL1passed[nMenus];
  int nL2passed[nMenus];
  int nEFpassed[nMenus];
  int n_OfflineMatchEvent[nMenus];
  int  n_L1MatchEvent[nMenus];
  int  n_L2MatchEvent[nMenus];
  int  n_EFMatchEvent[nMenus];


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

  // Counters
  int nEvent = 0;
  int nTrue = 0;
  int nGood = 0;
  int nMatched = 0;

  TrigTauBugFix bugfix (TrigTauBugFix::TTP9, TrigTauBugFix::RDO);
 
  // Loop over all events
  int nev = int(tree->GetEntries());
  //nev = 1000;
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    // Analyze this event
    analyzer.analyze(event, true, offlineType);

    nEvent++;
    nTrue += analyzer.truthList.nMCTauPure();
    nGood += analyzer.truthList.nMCTauGood();

    for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
      nMatch[iMenu]=0;
      nL1passed[iMenu]=0;
      nL2passed[iMenu]=0;
      nEFpassed[iMenu]=0;
    }
    // Loop over all good truth taus in this event
    unsigned int nTau = analyzer.truthList.nMCTauGood();
    for(unsigned int itau = 0; itau < nTau; ++itau ) {
      TrigTauMCTau* tau =  &analyzer.truthList.mcTauGoodEntry(itau);

      float visPt = tau->visible().Pt()/1000.;
      float visEta = tau->visible().Eta();

      // Fill denominator with all good taus
      hMC_InVisPt->Fill(visPt);
      hMC_InVisEta->Fill(visEta);

      nMatched++;

      hMCVisPt->Fill(visPt);
      hMCVisEta->Fill(visEta);

      // Now, loop through defined menus, apply selections, and look for
      // a matching trigger object within the allowed dR distance.
      // Note:  The object passing L2 doesn't necessarily need to come
      // from the ROI passing L1, but I think that is OK.
      for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) {

	if(visPt> Threshold[iMenu]) nMatch[iMenu]++;

	// Select the new menu
	trigMan.setTauMenu(theMenu[iMenu], cutLevel);

	// Refill the selected chain list based on the new menu
	analyzer.chainList.fillTriggerList(trigMan);

	// Find a matching L1 object
	TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
	if (l1Tau != 0) {
	  // Found a L1 trigger within dR range, fill numerator histograms
	  // with MC truth Pt and Eta
	  hL1VisPt[iMenu]->Fill(visPt);
	  hL1VisEta[iMenu]->Fill(visEta);
	  if(visPt> Threshold[iMenu]) nL1passed[iMenu]++;
	}

	// Because the dR range might be different between L1 and L2
	// it is possible (if unlikely) to find a L2 match without a L1.
	// In reality, this probably never happens...

	// Find a matching L2 object
	TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
	if (l2Tau != 0) {
	  hL2VisPt[iMenu]->Fill(visPt);
	  hL2VisEta[iMenu]->Fill(visEta);
	  if(visPt> Threshold[iMenu]) nL2passed[iMenu]++;
	}

	// Find a matching EF object
	TrigTauRecEFObject* efTau = analyzer.chainList.getEFTau(*tau);
	if (efTau != 0) {
	  hEFVisPt[iMenu]->Fill(visPt);
	  hEFVisEta[iMenu]->Fill(visEta);
	  if(visPt> Threshold[iMenu]) nEFpassed[iMenu]++;
	  if(visPt> Threshold[iMenu]) hEFnTracks[iMenu]->Fill((*efTau)->nTracks());
	}

      }  // End of loop over menus
    }  // End of loop over good taus in the event    

    for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {

      if(nMatch[iMenu]>0) n_OfflineMatchEvent[iMenu]++;
      if(nL1passed[iMenu]>0) n_L1MatchEvent[iMenu]++;
      if(nL2passed[iMenu]>0) n_L2MatchEvent[iMenu]++;
      if(nEFpassed[iMenu]>0) n_EFMatchEvent[iMenu]++;
    }

  }  // end loop over events

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
  plotfile += append;

  // Titles
  for(int i=0; i<nMenus; i++){
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

  int color[nMenus];
  color[0] = 51;
  color[1] = 8;
  color[2] = 60;
  color[3] = 96;
  color[4] = 66;
  color[5] = 100;
  color[6] = 84;
  int style[nMenus];
  style[0] = 20;
  style[1] = 26;
  style[2] = 21;
  style[3] = 24;
  style[4] = 23;
  style[5] = 25;
  style[6] = 22;


  //gStyle->SetHistLineWidth(2);
  gStyle->SetCanvasColor(0);
 
  TCanvas * c4 = (TCanvas *) gROOT->Get("c4");
  if( c4 ) delete c4;
  c4 = new TCanvas("c4","EF Selected",10,10,800,620);
  c4->cd();
  gStyle->SetHistLineColor(1);
  TLegend *l = new TLegend(0.75,0.2,0.9,0.5);
  l->SetFillStyle(0);
  int id = 2;
  gStyle->SetHistLineColor(color[id]);
  gStyle->SetMarkerStyle(style[id]);
  gStyle->SetMarkerColor(color[id]);
  gStyle->SetLineColor(color[id]);
	gStyle->SetFillColor(0);
  hEFPtEff[id]->SetLineColor(color[id]);
  hEFPtEff[id]->UseCurrentStyle();
  hEFPtEff[id]->SetMaximum(1.05);
  hEFPtEff[id]->Draw("P");
  //l->AddEntry(hEFPtEff[id],test[id],"p");

  //  TLegend *l = new TLegend(0.13,0.65,0.25,0.85);
  //l->SetFillStyle(0);
  //hEFPtEff[2]->UseCurrentStyle();
  //hEFPtEff[2]->Draw("P");
  //l->AddEntry(hEFPtEff[2],"tau20i","p");

  for (unsigned int iMenu=0; iMenu<nMenus; iMenu++) {
    gStyle->SetHistLineColor(color[iMenu]);
    //if(iMenu==3 || iMenu==5) continue;
    if (iMenu==2) continue;    

    gStyle->SetMarkerColor(color[iMenu]);
    gStyle->SetMarkerStyle(style[iMenu]);
    gStyle->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->SetLineColor(color[iMenu]);
    hEFPtEff[iMenu]->UseCurrentStyle(); 
    hEFPtEff[iMenu]->Draw("PSAME");
    l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");

    //gStyle->SetMarkerStyle(style[iMenu]);
    //gStyle->SetMarkerColor(color[iMenu]);
    //hEFPtEff[iMenu]->UseCurrentStyle(); 
    //hEFPtEff[iMenu]->Draw("PSAME");
    //l->AddEntry(hEFPtEff[iMenu],test[iMenu],"p");
  }

  l->Draw("SAME");

  ATLAS_LABEL(0.15,0.85);

  c4->Print(plotfile + ".eps");
 


  for(unsigned int iMenu = 0; iMenu<nMenus; iMenu++) { 
    // Select the new menu
    cout << endl << "Menu selection " << iMenu << endl << endl;
    trigMan.setTauMenu(theMenu[iMenu], cutLevel);
    trigMan.print();
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

  delete event;
  delete tree;
  //  delete file;

  return;
}

// double DeltaRad( TrigTauRecObject* t1, TrigTauMCTau* t2){
//   double dEta = t1->Eta() - t2->Eta();
//   double dPhi = t1->Phi() - t2->Phi();
//   if (dPhi >  +TMath::Pi()) dPhi -= TMath::TwoPi();
//   if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
//   return float(dEta*dEta + dPhi*dPhi);
// } 
// double DeltaRad( TrigTau1p3pObject* t1, TrigTauMCTau* t2){
//   double dEta = t1->Eta() - t2->Eta();
//   double dPhi = t1->Phi() - t2->Phi();
//   if (dPhi >  +TMath::Pi()) dPhi -= TMath::TwoPi();
//   if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
//   return float(dEta*dEta + dPhi*dPhi);
// } 

void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
}
