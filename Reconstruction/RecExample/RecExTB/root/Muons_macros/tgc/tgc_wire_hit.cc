/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void tgc_wire_hit(int SaveOpt=0) {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // Enabling only the branches we need
  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("tgc*",1);
  tree->SetBranchStatus("Event",1);
  tree->SetBranchStatus("Run",1);
  tree->SetBranchStatus("Time",1);
  tree->SetBranchStatus("IEvent",1);

  // build Canvas
  c11 = new TCanvas("c11","TGC Offline Monitor - wire hit");

  c11->Divide(2,4);

  entries = tree->GetEntries();
  cout<<entries<<" events\n"<<endl;

  c11->cd(1);
  tree->Draw("tgcchannel","tgcstation==42 && tgcisstrip==0 && tgcgasgap==1 ");

  c11->cd(2);
  tree->Draw("tgcchannel","tgcstation==42 && tgcisstrip==0 && tgcgasgap==2 ");

  c11->cd(3);
  tree->Draw("tgcchannel","tgcstation==42 && tgcisstrip==0 && tgcgasgap==3 ");

  c11->cd(5);
  tree->Draw("tgcchannel","tgcstation==44 && tgcisstrip==0 && tgcgasgap==1 ");

  c11->cd(6);
  tree->Draw("tgcchannel","tgcstation==44 && tgcisstrip==0 && tgcgasgap==2 ");

  c11->cd(7);
  tree->Draw("tgcchannel","tgcstation==46 && tgcisstrip==0 && tgcgasgap==1 ");

  c11->cd(8);
  tree->Draw("tgcchannel","tgcstation==46 && tgcisstrip==0 && tgcgasgap==2 ");

  if (SaveOpt==1) c11->Print("TGC_histograms.ps(");
  else if(SaveOpt==2) {sprintf(sname,"TGC_wire_hit.gif"); c11->Print(sname); }
  else if(SaveOpt==3) c11->Print(psfilename);

}
