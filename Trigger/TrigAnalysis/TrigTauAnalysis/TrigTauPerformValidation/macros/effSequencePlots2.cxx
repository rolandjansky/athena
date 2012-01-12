/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// The following picks one macro and plots the efficiency vs. PT
// for L1, L2, and EF on the same plot.
//

// minEt specifies fiducial cut on MC true Et (in GeV)

void effSequencePlots2(
		      //TTree tree,
 		      TrigTauFlags::TRIGGERMENU menu, 
		      float minPt = 0.,
		      TString menuMET="0", 
 		      TString mode="all",
		      bool reco = true) {

  gROOT->cd();
  //gROOT->SetStyle("ATLAS");
  //gROOT->ForceStyle(); 

  //mode = "3pr";//;"all";
  bool checkTau1p3p = true;//false;//true;//true;
  bool checkTauRec = true;//true;

  TrigTauSelectionManager::CUTLEVEL cutLevel = TrigTauSelectionManager::LOOSE;

  // Create our analyzer class
  TrigTauAnalyzer analyzer;

  // Set up selection manager and process selection options
  TString append;

  // Pick a menu set (12.0.6 by default)
  //analyzer.trigMan.setMenuVersion(TrigTauSelectionManager:: MENU1206);
  // analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENU13030pre);
  analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENU1400);
  // analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENUJUNE06);



  gStyle->SetPadGridY(true);
  gStyle->SetPadGridX(true);

  //gROOT->cd();

  // Open file
  //TFile* file = new TFile(filename);
  //if (!file->IsOpen()) {
    //cerr << "Can't open file " << filename << endl;
    //return;
  //}
 

  TChain * tree = new TChain("eventTree");
  tree->Add("../data/TTP12a.142252/5188/tmp.*");

  //TString filename = new TString("ntuples/TTP07/5107/trig1_misal1_csc11.005107.pythia_Wtauhad.RDO.v12003103.TTP07.merged.0.root"); 
  //TFile* file =new TFile("ntuples/TTP07/5107/trig1_misal1_csc11.005107.pythia_Wtauhad.RDO.v12003103.TTP07.merged.0.root");

  //TFile* file =new TFile("ntuples/sampleA/14.1.0/trigtau.5200.root");

  //TTree* tree = (TTree*)file->Get("eventTree");
  //tree.AddFriend("eventTree","ntuples/TTP07/5188/trig1_misal1_mc12.005188.A3_Ztautau_filter.RDO.v12000605.TTP07.merged.0.root");
  //tree.AddFriend("eventTree","ntuples/TTP07/5862/trig1_misal1_mc12.005862.A10_Atautau_filter.RDO.v12000605..TTP07.merged.0.root");
  //tree.AddFriend("eventTree","ntuples/TTP07/5862/trig1_misal1_mc12.005862.A10_Atautau_filter.RDO.v12000605..TTP07.merged.1.root");
  if (tree == 0) {
  cerr << "Can't find eventTree!" << endl;
  return;
  }

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);

  append +="_";
  append += TrigTauFlags::menuName(menu);
  analyzer.trigMan.setTauMenu(menu, cutLevel);


  if (menuMET == "00") {
  }
  else if (menuMET == "20") {
    append += "_met20";
    analyzer.trigMan.setL1MET(20000.);
  }
  else if (menuMET == "30") {
    append += "_met30";
    analyzer.trigMan.setL1MET(30000.);
  }
  else if (menuMET == "40") {
    append += "_met40";
    analyzer.trigMan.setL1MET(40000.);
  }

  // Set fiducial cuts
  analyzer.truthList.setMinVisPt(minPt); // 12 GeV
  analyzer.truthList.setMaxVisEta(2.5);
  //analyzer.truthList.setMinLeadingPt(6.0);

  // Select a particular tau decay mode (if desired)
  // Could also add pi, rho, etc.
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

  if (reco) append += "_reco";
  if(!reco) append +="_truth";


  // Formatting commands
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  //gROOT->cd();

  int npt =50;
  float minpt = 0;
  float maxpt = 120;

  // Create histograms to hold triggered object counts by pt and eta
  TH1F* hMCVisPt  = new TH1F("hMCVisPt", "", npt, minpt, maxpt); // MC visible (denominator)

  TH1F* hL1VisPt  = new TH1F("hL1VisPt", "", npt, minpt, maxpt);
  TH1F* hL2VisPt  = new TH1F("hL2VisPt", "", npt, minpt, maxpt);
  TH1F* hEFVisPt  = new TH1F("hEFVisPt", "", npt, minpt, maxpt);

  TH1F* hL1PtEff  = new TH1F("hL1PtEff", "", npt, minpt, maxpt);
  TH1F* hL2PtEff  = new TH1F("hL2PtEff", "", npt, minpt, maxpt);
  TH1F* hEFPtEff  = new TH1F("hEFPtEff", "", npt, minpt, maxpt);
			                 
  TH1F* hMCVisEta  = new TH1F("hMCVisEta", "", 31, -3.1, 3.1);

  TH1F* hL1VisEta  = new TH1F("hL1VisEta", "", 31, -3.1, 3.1);
  TH1F* hL2VisEta  = new TH1F("hL2VisEta", "", 31, -3.1, 3.1);
  TH1F* hEFVisEta  = new TH1F("hEFVisEta", "", 31, -3.1, 3.1);

  TH1F* hL1EtaEff  = new TH1F("hL1EtaEff", "", 31, -3.1, 3.1);
  TH1F* hL2EtaEff  = new TH1F("hL2EtaEff", "", 31, -3.1, 3.1);
  TH1F* hEFEtaEff  = new TH1F("hEFEtaEff", "", 31, -3.1, 3.1);


  hL1PtEff->Sumw2();
  hL2PtEff->Sumw2();
  hEFPtEff->Sumw2();

  hL1EtaEff->Sumw2();
  hL2EtaEff->Sumw2();
  hEFEtaEff->Sumw2();

  TH1F* hOffOPt = new TH1F("", "", npt, minpt, maxpt); // MC visible (denominator)
  TH1F* hL1OPt = new TH1F(*hOffOPt);   // L1 selected (numerator)
  TH1F* hL2OPt = new TH1F(*hOffOPt);   // L2 selected (numerator)
  TH1F* hEFOPt = new TH1F(*hOffOPt);   // EF selected (numerator)
  TH1F* hL1PtEff_off = new TH1F(*hOffOPt);
  TH1F* hL2PtEff_off = new TH1F(*hOffOPt);
  TH1F* hEFPtEff_off = new TH1F(*hOffOPt);

  hL1PtEff_off->Sumw2();
  hL2PtEff_off->Sumw2();
  hEFPtEff_off->Sumw2();

  TrigTauBugFix bugfix (TrigTauBugFix::TTP7, TrigTauBugFix::RDO);
  //TrigTauBugFix bugfix;
  //bugfix.setTTPVersion(filename);


  // Counters
  int nEvent = 0;
  int nTrue = 0;
  int nGood = 0;
  int nMatched = 0;

  analyzer.chainList.setRoiWordMatching(false);
  analyzer.chainList.setHLTAuthor(TrigTauFlags::TRTONLY);  

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    bugfix.fix(event);

    // Analyze this event
    analyzer.analyze(event);

    // Only use true taus if an offline tau is nearby
    if (reco) analyzer.truthList.requireRecoMatch(*event);

    nEvent++;
    nTrue += analyzer.truthList.nMCTauPure();
    nGood += analyzer.truthList.nMCTauGood();

    // Loop over all good truth taus in this event
    unsigned int nTau = analyzer.truthList.nMCTauGood();
    for(unsigned int itau = 0; itau < nTau; ++itau ) {
      TrigTauMCTau* tau =  &analyzer.truthList.mcTauGoodEntry(itau);

      float visPt = tau->visible().Pt()/1000.;
      float visEta = tau->visible().Eta();

      // Fill denominator with all good taus
      hMCVisPt->Fill(visPt);
      hMCVisEta->Fill(visEta);

      nMatched++;

      // Find a matching L1 object
      TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
      if (l1Tau != 0) {
	// Found a L1 trigger within dR range, fill numerator histograms
	// with MC truth Pt and Eta
	hL1VisPt->Fill(visPt);
	hL1VisEta->Fill(visEta);
      }

      // Because the dR range might be different between L1 and L2
      // it is possible (if unlikely) to find a L2 match without a L1.
      // In reality, this probably never happens...

      // Find a matching L2 object
      TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
      if (l2Tau != 0) {
	hL2VisPt->Fill(visPt);
	hL2VisEta->Fill(visEta);
      }

      // Find a matching EF object
      TrigTauRecEFObject* efTau = analyzer.chainList.getEFTau(*tau);
      if (efTau != 0) {
	hEFVisPt->Fill(visPt);
	hEFVisEta->Fill(visEta);
      }

    }  // End of loop over good taus in the event    
  }  // end loop over events

  //gROOT->cd();

  // Make ratios

  hL1PtEff->Divide(hL1VisPt,  hMCVisPt,  1., 1., "B");
  hL2PtEff->Divide(hL2VisPt,  hMCVisPt,  1., 1., "B");
  hEFPtEff->Divide(hEFVisPt,  hMCVisPt,  1., 1., "B");

  hL1EtaEff->Divide(hL1VisEta, hMCVisEta, 1., 1., "B");
  hL2EtaEff->Divide(hL2VisEta, hMCVisEta, 1., 1., "B");
  hEFEtaEff->Divide(hEFVisEta, hMCVisEta, 1., 1., "B");


  TString plotfile = "effSequencePlots";
  plotfile += append;

  // Titles
  hL1PtEff->GetXaxis()->SetTitle("True Visible E_{T} (GeV)");
  hL1PtEff->GetYaxis()->SetTitle("Efficiency");

  hL1EtaEff->GetXaxis()->SetTitle("True visible eta");
  hL1EtaEff->GetYaxis()->SetTitle("Efficiency");

  int color[3];
  color[0] = 1;//2;
  color[1] = 1;//3;
  color[2] = 1;//4;
  //gStyle->SetHistLineWidth(2);

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","Selected",10,10,800,620);
  //  c1->Divide(1,2);

  TLegend * legend = new TLegend(0.5,0.2,0.7,0.45);
  legend->SetFillStyle(0);
  c1->cd(1);
  hL1PtEff->SetMaximum(1.05);
  gStyle->SetHistLineColor(color[0]);
  gStyle->SetMarkerStyle(20); gStyle->SetMarkerColor(color[0]);
  hL1PtEff->UseCurrentStyle(); hL1PtEff->Draw();
  legend->AddEntry(hL1PtEff,"L1","p");
  gStyle->SetHistLineColor(color[1]);
  gStyle->SetMarkerStyle(26); gStyle->SetMarkerColor(color[1]);
  hL2PtEff->UseCurrentStyle(); hL2PtEff->Draw("SAME");
  legend->AddEntry(hL2PtEff,"L2","p");
  gStyle->SetHistLineColor(color[2]);
  gStyle->SetMarkerStyle(22); gStyle->SetMarkerColor(color[2]);
  hEFPtEff->UseCurrentStyle(); hEFPtEff->Draw("SAME");
  legend->AddEntry(hEFPtEff,"EF","p");

  legend->Draw("same");
  ATLAS_LABEL(0.55,0.55);


 TCanvas * c2 = (TCanvas *) gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c2","Selected",10,10,800,620);

  c2->cd(1);

  gStyle->SetHistLineColor(color[0]);
  gStyle->SetMarkerStyle(20); gStyle->SetMarkerColor(color[0]);
  hL1EtaEff->UseCurrentStyle(); hL1EtaEff->Draw();
  gStyle->SetHistLineColor(color[1]);
  gStyle->SetMarkerStyle(26); gStyle->SetMarkerColor(color[1]);
  hL2EtaEff->UseCurrentStyle(); hL2EtaEff->Draw("SAME");
  gStyle->SetHistLineColor(color[2]);
  gStyle->SetMarkerStyle(22); gStyle->SetMarkerColor(color[2]);
  hEFEtaEff->UseCurrentStyle(); hEFEtaEff->Draw("SAME");
  //  legend->Draw();

  
  c1->Print(plotfile + ".pt.eps");
  c2->Print(plotfile + ".eta.eps");


  analyzer.trigMan.print();


  // Print out matching statistics
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue  << endl;
  cout << "Good Taus:    " << nGood  << endl; 
  cout << "Matched Taus:    " << nMatched << " -> " 
       << float(nMatched)/nEvent << " Match/Ev" << endl;

  delete event;
  // delete tree;
  // delete file;

  return;
}

void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1) {

  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
}
