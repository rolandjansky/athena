/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void clusHits_vs_z(int SaveOpt=0) {

  cout<<"Building histograms..."<<endl;
  
  // Styles
  gStyle->SetOptStat("nemr");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_clus_*",1);
  
  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  TH1F *histo[MODPLANES];

  for (int i=0;i<MODPLANES;i++) {
    
    if(i%1 == 0) cout<<"."; fflush(stdout); 

    layer = i/4; phi = (i/2)%2; side = i%2;
    module = i/2; plane = 2*module+(side+1)%2;
    
    sprintf(name,"Histogram: [%d,%d,%d] mod: %d plane: %d",layer,phi,side,module,plane);
    sprintf(name2,"clusHits_vs_z[%d,%d,%d]",layer,phi,side);
    histo[i] = new TH1F(name2,name,150,-60.5,60.5);


    if (phi==0) histo[i]->SetFillColor(kRed); 
    else histo[i]->SetFillColor(kBlue); 
   
    sprintf(options,"sct_clus_layer==%d && sct_clus_phi==%d && sct_clus_side==%d",layer,phi,side);
    tree->Draw("sct_clus_z",options,"goff");

    for (int ii=0;ii<tree->GetSelectedRows();ii++)
      {
	clusHit=*(tree->GetV1()+ii);
	histo[i]->Fill(clusHit);
      }
    
    histo[i]->GetXaxis()->SetTitle("Z axis (mm)");
    histo[i]->GetYaxis()->SetTitle("Cluster Hits");
    histo[i]->SetTitle(name);
  }

  c11 = new TCanvas("clusHits_vs_z","SCT Offline Monitor - Cluster Hits vs Z");
  c11->Divide(4,4);
  
  for (int i=0;i<MODPLANES;i++) {
    c11->cd(i+1);
    histo[i]->Draw();
    c11->Update();
  }


  
  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("SCT_histograms.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCTclusHits_vs_z.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
