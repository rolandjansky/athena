/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// W. H. Bell
// A root macro to read the data members written into ntuple
// format by the CBNTAA_TileTTL1 algorithm.
//
// For release 13.0.30 use Cintex::Enable:
//
// root [0]  Cintex::Enable();                                  
// root [1] .x TileTTL1Plots.C("test.root","test.eps",false,15.0);
//
//

void TileTTL1Plots(char *rootFile, char *psFile, bool is_mbts, float threshold) 
{
  // Do a reset to clear memory.
  gROOT->Reset();

  // Open the file
  TFile f(rootFile);

  // Get the tree and set the addresses.
  TTree *tree = (TTree*)f.Get("CollectionTree");
  Int_t           nttl1;
  vector<long>    *side;
  vector<long>    *eta;
  vector<long>    *phi;
  vector<vector<float> > *samples;
 
  tree->SetBranchAddress("TileTTL1nttl1", &nttl1);
  tree->SetBranchAddress("TileTTL1side", &side);
  tree->SetBranchAddress("TileTTL1eta", &eta);
  tree->SetBranchAddress("TileTTL1phi", &phi);
  tree->SetBranchAddress("TileTTL1samples", &samples); 

  // Expected ranges of variables.
  int nttl_max, phi_max, eta_max; 
  int nttl_min = 0, phi_min = 0, eta_min = 0;
  if(!is_mbts) {
    nttl_max = 2048;
    phi_max = 63;
    eta_max = 15;
  }
  else {
    nttl_max = 32;
    phi_max = 7;
    eta_max = 1;
  }

  // Create output histograms
  Int_t nbins;
  Float_t min, max;

  min = -0.5;
  max = nttl_max + 0.5 + 0.1*nttl_max;
  nbins = (Int_t)(max - min);
  if(nbins>1000) {
    nbins /= 40;
  }
  TH1F *h_nttl1 = new TH1F("nttl1","nttl1",nbins,min,max);
  
  TH1D *h_sample_4;
  if(!is_mbts) { 
     h_sample_4 = new TH1D("sample_4","sample_4",140,-20.0,120.0);
  }
  else {
     h_sample_4 = new TH1D("sample_4","sample_4",150,-60.0,240.0);
  }
  TH2F *h_eta_phi_m1 = new TH2F("eta_phi_m1","eta_phi_m1",
				eta_max-eta_min,eta_min,eta_max,
				phi_max-phi_min,phi_min,phi_max);
  TH2F *h_eta_phi_p1 = new TH2F("eta_phi_p1","eta_phi_p1",
				eta_max-eta_min,eta_min,eta_max,
				phi_max-phi_min,phi_min,phi_max);
  TH1D *h_samples = new TH1D("samples","samples",9,-0.5,8.5);

  Long64_t i, nentries = tree->GetEntries();
  for(i=0;i<nentries;i++) { 
    tree->GetEntry(i);

    //std::cout << "nttl1=" << nttl1 << std::endl;
    h_nttl1->Fill(nttl1,1.0); // Histogram nttl1
    
    for(Int_t ittl1=0; ittl1<nttl1; ittl1++) { // loop over ttl1
      h_sample_4->Fill((*samples)[ittl1][4],1.0);
     
      if((*samples)[ittl1][4]>threshold) {
	for(Int_t isample=0; isample<9; isample++) { // loop over time samples.
	  h_samples->Fill(isample,(*samples)[ittl1][isample]);
	}
	if((*side)[ittl1]==-1) {
	  h_eta_phi_m1->Fill((*eta)[ittl1],(*phi)[ittl1],1.0);
	}
	else if ((*side)[ittl1]==1) {
	  h_eta_phi_p1->Fill((*eta)[ittl1],(*phi)[ittl1],1.0);
	}
	else {
	  std::cerr << "side[" << ittl1 << "]=" << (*side)[ittl1] << " is out of range." << std::endl;
	  continue;
	}
      }
    }
  }
  
  // Create plots
  TCanvas *c1 = new TCanvas("c1","TileTTL1 Plots",200,10,600,480);
  c1->Divide(2,3);
  c1_1->cd();
  h_nttl1->Draw();
  c1_2->cd();
  h_sample_4->Draw();
  c1_3->cd();
  h_eta_phi_m1->Draw("BOX");
  c1_4->cd();
  h_eta_phi_p1->Draw("BOX");
  c1_5->cd();
  h_samples->Draw();
  c1->Print(psFile);
}
