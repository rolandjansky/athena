/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TTreeReader.h"

#include <iostream>
#include <string>

#include "AtlasStyle.C"

void plot_zshift(std::string fname) {

  SetAtlasStyle();

  TCanvas c;

  auto infile = TFile::Open(fname.c_str());
  //TTree* tree = dynamic_cast<TTree*>(infile->Get("extrapolation_charged"));

  //std::cout << "tree = " << tree << std::endl;

  size_t slice_size = 3000;

  //tree->Draw("step_r:step_z>>slice1(1000, -1000, 1000, 200, 0, 200)", "sensitive", "", slice_size, 0);
  //TH2F* slice1 = dynamic_cast<TH2F*>(gDirectory->Get("slice1"));
  //std::cout << "slice1 = " << slice1 << std::endl;
  //slice1->SetMarkerColor(kRed);
  ////slice1->GetYaxis()->SetRange(0, 200);
  ////slice1->Draw();

  double zmin = 600, zmax = 800;
  double zbinw = 5;
  double rmin = 0, rmax = 200;

  TH1D zpos("zpos", "zpos", int(std::abs(zmax-zmin)/zbinw), zmin, zmax);

  TTreeReader rd("extrapolation_charged", infile);
  TTreeReaderValue<std::vector<float>> step_r_vec(rd, "step_r");
  TTreeReaderValue<std::vector<float>> step_z_vec(rd, "step_z");
  TTreeReaderValue<std::vector<int>> sensitive_vec(rd, "sensitive");
  TTreeReaderValue<int> evtNum(rd, "evtNum");
  
  size_t nEvents = rd.GetEntries(true);

  std::vector<TH2F> slices;
  size_t nSlices = nEvents / slice_size;
  std::cout << "nSlices = " << nSlices << std::endl;
  for(size_t i=0;i<nSlices;i++) {
    std::string hname = "slice_"+std::to_string(i+1);
    slices.emplace_back(hname.c_str(), hname.c_str(), 1000, 400, 800, 600, 0, 200);
    slices.back().SetMarkerSize(0.2);
    slices.back().GetXaxis()->SetTitle("z [mm]");
    slices.back().GetYaxis()->SetTitle("r [mm]");
  }

  std::vector<EColor> colors = {kRed, kBlue, kGreen, kMagenta, kOrange};

  std::vector<std::pair<int, double>> z_by_proc;
  std::vector<std::pair<int, double>> z_by_evt;

  size_t n = 0;
  while(rd.Next()) {

    

    size_t slice = (*evtNum) / slice_size;
    //size_t slice = n / slice_size;
    
    //std::cout << "slice = " << slice << std::endl;
    //if (slices.size() < slice+1) {
    //}
  
    size_t nSteps = (*step_z_vec).size();
    //std::cout << "nSteps:: " << nSteps << std::endl;

    for (size_t j=0;j<nSteps;j++) {
      bool sensitive = (*sensitive_vec).at(j) == 1;
      double step_z = (*step_z_vec).at(j);
      double step_r = (*step_r_vec).at(j);

      if (!sensitive) continue;
      //slices.at(slice).Fill((*step_z).at(j), (*step_r).at(j));
      slices.at(slice).Fill(step_z, step_r);

      if (zmin < step_z && step_z < zmax && rmin < step_r && step_r < rmax) {
        zpos.Fill(step_z);

        //g_z_n.SetPoint(g_z_n.GetN(), n, step_z);
        //g_z_eN.SetPoint(g_z_eN.GetN(), *evtNum, step_z);
        z_by_proc.emplace_back(n, step_z);
        z_by_evt.emplace_back(*evtNum, step_z);

      }
    }
    
    n++;
  }

  for(size_t i=0;i<slices.size();i++) {
    slices.at(i).SetMarkerColor(colors.at(i));
    if (i==0) slices.at(i).Draw();
    else slices.at(i).Draw("same");
  }
  c.SaveAs("zshift_rz.pdf");

  zpos.Draw();
  c.SaveAs("zshift.pdf");

  auto cmp = [](const auto& i1, const auto& i2) {
    return i1.first < i2.first;
  };
  std::sort(z_by_proc.begin(), z_by_proc.end(), cmp);
  std::sort(z_by_evt.begin(), z_by_evt.end(), cmp);

  TGraph g_z_proc;
  TGraph g_z_evt;

  g_z_proc.GetXaxis()->SetTitle("n");
  g_z_proc.GetYaxis()->SetTitle("z [mm]");

  g_z_proc.SetLineColor(kRed);
  g_z_evt.SetLineColor(kGreen);
  g_z_proc.SetMarkerColor(kRed);
  g_z_evt.SetMarkerColor(kGreen);

  
  g_z_proc.SetMarkerSize(0.2);
  g_z_evt.SetMarkerSize(0.2);

  for(size_t i=0;i<z_by_proc.size();i++) {
    g_z_proc.SetPoint(g_z_proc.GetN(), z_by_proc.at(i).first, z_by_proc.at(i).second);
    g_z_evt.SetPoint(g_z_evt.GetN(), z_by_evt.at(i).first, z_by_evt.at(i).second);
  }


  g_z_proc.Draw("ap");
  g_z_evt.Draw("p+same");
  c.SaveAs("z_over_n.pdf");

}
