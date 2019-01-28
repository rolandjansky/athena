/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TProfile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPad.h"
#include "TFile.h"

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect12c;
  ect12c.setESModel(egEnergyCorr::es2012c);
  ect12c.initialize();
  //  ect12c.setDebug(true);

  AtlasRoot::egammaEnergyCorrectionTool ect12a;
  ect12a.setESModel(egEnergyCorr::es2012a);
  ect12a.initialize();

  TH2D res2012c("res2012c","res012c", 48, -2.4, 2.4, 200, 5., 205.);
  TH2D res2012a("res2012a","res012a", 48, -2.4, 2.4, 200, 5., 205.);

  for(int i=1; i<=res2012c.GetNbinsX(); i++)
    for(int j=1; j<=res2012c.GetNbinsY(); j++) {

      double eta = res2012c.GetXaxis()->GetBinCenter(i);
      double ene  = res2012c.GetYaxis()->GetBinCenter(j) * 1000.;

      res2012c.SetBinContent(i, j, ect12c.resolution(ene, eta, eta, PATCore::ParticleType::Electron, false, false));
      res2012c.SetBinError(i, j, 0.);

      res2012a.SetBinContent(i, j, ect12a.resolution(ene, eta, eta, PATCore::ParticleType::Electron, false, false));
      res2012a.SetBinError(i, j, 0.);

    }


  TFile fOut("resoParam.root","recreate");
  res2012a.Write();
  res2012c.Write();
  fOut.Close();

  return 0;

}
