/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void t_vs_R_strawlayer(int trt_phi=0, int trt_layer=0){
  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
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

  TCanvas *c11 = new TCanvas("trt_TRK2","Tracking Offline Monitor: t vs R plots");

  switch(trt_layer){
  case 0 : c11->Divide(5,4); break;
  case 1 : c11->Divide(6,4); break;
  case 2 : c11->Divide(6,5); break;
  }
 
  const int layerTRT = STRAW_LAYER[trt_layer];

  TH3F *histo[layerTRT];
  for(int i=0;i<STRAW_LAYER[trt_layer];i++){ 
    sprintf(name,"TRT_plot_t_vs_R%d",i);
    sprintf(name2,"TRT plot: t vs R, phi=%d, layer=%d, straw_layer=%d",trt_phi,trt_layer,i);
    histo[i] = new TH3F(name,name2,30,0,30,100,-3,3,70,0,70);
  }
  
  for (int event=0; event<entries; event++) {

    if(event%100 == 0) cout<<"."; fflush(stdout);

    tree->GetEntry(event);  
    for (int track=0; track<n_tracks; track++) {
      for (int hit=0; hit<n_hits[track]; hit++) {
	if(trk_phi[track][hit]==trt_phi && trk_layer[track][hit]==trt_layer){
	  histo[trk_straw_layer[track][hit]]->Fill(trk_straw[track][hit],R[track][hit],t[track][hit]);
	}
      }
    }
  }


  for(i=0; i<layerTRT; i++){  
    c11->cd(i+1);
    sprintf(axisX,"straw, straw_layer=%d",i);
    sprintf(axisY,"R (mm)");
    sprintf(axisZ,"t (ns)");
    histo[i]->GetXaxis()->SetTitle(axisX);
    histo[i]->GetYaxis()->SetTitle(axisY);
    histo[i]->GetZaxis()->SetTitle(axisZ); 
    histo[i]->DrawCopy();
    c11->Update();
    delete histo[i]; 
  }

  cout << "!\n" << endl;
}

