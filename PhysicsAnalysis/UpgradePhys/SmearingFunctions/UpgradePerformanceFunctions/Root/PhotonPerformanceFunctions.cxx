/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONPERFORMANCEFUNCTIONS_CXX
#define PHOTONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "TFile.h"
#include <sstream>

#ifdef ROOTCORE
// Framework include(s):
#include "PathResolver/PathResolver.h"
#endif // ROOTCORE

void UpgradePerformanceFunctions::setPhotonWorkingPoint(PhotonCutLevel cutLevel) {
  m_photonCutLevel = cutLevel;
}

void UpgradePerformanceFunctions::initPhotonFakeHistograms(TString filename){    
  //    TString filename = "/afs/cern.ch/user/r/rpolifka/HGTDHInvAnalysis/UpgradePerformanceFunctions/share/PhotonFakes.root";
    std::cout << "Loading Photon Fake Rate histogram file " << filename << std::endl;
    std::string FakeFile = filename.Data();
#ifdef ROOTCORE
    // Get file from data path
    FakeFile = PathResolverFindCalibFile(FakeFile);
    std::cout << "Found Photon Fake Rate histogram file: " << FakeFile << std::endl;
#endif // ROOTCORE
    
    TFile *infile=new TFile(FakeFile.c_str(),"READ");
    
    // hardcode number of allowed bins
    
    // const int nHS = 200;
    // const int nPU = 30;

    TString hsname = "PhotHSJetIsTightBkg2D";
    TString puname = "PhotPUJetIsTightBkg2D";
    
    TH2D *hhs = (TH2D*)infile->Get(hsname);
    TH2D *hpu = (TH2D*)infile->Get(puname);
    
    //HS
    
    for (int i = 3; i <=50; i++){
        
      std::ostringstream hsstream;
      hsstream <<  "HSFake" << i;
      TString hnamehs = hsstream.str();
      TH1D *htemp = (TH1D*)hhs->ProjectionY(hnamehs,i,i);
      TH1D *htemp2 = (TH1D*)htemp->Clone();


      //      std::cout << " HS 2D bin " << i << " bin min: " << hhs->GetXaxis()->GetBinLowEdge(i) << " bin max: " <<  hhs->GetXaxis()->GetBinLowEdge(i)+ hhs->GetXaxis()->GetBinWidth(i) << " integral: " << htemp2->Integral() << std::endl;

      m_hsfakes.push_back(htemp2);
    }
    
    //    int nhhs = m_hsfakes.size();
    
    std::cout << " HS Fake Initialisation done" << std::endl;
    
    // for (int i = 0; i < nhhs;i++){
    //     TH1D *htemp = (TH1D*)m_hsfakes[i];
    // 	//        std::cout << " HS histogram " << i << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << std::endl;
    // }
    
    //PU
    
    
    for (int i = 5; i <=30; i++){
        
      std::ostringstream pustream;
        pustream <<  "PUFake" << i;
        TString hnamepu = pustream.str();
        TH1D *htemp = (TH1D*)hpu->ProjectionY(hnamepu,i,i);
        
        m_pufakes.push_back(htemp);
    }
    
    //    int nhpu = m_pufakes.size();

    std::cout << " PU Fake Initialisation done" << std::endl;
    
    // for (int i = 0; i < nhpu;i++){
    //     TH1D *htemp = (TH1D*)m_pufakes[i];
    // 	//        std::cout << " histogram " << i << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << std::endl;
    // }
    
}

