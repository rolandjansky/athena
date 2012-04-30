/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void t_R_perstraw(int SaveOpt=0, int trt_phi=0, int trt_layer=0, int trt_sl=0, int trt_straw=10){

  // Styles
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
  tree->SetBranchAddress("trk_Trt_layer",trk_layer);
  tree->SetBranchAddress("trk_Trt_strawlayer",trk_straw_layer);
  tree->SetBranchAddress("trk_Trt_straw",trk_straw);
  tree->SetBranchAddress("trk_Trt_phisector",trk_phi);

  TCanvas *c11 = new TCanvas("trt_tracking0","TRT Offline Monitor: t vs R plot [Tracking]");
  
  sprintf(name,"TRT plot: t vs R");
  sprintf(name2,"TRT plot: t vs R, phi=%d, layer=%d, straw_layer=%d, straw=%d",trt_phi,trt_layer,trt_sl,trt_straw);
  TH2F *histo = new TH2F(name,name2,100,-3,3,70,0,70);
  
  for (int event=0; event<entries; event++) {

    if(event%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(event);  
    for (int track=0; track<n_tracks; track++) {
      for (int hit=0; hit<n_hits[track]; hit++) {
	if(trk_phi[track][hit]==trt_phi && trk_layer[track][hit]==trt_layer && trk_straw_layer[track][hit]==trt_sl && trk_straw[track][hit]==trt_straw){
	  histo->Fill(R[track][hit],t[track][hit]);
	}
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

  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("TRT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRT_R_t.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}

