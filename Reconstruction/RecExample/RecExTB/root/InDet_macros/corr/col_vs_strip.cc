/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void col_vs_strip(int Phi=0) {

  // Phi=0 for SCT is Phi=1 for Pixels
  if (Phi==0) PixelsPhi = 1;
  else PixelsPhi = 0;

  gROOT->Reset();

  cout<<"Building the correlations between Pixels and SCT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_rdo_*",1);
  tree->SetBranchStatus("pixel*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // SCT
  tree->SetBranchAddress("sct_rdo_nhits",&sct_nhits);
  tree->SetBranchAddress("sct_rdo_strips",sct_strips);
  tree->SetBranchAddress("sct_rdo_layer",sct_layer);
  tree->SetBranchAddress("sct_rdo_phi",sct_phi);
  tree->SetBranchAddress("sct_rdo_side",sct_side);
  // Pixels
  tree->SetBranchAddress("pixel_nhits",&pixel_nhits);
  tree->SetBranchAddress("pixel_layer",pixel_layer);
  tree->SetBranchAddress("pixel_phi",pixel_phi);
  tree->SetBranchAddress("pixel_row",pixel_row);
  tree->SetBranchAddress("pixel_col",pixel_col);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  for (Int_t i=0;i<12;i++) {
    sprintf(name,"Pixels/SCT Corr %d",i);
    sprintf(name2,"corr_pixelsSCT_%d_phi%d",i,Phi);
    corrhisto[i] = new TH2F(name2,name,768,-0.5,767.5,144/COL_RESOLUTION,0.,144.);
  }


  for (Int_t i=0;i<entries;i++) {
    tree->GetEntry(i);
    for(int j=0; j<pixel_nhits; j++) {
      if (pixel_phi[j]==PixelsPhi) {
	
	for(int jj=0; jj<sct_nhits; jj++) {
	  if (sct_phi[jj]==Phi){
	    if (pixel_layer[j]==0) {
	      if (sct_layer[jj]==0 && rdo_side[jj]==0) corrhisto[0]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==1 && rdo_side[jj]==0) corrhisto[1]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==2 && rdo_side[jj]==0) corrhisto[2]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==3 && rdo_side[jj]==0) corrhisto[3]->Fill(sct_strips[jj],pixel_col[j]);
	    }
	    else if (pixel_layer[j]==1) {
	      if (sct_layer[jj]==0 && rdo_side[jj]==0) corrhisto[4]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==1 && rdo_side[jj]==0) corrhisto[5]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==2 && rdo_side[jj]==0) corrhisto[6]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==3 && rdo_side[jj]==0) corrhisto[7]->Fill(sct_strips[jj],pixel_col[j]);
	    }
	    else if (pixel_layer[j]==2) {
	      if (sct_layer[jj]==0 && rdo_side[jj]==0) corrhisto[8]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==1 && rdo_side[jj]==0) corrhisto[9]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==2 && rdo_side[jj]==0) corrhisto[10]->Fill(sct_strips[jj],pixel_col[j]);
	      else if (sct_layer[jj]==3 && rdo_side[jj]==0) corrhisto[11]->Fill(sct_strips[jj],pixel_col[j]);
	    }
	  }
	}
	
      }  
    }
  }
    
  

  sprintf(name,"Correlations Offline Monitor - Pixels/SCT");
  sprintf(name2,"corr1_pixelsSCT_phi_%d",Phi);
  c21 = new TCanvas(name2,name);
  c21->Divide(4,3);

  for (Int_t i=0;i<12;i++) {
    c21->cd(i+1);
    if (i<4) sprintf(axisY,"row (Pixels) - [%d,%d]",0,PixelsPhi);
    else if (i>3 and i<8) sprintf(axisY,"row (Pixels) - [%d,%d]",1,PixelsPhi);
    else sprintf(axisY,"row (Pixels) - [%d,%d]",2,PixelsPhi);

    if (i==0 || i==4 || i==8) sprintf(axisX,"strip (SCT) - [%d,%d,%d]",0,Phi,0);
    else if (i==1 || i==5 || i==9) sprintf(axisX,"strip (SCT) - [%d,%d,%d]",1,Phi,0);
    else if (i==2 || i==6 || i==10) sprintf(axisX,"strip (SCT) - [%d,%d,%d]",1,Phi,0);
    else sprintf(axisX,"strip (SCT) - [%d,%d,%d]",3,Phi,0);

    corrhisto[i]->GetXaxis()->SetTitle(axisX);
    corrhisto[i]->GetYaxis()->SetTitle(axisY);
    corrhisto[i]->Draw("COLZ");
    c21->Update();
  }
  gStyle->SetOptStat("nemr");

}
