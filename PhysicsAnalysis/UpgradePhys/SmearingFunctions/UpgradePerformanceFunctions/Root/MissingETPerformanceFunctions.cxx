/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETPERFORMANCEFUNCTIONS_CXX
#define MISSINGETPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "TFile.h"

#ifdef ROOTCORE
// Framework include(s):
#include "PathResolver/PathResolver.h"
#endif // ROOTCORE

void UpgradePerformanceFunctions::setMETRandomSeed(unsigned seed) {
  m_METRandom.SetSeed(seed);
}

void UpgradePerformanceFunctions::loadMETHistograms(TString filename) {
  std::cout << "Loading Missing ET histogram file " << filename << std::endl;
  std::string METFile = filename.Data();
#ifdef ROOTCORE
// Get file from data path
  METFile = PathResolverFindCalibFile(METFile);
  std::cout << "Found Missing ET histogram file: " << METFile << std::endl;
#endif // ROOTCORE

  TFile *infile=new TFile(METFile.c_str(),"READ");
 
//  m_SumEtH[0][nominal]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu60_th60_calibrated"));
//  m_SumEtH[0][process]=static_cast<TH1F*>(infile->Get("sumetPU_j0_mu60_th60_calibrated"));
//  m_SumEtH[0][thresholdUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu60_th80_uncalibrated"));
//  m_SumEtH[0][thresholdDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu60_th40_uncalibrated"));
//  m_SumEtH[0][resoUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu60_th60_calibrated"));
//  m_SumEtH[0][resoDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu60_th60_calibrated"));    
//  m_Ress[0]=static_cast<TGraphErrors*>(infile->Get("METresol_j0_mu60_th60_calibrated"));

//  m_SumEtH[1][nominal]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu80_th80_calibrated"));
//  m_SumEtH[1][process]=static_cast<TH1F*>(infile->Get("sumetPU_j0_mu80_th80_calibrated"));
//  m_SumEtH[1][thresholdUp]=static_cast<TH1F*>(infile->Get("sumetPU_j3_mu80_th100_uncalibrated"));
//  m_SumEtH[1][thresholdDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu80_th60_uncalibrated"));
//  m_SumEtH[1][resoUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu80_th80_calibrated"));
//  m_SumEtH[1][resoDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu80_th80_calibrated"));  
//  m_Ress[1]=static_cast<TGraphErrors*>(infile->Get("METresol_j0_mu80_th80_calibrated"));

//  m_SumEtH[2][nominal]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
//  m_SumEtH[2][process]=static_cast<TH1F*>(infile->Get("sumetPU_j0_mu140_th140_calibrated"));
//  m_SumEtH[2][thresholdUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu140_th200_uncalibrated"));
//  m_SumEtH[2][thresholdDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu140_th100_uncalibrated"));
//  m_SumEtH[2][resoUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
//  m_SumEtH[2][resoDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
//  m_Ress[2]=static_cast<TGraphErrors*>(infile->Get("METresol_j0_mu140_th140_calibrated"));

  m_SumEtH[3][nominal]=static_cast<TH1F*>(infile->Get("h_sumetPU"));
//  m_SumEtH[3][nominal]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
//  m_SumEtH[3][process]=static_cast<TH1F*>(infile->Get("sumetPU_j0_mu200_th200_uncalibrated"));
//  m_SumEtH[3][thresholdUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
//  m_SumEtH[3][thresholdDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu200_th140_uncalibrated"));
//  m_SumEtH[3][resoUp]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
//  m_SumEtH[3][resoDown]=static_cast<TH1F*>(infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
//  m_Ress[3]=static_cast<TGraphErrors*>(infile->Get("METresol_j0_mu200_th200_uncalibrated"));

}

UpgradePerformanceFunctions::MET UpgradePerformanceFunctions::getMETSmeared(float sumEtMeV, float METxMeV, float METyMeV, METSyst systValue) {
  double sumETPU;
  double sumET;
  double METPUreso;
  
  double METPUsmearX;
  double METPUsmearY;
  UpgradePerformanceFunctions::MET tmpMET;

  int PUcondition;
  //  typedef enum {mu60=0, mu80=1, mu140=2, mu200=3} mu;
  if (m_avgMu <= 60.) PUcondition = 0;
  else if (m_avgMu <= 80.) PUcondition = 1;
  else if (m_avgMu <= 140.) PUcondition = 2;
  else PUcondition = 3;

  if (PUcondition!=3 || systValue != nominal){
    std::cout<<"getMETSmeared error: The only parametrization approved is for the nominal working point <mu>=200!"<<std::endl;
    std::cout<<"       The tool will return the truth MET"<<std::endl;
    tmpMET.first=METxMeV;
    tmpMET.second=METyMeV;
    return tmpMET;
  }
 
  sumETPU=m_SumEtH[PUcondition][systValue]->GetRandom();
  sumET=(sumETPU*1000.)+sumEtMeV; // in MeV
  
  METPUreso=getMETResolution(sumET, systValue);

  METPUsmearX=m_METRandom.Gaus(0,METPUreso);
  METPUsmearY=m_METRandom.Gaus(0,METPUreso);
  
  tmpMET.first=METxMeV+METPUsmearX;
  tmpMET.second=METyMeV+METPUsmearY;

  return tmpMET;
}

