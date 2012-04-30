/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixel_TRT_y(int SaveOpt=0,int trtPhi=0, int PixelsPhi = 0) {

  cout<<"Building the correlations between Pixels and TRT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  const int MAX3 = 500000;

  int pixel_sp_nspacepoints;
  float pixel_sp_y[MAX3];
  int pixel_sp_layer[MAX3],pixel_sp_phi[MAX3];

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_*",1);
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
  tree->SetBranchAddress("pixel_sp_y",pixel_sp_y);
  tree->SetBranchAddress("pixel_sp_nspacepoints",&pixel_sp_nspacepoints);
  tree->SetBranchAddress("pixel_sp_layer",pixel_sp_layer);
  tree->SetBranchAddress("pixel_sp_phi",pixel_sp_phi);

  // TRT
  float strawY[MAX],trt_tot[MAX],trt_radius[MAX];

  tree->SetBranchAddress("trt_nDCs",&n_rdo);
  tree->SetBranchAddress("trt_DC_straw_y",strawY);
  tree->SetBranchAddress("trt_DC_layer",trtlayer);
  tree->SetBranchAddress("trt_DC_phisector",trtphi);
  tree->SetBranchAddress("trt_DC_strawlayer",straw_layer);
  tree->SetBranchAddress("trt_DC_timeoverthr",trt_tot);
  tree->SetBranchAddress("trt_DC_radius",trt_radius);

  entries = tree->GetEntries();
  cout<< entries << " events\n" << endl;

  TH2F *corrhisto[9];

  for (Int_t i=0;i<9;i++) {
    sprintf(name,"Pixels/TRT Corr %d",i);
    sprintf(name2,"corr_pixelsTRT_Y__%d_phi%d",i,trtPhi);
    if (i==0 || i==3 || i==6) corrhisto[i] = new TH2F(name2,name,200,-70.,0.,200,-15.,15.);
    else if (i==1 || i==4 || i==7) corrhisto[i] = new TH2F(name2,name,200,-70,0.,200,-15.,15.);
    else corrhisto[i] = new TH2F(name2,name,200,-70,0,200,-15.,15.);
  }
  
  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout);

    tree->GetEntry(i);
    for(int j=0; j<pixel_sp_nspacepoints; j++) {
      if (pixel_sp_phi[j]==PixelsPhi) {
	for(int jj=0; jj<n_rdo; jj++) {
	  if (trtphi[jj]==trtPhi){
	    // Time over Thr must be greater that 7
	    if (trt_tot > 7.) {
	      if (pixel_sp_layer[j]==0) {
		if (trtlayer[jj]==0) {
		  corrhisto[0]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[0]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[1]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[1]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[2]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[2]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
	      }
	      else if (pixel_sp_layer[j]==1) {
		if (trtlayer[jj]==0) {
		  corrhisto[3]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[3]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[4]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[4]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[5]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[5]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
	      }
	      else if (pixel_sp_layer[j]==2) {
		if (trtlayer[jj]==0) {
		  corrhisto[6]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[6]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[7]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[7]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[8]->Fill(strawY[jj]+trt_radius[jj],pixel_sp_y[j]);
		  corrhisto[8]->Fill(strawY[jj]-trt_radius[jj],pixel_sp_y[j]);
		}
	      }
	    }
	  }
	}
      }
    }
  }
    
  sprintf(name,"Correlations Offline Monitor - Pixels/TRT");
  sprintf(name2,"corr0_pixelsTRT_phi_%d",trtPhi);
  TCanvas *c11 = new TCanvas(name2,name);

  c11->Divide(3,3);

  for (Int_t i=0;i<9;i++) {

    c11->cd(i+1);

    if (i<3) sprintf(axisY,"y (Pixels) - [%d,%d]",0,PixelsPhi);
    else if (i>2 && i<6) sprintf(axisY,"y (Pixels) - [%d,%d]",1,PixelsPhi);
    else sprintf(axisY,"y (Pixels) - [%d,%d]",2,PixelsPhi);

    if (i==0 || i==3 || i==6) sprintf(axisX,"straw_y (TRT) - [%d,%d]",0,trtPhi);
    else if (i==1 || i==4 || i==7) sprintf(axisX,"straw_y (TRT) - [%d,%d]",1,trtPhi);
    else sprintf(axisX,"straw_y (TRT) - [%d,%d]",2,trtPhi);

    corrhisto[i]->GetXaxis()->SetTitle(axisX);
    corrhisto[i]->GetYaxis()->SetTitle(axisY);
    corrhisto[i]->DrawCopy("COLZ");
    c11->Update();
    delete corrhisto[i];
  }

  cout << "!\n" << endl;


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

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"PixelsTRT_corr0.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
