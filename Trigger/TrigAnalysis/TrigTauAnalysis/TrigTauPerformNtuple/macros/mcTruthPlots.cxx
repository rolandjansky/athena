/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void mcTruthPlots(const char* filename="trigtau.AOD.root") {

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

  // Initialize our MC analyzer
  TrigTauMCTauList mcList;

  // Optionally change fiducial cuts on good taus
  //  mcList.select1Prong();

  // Formatting commands
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat("emr");
  gROOT->cd();

  // MC Truth histograms
  TH1D* hNTrue = new TH1D("hNTrue", "", 5, -0.5, 4.5);
  hNTrue->GetXaxis()->SetTitle("Number of True Taus/Event");

  TH1D* hNGood = new TH1D("hNGood", "", 5, -0.5, 4.5);
  hNGood->GetXaxis()->SetTitle("Number of Good Taus/Event");

  TH1D* hTruPt  = new TH1D("hTruPt", "", 50, 0, 100);
  hTruPt->GetXaxis()->SetTitle("True Tau Pt (GeV)");
  hTruPt->GetYaxis()->SetTitle("Good Taus");

  TH1D* hVisPt  = new TH1D("hVisPt", "", 50, 0, 100);
  hVisPt->GetXaxis()->SetTitle("Visible Pt (GeV)");
  hVisPt->GetYaxis()->SetTitle("Good Taus");

  TH1D* hTruEta = new TH1D("hTruEta", "", 60, -3.0, 3.0);
  hTruEta->GetXaxis()->SetTitle("True Tau Eta");
  hTruEta->GetYaxis()->SetTitle("Good Taus");

  TH1D* hVisEta = new TH1D("hVisEta", "", 60, -3.0, 3.0);
  hVisEta->GetXaxis()->SetTitle("Vis Eta");
  hVisEta->GetYaxis()->SetTitle("Good Taus");

  // Counters
  int nEvent = 0;
  int nTrue = 0;
  int nGood = 0;

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);
    mcList.setMCEvent(event->mcEvent);
    
    nEvent++;
    nTrue += mcList.nMCTauPure();
    nGood += mcList.nMCTauGood();

    hNTrue->Fill(mcList.nMCTauPure());
    hNGood->Fill(mcList.nMCTauGood());

    unsigned int nTau = mcList.nMCTauGood();
    for(unsigned int itau = 0; itau < nTau; ++itau ) {
      TrigTauMCTau* tau =  &mcList.mcTauGoodEntry(itau);

      float truePt = tau->Pt()/1000.;
      float visPt = tau->visible().Pt()/1000.;
      float visEta = tau->visible().Eta();
      
      hTruPt->Fill(truePt);
      hTruEta->Fill(tau->Eta());
      hVisPt->Fill(visPt);
      hVisEta->Fill(visEta);
    }

    //    if (mcList.nMCTauPure() != 1) mcList.printPureList();
  }

  gROOT->cd();

  TString plotfile = "mcTruthPlots";

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","True Taus",10,10,800,620);
  c1->Divide(2,3);
  c1->cd(1); hNTrue->Draw();
  c1->GetPad(1)->SetLogy();
  c1->cd(2); hNGood->Draw();
  c1->GetPad(2)->SetLogy();
  c1->cd(3); hTruPt->Draw();
  c1->cd(4); hTruEta->Draw();
  c1->cd(5); hVisPt->Draw();
  c1->cd(6); hVisEta->Draw();
  c1->Print(plotfile + ".ps");

  // Print out matching statistics
  //  trigMan.print();
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  if (nEvent>0) {
  cout << "Good Taus:    " << nGood << " -> " 
       << float(nGood)/nEvent << " Good/Ev" << endl;
  }

  //  delete main;
  delete event;
  delete tree;
  delete file;

  // Output to plot
  return;
}
