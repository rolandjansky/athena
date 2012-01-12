/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Example macro to produce a plot of event rates (normalized according
// to some input argument) for a series of trigger menus.
// No MC information is used in producing these plots.
//
// Weight gives the event weight to be used in the weight plot.
// This could be a 1/sigma to give luminosity, or something
// similar to give rate in Hz.
//

// Forward declaration
TH1F* intRateHist(const TH1F* hist);

void eventRates(const char* filename="ntuples/trigtau.AOD.root",
		      TrigTauFlags::TRIGGERMENU menu, float weight=1) {

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

  // Formatting commands
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gROOT->cd();

  int nbins=50;
  float ptlo=0.;
  float pthi=100.;

  // L1 Rate Plots (both by ROI rate and event rate)
  TH1F* hL1Pt = new TH1F("hL1Pt", "", nbins, ptlo, pthi);
  hL1Pt->GetXaxis()->SetTitle("L1 Pt (GeV)");
  hL1Pt->GetYaxis()->SetTitle("L1 ROI Rate");

  TH1F* hL1EvPt = new TH1F("hL1EvPt", "", nbins, ptlo, pthi);
  hL1EvPt->GetXaxis()->SetTitle("L1 Pt (GeV)");
  hL1EvPt->GetYaxis()->SetTitle("Event Rate");

  // L2 Rate Plots
  TH1F* hL2Pt = new TH1F("hL2Pt", "", nbins, ptlo, pthi);
  hL2Pt->GetXaxis()->SetTitle("L2 Pt (GeV)");
  hL2Pt->GetYaxis()->SetTitle("L2 ROI Rate");

  TH1F* hL2EvPt = new TH1F("hL2EvPt", "", nbins, ptlo, pthi);
  hL2EvPt->GetXaxis()->SetTitle("L2 Pt (GeV)");
  hL2EvPt->GetYaxis()->SetTitle("Event Rate");

  // Counters
  int nEvent = 0;
  int nL1   = 0;
  int nL1Ev = 0;
  int nL2   = 0;
  int nL2Ev = 0;
  int nEF = 0;

  // Loop over all events
  int nev = int(tree->GetEntries());
  for(int iev = 0; iev<nev; ++iev) {
    event->reset();
    tree->GetEvent(iev);

    // Analyze this event
    analyzer.analyze(event);

    nEvent++;

    // Loop over all objects passing L1
    unsigned int numL1 = analyzer.chainList.nPassL1Chains();
    if (numL1==0) continue;  // No point going on

    nL1 += numL1;     // Keep track separately the number of ROIs and events
    nL1Ev++;

    float maxPt=0.;
    for (unsigned int iL1=0; iL1 < numL1; iL1++) {
      TrigTauTrigChain chain = analyzer.chainList.passL1ChainEntry(iL1);

      float l1Pt = chain.L1->m_TauEnergy/1000.;
      if (l1Pt > maxPt) maxPt = l1Pt;

      // One entry per ROI
      hL1Pt->Fill(l1Pt, weight);
    }
    // One entry per event, based on max Pt object found
    hL1EvPt->Fill(maxPt, weight);

    // Loop over all L2 passing chains (independent of truth)
    unsigned int numL2 = analyzer.chainList.nPassL2Chains();
    if (numL2==0) continue;

    nL2 += numL2;
    nL2Ev++;

    maxPt = 0.;
    for(unsigned int iL2 = 0; iL2 < numL2; iL2++) {
      TrigTauTrigChain chain = analyzer.chainList.passL2ChainEntry(iL2);

      float l2Pt = chain.L2->Pt()/1000.;
      if (l2Pt > maxPt) maxPt = l2Pt;

      // One entry per ROI
      hL2Pt->Fill(l2Pt, weight);
    }

    // One entry per event, based on max Pt object found
    hL2EvPt->Fill(maxPt, weight);

  } // end loop over events


  // Also make the integral rate histograms
  TH1F* hL1PtInt   = intRateHist(hL1Pt);
  hL1PtInt->GetYaxis()->SetTitle("Integral ROI Rate");
  TH1F* hL1EvPtInt = intRateHist(hL1EvPt);
  hL1EvPtInt->GetYaxis()->SetTitle("Integral Event Rate");
  TH1F* hL2PtInt   = intRateHist(hL2Pt);
  hL2PtInt->GetYaxis()->SetTitle("Integral ROI Rate");
  TH1F* hL2EvPtInt = intRateHist(hL2EvPt);
  hL2EvPtInt->GetYaxis()->SetTitle("Integral Event Rate");

  gROOT->cd();

  TString plotfile = "eventRates";
  plotfile += append;

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","L1 Selected",10,10,800,620);
  c1->Divide(2,2);
  c1->cd(1); hL1Pt->Draw(); gPad->SetLogy();
  c1->cd(2); hL1EvPt->Draw(); gPad->SetLogy();
  c1->cd(3); hL1PtInt->Draw(); gPad->SetLogy();
  c1->cd(4); hL1EvPtInt->Draw(); gPad->SetLogy();

  c1->Print(plotfile + ".ps(");

  TCanvas * c2 = (TCanvas *) gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c2","L2 Selected",10,10,800,620);
  c2->Divide(2,2);
  c2->cd(1); hL2Pt->Draw(); gPad->SetLogy();
  c2->cd(2); hL2EvPt->Draw(); gPad->SetLogy();
  c2->cd(3); hL2PtInt->Draw(); gPad->SetLogy();
  c2->cd(4); hL2EvPtInt->Draw(); gPad->SetLogy();
  c2->Print(plotfile + ".ps)");

  // Print out matching statistics
  analyzer.trigMan.print();
  cout << "Events:       " << nEvent << " " << weight*nEvent << endl;
  cout << "L1 Events:    " << nL1Ev << " " << weight*nL1Ev << endl;
  cout << "L1 ROIs:      " << nL1 << " " << weight*nL1 << endl;
  cout << "L2 Events:    " << nL2Ev << " " << weight*nL2Ev << endl;
  cout << "L2 ROIs:      " << nL2 << " " << weight*nL2 << endl;

  if (nL1Ev>0)
    cout << "L1 ROI/Ev:    " << float(nL1)/nL1Ev << endl;
  if (nL2Ev>0)
    cout << "L2 ROI/Ev:    " << float(nL2)/nL2Ev << endl;

  delete event;
  delete tree;
  delete file;

  // Output to plot
  return;
}

TH1F* intRateHist(const TH1F* h) {

  if (h == 0) return 0;

  TH1F* hint = new TH1F(*h);
  hint->Clear();
  unsigned int lastbin = h->GetNbinsX()-1;

  hint->SetBinContent(lastbin, h->GetBinContent(lastbin));

  // Now, go through and numerically integrate by adding previous bin
  for (int ibin=lastbin-1; ibin>=0; ibin--) {
    hint->SetBinContent(ibin, h->GetBinContent(ibin)+hint->GetBinContent(ibin+1));
    // Could also calculate errors, but not for now
  }
  return hint;
}
