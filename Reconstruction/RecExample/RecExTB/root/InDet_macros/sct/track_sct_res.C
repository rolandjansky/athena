/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_sct_res(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Run",1);

  // Get number of entries
  entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nSctHits", trk_sct_nhits);
  tree->SetBranchAddress("trk_SctRes", sct_res);
  tree->SetBranchAddress("trk_SctPull", sct_pull);

  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: SCT Residuals");
  c11->Divide(1,2);

  TH1F *histoRes;
  sprintf(name,"SCT Residuals");
  sprintf(name2,"SctRes");
  histoRes = new TH1F(name2,name,90,-0.15,0.15);
  histoRes->SetXTitle("r = #vec{n}#vec{x}_{track} - #vec{n}#vec{x}_{hit} [mm]");
  histoRes->GetXaxis()->SetTitleFont(22);

  TH1F *histoPull;
  sprintf(name,"SCT Pull");
  sprintf(name2,"SctPull");
  histoPull = new TH1F(name2,name,100,-5.,5.);
  histoPull->SetXTitle("#frac{ r }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
  histoPull->GetXaxis()->SetTitleSize(0.03);
  histoPull->GetXaxis()->SetTitleOffset(1.);
  histoPull->GetXaxis()->SetTitleFont(22);

  for (ievent=0; ievent<entries; ievent++) {
    // Get data for current event
    tree->GetEntry(ievent);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      // loop over all pixel hits of current track
      for (hit=0; hit<trk_sct_nhits[track]; hit++) {
        histoRes->Fill(sct_res[track][hit]);
	histoPull->Fill(sct_pull[track][hit]);
      }
    }
  }

  c11->cd(1);
  histoRes->Draw();
  histoRes->Fit("gaus");
  histoRes->GetFunction("gaus")->SetLineColor(kRed);
  histoRes->GetFunction("gaus")->SetLineWidth(2);
  c11->Update();

  c11->cd(2);
  histoPull->Draw();
  histoPull->Fit("gaus");
  histoPull->GetFunction("gaus")->SetLineColor(kRed);
  histoPull->GetFunction("gaus")->SetLineWidth(2);
  c11->Update();
  
  if (SaveOpt==1) c11->Print("SCT_track_res_pulls.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCT_track_res_pulls.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}

