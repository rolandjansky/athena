/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void strips_vs_straws(int SaveOpt=0,int trtPhi=0, int sctPhi = 0, int sctLayer=0, int trtLayer=2){

  cout<<"Building the correlations between SCT and TRT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
  
  char name[80],name1[80],name2[80],name3[80];
  int ievent,n_hit,n_rdo,straw[MAX],trt_layer[MAX],trt_phi[MAX],straw_layer[MAX];  
  float strip[MAX],sct_side[MAX],sct_phi[MAX],sct_layer[MAX];

  int entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // Strips
  tree->SetBranchAddress("sct_rdo_nhits",&n_hit);
  tree->SetBranchAddress("sct_rdo_layer",sct_layer);
  tree->SetBranchAddress("sct_rdo_phi",sct_phi);
  tree->SetBranchAddress("sct_rdo_side",sct_side);
  tree->SetBranchAddress("sct_rdo_strips",strip);
  // TRT
  tree->SetBranchAddress("trt_rdo_nrdos",&n_rdo);
  tree->SetBranchAddress("trt_rdo_straw",straw);
  tree->SetBranchAddress("trt_rdo_layer",trt_layer);
  tree->SetBranchAddress("trt_rdo_phisector",trt_phi);
  tree->SetBranchAddress("trt_rdo_strawlayer",straw_layer);

  //Defining the Canvas
  
  sprintf(name,"Correlations Offline Monitor - SCT/TRT");
  sprintf(name1,"corr SCT: layer %d,phi %d / TRT: layer %d, phi %d",sctLayer,sctPhi,trtLayer,trtPhi);
  c11 = new TCanvas(name1,name);
  c11->Divide(6,5);
 
  TH2F *corrhisto[31];
  for (int i=0;i<30;i++) {
    sprintf(name2,"SCT/TRT Corr %d",i);
    sprintf(name3,"corr SCT: layer %d,phi %d / TRT: layer %d, phi %d, straw_layer %d",sctLayer,sctPhi,trtLayer,trtPhi,i);
    corrhisto[i] = new TH2F(name3,name2,30,0,30,768,0,768);
     }


  //Filling the correlations
  
  for (int i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    for(int j=0; j<n_hit; j++) {
      if(sct_phi[j]==sctPhi && sct_side[j]==0 && sct_layer[j]==sctLayer){
        for(int jj=0; jj<n_rdo; jj++) {
	  for(int k=0;k<30;k++){
            if(trt_phi[jj]==trtPhi && trt_layer[jj]==trtLayer && straw_layer[jj]==k){
	      corrhisto[k]->Fill(straw[jj],strip[j]);
	    }
	  }
	}
      }
    }
  }




  char axisX[80],axisY[80];
  for (int k=0;k<30;k++){
    c11->cd(k+1);
    sprintf(axisY,"strips (SCT),phi=%d,layer=%d,side=%d",sctPhi,sctLayer,0);
    sprintf(axisX,"straw (TRT),phi=%d,layer=%d,straw_layer=%d",trtPhi,trtLayer,k);
    corrhisto[k]->GetXaxis()->SetTitle(axisX);
    corrhisto[k]->GetYaxis()->SetTitle(axisY);
    corrhisto[k]->Draw("COLZ");
    c11->Update();
    delete corrhisto[k];
  } 
  
  cout << "!\n" << endl;

   gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"SCTTRT_corr1gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
