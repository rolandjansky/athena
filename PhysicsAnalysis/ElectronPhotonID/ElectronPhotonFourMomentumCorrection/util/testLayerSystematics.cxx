/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TProfile.h"
#include "TCanvas.h"

using namespace std;

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect12;
  ect12.setESModel(egEnergyCorr::es2012c);  
  //  ect12.setDebug(true);
  ect12.initialize();

  TFile f("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) f.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) f.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) f.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) f.Get("ElectronProfiles/p3Data");

  int runnumber = 210000;

  TH1D h12PSUp("h12PSUp","h12PSUp",480,-2.4,2.4);
  TH1D h12PSDown("h12PSDown","h12PSDown",480,-2.4,2.4);
  TH1D h12PSNone("h12PSNone","h12PSNone",480,-2.4,2.4);

  TH1D h12S1Up("h12S1Up","h12S1Up",480,-2.4,2.4);
  TH1D h12S1Down("h12S1Down","h12S1Down",480,-2.4,2.4);
  TH1D h12S1None("h12S1None","h12S1None",480,-2.4,2.4);

  for(int i=1; i<=h12PSUp.GetNbinsX(); i++) {

    double eta = h12PSUp.GetBinCenter(i);
    double phi = 1.;

    double scale = 1./5.;

    double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3 * scale;
    double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3 * scale;
    double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3 * scale;
    double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3 * scale;

    double eIn = 40000.*cosh(eta) * scale;
    double eSystPS = 0.;
    double eSystS1 = 0.;
    double eMVANom = 0.;

    AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eIn, eta, phi);
    
    eSystPS = ect12.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				       egEnergyCorr::Scale::PSUp, egEnergyCorr::Resolution::None);
    
    eSystS1 = ect12.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				       egEnergyCorr::Scale::S12Up, egEnergyCorr::Resolution::None);
    
    eMVANom = ect12.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				       egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
    
    h12PSUp.SetBinContent(i, eSystPS/eMVANom - 1.);
    h12PSDown.SetBinContent(i, -eSystPS/eMVANom + 1.);

    h12S1Up.SetBinContent(i, eSystS1/eMVANom - 1.);
    h12S1Down.SetBinContent(i, -eSystS1/eMVANom + 1.);

  }

  TCanvas c;

  h12PSUp.SetLineColor(2); h12PSUp.SetLineStyle(2); h12PSUp.SetMinimum(-.05); h12PSUp.SetMaximum(.05); 
  h12PSUp.Draw();
  h12PSDown.SetLineColor(3); h12PSDown.Draw("same");
  c.SaveAs("systPS_es2012c.png");

  h12S1Up.SetLineColor(2); h12S1Up.SetLineStyle(2); h12S1Up.SetMinimum(-.05); h12S1Up.SetMaximum(.05); 
  h12S1Up.Draw();
  h12S1Down.SetLineColor(3); h12S1Down.Draw("same");
  c.SaveAs("systS1_es2012c.png");

}
