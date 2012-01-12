/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Example macro to make a series of resolution plots.
// Plots include L1,L2,EF object with respect to MC truth
// as well as "data" plots of L1 vs L2 position.
//
void resolutionPlots(const char* filename="ntuples/trigtau.AOD.root",
		     TrigTauFlags::TRIGGERMENU menu, TString decayMode="all") {

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

  // Pick a menu set (12.0.6 by default)
  //  analyzer.trigMan.setMenuVersion(TrigTauSelectionManager::MENUJUNE06);

  // Set up selection manager and process selection options
  TString append;

  analyzer.trigMan.setTauMenu(menu);
  append += "_"+TrigTauFlags::menuName(menu);

  // Could also add MET cut
  // analyzer.trigMan.setL1MET(20000.);

  // Set fiducial cuts (only applies to MC truth resolutions)
  analyzer.truthList.setMinVisPt(0.);
  analyzer.truthList.setMaxVisEta(2.5);

  if (decayMode == "all") {
  }
  else if (decayMode == "1pr") {
    append += "_1pr";
    analyzer.truthList.select1Prong();
  }
  else if (decayMode == "3pr") {
    append += "_3pr";
    analyzer.truthList.select3Prong();
  }
  else {
    cout << "Unkown decay decayMode: " << decayMode << endl;
    delete event; delete tree; delete file;
    return;
  }

  // Formatting commands
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat("emr");
  gROOT->cd();

  // L1 Resolution Plots
  TH1F* hL1dPt = new TH1F("hL1dPt", "", 50, -50, 50);
  hL1dPt->GetXaxis()->SetTitle("L1 - Vis Pt (GeV)");

  TH1F* hL1dR = new TH1F("hL1dR", "", 50, 0.0, 0.5);
  hL1dR->GetXaxis()->SetTitle("L1 - Vis Radius");

  TH1F* hL1dEta = new TH1F("hL1dEta", "", 50, -0.25, 0.25);
  hL1dEta->GetXaxis()->SetTitle("L1 - Vis Eta");

  TH1F* hL1dPhi = new TH1F("hL1dPhi", "", 50, -0.25, 0.25);
  hL1dPhi->GetXaxis()->SetTitle("L1 - Vis Phi");

  // L2 Resolution Plots
  TH1F* hL2dPt = new TH1F("hL2dPt", "", 50, -50, 50);
  hL2dPt->GetXaxis()->SetTitle("L2 - Vis Pt (GeV)");

  TH1F* hL2dR = new TH1F("hL2dR", "", 50, 0.0, 0.5);
  hL2dR->GetXaxis()->SetTitle("L2 - Vis Radius");

  TH1F* hL2dEta = new TH1F("hL2dEta", "", 50, -0.25, 0.25);
  hL2dEta->GetXaxis()->SetTitle("L2 - Vis Eta");

  TH1F* hL2dPhi = new TH1F("hL2dPhi", "", 50, -0.25, 0.25);
  hL2dPhi->GetXaxis()->SetTitle("L2 - Vis Phi");

  // EF Resolution Plots
  TH1F* hEFdPt = new TH1F("hEFdPt", "", 50, -50, 50);
  hEFdPt->GetXaxis()->SetTitle("EF - Vis Pt (GeV)");

  TH1F* hEFdR = new TH1F("hEFdR", "", 50, 0.0, 0.5);
  hEFdR->GetXaxis()->SetTitle("EF - Vis Radius");

  TH1F* hEFdEta = new TH1F("hEFdEta", "", 50, -0.25, 0.25);
  hEFdEta->GetXaxis()->SetTitle("EF - Vis Eta");

  TH1F* hEFdPhi = new TH1F("hEFdPhi", "", 50, -0.25, 0.25);
  hEFdPhi->GetXaxis()->SetTitle("EF - Vis Phi");

  // L2 Resolution vs L1 Plots
  TH1F* hL2L1dPt = new TH1F("hL2L1dPt", "", 50, -50, 50);
  hL2L1dPt->GetXaxis()->SetTitle("L2 - L1 Pt (GeV)");

  TH1F* hL2L1dR = new TH1F("hL2L1dR", "", 50, 0.0, 0.5);
  hL2L1dR->GetXaxis()->SetTitle("L2 - L1 Radius");

  TH1F* hL2L1dEta = new TH1F("hL2L1dEta", "", 50, -0.25, 0.25);
  hL2L1dEta->GetXaxis()->SetTitle("L2 - L1 Eta");

  TH1F* hL2L1dPhi = new TH1F("hL2L1dPhi", "", 50, -0.25, 0.25);
  hL2L1dPhi->GetXaxis()->SetTitle("L2 - L1 Phi");

  // EF Resolution vs L2 Plots
  TH1F* hEFL2dPt = new TH1F("hEFL2dPt", "", 50, -50, 50);
  hEFL2dPt->GetXaxis()->SetTitle("EF - L2 Pt (GeV)");

  TH1F* hEFL2dR = new TH1F("hEFL2dR", "", 50, 0.0, 0.5);
  hEFL2dR->GetXaxis()->SetTitle("EF - L2 Radius");

  TH1F* hEFL2dEta = new TH1F("hEFL2dEta", "", 50, -0.25, 0.25);
  hEFL2dEta->GetXaxis()->SetTitle("EF - L2 Eta");

  TH1F* hEFL2dPhi = new TH1F("hEFL2dPhi", "", 50, -0.25, 0.25);
  hEFL2dPhi->GetXaxis()->SetTitle("EF - L2 Phi");

  // L1 Resolution vs. Eta
  TH2F* hL1EtadPt = new TH2F("hL1EtadPt", "", 50, -2.5, 2.5, 50, -50, 50);
  hL1EtadPt->GetXaxis()->SetTitle("Visible Eta");
  hL1EtadPt->GetYaxis()->SetTitle("L1 - Vis Pt (GeV)");

  TH2F* hL1EtadR = new TH2F("hL1EtadR", "", 50, -2.5, 2.5, 50, 0.0, 0.5);
  hL1EtadR->GetXaxis()->SetTitle("Visible Eta");
  hL1EtadR->GetYaxis()->SetTitle("L1 - Vis Radius");

  TH2F* hL1EtadEta = new TH2F("hL1EtadEta", "", 50, -2.5, 2.5, 50, -0.25, 0.25);
  hL1EtadEta->GetXaxis()->SetTitle("Visible Eta");
  hL1EtadEta->GetYaxis()->SetTitle("L1 - Vis Eta");

  TH2F* hL1EtadPhi = new TH2F("hL1EtadPhi", "", 50, -2.5, 2.5, 50, -0.25, 0.25);
  hL1EtadPhi->GetXaxis()->SetTitle("Visible Eta");
  hL1EtadPhi->GetYaxis()->SetTitle("L1 - Vis Phi");

  // Counters
  int nEvent = 0;
  int nTrue = 0;
  int nGood = 0;
  int nL1 = 0;
  int nL2 = 0;
  int nEF = 0;

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
      float visPhi = tau->visible().Phi();

      //
      // Match truth with L1 trigger
      TrigTauL1Object* l1Tau = analyzer.chainList.getL1Tau(*tau);
      if (l1Tau == 0) continue;

      nL1++;
      float dPt  = l1Tau->Pt()/1000. - visPt;
      float dEta = l1Tau->Eta() - visEta;
      float dPhi = l1Tau->Phi() - visPhi;
      if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
      if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();

      hL1dPt->Fill(dPt);
      hL1dEta->Fill(dEta);
      hL1dPhi->Fill(dPhi);
      hL1dR->Fill(TMath::Sqrt(dEta*dEta+dPhi*dPhi));

      hL1EtadPt->Fill(visEta,dPt);
      hL1EtadEta->Fill(visEta,dEta);
      hL1EtadPhi->Fill(visEta,dPhi);
      hL1EtadR->Fill(visEta,TMath::Sqrt(dEta*dEta+dPhi*dPhi));

      //
      // Match truth with L2 trigger
      TrigTauL2Object* l2Tau = analyzer.chainList.getL2Tau(*tau);
      if (l2Tau == 0) continue;

      nL2++;
      dPt = l2Tau->Pt()/1000. - visPt;
      dEta = l2Tau->Eta() - visEta;
      dPhi = l2Tau->Phi() - visPhi;
      if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
      if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();

      hL2dPt->Fill(dPt);
      hL2dEta->Fill(dEta);
      hL2dPhi->Fill(dPhi);
      hL2dR->Fill(TMath::Sqrt(dEta*dEta+dPhi*dPhi));

      //
      // Match truth with EF trigger
      TrigTauRecEFObject* efTau = analyzer.chainList.getEFTau(*tau);
      if (efTau == 0) continue;

      nEF++;
      dPt = efTau->Pt()/1000. - visPt;
      dEta = efTau->Eta() - visEta;
      float dPhi = efTau->Phi() - visPhi;
      if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
      if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();

      hEFdPt->Fill(dPt);
      hEFdEta->Fill(dEta);
      hEFdPhi->Fill(dPhi);
      hEFdR->Fill(TMath::Sqrt(dEta*dEta+dPhi*dPhi));

    }

    // Loop over all L2 passing chains (independent of truth)
    unsigned int numL2 = analyzer.chainList.nPassL2Chains();
    for(unsigned int iL2 = 0; iL2 < numL2; iL2++) {
      TrigTauTrigChain chain = analyzer.chainList.passL2ChainEntry(iL2);

      float dPt = (chain.L2->Pt() - chain.L1->Pt())/1000.;
      float dEta = chain.L2->Eta() - chain.L1->Eta();
      float dPhi = chain.L2->Phi() - chain.L1->Phi();
      if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
      if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();

      hL2L1dPt->Fill(dPt);
      hL2L1dEta->Fill(dEta);
      hL2L1dPhi->Fill(dPhi);
      hL2L1dR->Fill(TMath::Sqrt(dEta*dEta+dPhi*dPhi));
    }

    // Loop over all EF passing chains (independent of truth)
    unsigned int numEF = analyzer.chainList.nPassEFChains();
    for(unsigned int iEF = 0; iEF < numEF; iEF++) {
      TrigTauTrigChain chain = analyzer.chainList.passEFChainEntry(iEF);

      float dPt = (chain.EF->Pt() - chain.L2->Pt())/1000.;
      float dEta = chain.EF->Eta() - chain.L2->Eta();
      float dEta = chain.EF->Phi() - chain.L2->Phi();
      if (dPhi < -TMath::Pi()) dPhi += TMath::TwoPi();
      if (dPhi > +TMath::Pi()) dPhi -= TMath::TwoPi();

      hEFL2dPt->Fill(dPt);
      hEFL2dEta->Fill(dEta);
      hEFL2dPhi->Fill(dPhi);
      hEFL2dR->Fill(TMath::Sqrt(dEta*dEta+dPhi*dPhi));
    }

  } // end loop over events

  gROOT->cd();

  TString plotfile = "resPlots";
  plotfile += append;

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","L1 Selected",10,10,800,620);
  c1->Divide(2,2);
  c1->cd(1); hL1dPt->Draw();
  c1->cd(2); hL1dR->Draw();
  c1->cd(3); hL1dEta->Draw();
  c1->cd(4); hL1dPhi->Draw();
  c1->Print(plotfile + ".ps(");

  TCanvas * c2 = (TCanvas *) gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c2","L2 Selected",10,10,800,620);
  c2->Divide(2,2);
  c2->cd(1); hL2dPt->Draw();
  c2->cd(2); hL2dR->Draw();
  c2->cd(3); hL2dEta->Draw();
  c2->cd(4); hL2dPhi->Draw();
  c2->Print(plotfile + ".ps");

  TCanvas * c3 = (TCanvas *) gROOT->Get("c3");
  if( c3 ) delete c3;
  c3 = new TCanvas("c3","EF Selected",10,10,800,620);
  c3->Divide(2,2);
  c3->cd(1); hEFdPt->Draw();
  c3->cd(2); hEFdR->Draw();
  c3->cd(3); hEFdEta->Draw();
  c3->cd(4); hEFdPhi->Draw();
  c3->Print(plotfile + ".ps");

  TCanvas * c4 = (TCanvas *) gROOT->Get("c4");
  if( c4 ) delete c4;
  c4 = new TCanvas("c4","L2 Selected",10,10,800,620);
  c4->Divide(2,2);
  c4->cd(1); hL2L1dPt->Draw();
  c4->cd(2); hL2L1dR->Draw();
  c4->cd(3); hL2L1dEta->Draw();
  c4->cd(4); hL2L1dPhi->Draw();
  c4->Print(plotfile + ".ps");

  TCanvas * c5 = (TCanvas *) gROOT->Get("c5");
  if( c5 ) delete c5;
  c5 = new TCanvas("c5","EF Selected",10,10,800,620);
  c5->Divide(2,2);
  c5->cd(1); hEFL2dPt->Draw();
  c5->cd(2); hEFL2dR->Draw();
  c5->cd(3); hEFL2dEta->Draw();
  c5->cd(4); hEFL2dPhi->Draw();
  c5->Print(plotfile + ".ps)");

