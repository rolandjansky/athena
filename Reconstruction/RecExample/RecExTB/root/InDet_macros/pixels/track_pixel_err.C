/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_pixel_err(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  const int MAX_PIX_HITS = 8;           //max no. of pixel hits per track
  const int MAX_TRACKS = 60;            //max no. of tracks per event
  const char HISTOTYPE[10] = "";

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  tree->SetBranchStatus("trk_mc_*",0);
  //  tree->SetBranchStatus("Event",1);
  
  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  // Get pointer to data
  int event, ntracks=0;
  int pixel_nhits[MAX_TRACKS];
  float pixel_errtrk_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_errtrk_eta[MAX_TRACKS][MAX_PIX_HITS];
  float pixel_errhit_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_errhit_eta[MAX_TRACKS][MAX_PIX_HITS];

  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nPixelHits", pixel_nhits);
  tree->SetBranchAddress("trk_PixErrTrkPhi", pixel_errtrk_phi);
  tree->SetBranchAddress("trk_PixErrTrkEta", pixel_errtrk_eta);
  tree->SetBranchAddress("trk_PixErrHitPhi", pixel_errhit_phi);
  tree->SetBranchAddress("trk_PixErrHitEta", pixel_errhit_eta);

  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: Pixel Errors");
  c11->Divide(2,2);

  TH1F *histoErrTrkPhi;
  sprintf(name,"Pixel Track Errors Phi");
  sprintf(name2,"PixErrTrkPhi");
  histoErrTrkPhi = new TH1F(name2,name,100,0.,0.01);

  TH1F *histoErrTrkEta;
  sprintf(name,"Pixel Track Errors Eta");
  sprintf(name2,"PixErrTrkEta");
  histoErrTrkEta = new TH1F(name2,name,100,0.0,0.15);

  TH1F *histoErrHitPhi;
  sprintf(name,"Pixel Hit Errors Phi");
  sprintf(name2,"PixErrHitPhi");
  histoErrHitPhi = new TH1F(name2,name,100,0.01,0.015);

  TH1F *histoErrHitEta;
  sprintf(name,"Pixel Hit Errors Eta");
  sprintf(name2,"PixErrHitEta");
  histoErrHitEta = new TH1F(name2,name,100,0.1,0.2);

  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      // loop over all pixel hits of current track
      for (hit=0; hit<pixel_nhits[track]; hit++) {
        histoErrTrkPhi->Fill(pixel_errtrk_phi[track][hit]);
	histoErrTrkEta->Fill(pixel_errtrk_eta[track][hit]);
	histoErrHitPhi->Fill(pixel_errhit_phi[track][hit]);
	histoErrHitEta->Fill(pixel_errhit_eta[track][hit]);
      }
    }
  }

  c11->cd(1);
  histoErrTrkPhi->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(2);
  histoErrTrkEta->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(3);
  histoErrHitPhi->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(4);
  histoErrHitEta->Draw(HISTOTYPE);
  c11->Update();

  if (SaveOpt==1) c11->Print("Track_Pixels_Errors.ps");
  else if(SaveOpt==2) { sprintf(sname,"Track_Pixels_Errors.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}

