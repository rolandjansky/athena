/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_trt_res(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPadBorderMode(0);
  gStyle->SetOptFit(1111);

  const int MAX_TRT_HITS = 100;           //max no. of trt hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("Run",1);

  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  int event, ntracks=0;
  int run;
  int trt_nhits[MAX_TRACKS];
  float trt_res[MAX_TRACKS][MAX_TRT_HITS];
  float trt_pull[MAX_TRACKS][MAX_TRT_HITS];

  tree->SetBranchAddress("Run", &run);
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nTrtHits", trt_nhits);
  tree->SetBranchAddress("trk_TrtRes", trt_res);
  tree->SetBranchAddress("trk_TrtPull", trt_pull);

  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: TRT Residuals");
  c11->Divide(1,2);
  
  /*
  TPad* c11_1 = (TPad*)(c11->GetPrimitive("c11_1"));
  c11_1->SetFillStyle(4000);
  TPad* c11_2 = (TPad*)(c11->GetPrimitive("c11_2"));
  c11_2->SetFillStyle(4000);
  */

  TH1F *histoRes;
  sprintf(name,"TRT Residuals");
  sprintf(name2,"TrtRes");
  histoRes = new TH1F(name2,name,100,-2.,2.);
  histoRes->SetXTitle("r = R_{track} - R_{hit} [mm]");
  histoRes->GetXaxis()->SetTitleFont(22);


  TH1F *histoPull;
  sprintf(name,"TRT Pull");
  sprintf(name2,"TrtPull");
  histoPull = new TH1F(name2,name,100,-15.,15.);
  histoPull->SetXTitle("#frac{ r }{ #sqrt{ #rho_{track}^{2} + #rho_{hit}^{2} } }");
  histoPull->GetXaxis()->SetTitleSize(0.03);
  histoPull->GetXaxis()->SetTitleOffset(1.);
  histoPull->GetXaxis()->SetTitleFont(22);


  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      // loop over all pixel hits of current track
      for (hit=0; hit<trt_nhits[track]; hit++) {
        histoRes->Fill(trt_res[track][hit]);
	histoPull->Fill(trt_pull[track][hit]);
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

  c11->cd();
  //sprintf(name,"%d events, Run %d", entries, run);
  sprintf(name,"Run %d", run);
  pt = new TPaveText(0.00230947,0.00322061,0.178984,0.037037,"br");
  pt->SetBorderSize(0);
  pt->SetFillColor(20);
  text = pt->AddText(name);

  sprintf(name,"%d events", entries);
  text = pt->AddText(name);
  pt->Draw();
  c11->Update();

  if (SaveOpt==1) c11->Print("TRT_track_res_pulls.ps");
  else if(SaveOpt==2) {sprintf(sname,"TRT_track_res_pulls.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

