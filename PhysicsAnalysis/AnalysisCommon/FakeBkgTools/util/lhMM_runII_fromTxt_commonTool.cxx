/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TFile.h>
#include <TRandom3.h>
#include "FakeBkgTools/LhoodMMEvent.h"
#include "FakeBkgTools/LhoodMM_tools.h"

int main ( int argc,char *argv[] ){

  TRandom3 rand(349867);

  LhoodMM_tools* l = new LhoodMM_tools();
  if (l ==0) {
    std::cout << "null!" << std::endl;
  }

  TH1F* h_ht = new TH1F("ht", "ht", 10, 0, 1000);
  float ht;
  TH1F* h_phi = new TH1F("phi", "phi", 10, 0, 7);
  float phi;
  TH2F* h_2d = new TH2F("ht_vs_phi", "ht_vs_phi", 10, 0, 7, 10, 0, 1000);
  l->register1DHistogram(h_ht, &ht);
  l->register1DHistogram(h_phi, &phi);
  l->register2DHistogram(h_2d, &phi, &ht);

  // loop over all the input txt files
  for (int itxtfile = 1; itxtfile < argc-1; itxtfile++) {
    
    char* txtFilename = argv[itxtfile];
    fstream txtFile(txtFilename);
     while (!txtFile.eof()) {
       std::cout << "reading..." << std::endl;
      int nlep;
      std::vector<bool> isTight_vals;
      std::vector<double> realEff_vals;
      std::vector<double> fakeEff_vals;
      txtFile >> nlep; 
      std::vector<FakeBkgTools::Particle> particles;
      for (int ilep = 0; ilep<nlep; ilep++) {
	bool is_tight;
	float real_eff;
	float fake_eff;
	FakeBkgTools::Particle p;
	txtFile >> is_tight >> real_eff >> fake_eff;
	p.is_tight = is_tight;
	p.real_eff = real_eff;
	p.fake_eff = fake_eff;

	particles.push_back(p);
      }

      ht = 1000.*rand.Uniform();
      phi = 2*TMath::Pi()*rand.Uniform();

      if (!txtFile.eof()) {
	l->BaseFakeBkgTool::addEvent(particles);
      }
     }
     
     l->fillHistograms();

     TFile f("test.root", "RECREATE");
     f.cd();
     h_ht->Write();
     h_phi->Write();
     h_2d->Write();
     f.Close();
  }
    
  double nfakes, poserr, negerr;
  // l->setPrintLevel(1);
  FakeBkgTools::Yield y = l->getTotalYield();  // need to update once
                                               // syntax interpreter is available

  nfakes = y.value;
  FakeBkgTools::Uncertainty u;
  u.index = 0;
  poserr = y.uncertainties[u].up;
  negerr = y.uncertainties[u].down;

  std::cout << "nfakes = " << nfakes << " + " << poserr << " - " << -1.0*negerr << std::endl;

  TFile rootFile(argv[argc-1], "RECREATE");
  TH1F* h_count = new TH1F("h_count", "h_count", 2, 0, 2);
  h_count->SetBinContent(1, nfakes);
  h_count->SetBinError(1, TMath::Max(TMath::Abs(poserr), TMath::Abs(negerr)));
  rootFile.cd();
  h_count->Write();
  rootFile.Close();

}
