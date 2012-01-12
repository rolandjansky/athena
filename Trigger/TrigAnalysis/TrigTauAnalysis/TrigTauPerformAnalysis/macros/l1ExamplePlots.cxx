/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void l1ExamplePlots(const char* filename="ntuples/trigtau.AOD.root",
		     TrigTauFlags::TRIGGERMENU menu,  TString mode="all") {

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

  // Set up selection manager and process selection options
  TString append;

  // Pick a menu set (12.0.6 by default)
  //  analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENUJUNE06);

  analyzer.trigMan.setTauMenu(menu);
  append += "_"+TrigTauFlags::menuName(menu);

  // Set fiducial cuts
  analyzer.truthList.setMinVisPt(0.);
  analyzer.truthList.setMaxVisEta(2.5);

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
  gStyle->SetOptStat("emr");
  gROOT->cd();

  // MC Truth histograms
  TH1F* hVisPt  = new TH1F("hVisPt", "", 50, 0, 100);
  hVisPt->GetXaxis()->SetTitle("Visible Pt (GeV)");
  hVisPt->GetYaxis()->SetTitle("Good Taus");

  TH1F* hVisEta = new TH1F("hVisEta", "", 61, -3.05, 3.05);
  hVisEta->GetXaxis()->SetTitle("Vis Eta");
  hVisEta->GetYaxis()->SetTitle("Good Taus");

  // L1 histograms
  TH1F* hL1VisPt  = new TH1F("hL1VisPt", "", 50, 0, 100);
  hL1VisPt->GetXaxis()->SetTitle("True Vis Pt (GeV)");
  hL1VisPt->GetYaxis()->SetTitle("L1 Taus");
  hL1VisPt->Sumw2();

  TH1F* hL1VisEta = new TH1F("hL1VisEta", "", 61, -3.05, 3.05);
  hL1VisEta->GetXaxis()->SetTitle("True Vis Eta");
  hL1VisEta->GetYaxis()->SetTitle("L1 Taus");
  hL1VisEta->Sumw2();

  TH1F* hL1PtEff  = new TH1F("hL1PtEff", "", 50, 0, 100);
  hL1PtEff->GetXaxis()->SetTitle("True Vis Pt (GeV)");
  hL1PtEff->GetYaxis()->SetTitle("L1/Good Taus");

  TH1F* hL1EtaEff = new TH1F("hL1EtaEff", "", 61, -3.05, 3.05);
  hL1EtaEff->GetXaxis()->SetTitle("True Vis Eta");
  hL1EtaEff->GetYaxis()->SetTitle("L1/Good Taus");

  TH1F* hL1Pt  = new TH1F("hL1Pt", "", 50, 0, 100);
  hL1Pt->GetXaxis()->SetTitle("L1 Et (GeV)");
  hL1Pt->GetYaxis()->SetTitle("L1 Passed");

  TH2F* hL1PtEt  = new TH2F("hL1PtEt", "", 50, 0, 100, 50, 0, 100);
  hL1PtEt->GetXaxis()->SetTitle("True Vis Pt (GeV)");
  hL1PtEt->GetYaxis()->SetTitle("L1 Et (GeV)");

  TH1F* hL1Eta = new TH1F("hL1Eta", "", 61, -3.05, 3.05);
  hL1Eta->GetXaxis()->SetTitle("L1 Eta");
  hL1Eta->GetYaxis()->SetTitle("L1 Taus");

  // Resolution Plots
  TH1F* hL1dPt = new TH1F("hL1dPt", "", 50, -50, 50);
  hL1dPt->GetXaxis()->SetTitle("L1 - Vis Pt (GeV)");

  TH1F* hL1dEta = new TH1F("hL1dEta", "", 50, -0.25, 0.25);
  hL1dEta->GetXaxis()->SetTitle("L1 - Vis Eta");


  // Counters
  int nEvent = 0;
  int nTrue = 0;
  int nGood = 0;
  int nL1 = 0;

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

    // Loop over all good truth taus
    unsigned int nTau = analyzer.truthList.nMCTauGood();
    for(unsigned int itau = 0; itau < nTau; ++itau ) {
      TrigTauMCTau* tau =  &analyzer.truthList.mcTauGoodEntry(itau);

      float visPt = tau->visible().Pt()/1000.;
      float visEta = tau->visible().Eta();

      hVisPt->Fill(visPt);
      hVisEta->Fill(visEta);

      //
      // Match truth with L1 trigger
      TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
      if (l1Tau == 0) continue;

      nL1++;
      float l1Pt  = l1Tau->m_TauEnergy/1000.;
      float l1Eta = l1Tau->Eta();
      hL1VisPt->Fill(visPt);
      hL1VisEta->Fill(visEta);

      hL1Pt->Fill(l1Pt);
      hL1PtEt->Fill(visPt, l1Pt);

      hL1Eta->Fill(l1Eta);

      hL1dPt->Fill(l1Pt - visPt);
      hL1dEta->Fill(l1Eta - visEta);

    }
    
  } // end loop over events

  gROOT->cd();

  // Make ratios
  hL1PtEff->  Divide(hL1VisPt,  hVisPt,  1., 1., "B");
  hL1EtaEff-> Divide(hL1VisEta, hVisEta, 1., 1., "B");

  TString plotfile = "l1EffPlots";
  plotfile += append;

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","L1 Selected",10,10,800,620);
  c1->Divide(2,2);
  c1->cd(1); hL1Pt->Draw();
  c1->cd(2); hL1Eta->Draw();
  c1->cd(3); hL1dPt->Draw();
  c1->cd(4); hL1dEta->Draw();
  c1->Print(plotfile + ".ps(");

  TCanvas * c2 = (TCanvas *) gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c2","L1 Selected",10,10,800,620);
  c2->Divide(2,2);
  gStyle->SetOptStat(0);
  c2->cd(1); hL1Pt->Draw();
  c2->cd(2); hL1PtEt->Draw();
  c2->cd(3); hL1PtEff->Draw();
  c2->cd(4); hL1EtaEff->Draw();
  c2->Print(plotfile + ".ps)");

  // Print out matching statistics
  analyzer.trigMan.print();
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << " -> " 
       << float(nGood)/nEvent << " Good/Ev" << endl;
  cout << "L1 Matched:   " << nL1 << " -> "
       << (100.*nL1)/nGood << "% L1/Good" << endl;

  delete event;
  delete tree;
  delete file;

  // Output to plot
  return;
}

