/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void t_R(int trt_phi=0, int trt_layer=0, int trt_sl=0) {
  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  int n_tracks,n_hits[MAX_TRACKS],hl[MAX_TRACKS][MAX_TRT_HITS],phi[MAX_TRACKS][MAX_TRT_HITS],straw_layer[MAX_TRACKS][MAX_TRT_HITS],layer[MAX_TRACKS][MAX_TRT_HITS],straw[MAX_TRACKS][MAX_TRT_HITS];
  float t[MAX_TRACKS][MAX_TRT_HITS],R[MAX_TRACKS][MAX_TRT_HITS];
  
  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  //Get branches needed 
  tree->SetBranchAddress("trk_nTracks", &n_tracks);
  tree->SetBranchAddress("trk_nTrtHits",n_hits);
  tree->SetBranchAddress("trk_Trt_HL",hl);
  tree->SetBranchAddress("trk_Trt_t",t);
  tree->SetBranchAddress("trk_Trt_R",R);
  tree->SetBranchAddress("trk_Trt_layer",layer);
  tree->SetBranchAddress("trk_Trt_strawlayer",straw_layer);
  tree->SetBranchAddress("trk_Trt_straw",straw);
  tree->SetBranchAddress("trk_Trt_phisector",phi);

  TCanvas *c11 = new TCanvas("track_trt","Tracking Offline Monitor: t vs R plots");
  
  sprintf(name,"TRT plot: t vs R");
  sprintf(name2,"TRT plot: t vs R, phi=%d, layer=%d, straw_layer=%d",trt_phi,trt_layer,trt_sl);
  TH3F *histo = new TH3F(name,name2,30,0,30,100,-3,3,70,0,70);
  
  for (int event=0; event<entries; event++) {

    if(event%100 == 0) cout<<"."; fflush(stdout);

    tree->GetEntry(event);  
    for (int track=0; track<n_tracks; track++) {
      for (int hit=0; hit<n_hits[track]; hit++) {
	if(phi[track][hit]==trt_phi && layer[track][hit]==trt_layer && straw_layer[track][hit]==trt_sl){
	  histo->Fill(straw[track][hit],R[track][hit],t[track][hit]);
	}
      }
    }
  }

  sprintf(axisX,"straw");
  sprintf(axisY,"R (mm)");
  sprintf(axisZ,"t (ns)");
  histo->GetXaxis()->SetTitle(axisX);
  histo->GetYaxis()->SetTitle(axisY);
  histo->GetZaxis()->SetTitle(axisZ); 
  histo->DrawCopy();
  c11->Update();
  delete histo;

  cout << "!\n" << endl;

}

