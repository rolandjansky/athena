/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <string>
#include <tuple>
#include <limits>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

#include "TH1D.h"
#include "TGraph.h"

TGraph
fill_geometry_hist_rz(std::string filename,
    //std::tuple<size_t, double, double> rbnd,
    //std::tuple<size_t, double, double> zbnd,
    long long maxEvents = std::numeric_limits<long long>::max()) {
  std::cout << "Producing geometry histogram" << std::endl;
  TGraph g;

  //g.GetYaxis()->SetLimits(0, 3000);

  auto infile = TFile::Open(filename.c_str());
  //auto tree = dynamic_cast<TTree*>(infile->Get("extrapolation_charged"))

  //size_t nbinsr, double rmin, double rmax;
  //std::tie(nbinsr, rmin, rmax) = rbnd;
  //size_t nbinsz, double zmin, double zmax;
  //std::tie(nbinsz, zmin, zmax) = zbnd;
  //TH2D hist("geometry", "geometry", nbinsr, rmin, rmax, nbinsz, zmin, zmax);
  TTreeReader reader("extrapolation_charged", infile);

  size_t nEvents = std::min(reader.GetEntries(true), maxEvents);
  size_t n = 0;

  //for(size_t i=0;i<nEvents;i++) {
  //if(i%1000 == 0) std::cout << i << "/" << nEvents << std::endl;


  //}
  

  std::cout << "nEvents=" << nEvents << std::endl;


  TTreeReaderValue<std::vector<float>> vec_step_r(reader, "step_r");
  TTreeReaderValue<std::vector<float>> vec_step_z(reader, "step_z");

  while(reader.Next()) {
    if(n >= nEvents) break; // done
    if(n%1000 == 0) std::cout << n << "/" << nEvents << std::endl;

    size_t nSteps = (*vec_step_r).size();
    for(size_t i=0;i<nSteps;i++) {
      double step_r = (*vec_step_r).at(i);
      double step_z = (*vec_step_z).at(i);
      //std::cout << "step_r=" << step_r << " step_z=" << step_z << std::endl;
      g.SetPoint(g.GetN(), step_z, step_r);
    }

    n++;
  }

  g.GetHistogram()->SetMinimum(0);
  g.GetHistogram()->SetMaximum(1200);
  g.SetMarkerStyle(kDot);
  g.SetMarkerSize(0.4);

  return g;
}
