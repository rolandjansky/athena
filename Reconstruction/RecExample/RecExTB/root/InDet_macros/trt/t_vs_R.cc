/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void t_vs_R(int SaveOpt=0){

  // Styles
  gStyle->SetOptStat("nemr");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
   
  // Get number of entries
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  //Get branches needed 
  tree->SetBranchAddress("trk_nTracks", &n_tracks);
  tree->SetBranchAddress("trk_nTrtHits",n_hits);
  tree->SetBranchAddress("trk_Trt_HL",trk_hl);
  tree->SetBranchAddress("trk_Trt_t",t);
  tree->SetBranchAddress("trk_Trt_R",R);

  TCanvas *c11 = new TCanvas("trt_track0","TRT Offline Monitor: t vs R plots [Tracking]");
  
  sprintf(name2,"TRT plot: t vs R");
  sprintf(name,"TRT plot: t vs R");
  
  TH2F *histo = new TH2F(name,name2,100,-3,3,70,0,70);
  
  for (int event=0; event<entries; event++) {

    if(event%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(event);  
    for (int track=0; track<n_tracks; track++) {
      for (int hit=0; hit<n_hits[track]; hit++) {
	histo->Fill(R[track][hit],t[track][hit]);
      }
    }
  }

  sprintf(axisX,"R (mm)");
  sprintf(axisY,"t (ns)");
  histo->GetXaxis()->SetTitle(axisX);
  histo->GetYaxis()->SetTitle(axisY); 
  histo->DrawCopy();
  c11->Update();
  delete histo;

  if (SaveOpt==1) c11->Print("TRT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRT_R_t.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

  cout << "!\n" << endl;
}

