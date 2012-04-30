/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ELAr_Muons_TRT(int SaveOpt=0){

  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  const int MAX_TRACKS = 60;            //max no. of tracks per event
  const int MAX_TRT_HITS = 100;          //max no. of trt hits per track

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Eh_*",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("nmdtdig",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, ntracks=0;
  int run;
  int trt_nhits[MAX_TRACKS];
  int trk_Trt_HL[MAX_TRACKS][MAX_TRT_HITS];
  float Eh_PresB;
  float Eh_EMB[3];
  int nmdtdig;

  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nTrtHits", trt_nhits);
  tree->SetBranchAddress("trk_Trt_HL", trk_Trt_HL);
  tree->SetBranchAddress("Eh_PresB", Eh_PresB);
  tree->SetBranchAddress("Eh_EMB", Eh_EMB);
  tree->SetBranchAddress("nmdtdig",&nmdtdig);

  char name[80],name2[80];

  TH1F *histoE;
  sprintf(name,"Energy LAr");
  sprintf(name2,"E");
  histoE = new TH1F(name2,name,100,-200.,140000.);
  histoE->SetXTitle("E_{LAr} (MeV)");
  histoE->GetXaxis()->SetTitleFont(22);

  TH1F *histoEmu;
  sprintf(name,"Energy LAr");
  sprintf(name2,"Emu");
  histoEmu = new TH1F(name2,name,100,-200.,140000.);
  histoEmu->SetXTitle("E_{LAr} (MeV)");
  histoEmu->GetXaxis()->SetTitleFont(22);

  TH1F *histoEnergy;
  sprintf(name,"Energy LAr");
  sprintf(name2,"Energy");
  histoEnergy = new TH1F(name2,name,100,-200.,140000.);
  histoEnergy->SetXTitle("E_{LAr} (MeV)");
  histoEnergy->GetXaxis()->SetTitleFont(22);

  TH1F *histoEnergyTRT;
  sprintf(name,"Energy LAr");
  sprintf(name2,"Energy2");
  histoEnergyTRT = new TH1F(name2,name,100,-200.,140000.);
  histoEnergyTRT->SetXTitle("E_{LAr} (MeV)");
  histoEnergyTRT->GetXaxis()->SetTitleFont(22);

  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);
    histoE->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);
    if (nmdtdig > 50) { // muon 
      histoEmu->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);      
    }
    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      int nHLhits = 0;
      //cout << "ntrthits " << trt_nhits[track] << endl;
      // loop over all trt hits of current track
      for (hit=0; hit<trt_nhits[track]; hit++) {
	if (trk_Trt_HL[track][hit]==1){
	  nHLhits++;
	}
      }
      if (trt_nhits[track]>0.){
	float frac = (float) nHLhits/trt_nhits[track];
	//cout << "frac " << frac << " " << nHLhits << " " << trt_nhits[track] << endl;
	if (frac>0.2) {
	  histoEnergyTRT->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);
	}
	histoEnergy->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);
      }
    }
  }


  TCanvas *c11 = new TCanvas("c11","Energy LAr");
  
  gPad->SetLogy();

  histoEnergyTRT->SetFillColor(kBlue);
  histoEmu->SetFillColor(kRed);
  c11->cd();
  //histoEnergy->Draw();
  //histoEnergy2->Draw("same");
  histoE->Draw();
  histoEmu->Draw("same");
  histoEnergyTRT->Draw("same");

  c11->Update();

  leg = new TLegend(0.3,0.78,0.6,0.89);
  leg->AddEntry(histoEnergy,"All events","f");
  leg->AddEntry(histoEmu,"Events with nmdtdig>50","f");
  leg->AddEntry(histoEnergyTRT,"Events with fraction of HL per track > 0.2","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("LAr / Muons / TRT");
  leg->Draw();

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"ELAr_Muons.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
