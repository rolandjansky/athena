/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void track_sct_err(int SaveOpt=0) {

  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only tracking branches
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trk_*",1);
  
  // Get number of entries
  entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;

  // Get pointer to data
  tree->SetBranchAddress("trk_nTracks", &ntracks);
  tree->SetBranchAddress("trk_nSctHits", trk_sct_nhits);
  tree->SetBranchAddress("trk_SctErrTrk", sct_errtrk);
  tree->SetBranchAddress("trk_SctErrHit", sct_errhit);

  TCanvas *c11 = new TCanvas("c11","Tracking Offline Monitor: SCT Errors");
  c11->Divide(1,2);

  TH1F *histoTrk;
  sprintf(name,"SCT Track Errors");
  sprintf(name2,"SctErrTrk");
  histoTrk = new TH1F(name2,name,100,0.,0.02);

  TH1F *histoHit;
  sprintf(name,"SCT Hit Errors");
  sprintf(name2,"SctErrHit");
  histoHit = new TH1F(name2,name,100,0.2,0.025);

  for (ievent=0; ievent<entries; ievent++) {
    // Get data for current event
    tree->GetEntry(ievent);

    // loop over all tracks
    for (int track=0; track<ntracks; track++) {
      // loop over all pixel hits of current track
      for (hit=0; hit<trk_sct_nhits[track]; hit++) {
        histoTrk->Fill(sct_errtrk[track][hit]);
	histoHit->Fill(sct_errhit[track][hit]);
      }
    }
  }

  c11->cd(1);
  histoTrk->DrawCopy();
  c11->Update();
  delete histoTrk;

  c11->cd(2);
  histoHit->DrawCopy();
  c11->Update();
  delete histoHit;

  if (SaveOpt==1) c11->Print("SCT_track_err.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCT_track_err.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

