/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void lar_energy_resolution() {

  // Styles
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);

  // TTree* tree=(TTree*)_file0->Get("TB/tree");
  
  // build Canvas
  c41 = new TCanvas("c41","LAr Offline Monitor - energy resolution");
  c41->Divide(1,1);
  c41->cd(1);

  tree->Draw("cl_e_tb_em"); //Energy resolution
  
}
