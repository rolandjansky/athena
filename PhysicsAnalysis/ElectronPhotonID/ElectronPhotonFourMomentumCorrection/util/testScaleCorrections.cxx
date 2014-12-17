/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TProfile.h"
#include "TCanvas.h"

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect11d;
  ect11d.setESModel(egEnergyCorr::es2011d);  
  ect11d.initialize();
  
  AtlasRoot::egammaEnergyCorrectionTool ect12c;
  ect12c.setESModel(egEnergyCorr::es2012c);  
  ect12c.initialize();
  
  TFile fp("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) fp.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) fp.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) fp.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) fp.Get("ElectronProfiles/p3Data");

  int runnumber = 210000;

  TH1D h2011d("h2011d","h2011d",500,-5,5);
  TH1D h2012c("h2012c","h2012c",500,-5,5);
  TH1D m2012c("m2012c","m2012c",500,-5,5);

  for(int i=1; i<=h2011d.GetNbinsX(); i++) {
    
    double eta = h2011d.GetBinCenter(i);
    double phi = 1.;
    
    double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3;
    double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3;
    double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3;
    double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3;

    double eIn = 40000.*cosh(eta);
    double eCorr = 0.;
    double eMVA = 0.;

    AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eIn, eta, phi);
    
    AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pifwd(0., 21.757415, 53.817732, 45.869443, 
								     -.5, 1., -.5, 50000., -.5, 1.);

    std::cout << ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Data, pifwd, 
					   egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::Nominal) << std::endl;

    std::cout << ect12c.getCorrectedEnergyPhoton(runnumber, PATCore::ParticleDataType::Data, 0., 2117.757415,
						 5340.817732, 45.869443, -3.5, 0., 2000., -3.5, 0., 1750., 1000., 900.,
						 1, 1, 1, 1, 100., egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None) << std::endl;

    // es2011d, Nominal
    
    eCorr = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Data, pi, 
				      egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::Nominal);
    eMVA = ect11d.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Data, pi, 
    				     egEnergyCorr::Scale::None, egEnergyCorr::Resolution::Nominal);
    h2011d.SetBinContent(i, eMVA/eCorr - 1.);

    // es2012c, Nominal

    eCorr = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Data, pi, 
				     egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::Nominal);
    eMVA = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Data, pi, 
				     egEnergyCorr::Scale::None, egEnergyCorr::Resolution::Nominal);
    h2012c.SetBinContent(i, eMVA/eCorr - 1.);

    // es2012c, Syst (MC)

    eCorr = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				     egEnergyCorr::Scale::ZeeSystUp, egEnergyCorr::Resolution::None);
    eMVA = ect12c.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi, 
				     egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
    m2012c.SetBinContent(i, eMVA/eCorr - 1.);

  }

  TFile f("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionData.root");

  TCanvas c;
  h2011d.SetLineColor(2); h2011d.SetLineStyle(2); h2011d.SetMinimum(-.05); h2011d.SetMaximum(.05); 
  h2011d.Draw();
  ((TH1D*)f.Get("Scales/es2011d/alphaZee_errStat"))->Draw("same");
  c.SaveAs("alpha_es2011d.png");

  h2012c.SetLineColor(2); h2012c.SetLineStyle(2); h2012c.SetMinimum(-.05); h2012c.SetMaximum(.05); 
  h2012c.Draw();
  ((TH1D*)f.Get("Scales/es2012c/alphaZee_errStat"))->Draw("same");
  c.SaveAs("alpha_es2012c.png");
 
  m2012c.SetLineColor(2); m2012c.SetLineStyle(2); m2012c.SetMinimum(-.005); m2012c.SetMaximum(.005); 
  m2012c.Draw();
  ((TH1D*)f.Get("Scales/es2012c/alphaZee_errSyst"))->Draw("same");
  c.SaveAs("alpha_es2012c_syst.png");
 
 return 0;
}
