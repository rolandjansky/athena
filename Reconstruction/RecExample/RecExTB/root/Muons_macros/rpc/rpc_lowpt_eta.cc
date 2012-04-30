/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void rpc_lowpt_eta(int SaveOpt=0) {

  gROOT->Reset();
  
  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("nrpcdig",1);
  tree->SetBranchStatus("r*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);
  
  // build Canvas
  c11 = new TCanvas("c11","RPC Offline Monitor - Low Pt eta profiles");
 
  c11->Divide(2,2);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11->cd(1);
  tree->Draw("rstrip","rmeasphi==0 && rstation==2 && rdoublr==1 && rdoublz==1 && rgasgap==1");

  c11->cd(2);
  tree->Draw("rstrip","rmeasphi==0 && rstation==2 && rdoublr==1 && rdoublz==2 && rgasgap==1");

  c11->cd(3);
  tree->Draw("rstrip","rmeasphi==0 && rstation==2 && rdoublr==1 && rdoublz==1 && rgasgap==2");

  c11->cd(4);
  tree->Draw("rstrip","rmeasphi==0 && rstation==2 && rdoublr==1 && rdoublz==2 && rgasgap==2");

  if (SaveOpt==1) c11->Print("RPC_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"RPC_lowpt_eta.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
