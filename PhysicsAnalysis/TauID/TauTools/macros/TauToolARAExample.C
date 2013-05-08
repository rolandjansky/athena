/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  
  //////////////////////////////////////////////////////////////////////////////////////////
  // Name    : TauToolARAExample.C
  // Package : offline/PhysicsAnalysis/TauID/TauTools
  // Author  : Jyothsna Komaragiri and Dugan O'Neil
  //
  // Description: 
  //          This example shows how to access TauExamples class in ARA CINT. 
  //
  // Usage:
  // % root -l TauToolARAExample.C
  // Don't forget to change the input ROOT file in test.py!!!!
  //
  //////////////////////////////////////////////////////////////////////////////////////////
  
  //Create the transient tree in ARA
  TPython::ExecScript("test.py");
  
  ////////////////////////////////////////////////////////////////////////
  // 0: Instantiate TauExamples class
  ////////////////////////////////////////////////////////////////////////
  TauExamples tauexample();

  ////////////////////////////////////////////////////////////////////////
  // 1: Set/Get the container keys to fetch from transient tree
  ////////////////////////////////////////////////////////////////////////
  tauexample.setTruthContainer("SpclMC");
  tauexample.setTauJetContainer("TauRecContainer");

  const TruthParticleContainer *mcpartTES = 0;  
  const Analysis::TauJetContainer *taurecTES = 0;

  tt = (TTree *)gROOT->Get("CollectionTree_trans");

  TBranch *br_mcpart = tt->GetBranch(tauexample.getTruthContainer().c_str());
  TBranch *br_taurec = tt->GetBranch(tauexample.getTauJetContainer().c_str());
  
  br_mcpart->SetAddress(&mcpartTES);
  br_taurec->SetAddress(&taurecTES);

  ////////////////////////////////////////////////////////////////////////
  // 2: Create histograms to be filled in
  ////////////////////////////////////////////////////////////////////////
  tauexample.CreateHistos();

  ////////////////////////////////////////////////////////////////////////
  // 3: Loop over the events and call the efficiency/resolution 
  // methods for each event
  ////////////////////////////////////////////////////////////////////////
  for (Long64_t jentry=0; jentry < tt.GetEntries(); jentry++) {
    
    br_mcpart->GetEntry(jentry);
    br_taurec->GetEntry(jentry);

    if(jentry%500 == 0) {
      std::cout << "---------------------------------------" << std::endl;
      std::cout << "entry:\t" << jentry << std::endl;
      std::cout << "---------------------------------------" << std::endl;
    }

    tauexample.getTauRecProngEfficiency(taurecTES, mcpartTES);
    tauexample.getTauRecResolution(taurecTES, mcpartTES);

  }//event loop

  ////////////////////////////////////////////////////////////////////////
  // 4: Finalize and dump the histograms in ROOT file
  ////////////////////////////////////////////////////////////////////////
  tauexample.finalizeProng();
  tauexample.finalizeResolution();

  //
  // uncomment the below lines if you want to store all the histos in a single ROOT file
  // otherwise the histos are stored seperately in two different ROOT files for each method.
  //TFile *exrootfile = new TFile("TauToolExamples.root","RECREATE");
  //tauexample.StoreHistos(exrootfile);

}
