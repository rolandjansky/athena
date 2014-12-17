/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>
#include "egammaLayerRecalibTool/egammaLayerRecalibTool.h"

#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"

using namespace std;

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect1;
  ect1.setESModel(egEnergyCorr::es2012c);  
  ect1.useLayer2Recalibration(false);
  ect1.initialize();

  AtlasRoot::egammaEnergyCorrectionTool ect2;
  ect2.setESModel(egEnergyCorr::es2012c);  
  ect2.initialize();

  TFile f("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) f.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) f.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) f.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) f.Get("ElectronProfiles/p3Data");

  TH1D hPS11("hPS11","PS Correction",480,-2.4,2.4);
  TH1D hPS12("hPS12","PS Correction",480,-2.4,2.4);

  TH1D hS111("hS111","S1 Correction",480,-2.4,2.4);
  TH1D hS112("hS112","S1 Correction",480,-2.4,2.4);

  TH1D hS211("hS211","S2 Correction",480,-2.4,2.4);
  TH1D hS212("hS212","S2 Correction",480,-2.4,2.4);

  TH1D hEr11("hEr11","Energy Correction",480,-2.4,2.4);
  TH1D hEr12("hEr12","Energy Correction",480,-2.4,2.4);

  for(int i=1; i<=hPS11.GetNbinsX(); i++) {

    double eta = hPS11.GetBinCenter(i);
    double phi = 1.;

    double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3;
    double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3;
    double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3;
    double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3;

    double eOld = 40000.*cosh(eta);

    AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eOld, eta, phi);

    // with layer corrections

    double eMVAData = ect2.getCorrectedEnergy(210000, PATCore::ParticleDataType::Data, pi,
					       egEnergyCorr::Scale::None, egEnergyCorr::Resolution::None);

    double e0new = ect2.getCalibInputs()->E0raw;
    if( e0new>0 )
      hPS12.SetBinContent(i,e0new/e0r);
    else
      hPS12.SetBinContent(i,0.);
    
    double e1new = ect2.getCalibInputs()->E1raw;
    if( e1new>0 )
      hS112.SetBinContent(i,e1new/e1r);
    else
      hS112.SetBinContent(i,0.);
    
    double e2new = ect2.getCalibInputs()->E2raw;
    if( e2new>0 )
      hS212.SetBinContent(i,e2new/e2r);
    else
      hS212.SetBinContent(i,0.);
    
    double eMVAMC = ect2.getCorrectedEnergy(210000, PATCore::ParticleDataType::Full, pi,
					     egEnergyCorr::Scale::None, egEnergyCorr::Resolution::None);

    hEr12.SetBinContent(i,eMVAData/eMVAMC);

    // no layer corrections

    eMVAData = ect1.getCorrectedEnergy(210000, PATCore::ParticleDataType::Data, pi,
					egEnergyCorr::Scale::None, egEnergyCorr::Resolution::None);
    
    e0new = ect1.getCalibInputs()->E0raw;
    if( e0new>0 )
      hPS11.SetBinContent(i,e0new/e0r);
    else
      hPS11.SetBinContent(i,0.);
    
    e1new = ect1.getCalibInputs()->E1raw;
    if( e1new>0 )
      hS111.SetBinContent(i,e1new/e1r);
    else
      hS111.SetBinContent(i,0.);

    e2new = ect1.getCalibInputs()->E2raw;
    if( e2new>0 )
      hS211.SetBinContent(i,e2new/e2r);
    else
      hS211.SetBinContent(i,0.);
    
    eMVAMC = ect1.getCorrectedEnergy(210000, PATCore::ParticleDataType::Full, pi,
				      egEnergyCorr::Scale::None, egEnergyCorr::Resolution::None);
    
    hEr11.SetBinContent(i,eMVAData/eMVAMC);
        
  }
  
  TCanvas c;
  gStyle->SetOptStat(0);

  hPS11.SetMinimum(.8); hPS11.SetMaximum(1.1); hPS11.SetLineColor(2);
  hPS11.Draw();
  hPS12.Draw("same");
  c.SaveAs("e0scaling_es2012c.png");

  hS111.SetMinimum(.8); hS111.SetMaximum(1.1); hS111.SetLineColor(2);
  hS111.Draw();
  hS112.Draw("same");
  c.SaveAs("e1scaling_es2012c.png");

  hS211.SetMinimum(.8); hS211.SetMaximum(1.1); hS211.SetLineColor(2);
  hS211.Draw();
  hS212.Draw("same");
  c.SaveAs("e2scaling_es2012c.png");

  hEr11.SetMinimum(.9); hEr11.SetMaximum(1.1); hEr11.SetLineColor(2);
  hEr11.Draw();
  hEr12.Draw("same");
  c.SaveAs("Eratio_es2012c.png");

}
