/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TROOT.h"


void plot_geometry(std::string file) {
  gROOT->SetBatch();
  std::cout << "Plotting " << file << std::endl;

  TFile tf(file.c_str());
  tf.cd();
  tf.ls();
  TTree *tree = dynamic_cast<TTree*>(tf.Get("extrapolation_charged"));
  //std::cout << file << std::endl;
  TCanvas c1("c1", "c1", 1600, 1200);
  tree->Draw("step_r:step_z", "sensitive||boundary");
  c1.SaveAs("geometry_rz.png");

  tree->Draw("step_x:step_y", "sensitive||boundary");
  c1.SaveAs("geometry_xy.png");

  tree->Draw("step_x:step_y", "abs(step_z)<800&&(sensitive||boundary)");
  c1.SaveAs("geometry_xy_brl.png");

  tree->Draw("step_x:step_y", "abs(step_z)>800&&(sensitive||boundary)");
  c1.SaveAs("geometry_xy_ec.png");
  
  tree->Draw("step_x:step_y:step_z", "sensitive||boundary");
  c1.SaveAs("geometry_xyz.png");
}
