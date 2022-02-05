/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void row_vs_straw_detail(int SaveOpt=0,int trtLayer=0, int trtPhi=0, int PixelsLayer=0, int PixelsPhi=1) {

  cout<<"Building the correlations between Pixels and SCT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
    
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_rdo_*",1);
  tree->SetBranchStatus("pixel*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // Pixels
  tree->SetBranchAddress("pixel_nhits",&pixel_nhits);
  tree->SetBranchAddress("pixel_layer",pixel_layer);
  tree->SetBranchAddress("pixel_phi",pixel_phi);
  tree->SetBranchAddress("pixel_row",pixel_row);
  tree->SetBranchAddress("pixel_col",pixel_col);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&n_rdo);
  tree->SetBranchAddress("trt_rdo_straw",straw);
  tree->SetBranchAddress("trt_rdo_layer",trtlayer);
  tree->SetBranchAddress("trt_rdo_phisector",trtphi);
  tree->SetBranchAddress("trt_rdo_strawlayer",straw_layer);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *corr_detail[30];

  for (Int_t i=0;i<30;i++) {
    sprintf(name,"Pixels/TRT Corr %d",i);
    sprintf(name2,"corrpixelsTRT_%d%d%d%d_%d",trtLayer,trtPhi,PixelsLayer,PixelsPhi,i);
    if (trtLayer==0) corr_detail[i] = new TH2F(name2,name,16,-0.5,16.5,328/ROW_RESOLUTION,0.,328.);
    else if (trtLayer==1) corr_detail[i] = new TH2F(name2,name,23,-0.5,23.5,328/ROW_RESOLUTION,0.,328.);
    else if (trtLayer==2) corr_detail[i] = new TH2F(name2,name,28,-0.5,28.5,328/ROW_RESOLUTION,0.,328.);
  }
  
  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    for(int j=0; j<pixel_nhits; j++) {
      if (pixel_phi[j]==PixelsPhi) {
	for(int jj=0; jj<n_rdo; jj++) {
	  if (trtphi[jj]==trtPhi){
	    if (pixel_layer[j]==PixelsLayer) {
	      if (trtlayer[jj]==trtLayer) {
		int number = straw_layer[jj];
		corr_detail[number]->Fill(straw[jj],pixel_row[j]);
	      }
	    }
	  }
	}
      }
    }
  }
  
  
  sprintf(name,"Correlations Offline Monitor - Pixels/TRT");
  sprintf(name2,"Corr1_pixelsTRT_%d%d%d%d",trtLayer,trtPhi,PixelsLayer,PixelsPhi);
  
  TCanvas *c11 = new TCanvas(name2,name);

  if (trtLayer==0) c11->Divide(5,4);
  else if (trtLayer==1) c11->Divide(6,4); 
  else c11->Divide(6,5);

  c11->Clear("D");

  for (Int_t i=0;i<STRAW_LAYER[trtLayer];i++) {
    
    c11->cd(i+1);
    sprintf(name,"c11_%d",i);

    
    if (trtLayer==0 && i==19) return;
    
    sprintf(axisY,"row (Pixels) - [%d,%d]",PixelsLayer,PixelsPhi);
    sprintf(axisX,"straw (TRT) - [%d,%d,%d]",trtLayer,trtPhi,i);
    corr_detail[i]->GetXaxis()->SetTitle(axisX);
    corr_detail[i]->GetYaxis()->SetTitle(axisY);
    corr_detail[i]->Draw("COLZ");

    c11->Update();
  }

  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"PixelsTRT_corr1.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
