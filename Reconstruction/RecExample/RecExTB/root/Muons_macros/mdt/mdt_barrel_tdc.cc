/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void mdt_barrel_tdc(int SaveOpt=0) {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",1);
  
  // build Canvas
  c11 = new TCanvas("c11","MDT Offline Monitor - barrel TDC spectra");
 
  c11->Divide(2,3);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11->cd(1);
  tree->Draw("mdttdc","mstation==0 && meta==1");

  c11->cd(2);
  tree->Draw("mdttdc","mstation==0 && meta==2");

  c11->cd(3);
  tree->Draw("mdttdc","mstation==2 && meta==1");

  c11->cd(4);
  tree->Draw("mdttdc","mstation==2 && meta==2");

  c11->cd(5);
  tree->Draw("mdttdc","mstation==4 && meta==1");

  c11->cd(6);
  tree->Draw("mdttdc","mstation==4 && meta==2");

  if (SaveOpt==1) c11->Print("MDT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"MDT_barrel_tdc.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);
}
