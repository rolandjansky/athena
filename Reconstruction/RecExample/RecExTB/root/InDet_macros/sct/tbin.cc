/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void tbin(int SaveOpt=0) {

  cout<<"Building the histograms..."<<endl;
  
  // Styles
  gStyle->SetOptStat("");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only the branches we need
  tree->SetBranchStatus("sct_rdo_*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);
  
  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  TCanvas *c11 = new TCanvas("tbin","SCT Offline Monitor - TimeBin");
  c11->Divide(4,4);

  for (int i=0;i<MODPLANES;i++) {
    
    if(i%1 == 0) cout<<"."; fflush(stdout);  

    layer = i/4; phi = (i/2)%2; side = i%2;
    module = i/2; plane = 2*module+(side+1)%2;
    
    sprintf(name,"tbin: [%d,%d,%d] mod: %d plane: %d",layer,phi,side,module,plane);
    sprintf(name2,"tbin[%d,%d,%d]",layer,phi,side);
    TH2F *histo2 = new TH2F(name2,name,entries,0.,entries,3.,0.,2.);
    histo2->GetXaxis()->SetTitle("event");
    histo2->GetYaxis()->SetTitle("tbin");
    
    sprintf(options,"sct_rdo_layer==%d && sct_rdo_phi==%d && sct_rdo_side==%d",layer,phi,side);
    tree->Draw("Event:sct_rdo_tbin",options,"goff");
    
    TGraph *gr = new TGraph (tree->GetSelectedRows(),tree->GetV1(),tree->GetV2());
    
    if (phi==0) gr->SetMarkerColor(kRed); 
    else gr->SetMarkerColor(kBlue);
    
    c11->cd(i+1);
    histo2->DrawCopy();
    gr->Draw("P");
    c11->Update();
    delete histo2;
  }

  cout << "!\n" << endl;

  // Setting the default option again
  gStyle->SetOptStat("nemr");
  
  if (SaveOpt==1) c11->Print("SCT_histograms.ps");
  else if(SaveOpt==2) {sprintf(sname,"SCTtbin.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
