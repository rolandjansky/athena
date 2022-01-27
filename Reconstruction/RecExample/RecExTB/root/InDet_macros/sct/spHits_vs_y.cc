/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void spHits_vs_y(int SaveOpt=0) {

  cout<<"Building histograms..."<<endl;
  
  // Styles
  gStyle->SetOptStat("nemr");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  int j=1,jj=5; // for better visualization
      
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("sct_sp_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11 = new TCanvas("spHits_vs_y","SCT Offline Monitor - SpacePoint Hits vs Y");
  c11->Divide(4,2);

  for (int i=0;i<MODS;i++) {
    
    if(i%1 == 0) cout<<"."; fflush(stdout); 

    layer = i/2; phi = i%2; module = i;
    
    sprintf(name,"histogram: [%d,%d] mod: %d",layer,phi,module);
    sprintf(name2,"spHits_vs_y[%d,%d]",layer,phi);

    if (phi==0) {
      TH1F *histo = new TH1F(name2,name,150,-0.5,60.5);
      histo->SetFillColor(kRed); 
      c11->cd(j);
      j++;
    }
    else {
      TH1F *histo = new TH1F(name2,name,150,-60.5,0.5);
      histo->SetFillColor(kBlue); 
      c11->cd(jj);
      jj++;
    }

    sprintf(options,"sct_sp_layer==%d && sct_sp_phi==%d",layer,phi);
    tree->Draw("sct_sp_y",options,"goff");
    
    for (int ii=0;ii<tree->GetSelectedRows();ii++)
      {
	spHit0=*(tree->GetV1()+ii);
	histo->Fill(spHit0);
      }
    
    histo->GetXaxis()->SetTitle("Y axis (mm)");
    histo->GetYaxis()->SetTitle("SpacePoint Hits");
    histo->SetTitle(name);    
    histo->DrawCopy();
    c11->Update();
    delete histo;
  }
  
  cout << "!\n" << endl;

  if (SaveOpt==1) c11->Print("SCT_histograms.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCTspHits_vs_y.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
