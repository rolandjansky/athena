/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void straw_correls(int SaveOpt=0,int layer_TRT=0, int phiTRT=0) {

  cout<<"Building the correlations..."<<endl;
  
  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  int entries = tree->GetEntries();
  cout<<entries<<" entries"<<endl;
  
  tree->SetBranchAddress("trt_rdo_strawlayer",straw_layer);
  tree->SetBranchAddress("trt_rdo_layer",trtlayer); 
  tree->SetBranchAddress("trt_rdo_nrdos",&n_rdo);
  tree->SetBranchAddress("trt_rdo_phisector",trtphi);
  tree->SetBranchAddress("trt_rdo_straw",straw); 
 
  const int layerTRT = layer_TRT;
  
  TH2F *histo[STRAW_LAYER[layerTRT]];
  
  for (int i=0;i<STRAW_LAYER[layerTRT];i++) {
    sprintf(name,"Straw Layer %d-%d",i,i+1);
    sprintf(name2,"%d_Straw_layer",i);
    histo[i] = new TH2F(name2,name,25,-0.5,24.5,25,-0.5,24);
  }
  
  float n[30][MAX];
  for(int k=0;k<entries;k++) {

    if(k%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(k); 
    int index[30]={0};
    
    for(int j=0;j<n_rdo;j++){
      if (trtlayer[j]==layerTRT && trtphi[j]==phiTRT) {
	n[straw_layer[j]][index[straw_layer[j]]] = straw[j];
	index[straw_layer[j]]++;
      }
    }
    
    for(int i=0;i<(STRAW_LAYER[layerTRT]-1);i++){
      for(int ii=0;ii<index[i];ii++){
	for(int iii=0;iii<index[i+1];iii++){
	  histo[i]->Fill(n[i+1][iii],n[i][ii]);
	}
      } 
    }
  }
  
  sprintf(name,"TRT y-coordinate Correlations [%d,%d]",layerTRT,phiTRT);
  TCanvas *c11 = new TCanvas("trt_corr_y",name);
  if (layerTRT==0) c11->Divide(6,3); 
  else if (layerTRT==1) c11->Divide(6,4); 
  else if (layerTRT==2) c11->Divide(6,5); 

  c11->Clear("D");

  for(int i=0;i<STRAW_LAYER[layerTRT];i++){
    c11->cd(i+1);
    sprintf(axisY,"straw, straw_layer=%d",i+1);
    sprintf(axisX,"straw, straw_layer=%d",i);
    histo[i]->GetXaxis()->SetTitle(axisX);
    histo[i]->GetYaxis()->SetTitle(axisY);
    histo[i]->DrawCopy("COLZ");
    c11->Update();
    delete histo[i];
  }

  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("TRT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TRThitmaps.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}


