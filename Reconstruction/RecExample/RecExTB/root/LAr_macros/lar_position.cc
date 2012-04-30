/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void lar_position() {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // TTree* tree=(TTree*)_file0->Get("TB/tree");
  
  // build Canvas
  c42 = new TCanvas("c42","LAr Offline Monitor - position");
  c42->Divide(1,2);

  c42->cd(1);  
  tree->Draw("cl_eta_tb_em"); //eta position (impact point)
  c42->cd(2);  
  tree->Draw("cl_phi_tb_em"); //phi position (impact point)
  
}
