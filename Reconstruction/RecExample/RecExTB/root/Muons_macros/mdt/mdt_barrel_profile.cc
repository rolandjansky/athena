/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void mdt_barrel_profile(int SaveOpt=0) {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",1);
  
  // build Canvas
  c11 = new TCanvas("c11","MDT Offline Monitor - barrel profiles");
 
  c11->Divide(2,3);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11->cd(1);
  tree->Draw("mwire","mstation==0 && meta==1 && mmulti==1 && mlayer==1");

  c11->cd(2);
  tree->Draw("mwire","mstation==0 && meta==2 && mmulti==1 && mlayer==1");

  c11->cd(3);
  tree->Draw("mwire","mstation==2 && meta==1 && mmulti==1 && mlayer==1");

  c11->cd(4);
  tree->Draw("mwire","mstation==2 && meta==2 && mmulti==1 && mlayer==1");

  c11->cd(5);
  tree->Draw("mwire","mstation==4 && meta==1 && mmulti==1 && mlayer==1");

  c11->cd(6);
  tree->Draw("mwire","mstation==4 && meta==2 && mmulti==1 && mlayer==1");



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

  if (SaveOpt==1) c11->Print("MDT_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"MDT_barrel_profile.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