float UpgradePerformanceFunctions::getMETResolution(float sumEtMeV, METSyst systValue) {
  // The sumEtMeV input should include any pileup contribution
  // This is done correctly in getMETSmeared (see above)
  double METPUreso=0;
//  double METPUresoNom=0;
//  double METPUresoVar=0;
//  double METPUresoTh=0;
//  double METPUDelta=0;
  float sumEtGeV = sumEtMeV/1000.;

  int PUcondition;
  //  typedef enum {mu60=0, mu80=1, mu140=2, mu200=3} mu;
  if (m_avgMu <= 60.) PUcondition = 0;
  else if (m_avgMu <= 80.) PUcondition = 1;
  else if (m_avgMu <= 140.) PUcondition = 2;
  else PUcondition = 3;

  if (PUcondition!=3 || systValue != nominal){
    std::cout<<"getMETResolution error: The only parametrization approved is for the nominal working point <mu>=200!"<<std::endl;
    std::cout<<"       The tool will return 0"<<std::endl;
    return 0.;
  }

//  double m_par1[4] = {18.71, 24.01, 32.12, 30.58};
//  double m_par2[4] = {0.6502, 0.6793, 0.6502, 0.872};
//  double m_lowsumet[4] = {700., 900., 1300., 1500.};
//  double m_highsumet[4] = {1100., 1100., 1700., 1900.};
//
//  /*MinBias*/   
//  if (sumEtGeV<m_lowsumet[PUcondition]) 
//    METPUresoNom=m_Ress[PUcondition]->Eval(sumEtGeV);
//  /*Transition*/
//  else if (sumEtGeV<m_highsumet[PUcondition]) 
//    METPUresoNom=m_Ress[PUcondition]->Eval(m_lowsumet[PUcondition])+((sumEtGeV-m_lowsumet[PUcondition])*(m_par1[PUcondition]+m_par2[PUcondition]*sqrt(m_highsumet[PUcondition])-m_Ress[PUcondition]->Eval(m_lowsumet[PUcondition]))/(m_highsumet[PUcondition]-m_lowsumet[PUcondition]));
//  /*Z' fit*/   
//  else METPUresoNom=m_par1[PUcondition]+m_par2[PUcondition]*sqrt(sumEtGeV);
//  
//  //5% systematics sample dependence, and extrapolation at low sumET wiht the fit
//  METPUresoVar=(m_par1[PUcondition]+m_par2[PUcondition]*sqrt(sumEtGeV))*0.95;
//  METPUresoVar=(METPUresoNom-METPUresoVar);    
//  
//  //5 GeV systematics for the threshold effect
//  METPUresoTh=5;
//  
//  //Sum in quadrature
//  METPUDelta=sqrt(METPUresoTh*METPUresoTh+METPUresoVar*METPUresoVar);
//  METPUreso=METPUresoNom;

  double fitParsGold[4] = {36.133, 0.00125496, 3.13886e-06, -3.79944e-10};
  double fitParsSilver[4] = {37.5551, -0.000627389, 6.11304e-06, -6.52893e-10};
  double fitParsBronze[4] = {39.049, -0.00202729, 8.30795e-06, -9.24871e-10};

  if (m_layout==gold) {
    METPUreso = ((fitParsGold[3]*sumEtGeV + fitParsGold[2])*sumEtGeV + fitParsGold[1])*sumEtGeV + fitParsGold[0];
  } else if (m_layout==silver) {
    METPUreso = ((fitParsSilver[3]*sumEtGeV + fitParsSilver[2])*sumEtGeV + fitParsSilver[1])*sumEtGeV + fitParsSilver[0];
  } else if (m_layout==bronze) {
    METPUreso = ((fitParsBronze[3]*sumEtGeV + fitParsBronze[2])*sumEtGeV + fitParsBronze[1])*sumEtGeV + fitParsBronze[0];
  } else {
    std::cout << "getMETResolution error: layout " << m_layout << " is not implemented.  Returning 0." << std::endl;
    METPUreso = 0.;
  }

//  if (systValue==resoDown ){
//    METPUreso=METPUresoNom-METPUDelta;
//    if (METPUreso<0) METPUreso=0;//to be sure we do not have negative smearings.
//  }
//  if (systValue==resoUp){
//    METPUreso=METPUresoNom+METPUDelta;
//  }

  return METPUreso*1000.; // conversion to MeV
}

#endif
