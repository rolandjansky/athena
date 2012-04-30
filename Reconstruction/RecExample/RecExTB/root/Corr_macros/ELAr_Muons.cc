/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ELAr_Muons(int SaveOpt=0){

  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("Eh_*",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("nmdtdig",1);
  tree->SetBranchStatus("nbol",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, ntracks=0;
  int run;
  float Eh_PresB;
  float Eh_EMB[3];
  int nmdtdig;
  float nbol;

  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("Eh_PresB", Eh_PresB);
  tree->SetBranchAddress("Eh_EMB", Eh_EMB);
  tree->SetBranchAddress("nmdtdig",&nmdtdig);
  tree->SetBranchAddress("nbol",&nbol);

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

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);
    histoE->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);
    if (nmdtdig > 50) { // muon 
    //if (nbol > 0) { // muon 
      histoEmu->Fill(Eh_PresB+Eh_EMB[0]+Eh_EMB[1]+Eh_EMB[2]);      
    }
  }

  TCanvas *c11 = new TCanvas("c22","Energy LAr");
  
  gPad->SetLogy();

  histoE->SetFillColor(kBlue);
  histoEmu->SetFillColor(kRed);
  c11->cd();
  histoE->Draw();
  histoEmu->Draw("same");

  c11->Update();

  leg = new TLegend(0.3,0.78,0.6,0.89);
  leg->AddEntry(histoE,"All events","f");
  leg->AddEntry(histoEmu,"Events with nmdtdig>50","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("LAr / Muons");
  leg->Draw();

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"ELAr_Muons.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