//   TCanvas * c6 = (TCanvas *) gROOT->Get("c6");
//   if( c6 ) delete c6;
//   c6 = new TCanvas("c6","L1 Selected",10,10,800,620);
//   c6->Divide(2,2);
//   c6->cd(1); hL1EtadPt->Draw();
//   c6->cd(2); hL1EtadR->Draw();
//   c6->cd(3); hL1EtadEta->Draw();
//   c6->cd(4); hL1EtadPhi->Draw();
//   c6->Print(plotfile + ".ps");

  // Print out matching statistics
  analyzer.trigMan.print();
  cout << "Events:       " << nEvent << endl;
  cout << "True Taus:    " << nTrue << endl;
  cout << "Good Taus:    " << nGood << " -> " 
       << float(nGood)/nEvent << " Good/Ev" << endl;
  if (nGood>0) {
    cout << "L1 Matched:   " << nL1 << " -> "
	 << (100.*nL1)/nGood << "% L1/Good" << endl;
    cout << "L2 Matched:   " << nL2 << " -> "
	 << (100.*nL2)/nGood << "% L2/Good" << endl;
    cout << "EF Matched:   " << nEF << " -> "
	 << (100.*nEF)/nGood << "% EF/Good" << endl;
  }

  delete event;
  delete tree;
  delete file;

  // Output to plot
  return;
}

