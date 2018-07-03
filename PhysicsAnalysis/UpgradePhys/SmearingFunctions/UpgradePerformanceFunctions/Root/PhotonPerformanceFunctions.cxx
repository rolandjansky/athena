/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONPERFORMANCEFUNCTIONS_CXX
#define PHOTONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"
#include <sstream>

namespace Upgrade {

StatusCode UpgradePerformanceFunctions::initPhotonFakeHistograms() {
  //    TString filename = "/afs/cern.ch/user/r/rpolifka/HGTDHInvAnalysis/UpgradePerformanceFunctions/share/PhotonFakes.root";
  ATH_MSG_INFO("Loading Photon Fake Rate histogram file");
  std::string FakeFile = PathResolverFindCalibFile(m_photonFakeFilename);
  ATH_MSG_INFO("Found Photon Fake Rate histogram file: " << FakeFile);

  TFile *m_infile = new TFile(FakeFile.c_str(), "READ");

  TString hsname = "PhotHSJetIsTightBkg2D";
  TString puname = "PhotPUJetIsTightBkg2D";

  TH2D *hhs = (TH2D*)m_infile->Get(hsname);
  TH2D *hpu = (TH2D*)m_infile->Get(puname);

  //HS

  for (int i = 3; i <= 50; i++) {

    std::ostringstream hsstream;
    hsstream <<  "HSFake" << i;
    TString hnamehs = hsstream.str();
    TH1D *htemp = (TH1D*)hhs->ProjectionY(hnamehs, i, i);
    TH1D *htemp2 = (TH1D*)htemp->Clone();


    //      std::cout << " HS 2D bin " << i << " bin min: " << hhs->GetXaxis()->GetBinLowEdge(i) << " bin max: " <<  hhs->GetXaxis()->GetBinLowEdge(i)+ hhs->GetXaxis()->GetBinWidth(i) << " integral: " << htemp2->Integral() << std::endl;

    hsfakes.push_back(htemp2);
  }

  //    int nhhs = hsfakes.size();

  ATH_MSG_DEBUG(" HS Fake Initialisation done");

  // for (int i = 0; i < nhhs;i++){
  //     TH1D *htemp = (TH1D*)hsfakes[i];
  //  //        std::cout << " HS histogram " << i << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << std::endl;
  // }

  //PU


  for (int i = 5; i <= 30; i++) {

    std::ostringstream pustream;
    pustream <<  "PUFake" << i;
    TString hnamepu = pustream.str();
    TH1D *htemp = (TH1D*)hpu->ProjectionY(hnamepu, i, i);

    pufakes.push_back(htemp);
  }

  //    int nhpu = pufakes.size();

  ATH_MSG_DEBUG(" PU Fake Initialisation done");

  // for (int i = 0; i < nhpu;i++){
  //     TH1D *htemp = (TH1D*)pufakes[i];
  //  //        std::cout << " histogram " << i << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << std::endl;
  // }

  return StatusCode::SUCCESS;

}

float UpgradePerformanceFunctions::getPhotonEfficiency(float ptMeV) {
  if (ptMeV < 20.*1000.) {
    // Hard cut at pT=20 GeV
    return 0.0;
  } else {
    if (m_photonCutLevel == loosePhoton) {
      ATH_MSG_WARNING("getPhotonEfficiency: No efficiency for loosePhotons");
      return 0.0;
    } else if (m_photonCutLevel == tightPhoton) {
      //    return 0.76 - 1.98*exp(-1.0*ptMeV/16100.); old param
      return 0.862882 - 1.10804 * exp(-1.0 * ptMeV / 33561.5);
    } else {
      ATH_MSG_ERROR("getPhotonEfficiency: Invalid working point.");
      return 0.0;
    }
  }
}

float UpgradePerformanceFunctions::getPhotonFakeRate(float ptMeV) {

  float fakerate = 0.;

  // based on Crystal Ball function
  double m = ptMeV / 1000.;
  double alpha = 0.218954;
  double n = 0.708690;
  double m0 = 60.;
  double sigma = 16.2482;
  double N = 0.000538849;

  Double_t t = (m - m0) / sigma;
  if (alpha > 0) t = -t;

  Double_t absAlpha = fabs((Double_t)alpha);

  if (t >= -absAlpha) {
    fakerate = N * exp(-0.5 * t * t);
  }
  else {
    Double_t a = TMath::Power(n / absAlpha, n) * exp(-0.5 * absAlpha * absAlpha);
    Double_t b = n / absAlpha - absAlpha;

    fakerate = N * a / TMath::Power(b - t, n);
  }

  if (ptMeV < 20.*1000.) {
    // Hard cut at 20 GeV
    return 0.0;
  } else {
    if (m_photonCutLevel == loosePhoton) {
      return 0.99;
    } else if (m_photonCutLevel == tightPhoton) {
      //    return 9.3E-3*exp(-1.0*ptMeV/27500.); old parametrisation
      return fakerate;
    } else {
      ATH_MSG_ERROR("getPhotonFakeRate: Invalid working point.");
      return 0.0;
    }
  }
}

float UpgradePerformanceFunctions::getElectronToPhotonFakeRate(float ptMeV, float eta) {
  if (ptMeV < 20.*1000.) {
    // Hard cut-off at 20 GeV
    return 0.0;
  } else {
    // Private communication with Monika Wielers
    if (fabs(eta) < 1.37) return 0.02;
    else if (fabs(eta) > 1.52) return 0.05;
    else return 0.0;
  }
}

float UpgradePerformanceFunctions::getPhotonPileupFakeRate(float ptMeV) {

  double m = ptMeV / 1000.;
  float fakerate = 0.00190945 * TMath::Exp(-61.4829 * TMath::Exp(- 0.0758085 * m));

  if (ptMeV < 20.*1000.) {
    return 0.0;
  } else {
    if (m_photonCutLevel == loosePhoton) {
      return 0.99;
    } else if (m_photonCutLevel == tightPhoton) {
      //    return 9.3E-3*exp(-1.0*ptMeV/27500.); old parametrisation
      return fakerate;
    } else {
      ATH_MSG_ERROR("getPhotonFakeRate: Invalid working point.");
      return 0.0;
    }
  }
}

float UpgradePerformanceFunctions::getPhotonEnergyResolution(float eMeV, float eta) {
  //--- use the same energy resolution as for electrons
  float energyResolution = 0;
  float eGeV = eMeV / 1000.;
  if (m_photonNoiseScaling >= 0 && m_layout == UpgradePerformanceFunctions::Step1p6) {
    // values taken from fit in https://indico.cern.ch/event/657138/contributions/2687064/attachments/1506255/2347611/PhotonResolution_HL-LHC_Aug09.pdf
    int etaBin = 0;
    if ((fabs(eta) > 1.37 && fabs(eta) < 1.52) || fabs(eta) > 2.37) {
      ATH_MSG_WARNING("getPhotonEnergyResoluion: not a valid eta value " << eta);
      return 0;
    }
    if (fabs(eta) > 1.52) etaBin = 1;
    int optBin = 0;
    if (m_photonNoiseScaling < 1) optBin = 1; //use optimistic resolution
    //recommended scaling is 0.375
    //
    const float samplingErrors[2]    = {0.091392, 0.144469 };
    const float pileupNoise[2]       = {0.114, 0.245 };
    const float intrinsicNoise[2]    = {0.424254, 0.910996 };
    const float constantTerms[2][2]  = { {0.010, 0.014}, {0.007, 0.007} };
    float samplingError = samplingErrors[etaBin];
    float noiseError = sqrt(intrinsicNoise[etaBin] * intrinsicNoise[etaBin] + m_photonNoiseScaling * m_avgMu * pileupNoise[etaBin] * pileupNoise[etaBin]);
    float constantTerm = constantTerms[optBin][etaBin];
    float relativeResolution = sqrt(samplingError * samplingError / eGeV + noiseError * noiseError / eGeV / eGeV + constantTerm * constantTerm);
    energyResolution = relativeResolution * eMeV;
  } else {
    energyResolution = getElectronEnergyResolution(eMeV, eta);
  }
  return energyResolution;
}

float UpgradePerformanceFunctions::getPhotonSmearedEnergy(float eMeV, float eta) {
  double deltaE = m_photonRandom.Gaus(0, getPhotonEnergyResolution(eMeV, eta));
  return (eMeV + deltaE);
}

TLorentzVector UpgradePerformanceFunctions::getPhotonSmearedVector(const TLorentzVector* init) {
  double eta = init->Eta();
  double phi = init->Phi();
  double EMeV = init->E();
  double EGeV = init->E() / 1000.; // needed for angular smearing

  const double sigma_theta_cst = 0.080; //conservatively,from Fig 12 page 51  in  http://arxiv.org/abs/0901.0512
  double sigma_eta = cosh(eta) * sigma_theta_cst / sqrt(EGeV);
  double eta_sm = eta + m_photonRandom.Gaus(0., sigma_eta);

  const double sigma_phi_cst = 0.010; //(from Fig 10 page 51 of http://arxiv.org/abs/0901.0512)
  double sigma_phi = sigma_phi_cst / sqrt(EGeV);
  double phi_sm = phi + m_photonRandom.Gaus(0., sigma_phi);

  //  float E_sm = getPhotonSmearedEnergy(EMeV, eta_sm);
  float E_sm = getPhotonSmearedEnergy(EMeV, eta);  //Need to use unsmeared eta since resolution is not defined in the gap or forward region
  double pt_sm = E_sm / cosh(fabs(eta_sm));

  TLorentzVector return_vec;
  return_vec.SetPtEtaPhiE(pt_sm, eta_sm, phi_sm, E_sm);
  return return_vec;
}

float UpgradePerformanceFunctions::getPhotonFakeRescaledET(float eTMeV) {
  //  return (eMeV*(m_photonRandom.Gaus(0.74,0.126)));

  float fakedEt = 0.;

  // minimum is 20 GeV, maximum is 500 GeV

  if (eTMeV < 20000. || eTMeV > 500000.) {
    ATH_MSG_WARNING("UpgradePerformanceFunctions::getPhotonFakeRescaledET(" << eTMeV << "): ET out of allowed range os 20000-500000MeV");
    return 0.;
  } else {
    // GetRandom() uses ROOT gRandom, which uses process-specific seed
    // For reproducibility, we reset the seed using our controlled m_METRandom
    gRandom->SetSeed( int(1. / m_photonRandom.Rndm()) );

    // estimate which histogram should be called
    int ind = TMath::Floor( (eTMeV - 20000) / 10000.);

    TH1D *htemp = (TH1D*)hsfakes.at(ind);
    fakedEt = htemp->GetRandom() * 1000.;

    return fakedEt;
  }

}

float UpgradePerformanceFunctions::getPhotonPileupFakeRescaledET(float eTMeV) {
  //  return (eMeV*(m_photonRandom.Gaus(0.74,0.126)));

  float fakedEt = 0.;

  // minimum is 20 GeV, maximum is 500 GeV

  if (eTMeV < 20000 || eTMeV > 150000.) {
    ATH_MSG_WARNING("UpgradePerformanceFunctions::getPhotonPileupFakeRescaledET(" << eTMeV << "): ET out of allowed range os 20000-150000MeV");
    return 0.;
  } else {
    // GetRandom() uses ROOT gRandom, which uses process-specific seed
    // For reproducibility, we reset the seed using our controlled m_METRandom
    gRandom->SetSeed( int(1. / m_photonRandom.Rndm()) );

    // estimate which histogram should be called
    int ind = TMath::Floor( (eTMeV - 20000) / 5000.);

    TH1D *htemp = (TH1D*)pufakes.at(ind);
    fakedEt = htemp->GetRandom() * 1000.;

    return fakedEt;
  }

}

}

#endif
