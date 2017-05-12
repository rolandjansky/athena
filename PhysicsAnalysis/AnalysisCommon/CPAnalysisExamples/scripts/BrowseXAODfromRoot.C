/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gROOT.Reset();

  // Load the RootCore libraries

  gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");

  // Initialize root to use xAOD

  xAOD::Init();

  TString filename = "/afs/cern.ch/work/k/krasznaa/public/xAOD/19.0.X_rel_4/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.AOD.19.0.X_rel_4.pool.root";

  TFile * f = TFile::Open(filename,"read"); // Just an example xAOD file
  
  if (!f) std::cerr << "Can't open " << filename << std::endl;

  // Get a tree from the xAOD file 

  TTree * thetree = xAOD::MakeTransientTree(f); 

  // Ready to play with the tree 

  TCanvas C1;
  C1.cd();

  // Draw eta of the full electron container

  thetree.Draw("ElectronCollection.eta()");

  TCanvas C2; 
  C2.cd();

  // Draw the eta of the associated track

  thetree.Draw("ElectronCollection.trackParticle().eta()");

  TCanvas C3; 

  // Plot the number of jet constituents Vs jet pt

  TH2F * constHist = new TH2F("constHist","my favourite plot",100,0,500000,50,0,50);

  thetree.Draw("AntiKt4LCTopoJets.numConstituents():AntiKt4LCTopoJets.pt() >> constHist");

  constHist->Draw("COLZ");

}
