/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelSP_vs_sctSP(int SaveOpt=0,int Phi=0) {

  // Phi=0 for SCT is Phi=1 for Pixels
  if (Phi==0) PixelsPhi = 1;
  else PixelsPhi = 0;

  cout<<"Building the correlations between Pixels and SCT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
    
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_sp_*",1);
  tree->SetBranchStatus("pixel_sp_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  const int MAX3 = 5000000;

  int sct_sp_nsps, pixel_sp_nsps;
  float sct_sp_x[MAX3],sct_sp_y[MAX3],sct_sp_z[MAX3];
  int sct_sp_layer[MAX3],sct_sp_phi[MAX3];
  float pixel_sp_x[MAX3],pixel_sp_y[MAX3],pixel_sp_z[MAX3];
  int pixel_sp_layer[MAX3],pixel_sp_phi[MAX3];

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // SCT
  tree->SetBranchAddress("sct_sp_nspacepoints",&sct_sp_nsps);
  tree->SetBranchAddress("sct_sp_x",sct_sp_x);
  tree->SetBranchAddress("sct_sp_y",sct_sp_y);
  tree->SetBranchAddress("sct_sp_z",sct_sp_z);
  tree->SetBranchAddress("sct_sp_layer",sct_sp_layer);
  tree->SetBranchAddress("sct_sp_phi",sct_sp_phi);
  tree->SetBranchAddress("sct_sp_phi",sct_sp_phi);
  // Pixels
  tree->SetBranchAddress("pixel_sp_nspacepoints",&pixel_sp_nsps);
  tree->SetBranchAddress("pixel_sp_x",pixel_sp_x);
  tree->SetBranchAddress("pixel_sp_y",pixel_sp_y);
  tree->SetBranchAddress("pixel_sp_z",pixel_sp_z);
  tree->SetBranchAddress("pixel_sp_layer",pixel_sp_layer);
  tree->SetBranchAddress("pixel_sp_phi",pixel_sp_phi);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *corrhisto[12];

  for (Int_t i=0;i<12;i++) {
    sprintf(name,"Pixels/SCT Corr with SP %d",i);
    sprintf(name2,"corr_pixel_SP_SCT_%d_phi%d",i,Phi);
    if (Phi==0) corrhisto[i] = new TH2F(name2,name,600,-0.5,60.5,160,-2.5,14.5);
    else if (Phi==1) corrhisto[i] = new TH2F(name2,name,600,-60.5,0.5,160,-2.5,14.5);
  }

  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout); 

    tree->GetEntry(i);
    for(int j=0; j<pixel_sp_nsps; j++) {
      if (pixel_sp_phi[j]==PixelsPhi) {
	
	for(int jj=0; jj<sct_sp_nsps; jj++) {
	  if (sct_sp_phi[jj]==Phi){
	    if (pixel_sp_layer[j]==0) {
	      if (sct_sp_layer[jj]==0) corrhisto[0]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==1) corrhisto[1]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==2) corrhisto[2]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==3) corrhisto[3]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	    }
	    else if (pixel_sp_layer[j]==1) {
	      if (sct_sp_layer[jj]==0) corrhisto[4]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==1) corrhisto[5]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==2) corrhisto[6]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==3) corrhisto[7]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	    }
	    else if (pixel_sp_layer[j]==2) {
	      if (sct_sp_layer[jj]==0) corrhisto[8]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==1) corrhisto[9]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==2) corrhisto[10]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	      else if (sct_sp_layer[jj]==3) corrhisto[11]->Fill(sct_sp_y[jj],pixel_sp_y[j]);
	    }
	  }
	}
	
      }  
    }
  }
    
  
  sprintf(name,"Correlations Offline Monitor - Pixels/SCT");
  sprintf(name2,"corr4_pixelsSCT_phi_%d",Phi);
  c11 = new TCanvas(name2,name);
  c11->Divide(4,3);

  for (Int_t i=0;i<12;i++) {
    c11->cd(i+1);
    if (i<4) sprintf(axisY,"y (Pixels) - [%d,%d]",0,PixelsPhi);
    else if (i>3 and i<8) sprintf(axisY,"y (Pixels) - [%d,%d]",1,PixelsPhi);
    else sprintf(axisY,"y (Pixels) - [%d,%d]",2,PixelsPhi);

    if (i==0 || i==4 || i==8) sprintf(axisX,"y (SCT) - [%d,%d,%d]",0,Phi,0);
    else if (i==1 || i==5 || i==9) sprintf(axisX,"y (SCT) - [%d,%d,%d]",1,Phi,0);
    else if (i==2 || i==6 || i==10) sprintf(axisX,"y (SCT) - [%d,%d,%d]",2,Phi,0);
    else sprintf(axisX,"y (SCT) - [%d,%d,%d]",3,Phi,0);

    corrhisto[i]->GetXaxis()->SetTitle(axisX);
    corrhisto[i]->GetYaxis()->SetTitle(axisY);
    corrhisto[i]->DrawCopy("COLZ");
    c11->Update();
    delete corrhisto[i];
  }

  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"PixelsSCT_corr2.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
