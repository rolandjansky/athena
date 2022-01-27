/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void strip_vs_straw_detail(int SaveOpt=0,int trtLayer=0, int trtPhi=0, int SCTLayer=0, int SCTPhi=1) {

  cout<<"Building the correlations between SCT and TRT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
    
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_rdo_*",1);
  tree->SetBranchStatus("sct_rdo*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // SCT
  tree->SetBranchAddress("sct_rdo_nhits",&rdo_nhits);
  tree->SetBranchAddress("sct_rdo_layer",rdo_layer);
  tree->SetBranchAddress("sct_rdo_phi",rdo_phi);
  tree->SetBranchAddress("sct_rdo_side",rdo_side);
  tree->SetBranchAddress("sct_rdo_strips",rdo_strips);
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
    sprintf(name,"SCT/TRT Corr %d",i);
    sprintf(name2,"SCTTRT_%d%d%d%d_%d",trtLayer,trtPhi,SCTLayer,SCTPhi,i);
    if (trtLayer==0) corr_detail[i] = new TH2F(name2,name,16,-0.5,16.5,768,-0.5,767.5);
    else if (trtLayer==1) corr_detail[i] = new TH2F(name2,name,23,-0.5,23.5,768,-0.5,767.5);
    else if (trtLayer==2) corr_detail[i] = new TH2F(name2,name,28,-0.5,28.5,768,-0.5,767.5);
  }
  
  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    for(int j=0; j<rdo_nhits; j++) {
      if (rdo_phi[j]==SCTPhi && rdo_side[j]==0) {
	for(int jj=0; jj<n_rdo; jj++) {
	  if (trtphi[jj]==trtPhi){
	    if (sct_layer[j]==SCTLayer) {
	      if (trtlayer[jj]==trtLayer) {
		int number = straw_layer[jj];
		corr_detail[number]->Fill(straw[jj],rdo_strips[j]);
	      }
	    }
	  }
	}
      }
    }
  }
  
  
  sprintf(name,"Correlations Offline Monitor - SCT/TRT");
  sprintf(name2,"Corr1_sctTRT_%d%d%d%d",trtLayer,trtPhi,SCTLayer,SCTPhi);
  
  TCanvas *c11 = new TCanvas(name2,name);

  if (trtLayer==0) c11->Divide(5,4);
  else if (trtLayer==1) c11->Divide(6,4); 
  else c11->Divide(6,5);

  c11->Clear("D");

  for (Int_t i=0;i<STRAW_LAYER[trtLayer];i++) {
    
      c11->cd(i+1);

    if (trtLayer==0 && i==19) return;
    
    sprintf(axisY,"strip (SCT) - [%d,%d,%d]",SCTLayer,SCTPhi,0);
    sprintf(axisX,"straw (TRT) - [%d,%d,%d]",trtLayer,trtPhi,i);
    corr_detail[i]->GetXaxis()->SetTitle(axisX);
    corr_detail[i]->GetYaxis()->SetTitle(axisY);
    corr_detail[i]->DrawCopy("COLZ");
    c11->Update();
    delete corr_detail[i];
  }

  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");


  sprintf(title, "run %d (%d events)", run, entries);
  c11->cd();
  TPaveText *pt = new TPaveText(0.397, 0.001, 0.6, 0.011);
  pt->SetBorderSize(0);
  pt->SetFillColor(1);
  TText *text = pt->AddText(title);
  text->SetTextSize(0.013);
  text->SetTextColor(10);
  pt->Draw();
  c11->Update();

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"SCTTRT_corr0.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
