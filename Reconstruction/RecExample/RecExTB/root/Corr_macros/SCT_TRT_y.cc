/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCT_TRT_y(int SaveOpt=0,int trtPhi=1, int SCTPhi=0) {

  gROOT->Reset();

  cout<<"Building the correlations between SCT and TRT..."<<endl;

  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);
    
 // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("trt_*",1);
  tree->SetBranchStatus("sct_sp_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  const int MAX3 = 5000000;

  int sct_sp_nsps;
  float sct_sp_x[MAX3],sct_sp_y[MAX3],sct_sp_z[MAX3];
  int sct_sp_layer[MAX3],sct_sp_phi[MAX3];

  // Get branches
  tree->SetBranchAddress("Event",&ievent);
  // SCT
  tree->SetBranchAddress("sct_sp_nspacepoints",&sct_sp_nsps);
  tree->SetBranchAddress("sct_sp_x",sct_sp_x);
  tree->SetBranchAddress("sct_sp_y",sct_sp_y);
  tree->SetBranchAddress("sct_sp_z",sct_sp_z);
  tree->SetBranchAddress("sct_sp_layer",sct_sp_layer);
  tree->SetBranchAddress("sct_sp_phi",sct_sp_phi);

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

  TH2F *corrhisto[12];

  for (Int_t i=0;i<12;i++) {
    sprintf(name,"SCT/TRT Corr %d",i);
    sprintf(name2,"corr_SCTTRT_Y__%d_phi%d",i,trtPhi);
    if (i==0 || i==3 || i==6 || i==9) corrhisto[i] = new TH2F(name2,name,200,-70.,0.,300,-15.,15.);
    else if (i==1 || i==4 || i==7 || i==10) corrhisto[i] = new TH2F(name2,name,200,-70.,0.,300,-15.,15.);
    else corrhisto[i] = new TH2F(name2,name,200,-70.,0.,300,-15.,15.);
  }

  for (Int_t i=0;i<entries;i++) {

    if(i%100 == 0) cout<<"."; fflush(stdout);

    tree->GetEntry(i);
    for(int j=0; j<sct_sp_nsps; j++) {
      if (sct_sp_phi[j]==SCTPhi) {
	for(int jj=0; jj<n_rdo; jj++) {
	  if (trtphi[jj]==trtPhi){
	    // Time over Thr must be greater that 7
	    if (trt_tot > 7.) {
	      if (sct_sp_layer[j]==0) {
		if (trtlayer[jj]==0) {
		  corrhisto[0]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[0]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[1]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[1]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[2]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[2]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
	      }
	      else if (sct_sp_layer[j]==1) {
		if (trtlayer[jj]==0) {
		  corrhisto[3]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[3]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[4]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[4]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[5]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[5]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
	      }
	      else if (sct_sp_layer[j]==2) {
		if (trtlayer[jj]==0) {
		  corrhisto[6]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[6]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[7]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[7]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[8]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[8]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
	      }
	      else if (sct_sp_layer[j]==3) {
		if (trtlayer[jj]==0) {
		  corrhisto[9]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[9]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==1) {
		  corrhisto[10]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[10]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
		else if (trtlayer[jj]==2) {
		  corrhisto[11]->Fill(strawY[jj]+trt_radius[jj],sct_sp_y[j]);
		  corrhisto[11]->Fill(strawY[jj]-trt_radius[jj],sct_sp_y[j]);
		}
	      }
      	    }
       	  }
	}
      }
    }
  }
    
  sprintf(name,"Correlations Offline Monitor - SCT/TRT");
  sprintf(name2,"corr0_SCTTRT_phi_%d",trtPhi);
  TCanvas *c11 = new TCanvas(name2,name);

  c11->Divide(3,4);

  for (Int_t i=0;i<12;i++) {

    c11->cd(i+1);

    if (i<3) sprintf(axisY,"y (SCT) - [%d,%d]",0,SCTPhi);
    else if (i>2 && i<6) sprintf(axisY,"y (SCT) - [%d,%d]",1,SCTPhi);
    else if (i>5 && i<9) sprintf(axisY,"y (SCT) - [%d,%d]",2,SCTPhi);
    else sprintf(axisY,"y (SCT) - [%d,%d]",3,SCTPhi);

    if (i==0 || i==3 || i==6 || i==9) sprintf(axisX,"straw_y (TRT) - [%d,%d]",0,trtPhi);
    else if (i==1 || i==4 || i==7 || i==10) sprintf(axisX,"straw_y (TRT) - [%d,%d]",1,trtPhi);
    else sprintf(axisX,"straw_y (TRT) - [%d,%d]",2,trtPhi);

    corrhisto[i]->GetXaxis()->SetTitle(axisX);
    corrhisto[i]->GetYaxis()->SetTitle(axisY);
    corrhisto[i]->DrawCopy("COLZ");
    c11->Update();
    delete corrhisto[i];
  }

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("Correlations_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"SCTTRT_corr0.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