void UpgradePerformanceFunctions::setPhotonRandomSeed(unsigned seed) {
  m_photonRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getPhotonEfficiency(float ptMeV) {
  if (ptMeV < 20.*1000.) {
    // Hard cut at pT=20 GeV
    return 0.0;
  } else {
    if (m_photonCutLevel == loosePhoton) {
      std::cout << "getPhotonEfficiency: No efficiency for loosePhotons" << std::endl;
      return 0.0;
    } else if (m_photonCutLevel == tightPhoton) {
      //    return 0.76 - 1.98*exp(-1.0*ptMeV/16100.); old param
      return 0.862882 - 1.10804*exp(-1.0*ptMeV/33561.5);
    } else {
      std::cout << "getPhotonEfficiency: Invalid working point." << std::endl;
      return 0.0;
    }
  }
}

float UpgradePerformanceFunctions::getPhotonFakeRate(float ptMeV) {
    
    float fakerate = 0.;
    
    // based on Crystal Ball function
    double m = ptMeV/1000.;
    double alpha = 0.218954;
    double n = 0.708690;
    double m0 = 60.;
    double sigma = 16.2482;
    double N = 0.000538849;
    
    Double_t t = (m-m0)/sigma;
    if (alpha > 0) t = -t;
    
    Double_t absAlpha = fabs((Double_t)alpha);
    
    if (t >= -absAlpha) {
        fakerate = N*exp(-0.5*t*t);
    }
    else {
        Double_t a = TMath::Power(n/absAlpha,n)*exp(-0.5*absAlpha*absAlpha);
        Double_t b= n/absAlpha - absAlpha;
        
        fakerate = N*a/TMath::Power(b - t, n);
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
	std::cout << "getPhotonFakeRate: Invalid working point." << std::endl;
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
    if (fabs(eta)<1.37) return 0.02;
    else if (fabs(eta)>1.52) return 0.05;
    else return 0.0;
  }
}

float UpgradePerformanceFunctions::getPhotonPileupFakeRate(float ptMeV) {
    
    double m = ptMeV/1000.;
    float fakerate = 0.00190945*TMath::Exp(-61.4829*TMath::Exp(- 0.0758085*m));

    if (ptMeV < 20.*1000.) {
      return 0.0;
    } else {
      if (m_photonCutLevel == loosePhoton) {
        return 0.99;
      } else if (m_photonCutLevel == tightPhoton) {
        //    return 9.3E-3*exp(-1.0*ptMeV/27500.); old parametrisation
        return fakerate;
      } else {
      std::cout << "getPhotonFakeRate: Invalid working point." << std::endl;
      return 0.0;
      }
    }
}

float UpgradePerformanceFunctions::getPhotonEnergyResolution(float eMeV, float eta) {
  //--- use the same energy resolution as for electrons
  float energyResolution = getElectronEnergyResolution(eMeV, eta);
  return energyResolution;
}

float UpgradePerformanceFunctions::getPhotonSmearedEnergy(float eMeV, float eta) {
  double deltaE = m_photonRandom.Gaus(0, getPhotonEnergyResolution(eMeV, eta));
  return (eMeV+deltaE);
}

TLorentzVector UpgradePerformanceFunctions::getPhotonSmearedVector(const TLorentzVector* init) {
  double eta = init->Eta();
  double phi = init->Phi();
  double EMeV = init->E();
  double EGeV = init->E()/1000.; // needed for angular smearing

  const double sigma_theta_cst=0.080;//conservatively,from Fig 12 page 51  in  http://arxiv.org/abs/0901.0512 
  double sigma_eta = cosh(eta)*sigma_theta_cst/sqrt(EGeV);
  double eta_sm = eta + m_photonRandom.Gaus(0.,sigma_eta);

  const double sigma_phi_cst = 0.010; //(from Fig 10 page 51 of http://arxiv.org/abs/0901.0512)
  double sigma_phi = sigma_phi_cst/sqrt(EGeV);
  double phi_sm = phi + m_photonRandom.Gaus(0.,sigma_phi);

  float E_sm = getPhotonSmearedEnergy(EMeV, eta_sm);
  double pt_sm = E_sm/cosh(fabs(eta_sm));

  TLorentzVector return_vec;
  return_vec.SetPtEtaPhiE(pt_sm, eta_sm, phi_sm, E_sm);
  return return_vec;
}

float UpgradePerformanceFunctions::getPhotonFakeRescaledET(float eTMeV) {
  //  return (eMeV*(m_photonRandom.Gaus(0.74,0.126)));
    
    float fakedEt = 0.;
    
    // minimum is 20 GeV, maximum is 500 GeV
    
    if (eTMeV < 20000. || eTMeV > 500000.){
        std::cout << "UpgradePerformanceFunctions::getPhotonFakeRescaledET("<< eTMeV <<"): ET out of allowed range os 20000-500000MeV" << std::endl;
        return 0.;
    } else{
    
        // estimate which histogram should be called
        
        int ind = TMath::Floor( (eTMeV-20000)/10000.);
    
        TH1D *htemp = (TH1D*)m_hsfakes.at(ind);

        fakedEt = htemp->GetRandom()*1000.;
        
	//	std::cout << " HS histogram " << ind << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << " --> jet pt: " << eTMeV << " faked ET: " << fakedEt << std::endl;
        
        return fakedEt;
    }
    
}

float UpgradePerformanceFunctions::getPhotonPileupFakeRescaledET(float eTMeV) {
    //  return (eMeV*(m_photonRandom.Gaus(0.74,0.126)));
    
    float fakedEt = 0.;
    
    // minimum is 20 GeV, maximum is 500 GeV
    
    if (eTMeV < 20000 || eTMeV > 150000.){
        std::cout << "UpgradePerformanceFunctions::getPhotonPileupFakeRescaledET("<< eTMeV <<"): ET out of allowed range os 20000-150000MeV" << std::endl;
        return 0.;
    } else{
        
        // estimate which histogram should be called
        
        int ind = TMath::Floor( (eTMeV-20000)/5000.);
        
        TH1D *htemp = (TH1D*)m_pufakes.at(ind);
        
        fakedEt = htemp->GetRandom()*1000.;
        
	//std::cout << " PU histogram " << ind << " with integral: " << htemp->Integral() << " nbins: " << htemp->GetNbinsX() << " xmin: " << htemp->GetBinLowEdge(1) << " xmax: " << htemp->GetBinLowEdge(htemp->GetNbinsX())+htemp->GetBinWidth(htemp->GetNbinsX()) << " --> jet pt: " << eTMeV << " faked ET: " << fakedEt << std::endl;
        
        return fakedEt;
    }
    
}


#endif
