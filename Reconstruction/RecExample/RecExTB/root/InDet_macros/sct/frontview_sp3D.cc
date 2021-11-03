/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void frontview_sp3D(int SaveOpt=0,char option[80]="") {

  cout<<"Building histograms..."<<endl;
  
  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPalette(1);

  int j=1,jj=5; // for better visualization
    
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_sp_*",1);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11 = new TCanvas("frontview_sp3D","SCT Offline Monitor - 3D frontview with SpacePoints");
  c11->Divide(4,2);

  for (int i=0;i<MODS;i++) {
    
    if(i%1 == 0) cout<<"."; fflush(stdout); 

    layer = i/2; phi = i%2; module = i;
    
    sprintf(name,"hitgram: [%d,%d] mod: %d",layer,phi,module);
    sprintf(name2,"frontview[%d,%d]",layer,phi);

    if (phi==0) {
      TH2F *histo2 = new TH2F(name2,name,240,-60.5,60.5,120,-0.5,60.5);
      histo2->SetFillColor(kRed); 
      c11->cd(j);
      j++;
    }
    else {
      TH2F *histo2 = new TH2F(name2,name,240,-60.5,60.5,120,-60.5,0.5);
      histo2->SetFillColor(kBlue); 
      c11->cd(jj);
      jj++;
    }

    sprintf(options,"sct_sp_layer==%d && sct_sp_phi==%d",layer,phi);
    tree->Draw("sct_sp_y:sct_sp_z",options,"goff");

    for (int ii=0;ii<tree->GetSelectedRows();ii++)
      {
	spHit0=*(tree->GetV2()+ii);
	spHit1=*(tree->GetV1()+ii);
	histo2->Fill(spHit0,spHit1);
      }

    histo2->GetXaxis()->SetTitle("Y axis (mm)");
    histo2->GetYaxis()->SetTitle("Z axis (mm)");
    histo2->GetZaxis()->SetLabelSize(0.03);
    histo2->SetTitle(name);    
    histo2->DrawCopy(option);
    c11->Update();
    delete histo2;
  }
  
  cout << "!\n" << endl;

  gStyle->SetOptStat("nemr");

  if (SaveOpt==1) c11->Print("SCT_histograms.ps)");
  else if(SaveOpt==2) {sprintf(sname,"SCTfrontview_sp3D.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
