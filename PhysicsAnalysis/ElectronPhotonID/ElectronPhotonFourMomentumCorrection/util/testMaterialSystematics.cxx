/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1D.h"

using namespace std;

int main() {

 AtlasRoot::egammaEnergyCorrectionTool ect;
 ect.setESModel(egEnergyCorr::es2012c);  
 ect.initialize();
 ect.setDebug(true);

 int runnumber = 210000;

 TH1D hCnvNom("hCnvNom","hCnvNom",500,-2.5,2.5);
 TH1D hUncNom("hUncNom","hUncNom",500,-2.5,2.5);

 TH1D hCnvCaloUp("hCnvCaloUp","hCnvCaloUp",500,-2.5,2.5);
 TH1D hCnvCaloDown("hCnvCaloDown","hCnvCaloDown",500,-2.5,2.5);

 TH1D hUncCaloUp("hUncCaloUp","hUncCaloUp",500,-2.5,2.5);
 TH1D hUncCaloDown("hUncCaloDown","hUncCaloDown",500,-2.5,2.5);
 TH1D hUncCaloDiff("hUncCaloDiff","hUncCaloDiff",500,-2.5,2.5);

 TH1D hUncCryoUp("hUncCryoUp","hUncCryoUp",500,-2.5,2.5);
 TH1D hUncCryoDown("hUncCryoDown","hUncCryoDown",500,-2.5,2.5);
 TH1D hUncCryoDiff("hUncCryoDiff","hUncCryoDiff",500,-2.5,2.5);


 for(int i=1; i<=hCnvNom.GetNbinsX(); i++) {

   double eta = hCnvNom.GetBinCenter(i);
   double phi = 1.;

   if( fabs(eta)>1.37 && fabs(eta)<1.52 )
     continue;

   // Average values

   // ... energy

   double e0r = 1617*cosh(eta);
   double e1r = 11030*cosh(eta);
   double e2r = 38460*cosh(eta);
   double e3r = 408*cosh(eta);

   // ... average conversion variables

   double ptconv = 93580.;
   double pt1conv = 55200.;
   double pt2conv = 38400.;
   double Rconv = 268.;
   int n1PixHits = 1;
   int n2PixHits = 1;
   int n1SCTHits = 6;
   int n2SCTHits = 5;

   double eIn = 55360.*cosh(eta);
   double eNom, eCaloUp, eCaloDown, eCryoUp, eCryoDown;
   eNom = eCaloUp = eCaloDown = eCryoUp = eCryoDown = 0.;

   AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation piUnc(e0r, e1r, e2r, e3r, eta, phi, eIn, eta, phi,
								    0., 0., 0, 0, 0, 0, 0, -1.);

   AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation piCnv(e0r, e1r, e2r, e3r, eta, phi, eIn, eta, phi,
								    ptconv, pt1conv, pt2conv, n1PixHits, n1SCTHits, n2PixHits, n2SCTHits, Rconv);

   
   cout << endl << " Unconverted : Nominal " << endl << endl;

   eNom = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piUnc,
				 egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None);
   hUncNom.SetBinContent(i, eIn/eNom - 1.);

   cout << endl << " Unconverted : Calo variation " << endl << endl;
  
   eCaloUp = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piUnc,
				 egEnergyCorr::Scale::MatCaloUp, egEnergyCorr::Resolution::None);
   hUncCaloUp.SetBinContent(i, eIn/eCaloUp - 1.);
   
   eCaloDown = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piUnc,
				 egEnergyCorr::Scale::MatCaloDown, egEnergyCorr::Resolution::None);
   hUncCaloDown.SetBinContent(i, eIn/eCaloDown - 1.);

   hUncCaloDiff.SetBinContent(i, eCaloUp/eNom - 1.);
   
   cout << endl << " Unconverted : Cryo variation " << endl << endl;
  
   eCryoUp = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piUnc,
				 egEnergyCorr::Scale::MatCryoUp, egEnergyCorr::Resolution::None);
   hUncCryoUp.SetBinContent(i, eIn/eCryoUp - 1.);
   
   eCryoDown = ect.getCorrectedEnergy(runnumber, PATCore::ParticleDataType::Full, piUnc,
				 egEnergyCorr::Scale::MatCryoDown, egEnergyCorr::Resolution::None);
   hUncCryoDown.SetBinContent(i, eIn/eCryoDown - 1.);

   hUncCryoDiff.SetBinContent(i, eCryoUp/eNom - 1.);
   



 }

 TCanvas c;
 gStyle->SetOptStat(0);

 hUncCaloUp.SetLineColor(2); hUncCaloUp.SetMinimum(-.05); hUncCaloUp.SetMaximum(.05); 
 hUncCaloUp.SetTitle(""); hUncCaloUp.Draw();
 hUncCaloDown.SetLineColor(2); hUncCaloDown.SetLineStyle(2);
 hUncCaloDown.Draw("same");
 hUncNom.SetLineColor(1); hUncNom.Draw("same");
 c.SaveAs("alpha_Unc_MatCalo.png");

 hUncCaloDiff.Draw();
 c.SaveAs("diff_Unc_MatCalo.png");

 hUncCryoUp.SetLineColor(2); hUncCryoUp.SetMinimum(-.05); hUncCryoUp.SetMaximum(.05); 
 hUncCryoUp.SetTitle(""); hUncCryoUp.Draw();
 hUncCryoDown.SetLineColor(2); hUncCryoDown.SetLineStyle(2);
 hUncCryoDown.Draw("same");
 hUncNom.SetLineColor(1); hUncNom.Draw("same");
 c.SaveAs("alpha_Unc_MatCryo.png");

 hUncCryoDiff.Draw();
 c.SaveAs("diff_Unc_MatCryo.png");

 
  // // ID+Cryostat material

  // TH1D hConv2Nom("hConv2Nom","hConv2Nom",480,-2.4,2.4);
  // TH1D hConv2Up("hConv2Up","hConv2Up",480,-2.4,2.4);
  // TH1D hConv2Down("hConv2Down","hConv2Down",480,-2.4,2.4);

  // TH1D hUnc2Nom("hUnc2Nom","hUnc2Nom",480,-2.4,2.4);
  // TH1D hUnc2Up("hUnc2Up","hUnc2Up",480,-2.4,2.4);
  // TH1D hUnc2Down("hUnc2Down","hUnc2Down",480,-2.4,2.4);

  // for(int i=1; i<=hConv2Nom.GetNbinsX(); i++) {
    
  //   double eta = hConv2Nom.GetBinCenter(i);


  //   double eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::UnconvertedPhoton,
  // 					  eta, eta, eIn, egEnergyCorr::Scale::MatCryoUp, egEnergyCorr::Resolution::None );
  //   hUnc2Up.SetBinContent(i, eIn/eOut - 1.);

  //   eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::UnconvertedPhoton,
  // 				   eta, eta, eIn, egEnergyCorr::Scale::MatCryoDown, egEnergyCorr::Resolution::None );
  //   hUnc2Down.SetBinContent(i, eIn/eOut - 1.);

  //   eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::UnconvertedPhoton,
  // 				   eta, eta, eIn, egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None );
  //   hUnc2Nom.SetBinContent(i, eIn/eOut - 1.);


  //   eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::ConvertedPhoton,
  // 				   eta, eta, eIn, egEnergyCorr::Scale::MatCryoUp, egEnergyCorr::Resolution::None );
  //   hConv2Up.SetBinContent(i, eIn/eOut - 1.);

  //   eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::ConvertedPhoton,
  // 				   eta, eta, eIn, egEnergyCorr::Scale::MatCryoDown, egEnergyCorr::Resolution::None );
  //   hConv2Down.SetBinContent(i, eIn/eOut - 1.);

  //   eOut = ect.getCorrectedEnergy( runnumber, PATCore::ParticleDataType::Full, PATCore::ParticleType::ConvertedPhoton,
  // 				   eta, eta, eIn, egEnergyCorr::Scale::Nominal, egEnergyCorr::Resolution::None );
  //   hConv2Nom.SetBinContent(i, eIn/eOut - 1.);


  // }

  // hUnc2Up.SetLineColor(2); hUnc2Up.SetMinimum(-.01); hUnc2Up.SetMaximum(.01); 
  // hUnc2Up.SetTitle(""); hUnc2Up.Draw();
  // hUnc2Down.SetLineColor(2); hUnc2Down.SetLineStyle(2);
  // hUnc2Down.Draw("same");
  // hUnc2Nom.SetLineColor(1); hUnc2Nom.Draw("same");
  // hConv2Up.SetLineColor(4);
  // hConv2Up.Draw("same");
  // hConv2Down.SetLineColor(4); hConv2Down.SetLineStyle(2);
  // hConv2Down.Draw("same");
  // hConv2Nom.SetLineColor(1); hConv2Nom.Draw("same");
  // c.SaveAs("deltaAlpha_MatCryo.png");

 

}
