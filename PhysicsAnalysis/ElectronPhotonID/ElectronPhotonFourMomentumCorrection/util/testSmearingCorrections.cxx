/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TCanvas.h"
#include "TStyle.h"
#include "TProfile.h"

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect;
  ect.setESModel(egEnergyCorr::es2012c);  
  ect.initialize();

  int runnumber = 210000;
  int nevt = 1000;

  TFile fp("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root");
  TProfile* p0 = (TProfile*) fp.Get("ElectronProfiles/p0Data");
  TProfile* p1 = (TProfile*) fp.Get("ElectronProfiles/p1Data");
  TProfile* p2 = (TProfile*) fp.Get("ElectronProfiles/p2Data");
  TProfile* p3 = (TProfile*) fp.Get("ElectronProfiles/p3Data");

  TProfile hResolNom("hResolNom","hResolNom",480,-2.4,2.4,"RMS");

  TProfile hResolZeeUp("hResolZeeUp","hResolZeeUp",480,-2.4,2.4,"RMS");
  TProfile hResolZeeDown("hResolZeeDown","hResolZeeDown",480,-2.4,2.4,"RMS");

  TProfile hResolSmpUp("hResolSmpUp","hResolSmpUp",480,-2.4,2.4,"RMS");
  TProfile hResolSmpDown("hResolSmpDown","hResolSmpDown",480,-2.4,2.4,"RMS");

  TProfile hResolMatUp("hResolMatUp","hResolMatUp ID",480,-2.4,2.4,"RMS");
  TProfile hResolMatDown("hResolMatDown","hResolMatDown ID",480,-2.4,2.4,"RMS");

  TProfile hResolPUUp("hResolPUUp","hResolPUUp",480,-2.4,2.4,"RMS");
  TProfile hResolPUDown("hResolPUDown","hResolPUDown",480,-2.4,2.4,"RMS");

  TProfile hResolErrorUp("hResolErrorUp","hResolErrorUp",480,-2.4,2.4,"RMS");
  TProfile hResolErrorDown("hResolErrorDown","hResolErrorDown",480,-2.4,2.4,"RMS");


  for(int i=1; i<=hResolNom.GetNbinsX(); i++)
    for(int j=0; j<nevt; j++) {
      
      double eta = hResolNom.GetBinCenter(i);
      double phi = 1.;
      
      double e0r = p0->GetBinContent( p0->FindBin(eta) ) * 1E3;
      double e1r = p1->GetBinContent( p1->FindBin(eta) ) * 1E3;
      double e2r = p2->GetBinContent( p2->FindBin(eta) ) * 1E3;
      double e3r = p3->GetBinContent( p3->FindBin(eta) ) * 1E3;

      double eIn = 40000.*cosh(eta);
      double eCorr = 0.;
      double eMVA = 0.;

      AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation pi(e0r, e1r, e2r, e3r, eta, phi, eta, eIn, eta, phi);

      // es2012a

      eMVA = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);


      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::ZSmearingUp);
      hResolZeeUp.Fill(eta, eCorr/eMVA - 1.);
      
      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::ZSmearingDown);
      hResolZeeDown.Fill(eta, eCorr/eMVA - 1.);
      

      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::SamplingTermUp);
      hResolSmpUp.Fill(eta, eCorr/eMVA - 1.);
      
      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::SamplingTermDown);
      hResolSmpDown.Fill(eta, eCorr/eMVA - 1.);
      

      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::MaterialIDUp);
      hResolMatUp.Fill(eta, eCorr/eMVA - 1.);
      
      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::MaterialIDDown);
      hResolMatDown.Fill(eta, eCorr/eMVA - 1.);
      

      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::PileUpUp);
      hResolPUUp.Fill(eta, eCorr/eMVA - 1.);
      
      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::PileUpDown);
      hResolPUDown.Fill(eta, eCorr/eMVA - 1.);
      

      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::AllUp);
      hResolErrorUp.Fill(eta, eCorr/eMVA - 1.);
      
      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::AllDown);
      hResolErrorDown.Fill(eta, eCorr/eMVA - 1.);
      

      eCorr = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, pi,
				    egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::Nominal);
      hResolNom.Fill(eta, eCorr/eMVA - 1.);
      
    }
  
  TFile f("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionData.root");

  TCanvas c;
  gStyle->SetOptStat(0);
  
  hResolNom.SetTitle("");
  hResolNom.SetMinimum(-.04); hResolNom.SetMaximum(.04); 
  hResolNom.Draw();
  TH1D* href = (TH1D*)f.Get("Resolution/es2012c/ctZee_errStat");
  href->SetLineColor(2); href->SetLineWidth(2);
  href->Draw("same");
  c.SaveAs("electronResol_Nominal.png");

  hResolZeeUp.SetTitle("");
  hResolZeeUp.SetMinimum(-.04); hResolZeeUp.SetMaximum(.04); 
  hResolZeeUp.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_ZeeUp.png");

  hResolZeeDown.SetTitle("");
  hResolZeeDown.SetMinimum(-.04); hResolZeeDown.SetMaximum(.04); 
  hResolZeeDown.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_ZeeDown.png");

  hResolSmpUp.SetTitle("");
  hResolSmpUp.SetMinimum(-.04); hResolSmpUp.SetMaximum(.04); 
  hResolSmpUp.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_SmpUp.png");

  hResolSmpDown.SetTitle("");
  hResolSmpDown.SetMinimum(-.04); hResolSmpDown.SetMaximum(.04); 
  hResolSmpDown.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_SmpDown.png");

  hResolMatUp.SetTitle("");
  hResolMatUp.SetMinimum(-.04); hResolMatUp.SetMaximum(.04); 
  hResolMatUp.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_MatUp.png");

  hResolMatDown.SetTitle("");
  hResolMatDown.SetMinimum(-.04); hResolMatDown.SetMaximum(.04); 
  hResolMatDown.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_MatDown.png");

  hResolPUUp.SetTitle("");
  hResolPUUp.SetMinimum(-.04); hResolPUUp.SetMaximum(.04); 
  hResolPUUp.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_PUUp.png");

  hResolPUDown.SetTitle("");
  hResolPUDown.SetMinimum(-.04); hResolPUDown.SetMaximum(.04); 
  hResolPUDown.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_PUDown.png");

  hResolErrorUp.SetTitle("");
  hResolErrorUp.SetMinimum(-.04); hResolErrorUp.SetMaximum(.04); 
  hResolErrorUp.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_ErrorUp.png");

  hResolErrorDown.SetTitle("");
  hResolErrorDown.SetMinimum(-.04); hResolErrorDown.SetMaximum(.04); 
  hResolErrorDown.Draw();
  href->Draw("same");
  c.SaveAs("electronResol_ErrorDown.png");

}
