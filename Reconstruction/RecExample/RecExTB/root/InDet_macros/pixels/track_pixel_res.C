/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_pixel_res(int SaveOpt=0) {

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
  float pixel_res_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_res_eta[MAX_TRACKS][MAX_PIX_HITS];
  float pixel_pull_phi[MAX_TRACKS][MAX_PIX_HITS], pixel_pull_eta[MAX_TRACKS][MAX_PIX_HITS];

  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nPixelHits", pixel_nhits);
  tree->SetBranchAddress("trk_PixResPhi", pixel_res_phi);
  tree->SetBranchAddress("trk_PixResEta", pixel_res_eta);
  tree->SetBranchAddress("trk_PixPullPhi", pixel_pull_phi);
  tree->SetBranchAddress("trk_PixPullEta", pixel_pull_eta);


  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: Pixel Residuals");
  c11->Divide(2,2);

  TH1F *histoResPhi;
  sprintf(name,"Pixel Residuals Phi");
  sprintf(name2,"PixResPhi");
  histoResPhi = new TH1F(name2,name,100,-0.05,0.05);

  TH1F *histoResEta;
  sprintf(name,"Pixel Residuals Eta");
  sprintf(name2,"PixResEta");
  histoResEta = new TH1F(name2,name,100,-0.3,0.3);

  TH1F *histoPullPhi;
  sprintf(name,"Pixel Pull Phi");
  sprintf(name2,"PixPullPhi");
  histoPullPhi = new TH1F(name2,name,100,-6.,6.);

  TH1F *histoPullEta;
  sprintf(name,"Pixel Pull Eta");
  sprintf(name2,"PixPullEta");
  histoPullEta = new TH1F(name2,name,100,-6.,6.);

  int hit;

  for (event=0; event<entries; event++) {
    // Get data for current event
    tree->GetEntry(event);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      //if (pixel_nhits[track] == 3){
      // loop over all pixel hits of current track
      for (hit=0; hit<pixel_nhits[track]; hit++) {
        histoResPhi->Fill(pixel_res_phi[track][hit]);
	histoResEta->Fill(pixel_res_eta[track][hit]);
	histoPullPhi->Fill(pixel_pull_phi[track][hit]);
	histoPullEta->Fill(pixel_pull_eta[track][hit]);
      }
      //}
    }
  }

  c11->cd(1);
  histoResPhi->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(2);
  histoResEta->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(3);
  histoPullPhi->Draw(HISTOTYPE);
  c11->Update();

  c11->cd(4);
  histoPullEta->Draw(HISTOTYPE);
  c11->Update();

  if (SaveOpt==1) c11->Print("Track_Pixels_Res_pulls.ps");
  else if(SaveOpt==2) { sprintf(sname,"Track_Pixels_Res_pulls.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}

