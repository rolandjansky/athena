/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// utility to take a root file full of TGraph2DErrors and create a root file with TF2s.
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

#include "TFile.h"
#include "TGraph2DErrors.h"
#include "TF2.h"
#include "TFitResult.h"

#include "CxxUtils/make_unique.h"

using CxxUtils::make_unique;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char** argv) {
  if (argc !=3) {
    cout << "Usage: " << argv[0] << " input.root output.root" << endl;
    return 1;
  }
  
  auto fin = make_unique<TFile>(argv[1], "READ");
  if (fin == nullptr) {
    cerr << "Could not open input file " << argv[1] << endl;
    return 2;
  }
  auto fout = make_unique<TFile>(argv[2], "RECREATE");
  if (fout == nullptr) {
    cerr << "Could not open output file " << argv[2] << endl;
    return 2;
  }

  auto etaLegFunc = [](double* x, double* par){
    double tanhEtaSq = std::tanh(x[0]);
    tanhEtaSq = tanhEtaSq*tanhEtaSq;
    //    tanhEtaSq = x[0]*x[0]/6.25; // try just eta
    //    double lp0 = 1.
    double lp2 = 1.5*tanhEtaSq - 0.5;// 2nd order legendre poly
    double lp4 = 0.125*(35.*tanhEtaSq*tanhEtaSq - 30.*tanhEtaSq + 3.);
    double lp6 = 0.0625*(231*tanhEtaSq*tanhEtaSq*tanhEtaSq - 315*tanhEtaSq*tanhEtaSq + 105*tanhEtaSq - 5);
    return par[0]*(1. + par[2]*(lp2-1) + par[4]*(lp4-1) + par[6]*(lp6-1)) + 
    par[1]*(1. + par[3]*(lp2-1) + par[5]*(lp4-1) + par[7]*(lp6-1))/x[1];
  };

  // loop through histograms in infile
  TIter next(fin->GetListOfKeys());
  while (TObject* obj = next()) {
    string objName = obj->GetName();
    cout << "Fitting " << objName << endl;
    TGraph2DErrors* gr = nullptr;
    fin->GetObject(obj->GetName(), gr);
    if (gr == nullptr) {
      cerr << "Could not retrive " << obj->GetName()
	   << " as a TGraph2DErrors." << endl;
      continue;
    }
    TF2* func = new TF2(objName.data(), etaLegFunc, -2.5, 2.5, 0.1, 100.0, 8);
    //    func->SetNpx(200);
    func->SetNpy(200);
    func->SetParameters(0.02, 0.1, 0.1, 0.1, 0., 0., 0., 0.);
    func->SetParLimits(0, 0., 5.);
    func->SetParLimits(1, 0., 5.);
    double ext = 0.7;
    func->SetParLimits(2, -ext, ext);
    func->SetParLimits(3, -ext, ext);
    func->SetParLimits(4, -ext, ext);
    func->SetParLimits(5, -ext, ext);
    func->SetParLimits(6, -ext, ext);
    func->SetParLimits(7, -ext, ext);

    // func->FixParameter(4, 0.);
    // func->FixParameter(5, 0.);
    func->FixParameter(6, 0.);
    func->FixParameter(7, 0.);
    auto result = gr->Fit(func, "S"); // "R" to use function range
    cout << "ndf, g.N = " << result->Ndf() << ", " << gr->GetN() << endl;
    func->Write();
  }

  
  return 0;
}
