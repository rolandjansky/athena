/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETPERFORMANCEFUNCTIONS_CXX
#define MISSINGETPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"

namespace Upgrade {

StatusCode UpgradePerformanceFunctions::loadMETHistograms() {
  ATH_MSG_INFO("Loading Missing ET histogram file");
  std::string METFile = PathResolverFindCalibFile(m_METFilename);

  TFile *m_infile=new TFile(METFile.c_str(),"READ");
  if(m_layout!=run2){  //upgrade settings
    //  m_SumEtH[0][nominal]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu60_th60_calibrated"));
    //  m_SumEtH[0][process]=static_cast<TH1F*>(m_infile->Get("sumetPU_j0_mu60_th60_calibrated"));
    //  m_SumEtH[0][thresholdUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu60_th80_uncalibrated"));
    //  m_SumEtH[0][thresholdDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu60_th40_uncalibrated"));
    //  m_SumEtH[0][resoUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu60_th60_calibrated"));
    //  m_SumEtH[0][resoDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu60_th60_calibrated"));    
    //  m_Ress[0]=static_cast<TGraphErrors*>(m_infile->Get("METresol_j0_mu60_th60_calibrated"));
    
    //  m_SumEtH[1][nominal]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu80_th80_calibrated"));
    //  m_SumEtH[1][process]=static_cast<TH1F*>(m_infile->Get("sumetPU_j0_mu80_th80_calibrated"));
    //  m_SumEtH[1][thresholdUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_j3_mu80_th100_uncalibrated"));
    //  m_SumEtH[1][thresholdDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu80_th60_uncalibrated"));
    //  m_SumEtH[1][resoUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu80_th80_calibrated"));
    //  m_SumEtH[1][resoDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu80_th80_calibrated"));  
    //  m_Ress[1]=static_cast<TGraphErrors*>(m_infile->Get("METresol_j0_mu80_th80_calibrated"));
    
    //  m_SumEtH[2][nominal]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
    //  m_SumEtH[2][process]=static_cast<TH1F*>(m_infile->Get("sumetPU_j0_mu140_th140_calibrated"));
    //  m_SumEtH[2][thresholdUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu140_th200_uncalibrated"));
    //  m_SumEtH[2][thresholdDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu140_th100_uncalibrated"));
    //  m_SumEtH[2][resoUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
    //  m_SumEtH[2][resoDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu140_th140_calibrated"));
    //  m_Ress[2]=static_cast<TGraphErrors*>(m_infile->Get("METresol_j0_mu140_th140_calibrated"));

    m_SumEtH[3][nominal]=static_cast<TH1F*>(m_infile->Get("h_sumetPU"));
    //  m_SumEtH[3][nominal]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
    //  m_SumEtH[3][process]=static_cast<TH1F*>(m_infile->Get("sumetPU_j0_mu200_th200_uncalibrated"));
    //  m_SumEtH[3][thresholdUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
    //  m_SumEtH[3][thresholdDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu200_th140_uncalibrated"));
    //  m_SumEtH[3][resoUp]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
    //  m_SumEtH[3][resoDown]=static_cast<TH1F*>(m_infile->Get("sumetPU_zprime_mu200_th200_uncalibrated"));
    //  m_Ress[3]=static_cast<TGraphErrors*>(m_infile->Get("METresol_j0_mu200_th200_uncalibrated"));
  }
  else{ //Run2 settings

    //kindly provided by M. Valente
    //https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PLOTS/JETM-2017-001/fig_01.png
    //https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PLOTS/JETM-2017-001/fig_03.png
    //https://atlas.web.cern.ch/Atlas/GROUPS/PHYSICS/PLOTS/JETM-2017-001/fig_05.png

    m_met_resoperp.reset(dynamic_cast<TGraphErrors*>( m_infile->Get("Zee/Data_perpendicular_reso2") ) ); //MT: it's squared!
    m_met_resopara.reset(dynamic_cast<TGraphErrors*>( m_infile->Get("Zee/Data_parallel_reso2") ) );      //MT: it's squared!
    m_met_shiftpara.reset(dynamic_cast<TGraphErrors*>( m_infile->Get("Zee/Data_parallel_scale") ) );

  }

  return StatusCode::SUCCESS;
}

UpgradePerformanceFunctions::MET UpgradePerformanceFunctions::getMETSmeared(float truthSumEtMeV, float METxMeV, float METyMeV, METSyst systValue) {
  double METPUreso;
  double METPUsmearX;
  double METPUsmearY;
  UpgradePerformanceFunctions::MET tmpMET;

  // GetRandom() uses ROOT gRandom, which uses process-specific seed
  // For reproducibility, we reset the seed using our controlled m_METRandom
  gRandom->SetSeed( int(1./m_METRandom.Rndm()) );
  
  METPUreso=getMETResolution(getSumETwithPileup(truthSumEtMeV, systValue),
			     systValue);

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
    ATH_MSG_ERROR("getMETResolution error: The only parametrization approved is for the nominal working point <mu>=200!");
    ATH_MSG_ERROR("The tool will return 0");
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
//  /*Z´ fit*/   
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

  if (m_layout==gold || m_layout==Step1p6) {
    METPUreso = ((fitParsGold[3]*sumEtGeV + fitParsGold[2])*sumEtGeV + fitParsGold[1])*sumEtGeV + fitParsGold[0];
  } else if (m_layout==silver) {
    METPUreso = ((fitParsSilver[3]*sumEtGeV + fitParsSilver[2])*sumEtGeV + fitParsSilver[1])*sumEtGeV + fitParsSilver[0];
  } else if (m_layout==bronze) {
    METPUreso = ((fitParsBronze[3]*sumEtGeV + fitParsBronze[2])*sumEtGeV + fitParsBronze[1])*sumEtGeV + fitParsBronze[0];
  } else {
    ATH_MSG_ERROR("getMETResolution error: layout " << m_layout << " is not implemented.  Returning 0.");
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

float UpgradePerformanceFunctions::getSumETwithPileup(float truthSumEtMeV, METSyst systValue) {
  // This method accesses the corrected sumET = truthSumET + pileupSumET

  int PUcondition;
  //  typedef enum {mu60=0, mu80=1, mu140=2, mu200=3} mu;
  if (m_avgMu <= 60.) PUcondition = 0;
  else if (m_avgMu <= 80.) PUcondition = 1;
  else if (m_avgMu <= 140.) PUcondition = 2;
  else PUcondition = 3;
  
  if (PUcondition!=3 || systValue != nominal) {
    ATH_MSG_ERROR("getSumETwithPileup error: The only parametrization approved is for the nominal working point <mu>=200!");
    ATH_MSG_ERROR("       The tool will return the truth sum ET");
    return truthSumEtMeV;
  } else {
    // GetRandom() uses ROOT gRandom, which uses process-specific seed
    // For reproducibility, we reset the seed using our controlled m_METRandom
    gRandom->SetSeed( int(1./m_METRandom.Rndm()) );
    double sumETPU=m_SumEtH[PUcondition][systValue]->GetRandom();
    
    return sumETPU*1000. + truthSumEtMeV; // in MeV
  }
}

TVector3 UpgradePerformanceFunctions::getTSTsmearing(TVector3 pthard) { //input in MeV
  
  float pth = pthard.Mag()*0.001; 

  float shift = m_met_shiftpara->Eval(pth) * 1000.;   //convert GeV to MeV
  float reso_para = sqrt(m_met_resopara->Eval(pth)) * 1000.;
  float reso_perp = sqrt(m_met_resoperp->Eval(pth)) * 1000.;
  
  // std::cout << "shift = " << shift << std::endl;
  // std::cout << "reso_para = " << reso_para << std::endl;
  // std::cout << "reso_perp = " << reso_perp << std::endl;

  auto para = pthard.Unit() * (shift + m_METRandom.Gaus(0,reso_para));
  auto perp = pthard.Orthogonal().Unit() * m_METRandom.Gaus(0,reso_perp);

  return (para+perp);  //in MeV
  
}

}

#endif
