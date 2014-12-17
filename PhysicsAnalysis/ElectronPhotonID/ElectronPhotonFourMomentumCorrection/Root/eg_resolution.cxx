/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/eg_resolution.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

eg_resolution::eg_resolution()
{
  std::cout << " Initialize eg_resolution " << std::endl;

#ifdef ROOTCORE
  file0 = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/resolutionFit_electron.root");
  file1 = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/resolutionFit_recoUnconv.root");
  file2 = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/resolutionFit_recoConv.root");
  file3 = TFile::Open("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/resolutionFit_trueUnconv.root");
#else
  file0 = TFile::Open( PathResolver::find_file("ElectronPhotonFourMomentumCorrection/resolutionFit_electron.root", "XMLPATH").c_str() );
  file1 = TFile::Open( PathResolver::find_file("ElectronPhotonFourMomentumCorrection/resolutionFit_recoUnconv.root", "XMLPATH").c_str() );
  file2 = TFile::Open( PathResolver::find_file("ElectronPhotonFourMomentumCorrection/resolutionFit_recoConv.root", "XMLPATH").c_str() );
  file3 = TFile::Open( PathResolver::find_file("ElectronPhotonFourMomentumCorrection/resolutionFit_trueUnconv.root", "XMLPATH").c_str() );
#endif

  hSampling[0][0] = (TH1D*) file0->Get("hsamplingG");
  hSampling[0][1] = (TH1D*) file0->Get("hsampling80");
  hSampling[0][2] = (TH1D*) file0->Get("hsampling90");
  hSampling[1][0] = (TH1D*) file1->Get("hsamplingG");
  hSampling[1][1] = (TH1D*) file1->Get("hsampling80");
  hSampling[1][2] = (TH1D*) file1->Get("hsampling90");
  hSampling[2][0] = (TH1D*) file2->Get("hsamplingG");
  hSampling[2][1] = (TH1D*) file2->Get("hsampling80");
  hSampling[2][2] = (TH1D*) file2->Get("hsampling90");
  hSampling[3][0] = (TH1D*) file3->Get("hsamplingG");
  hSampling[3][1] = (TH1D*) file3->Get("hsampling80");
  hSampling[3][2] = (TH1D*) file3->Get("hsampling90");

  hNoise[0][0] = (TH1D*) file0->Get("hnoiseG");
  hNoise[0][1] = (TH1D*) file0->Get("hnoise80");
  hNoise[0][2] = (TH1D*) file0->Get("hnoise90");
  hNoise[1][0] = (TH1D*) file1->Get("hnoiseG");
  hNoise[1][1] = (TH1D*) file1->Get("hnoise80");
  hNoise[1][2] = (TH1D*) file1->Get("hnoise90");
  hNoise[2][0] = (TH1D*) file2->Get("hnoiseG");
  hNoise[2][1] = (TH1D*) file2->Get("hnoise80");
  hNoise[2][2] = (TH1D*) file2->Get("hnoise90");
  hNoise[3][0] = (TH1D*) file3->Get("hnoiseG");
  hNoise[3][1] = (TH1D*) file3->Get("hnoise80");
  hNoise[3][2] = (TH1D*) file3->Get("hnoise90");

  hConst[0][0] = (TH1D*) file0->Get("hconstG");
  hConst[0][1] = (TH1D*) file0->Get("hconst80");
  hConst[0][2] = (TH1D*) file0->Get("hconst90");
  hConst[1][0] = (TH1D*) file1->Get("hconstG");
  hConst[1][1] = (TH1D*) file1->Get("hconst80");
  hConst[1][2] = (TH1D*) file1->Get("hconst90");
  hConst[2][0] = (TH1D*) file2->Get("hconstG");
  hConst[2][1] = (TH1D*) file2->Get("hconst80");
  hConst[2][2] = (TH1D*) file2->Get("hconst90");
  hConst[3][0] = (TH1D*) file3->Get("hconstG");
  hConst[3][1] = (TH1D*) file3->Get("hconst80");
  hConst[3][2] = (TH1D*) file3->Get("hconst90");

  TAxis* aa=hSampling[0][0]->GetXaxis();
  etaBins = aa->GetXbins();

}

//=========================================================================
eg_resolution::~eg_resolution()
{
  file0->Close();
  file1->Close();
  file2->Close();
  file3->Close();
}

//============================================================================
// inputs are particle_type (0=elec, 1=reco unconv photon, 2=reco conv photon, 3=true unconv photon)
//            energy in MeV
//            eta
//            resolution_type (0=gaussian core fit, 1=sigmaeff 80%, 2=sigma eff 90%)
//
// returned value is sigmaE/E
//
double eg_resolution::getResolution(int particle_type, double energy, double eta, int resolution_type) const
{

   if (particle_type<0 || particle_type>3) return -1;
   if (resolution_type<0 || resolution_type>2) return -1;

   float aeta=fabs(eta);
   int ibinEta=etaBins->GetSize()-2;
   for (int i=1;i<etaBins->GetSize();i++) {
      if (aeta<etaBins->GetAt(i)) {
         ibinEta=i-1;
         break;
      }
   }

   if (ibinEta<0 || ibinEta>= etaBins->GetSize()) return -1;

   double energyGeV = energy*0.001;

   double rsampling = hSampling[particle_type][resolution_type]->GetBinContent(ibinEta+1);
   double rnoise    = hNoise[particle_type][resolution_type]->GetBinContent(ibinEta+1);
   double rconst    = hConst[particle_type][resolution_type]->GetBinContent(ibinEta+1);

   float sigma = rsampling*rsampling/energyGeV + rnoise*rnoise/energyGeV/energyGeV + rconst*rconst;
   sigma = sqrt(sigma);


   return sigma;

}
