/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "physics.h"

#include "../MuonTriggerTool/MuonTriggerMatching.h"

#include <TFile.h>
#include <TChain.h>
#include <iostream>

void
example()
{
  //  TFile *f = new TFile("d3pd.root");

  TChain* t_phys = new TChain("physics");
  TChain* t_conf = new TChain("physicsMeta/TrigConfTree");
  t_phys->Add("d3pd.root"); 
  t_conf->Add("d3pd.root");
  //  t_phys->Add("d3pd2.root"); t_conf->Add("d3pd2.root");


  physics *d3pd = new physics(t_phys);
  MuonTriggerMatching mtm(d3pd, t_conf);
//   MuonTriggerMatching mtm(d3pd);
  mtm.setDeltaR(0.001);

  int nentries = t_phys->GetEntries();
  nentries = 10;
  for (int ientry = 0; ientry < nentries; ++ientry) {
    std::cout << "\n********** entry: " << ientry << " **********" << std::endl;
    d3pd->GetEntry(ientry);
    std::cout << "Number of Staco Muon = " << d3pd->mu_staco_n << std::endl;

    for (int ii = 0; ii < d3pd->mu_staco_n; ++ii) {
      std::cout << "===== Muon: " << ii << " =====" << std::endl;
      float pt = d3pd->mu_staco_pt->at(ii);
      float eta = d3pd->mu_staco_eta->at(ii);
      float phi = d3pd->mu_staco_phi->at(ii);
      std::cout << "OFFLINE: pt=" << pt << ", eta=" << eta << ", phi=" << phi <<std::endl;
      if (mtm.match(eta, phi, "EF_mu18_MG", "MU11")) std::cout << "~~~~~ Matched ~~~~~" << std::endl;; //emulate EF_mu18_MG_medium 
    }
  }
}
// eof
