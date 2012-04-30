/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trtHL_MDT(int SaveOpt=0) {

  cout<<"Building the correlations between TRT and MDT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt*",1);
  tree->SetBranchStatus("nbol",1);
  tree->SetBranchStatus("nmdtdig",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  int trt_nrdos,nbol,nmdtdig;
  float trt_rdo_hl[MAX],trt_tot[MAX];

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&trt_nrdos);
  tree->SetBranchAddress("trt_rdo_highlevel",trt_rdo_hl);
  tree->SetBranchAddress("trt_rdo_timeoverthr",trt_tot);
  // MDT
  tree->SetBranchAddress("nbol",&nbol);
  tree->SetBranchAddress("nmdtdig",&nmdtdig);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH1F *href = new TH1F("href","Correlation TRT / MDT",2,-0.5,1.5);
  TH1F *h1 = new TH1F("h1","h1",2,-0.5,1.5);

  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    for(int j=0; j<trt_nrdos; j++) {
      if (trt_tot[j]>7) {
	// nbol > 0  means that exists Muons (MDT)
	// HL = 1 means Electrons (TRT)
	// if (nbol>0) h1->Fill(trt_rdo_hl[j]);
	if (nmdtdig > 50) h1->Fill(trt_rdo_hl[j]);
	else href->Fill(trt_rdo_hl[j]);
      }
    }
  }

  sprintf(name,"Correlations Offline Monitor - TRT/MDT");
  sprintf(name2,"corr3_TRTMDT");
  TCanvas *c11 = new TCanvas(name2,name);
  c11->cd(1);
  href->SetFillStyle(3004);
  href->SetFillColor(kRed);

  float total = href->Integral();
  float bin1 = href->GetBinContent(1);  
  float bin2 = href->GetBinContent(2);  
  bin1 = 100*(bin1/total);
  bin2 = 100*(bin2/total);
  href->SetBinContent(1,bin1);
  href->SetBinContent(2,bin2);

  href->GetXaxis()->SetNdivisions(2,kTRUE);
  href->GetXaxis()->SetTitle("HighLevel RDOs");
  href->GetYaxis()->SetTitle("Value (%)");
  
  href->DrawCopy();

  float total = h1->Integral();
  float bin1 = h1->GetBinContent(1);  
  float bin2 = h1->GetBinContent(2);  
  bin1 = 100*(bin1/total);
  bin2 = 100*(bin2/total);
  h1->SetBinContent(1,bin1);
  h1->SetBinContent(2,bin2);

  h1->SetFillStyle(3017);
  h1->SetFillColor(kBlue);
  h1->DrawCopy("SAME");

  c11->Update();

  leg = new TLegend(0.68,0.78,0.89,0.89);
  leg->AddEntry(href,"no reconstructed Moore track","f");
  leg->AddEntry(h1,"reconstruted Moore track","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("TRT / MDT");
  leg->Draw();

  c11->Update();
  
  delete href;
  delete h1;

  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRTMDT_corr0.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
