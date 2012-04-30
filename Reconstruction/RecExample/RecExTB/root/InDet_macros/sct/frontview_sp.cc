/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void frontview_sp(int SaveOpt=0) {

  cout<<"Building viewes..."<<endl;
  
  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  int j=1,jj=5; // for better visualization
    
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_sp_*",1);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  TGraph *gr[MODS];

  for (int i=0;i<MODS;i++) {
    
    if(i%1 == 0) cout<<"."; fflush(stdout); 

    layer = i/2; phi = i%2; module = i;
    
    sprintf(name,"Frontview: [%d,%d] mod: %d",layer,phi,module);
    sprintf(name2,"frontview_sp[%d,%d]",layer,phi);

    sprintf(options,"sct_sp_layer==%d && sct_sp_phi==%d",layer,phi);
    tree->Draw("sct_sp_z:sct_sp_y",options,"goff");
    
    gr[i] = new TGraph (tree->GetSelectedRows(),tree->GetV1(),tree->GetV2());
    gr[i]->GetXaxis()->SetTitle("Z axis (mm)");
    gr[i]->GetYaxis()->SetTitle("Y axis (mm)");
    gr[i]->SetTitle(name);
  }

  c11 = new TCanvas("frontview_sp","SCT Offline Monitor - Frontview with SpacePoints");
  c11->Divide(4,2);

  for (int i=0;i<MODS;i++) {
    
    phi = i%2;
    
    if (phi==0) {
      gr[i]->SetMarkerColor(kRed); 
      c11->cd(j);
      j++;
    }
    else {
      gr[i]->SetMarkerColor(kBlue);
      c11->cd(jj);
      jj++;
    }
  
    gr[i]->Draw("AP");
    c11->Update();
  }

  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("SCT_histograms.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCTfrontview_sp.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
