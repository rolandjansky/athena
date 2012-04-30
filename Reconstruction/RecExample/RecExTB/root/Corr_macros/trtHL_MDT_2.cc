/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trtHL_MDT_2(int SaveOpt=0) {

  cout<<"Building the correlations between TRT and MDT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",1);
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_rdo*",1);
  tree->SetBranchStatus("nmdtdig",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&n_rdo);
  tree->SetBranchAddress("trt_rdo_highlevel",hl);
  tree->SetBranchAddress("trt_rdo_timeoverthr",trttot);
  // MDT
  tree->SetBranchAddress("nmdtdig",&nmdtdig);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH1F *h_noTrack = new TH1F("h_noTrack","Correlation TRT / MDT",40,-0.5,39.5);
  TH1F *h_MooreTrack = new TH1F("h_MooreTrack","Correlation TRT / MDT",40,-0.5,39.5);
  
  for (int i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    int counter=0;
    // nbol > 0  means that exists Muons (MDT)
    // reconstructed Moore track
    //    if (nbol>0) {
    //    if (abs(alphabarrel) < 0.2) {
    if (nmdtdig > 50) {
      for(int j=0; j<n_rdo; j++) {
	// HighLevel = 1 means Electrons (TRT)
	if (hl[j]==1 && trttot[j]>7) counter++;
      }
      
      h_MooreTrack->Fill(counter);  
    }
    
    else {
      for(int j=0; j<n_rdo; j++) {
	// HighLevel = 1 means Electrons (TRT)
	if (hl[j]==1 && trttot[j]>7) counter++;
      }
      h_noTrack->Fill(counter);  
    }
    
  }
  
  sprintf(name,"Correlations Offline Monitor - TRT/MDT");
  sprintf(name2,"trtHL_MDT_2");
  TCanvas *c11 = new TCanvas(name2,name);
  c11->cd(1);

  h_noTrack->SetFillStyle(3004);
  h_noTrack->SetFillColor(kRed);

  h_noTrack->GetXaxis()->SetTitle("TRT RDOs (HL=1)");
  h_noTrack->GetYaxis()->SetTitle("Value");
  
  h_noTrack->DrawCopy();

  c11->Update();

  h_MooreTrack->SetFillColor(kBlue);
  h_MooreTrack->DrawCopy("SAME");

  c11->Update();

  leg = new TLegend(0.68,0.78,0.89,0.89);
  leg->AddEntry(h_noTrack,"no reconstructed Moore track","f");
  leg->AddEntry(h_MooreTrack,"reconstruted Moore track","f");
  leg->Draw();
  // and add a header (or "title") for the legend
  leg->SetHeader("TRT / MDT");
  leg->Draw();

  // Logarithmic scale
  // gPad->SetLogy();

  c11->Update();

  cout << "!\n" << endl;

  delete h_noTrack;
  delete h_MooreTrack;


  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();


  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRTMDT_corr1.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
