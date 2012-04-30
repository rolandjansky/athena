/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void muctpi_pt(int SaveOpt=0) {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("MuCTPI*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // build Canvas
  c11 = new TCanvas("c11","MUCTPI Offline Monitor - PT");

  c11->Divide(1,1);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11->cd(1);
  tree->Draw("MuCTPI_pT");


  if (SaveOpt==1) c11->Print("MUCTPI_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"MUCTPI_pt.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
