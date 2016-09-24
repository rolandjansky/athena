/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZdcAnalysisTool.h"
#include "TGraph.h"
#include "TEnv.h"
#include "TSystem.h"
#include "ZdcAnalysis/ZdcSincInterp.h"
#include "xAODEventInfo/EventInfo.h"
#include "TFile.h"
#include <sstream>
#include <memory>
#include <xAODForward/ZdcModuleAuxContainer.h>
#include "PathResolver/PathResolver.h"

namespace ZDC
{

ZdcAnalysisTool::ZdcAnalysisTool(const std::string& name) : asg::AsgTool(name), m_name(name), m_init(false), m_zdcTriggerEfficiency(0)
{

#ifndef XAOD_STANDALONE
   declareInterface<IZdcAnalysisTool>(this);
#endif

  declareProperty("ZdcModuleContainerName",m_zdcModuleContainerName="ZdcModules","Location of ZDC processed data");
  declareProperty("ZdcAnalysisConfigPath",m_zdcAnalysisConfigPath="$ROOTCOREBIN/data/ZdcAnalysis","ZDC Analysis config file path");
  declareProperty("FlipEMDelay",m_flipEMDelay=false);
  declareProperty("LowGainOnly",m_lowGainOnly=false);
  declareProperty("DoCalib",m_doCalib=true);
  declareProperty("ForceCalibRun",m_forceCalibRun=287931); // last run of Pb+Pb 2015
  declareProperty("ForceCalibLB",m_forceCalibLB=814); // last LB of Pb+Pb 2015

  //ATH_MSG_INFO("Creating ZdcAnalysisoTool named " << m_name);
  //ATH_MSG_INFO("ZDC config file path " << m_zdcAnalysisConfigPath);

  if (m_forceCalibRun>-1)
    {
      ATH_MSG_INFO("CAREFUL: forcing calibration run/LB =" << m_forceCalibRun << "/" << m_forceCalibLB);
    }

}

ZdcAnalysisTool::~ZdcAnalysisTool()
{
  ATH_MSG_DEBUG("Deleting ZdcAnalysisTool named " << m_name);
  //SafeDelete( m_zdcDataAnalyzer );
}

void ZdcAnalysisTool::initializeTriggerEffs()
{
  if (!m_zdcTriggerEfficiency)
    {
      m_zdcTriggerEfficiency = new ZDCTriggerEfficiency();
    }

  std::string filename = PathResolverFindCalibFile( ("ZdcAnalysis/"+m_zdcTriggerEffParamsFileName) );
  //std::cout << "Found trigger config file " << filename << std::endl;

  TFile* file = TFile::Open(filename.c_str());
  if (!file->IsOpen()) 
    {
      ATH_MSG_ERROR("No trigger efficiencies at "  << filename);
      return;
    }

  file->Print();

  stringstream Aalpha_name;
  Aalpha_name<<"A_alpha_"<<m_runNumber;
  TSpline3* par_A_alpha = (TSpline3*)file->GetObjectChecked(Aalpha_name.str().c_str(),"TSpline3");

  if (!par_A_alpha)
    {
      ATH_MSG_ERROR("No trigger efficiencies for run number " << m_runNumber);
      m_doCalib = false;
    }

  stringstream Abeta_name;
  Abeta_name<<"A_beta_"<<m_runNumber;
  TSpline3* par_A_beta = (TSpline3*)file->GetObjectChecked(Abeta_name.str().c_str(),"TSpline3");
  stringstream Atheta_name;
  Atheta_name<<"A_theta_"<<m_runNumber;
  TSpline3* par_A_theta = (TSpline3*)file->GetObjectChecked(Atheta_name.str().c_str(),"TSpline3");
  
  stringstream Calpha_name;
  Calpha_name<<"C_alpha_"<<m_runNumber;
  TSpline3* par_C_alpha = (TSpline3*)file->GetObjectChecked(Calpha_name.str().c_str(),"TSpline3");
  stringstream Cbeta_name;
  Cbeta_name<<"C_beta_"<<m_runNumber;
  TSpline3* par_C_beta = (TSpline3*)file->GetObjectChecked(Cbeta_name.str().c_str(),"TSpline3");
  stringstream Ctheta_name;
  Ctheta_name<<"C_theta_"<<m_runNumber;
  TSpline3* par_C_theta = (TSpline3*)file->GetObjectChecked(Ctheta_name.str().c_str(),"TSpline3");
  
  stringstream Err_Aalpha_name;
  Err_Aalpha_name<<"A_alpha_error_"<<m_runNumber;
  TSpline3* parErr_A_alpha = (TSpline3*)file->GetObjectChecked(Err_Aalpha_name.str().c_str(),"TSpline3");
  stringstream Err_Abeta_name;
  Err_Abeta_name<<"A_beta_error_"<<m_runNumber;
  TSpline3* parErr_A_beta = (TSpline3*)file->GetObjectChecked(Err_Abeta_name.str().c_str(),"TSpline3");
  stringstream Err_Atheta_name;
  Err_Atheta_name<<"A_theta_error_"<<m_runNumber;
  TSpline3* parErr_A_theta = (TSpline3*)file->GetObjectChecked(Err_Atheta_name.str().c_str(),"TSpline3");
  
  stringstream Err_Calpha_name;
  Err_Calpha_name<<"C_alpha_error_"<<m_runNumber;
  TSpline3* parErr_C_alpha = (TSpline3*)file->GetObjectChecked(Err_Calpha_name.str().c_str(),"TSpline3");
  stringstream Err_Cbeta_name;
  Err_Cbeta_name<<"C_beta_error_"<<m_runNumber;
  TSpline3* parErr_C_beta = (TSpline3*)file->GetObjectChecked(Err_Cbeta_name.str().c_str(),"TSpline3");
  stringstream Err_Ctheta_name;
  Err_Ctheta_name<<"C_theta_error_"<<m_runNumber;
  TSpline3* parErr_C_theta = (TSpline3*)file->GetObjectChecked(Err_Ctheta_name.str().c_str(),"TSpline3");
  
  
  stringstream Cov_A_alpha_beta_name;
  Cov_A_alpha_beta_name<<"cov_A_alpha_beta_"<<m_runNumber;
  TSpline3* cov_A_alpha_beta = (TSpline3*)file->GetObjectChecked(Cov_A_alpha_beta_name.str().c_str(),"TSpline3");
  stringstream Cov_A_alpha_theta_name;
  Cov_A_alpha_theta_name<<"cov_A_alpha_theta_"<<m_runNumber;
  TSpline3* cov_A_alpha_theta = (TSpline3*)file->GetObjectChecked(Cov_A_alpha_theta_name.str().c_str(),"TSpline3");
  stringstream Cov_A_beta_theta_name;
  Cov_A_beta_theta_name<<"cov_A_beta_theta_"<<m_runNumber;
  TSpline3* cov_A_beta_theta = (TSpline3*)file->GetObjectChecked(Cov_A_beta_theta_name.str().c_str(),"TSpline3");
  
  stringstream Cov_C_alpha_beta_name;
  Cov_C_alpha_beta_name<<"cov_C_alpha_beta_"<<m_runNumber;
  TSpline3* cov_C_alpha_beta = (TSpline3*)file->GetObjectChecked(Cov_C_alpha_beta_name.str().c_str(),"TSpline3");
  stringstream Cov_C_alpha_theta_name;
  Cov_C_alpha_theta_name<<"cov_C_alpha_theta_"<<m_runNumber;
  TSpline3* cov_C_alpha_theta = (TSpline3*)file->GetObjectChecked(Cov_C_alpha_theta_name.str().c_str(),"TSpline3");
  stringstream Cov_C_beta_theta_name;
  Cov_C_beta_theta_name<<"cov_C_beta_theta_"<<m_runNumber;
  TSpline3* cov_C_beta_theta = (TSpline3*)file->GetObjectChecked(Cov_C_beta_theta_name.str().c_str(),"TSpline3");
  
  std::array<std::vector<TSpline3*>, 2> effparams;
  std::array<std::vector<TSpline3*>, 2> effparamErrors;
  std::array<std::vector<TSpline3*>, 2> effparamsCorrCoeffs;
  //side0: C; side1: A
  effparams[0] = {par_C_alpha, par_C_beta, par_C_theta};	
  effparams[1] = {par_A_alpha, par_A_beta, par_A_theta};	
  effparamErrors[0] = {parErr_C_alpha, parErr_C_beta, parErr_C_theta};
  effparamErrors[1] = {parErr_A_alpha, parErr_A_beta, parErr_A_theta};
  effparamsCorrCoeffs[0] = {cov_C_alpha_beta, cov_C_alpha_theta, cov_C_beta_theta};
  effparamsCorrCoeffs[1] = {cov_A_alpha_beta, cov_A_alpha_theta, cov_A_beta_theta};
  
  m_zdcTriggerEfficiency->SetEffParamsAndErrors(effparams, effparamErrors);
  m_zdcTriggerEfficiency->SetEffParamCorrCoeffs(effparamsCorrCoeffs);
  
  return;
  
}

void ZdcAnalysisTool::initialize40MHz()
{

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1 = {4.2, 3.8, 5.2, 5.0,
                                               5.0, 3.7, 3.5, 3.5};

  // identical to 80 MHz -- is this right
  ZDCDataAnalyzer::ZDCModuleFloatArray tau2 = {20.0, 20.4, 18.9, 20.8,
                                               19.1, 21.9, 22.6, 23.4};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinSamples = {1, 1, 2, 1,
                                                                 1, 1, 1, 1};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG = {-8, -8, -8, -8,
								      -8, -8, -8, -8};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsLG = {-4, -4, -4, -4,
								      -4, -4, -4, -4};

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {800, 800, 800, 800, 800, 800, 800, 800};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {1020, 1020, 1020, 1020, 1020, 1020, 1020, 1020};

  // Set Tau and nominal timing offsets                                                                                                                          
  ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;

  bool fixTau1 = true;
  bool fixTau2 = true;

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      fixTau1Arr[side][module] = fixTau1;
      fixTau2Arr[side][module] = fixTau2;
    }
  }

  ZDCDataAnalyzer::ZDCModuleFloatArray t0HG = {53.942, 49.887, 59.633, 46.497,
					       46.314, 42.267, 50.327, 41.605};
  ZDCDataAnalyzer::ZDCModuleFloatArray t0LG = {51.771, 47.936, 57.438, 44.191,
					       44.295, 41.755, 48.081, 40.175};


  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {8, 8, 8, 11, 8, 10, 8, 12};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {8, 8, 8, 11, 8, 10, 8, 12};

  std::array<std::array<std::vector<float>, 4>, 2> slewingParamsHG, slewingParamsLG;

  slewingParamsHG[0][0] = {0, -7.904e-02, 4.686e-02, 1.530e-03 };
  slewingParamsHG[0][1] = {0, 2.250e-02, 4.732e-02, 6.050e-03  };
  slewingParamsHG[0][2] = {0, 4.388e-02, 6.707e-02, -5.526e-05 };
  slewingParamsHG[0][3] = {0, 1.205e-01, 2.726e-02, 2.610e-03  };

  slewingParamsHG[1][0] = {0, 6.861e-02, 5.175e-03, -9.018e-04  };
  slewingParamsHG[1][1] = {0, 3.855e-01, -4.442e-02, -2.022e-02 };
  slewingParamsHG[1][2] = {0, -4.337e-03, 3.841e-02, 4.661e-03  };
  slewingParamsHG[1][3] = {0, 3.623e-01, -3.882e-02, -1.805e-02 };

  slewingParamsLG[0][0] = {0, 1.708e-02, 7.929e-02, 5.079e-03   };
  slewingParamsLG[0][1] = {0, 1.406e-01, 1.209e-01, -1.922e-04  };
  slewingParamsLG[0][2] = {0, 1.762e-01, 1.118e-01, 1.679e-04   };
  slewingParamsLG[0][3] = {0, 1.361e-02, -2.685e-02, -4.168e-02 };

  slewingParamsLG[1][0] = {0, 1.962e-01, -5.025e-03, -2.001e-02 };
  slewingParamsLG[1][1] = {0, 3.258e-01, 1.229e-02, -2.925e-02  };
  slewingParamsLG[1][2] = {0, 1.393e-01, 8.113e-02, -2.594e-03  };
  slewingParamsLG[1][3] = {0, 1.939e-01, 2.188e-02, -5.579e-02  };

  std::array<std::array<std::vector<float>, 4>, 2> moduleHGNonLinCorr;
  moduleHGNonLinCorr[0][0] = {-3.76800e-02, 4.63597e-02};
  moduleHGNonLinCorr[0][1] = {-1.02185e-01, -1.17548e-01};
  moduleHGNonLinCorr[0][2] = {-8.78451e-02, -1.52174e-01};
  moduleHGNonLinCorr[0][3] = {-1.04835e-01, -1.96514e-01};
  moduleHGNonLinCorr[1][0] = {-6.83115e-02, 3.57802e-02};
  moduleHGNonLinCorr[1][1] = {-1.08162e-01, -1.91413e-01};
  moduleHGNonLinCorr[1][2] = {-7.82514e-02, -1.21218e-01};
  moduleHGNonLinCorr[1][3] = {-2.34354e-02, -2.52033e-01};

  m_zdcDataAnalyzer_40MHz = new ZDCDataAnalyzer(7,25,0,"FermiExp",peak2ndDerivMinSamples,
						peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG,m_lowGainOnly);

  m_zdcDataAnalyzer_40MHz->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  m_zdcDataAnalyzer_40MHz->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0HG, t0LG);
  m_zdcDataAnalyzer_40MHz->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);
  m_zdcDataAnalyzer_40MHz->SetTimingCorrParams(slewingParamsHG, slewingParamsLG);
  m_zdcDataAnalyzer_40MHz->SetNonlinCorrParams(moduleHGNonLinCorr);

}

void ZdcAnalysisTool::initialize80MHz()
{

  _peak2ndDerivMinSamples = {3, 2, 3, 2,
                             2, 2, 2, 2};

  _peak2ndDerivMinThresholdsHG = {-8, -8, -8, -8,
				  -8, -8, -8, -8};

  _peak2ndDerivMinThresholdsLG = {-4, -4, -4, -4,
				  -4, -4, -4, -4};

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {800, 800, 800, 800, 800, 800, 800, 800};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {950, 950, 950, 950, 950, 950, 950, 950};

  // Set Tau and nominal timing offsets                                                                                                                          
  ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;

  bool fixTau1 = true;
  bool fixTau2 = true;

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      fixTau1Arr[side][module] = fixTau1;
      fixTau2Arr[side][module] = fixTau2;
    }
  }

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1 = {3.9, 3.4, 4.1, 4.2,
                                               4.2, 3.6, 3.3, 3.4};

  ZDCDataAnalyzer::ZDCModuleFloatArray tau2 = {20.0, 20.4, 18.9, 20.8,
                                               19.1, 21.9, 22.6, 23.4};

  ZDCDataAnalyzer::ZDCModuleFloatArray t0HG = {44.24, 40.35, 49.3, 36.0,
					       36.0, 31.1, 40.75, 30.5};
					       
  ZDCDataAnalyzer::ZDCModuleFloatArray t0LG = {42.65, 38.5, 47.4, 34,
					       33.7, 29.9, 39.0, 29.3};

  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {8, 8, 8, 11, 8, 10, 8, 12};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {8, 8, 8, 11, 8, 10, 8, 12};

  std::array<std::array<std::vector<float>, 4>, 2> slewingParamsHG, slewingParamsLG;

  slewingParamsHG[0][0] = {0, -6.5e-2,  2.85e-2, -2.83e-3};
  slewingParamsHG[0][1] = {0, -5.5e-2,  5.13e-2,  5.6e-3};
  slewingParamsHG[0][2] = {0, -1.45e-3, 9.3e-2,   3.9e-3};
  slewingParamsHG[0][3] = {0, -2.36e-2, 8.3e-2,   1.1e-3};

  slewingParamsHG[1][0] = {0, -6.5e-2,  4.84e-2, -3.7e-3};
  slewingParamsHG[1][1] = {0,  1.34e-2, 6.57e-2,  5.37e-3};
  slewingParamsHG[1][2] = {0, -5.37e-2, 3.49e-2,  3.8e-3};
  slewingParamsHG[1][3] = {0, -3.3e-2,  3.9e-2,   2.2e-3};

  slewingParamsLG[0][0] = {0, -9.6e-2,  4.39e-2,  2.93e-3 };
  slewingParamsLG[0][1] = {0, -5.0e-2, 14.9e-2,  20.6e-3  };
  slewingParamsLG[0][2] = {0, -4.4e-2,  5.3e-2,   0,      };
  slewingParamsLG[0][3] = {0, -9.90e-2, 4.08e-2,  0,      };

  slewingParamsLG[1][0] = {0,  -8.7e-2,  4.2e-2,  -3.2e-3 };
  slewingParamsLG[1][1] = {0,  -3.26e-2, 3.84e-2, -2.32e-3};
  slewingParamsLG[1][2] = {0, -26.8e-2, -2.64e-2, -5.3e-3 };
  slewingParamsLG[1][3] = {0, -13.2e-2,  0.45e-2, -2.4e-3 };

  std::array<std::array<std::vector<float>, 4>, 2> moduleHGNonLinCorr;
  moduleHGNonLinCorr[0][0] = {-3.76800e-02, 4.63597e-02};
  moduleHGNonLinCorr[0][1] = {-1.02185e-01, -1.17548e-01};
  moduleHGNonLinCorr[0][2] = {-8.78451e-02, -1.52174e-01};
  moduleHGNonLinCorr[0][3] = {-1.04835e-01, -1.96514e-01};
  moduleHGNonLinCorr[1][0] = {-6.83115e-02, 3.57802e-02};
  moduleHGNonLinCorr[1][1] = {-1.08162e-01, -1.91413e-01};
  moduleHGNonLinCorr[1][2] = {-7.82514e-02, -1.21218e-01};
  moduleHGNonLinCorr[1][3] = {-2.34354e-02, -2.52033e-01};

  m_zdcDataAnalyzer_80MHz = new ZDCDataAnalyzer(7,12.5,0,"FermiExp",_peak2ndDerivMinSamples,
						_peak2ndDerivMinThresholdsHG, _peak2ndDerivMinThresholdsLG,m_lowGainOnly);

  m_zdcDataAnalyzer_80MHz->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  m_zdcDataAnalyzer_80MHz->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0HG, t0LG);
  m_zdcDataAnalyzer_80MHz->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);
  m_zdcDataAnalyzer_80MHz->SetTimingCorrParams(slewingParamsHG, slewingParamsLG);
  m_zdcDataAnalyzer_80MHz->SetNonlinCorrParams(moduleHGNonLinCorr);
}

StatusCode ZdcAnalysisTool::initializeTool()
{
  m_init = true;
  tf1SincInterp = new TF1("SincInterp",ZDC::SincInterp,-5.,160.,8);
  tf1SincInterp->SetNpx(300);

  /*
  char* path = gSystem->ExpandPathName(m_zdcAnalysisConfigPath.c_str());
  ATH_MSG_INFO("Resolved file path " << path);

  TString zdcConf(path);
  zdcConf += "/ZdcAnalysisConfig.conf";

  TEnv env(zdcConf);
  */

  std::string filename = PathResolverFindCalibFile( "ZdcAnalysis/ZdcAnalysisConfig.conf" );
  //std::cout << "Found config file " << filename << std::endl;
  TEnv env(filename.c_str());

  m_zdcEnergyCalibFileName = std::string(env.GetValue("ZdcEnergyCalibFileName","ZdcCalibrations_v1.root"));
  ATH_MSG_INFO("ZDC energy calibration filename " << m_zdcEnergyCalibFileName);
  m_zdcTimeCalibFileName = std::string(env.GetValue("ZdcTimeCalibFileName","ZdcTimeCalibrations_v1.root"));
  ATH_MSG_INFO("ZDC time calibration filename " << m_zdcTimeCalibFileName);
  m_zdcTriggerEffParamsFileName = std::string(env.GetValue("ZdcTriggerEffFileName", "ZdcTriggerEffParameters_v3.root"));
  ATH_MSG_INFO("ZDC trigger efficiencies filename " << m_zdcTriggerEffParamsFileName);
  
  initialize80MHz();
  initialize40MHz();

  m_zdcDataAnalyzer = m_zdcDataAnalyzer_80MHz; // default

  return StatusCode::SUCCESS;

}

StatusCode ZdcAnalysisTool::recoZdcModule(const xAOD::ZdcModule& module)  
{
  ATH_MSG_DEBUG("Processing ZDC module S/T/M/C = " 
	       << module.side() << " " 
	       << module.type() << " " 
	       << module.zdcModule() << " " 
	       << module.channel() 
	       );
  const std::vector<unsigned short>* adc0;
  const std::vector<unsigned short>* adc1;

  
  if (module.type()==0 && module.zdcModule()==0 && m_flipEMDelay) // flip delay/non-delay for EM big tube
    {
      adc0 = &(*(module.TTg0d1Link()))->adc();
      adc1 = &(*(module.TTg1d1Link()))->adc();
    }
  else
    {
      adc0 = &(*(module.TTg0d0Link()))->adc();
      adc1 = &(*(module.TTg1d0Link()))->adc();
    }

  float amp;
  float time;
  float qual;

  float deltaT = 12.5;

  sigprocMaxFinder(*adc0,deltaT,amp,time,qual);
  module.auxdecor<float>("amplitudeG0_mf")=amp;
  module.auxdecor<float>("timeG0_mf")=time;

  sigprocMaxFinder(*adc1,deltaT,amp,time,qual);
  module.auxdecor<float>("amplitudeG1_mf")=amp;
  module.auxdecor<float>("timeG1_mf")=time;

  if (module.type()==0)
    {
      sigprocSincInterp(*adc0,deltaT,amp,time,qual);
      module.auxdecor<float>("amplitudeG0_si")=amp;
      module.auxdecor<float>("timeG0_si")=time;
    }

  return StatusCode::SUCCESS;
}

StatusCode ZdcAnalysisTool::recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) 
{
  if (!m_eventReady)
    {
      ATH_MSG_INFO("Event not ready for ZDC reco!");
      return StatusCode::FAILURE;
    }

  /*
  for (const auto zdcModule : moduleContainer)
    {
      recoZdcModule(*zdcModule);
    }
  */
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK(evtStore()->retrieve(eventInfo,"EventInfo"));

  // check for new run number, replace it by the forced value if it exists
  int thisRunNumber = eventInfo->runNumber();
  if (m_forceCalibRun>-1) thisRunNumber = m_forceCalibRun;

  if (thisRunNumber != m_runNumber)
    {

      m_runNumber = thisRunNumber;

      ATH_MSG_INFO("ZDC analysis tool will be configured for run " << m_runNumber);      

      m_zdcDataAnalyzer = m_zdcDataAnalyzer_80MHz;
      if (m_runNumber<287222) m_zdcDataAnalyzer = m_zdcDataAnalyzer_40MHz;

      if (m_doCalib) setEnergyCalibrations();
      //setTimeCalibrations();      
      
      if (m_doCalib) initializeTriggerEffs(); // if energy calibrations fail to load, then so will trigger efficiencies

    }

  
  if (m_forceCalibRun>-1 && m_forceCalibLB>-1) 
    m_lumiBlock = m_forceCalibLB;
  else
    m_lumiBlock = eventInfo->lumiBlock();

  m_zdcDataAnalyzer->StartEvent(m_lumiBlock);

  const std::vector<unsigned short>* adc0;
  const std::vector<unsigned short>* adc1;

  for (const auto zdcModule : moduleContainer)
    {
      
      if (zdcModule->type()!=0) continue;

      if (zdcModule->zdcModule()==0 && m_flipEMDelay) // flip delay/non-delay for EM big tube
	{
	  adc0 = &(*(zdcModule->TTg0d1Link()))->adc();
	  adc1 = &(*(zdcModule->TTg1d1Link()))->adc();
	}
      else
	{
	  adc0 = &(*(zdcModule->TTg0d0Link()))->adc();
	  adc1 = &(*(zdcModule->TTg1d0Link()))->adc();
	}
      
      static std::vector<float> HGADCSamples(7);
      static std::vector<float> LGADCSamples(7);

      std::copy(adc0->begin(),adc0->end(),LGADCSamples.begin());
      std::copy(adc1->begin(),adc1->end(),HGADCSamples.begin());
      
      int side = (zdcModule->side()==-1) ? 0 : 1 ;

      //std::cout << "LG: ";for (int i = 0;i<7;i++){std::cout<<LGADCSamples.at(i)<< " ";};std::cout<<std::endl;
      //std::cout << "HG: ";for (int i = 0;i<7;i++){std::cout<<HGADCSamples.at(i)<< " ";};std::cout<<std::endl;

      m_zdcDataAnalyzer->LoadAndAnalyzeData(side,zdcModule->zdcModule(),HGADCSamples, LGADCSamples);
    }

  m_zdcDataAnalyzer->FinishEvent();

  for (const auto zdcModule : moduleContainer)
    {

      if (zdcModule->type()!=0) continue;

      int side = (zdcModule->side()==-1) ? 0 : 1 ;
      int mod = zdcModule->zdcModule();

      float calibEnergy = m_zdcDataAnalyzer->GetModuleCalibAmplitude(side, mod);
      zdcModule->auxdecor<float>("CalibEnergy") = calibEnergy;
      const ZDCPulseAnalyzer* pulseAna_p = m_zdcDataAnalyzer->GetPulseAnalyzer(side, mod);
      zdcModule->auxdecor<unsigned int>("Status") = pulseAna_p->GetStatusMask();
      zdcModule->auxdecor<float>("Amp") = pulseAna_p->GetAmplitude();
      zdcModule->auxdecor<float>("FitT0") = pulseAna_p->GetFitT0();
      zdcModule->auxdecor<float>("T0Corr") = pulseAna_p->GetT0Corr();
      zdcModule->auxdecor<float>("FitChisq") = pulseAna_p->GetChisq();

      /*      
      std::cout << "side = " << side << " module=" << zdcModule->zdcModule() << " CalibEnergy=" << zdcModule->auxdecor<float>("CalibEnergy") 
		<< " should be " << m_zdcDataAnalyzer->GetModuleCalibAmplitude(side,mod) << std::endl;
      */
    }

  // Record sum objects  

  //xAOD::ZdcModuleContainer* newModuleContainer = new xAOD::ZdcModuleContainer();
  //xAOD::ZdcModuleAuxContainer* newModuleAuxContainer = new xAOD::ZdcModuleAuxContainer() ;

  std::unique_ptr<xAOD::ZdcModuleContainer> newModuleContainer( new xAOD::ZdcModuleContainer() );
  std::unique_ptr<xAOD::ZdcModuleAuxContainer> newModuleAuxContainer( new xAOD::ZdcModuleAuxContainer() );

  newModuleContainer->setStore( newModuleAuxContainer.get() );

  for (int iside=0;iside<2;iside++)
    {
      xAOD::ZdcModule* zdc_sum = new xAOD::ZdcModule;
      newModuleContainer.get()->push_back(zdc_sum);
      zdc_sum->setSide(iside);

      float calibEnergy = getCalibModuleSum(iside);
      zdc_sum->auxdecor<float>("CalibEnergy") = calibEnergy;

      float finalEnergy = calibEnergy;
      if (iside==0) finalEnergy = finalEnergy*(1 + 7e-7 * finalEnergy); // nonlinear correction for side C

      zdc_sum->auxdecor<float>("FinalEnergy") = finalEnergy;
      zdc_sum->auxdecor<float>("AverageTime") = getAverageTime(iside);
      zdc_sum->auxdecor<unsigned int>("Status") = !sideFailed(iside);
      zdc_sum->auxdecor<unsigned int>("ModuleMask") = (getModuleMask()>>(4*iside)) & 0xF;
    }

  ATH_CHECK( evtStore()->record( std::move(newModuleContainer) , "ZdcSums") ) ;
  ATH_CHECK( evtStore()->record( std::move(newModuleAuxContainer) , "ZdcSumsAux.") );

  return StatusCode::SUCCESS;
}

void ZdcAnalysisTool::setEnergyCalibrations()
{

  char name[128];
  /*
  sprintf(name,"%s/%s",m_zdcAnalysisConfigPath.c_str(),m_zdcEnergyCalibFileName.c_str());
  ATH_MSG_INFO("Opening energy calibration file " << name);
  TFile* fCalib = TFile::Open(name);
  */

  std::string filename = PathResolverFindCalibFile( ("ZdcAnalysis/"+m_zdcEnergyCalibFileName).c_str() );
  ATH_MSG_INFO("Opening energy calibration file " << filename);
  TFile* fCalib = TFile::Open(filename.c_str());
  
  //std::array<std::array<TSpline*, 4>, 2> splines;

  for (int iside=0;iside<2;iside++)
    {
      for(int imod=0;imod<4;imod++)
	{
	  // need to delete old splines
	  SafeDelete( m_splines[iside][imod] );

	  sprintf(name,"ZDC_Gcalib_run%d_s%d_m%d",m_runNumber,iside,imod);
	  ATH_MSG_INFO("Searching for graph " << name);
	  TGraph* g = (TGraph*) fCalib->GetObjectChecked(name,"TGraph");
	  if (!g && m_doCalib)
	    {
	      ATH_MSG_ERROR("No calibrations for run " << m_runNumber);
	      m_doCalib = false;
	    }

	  TSpline3* spline = 0;
	  if (g)
	    {
	      m_splines[iside][imod] = new TSpline3(name,g);
	    }
	  else
	    {
	      ATH_MSG_ERROR("No graph " << name);
	    }
	}
    }
  fCalib->Close();
  if (m_doCalib) m_zdcDataAnalyzer->LoadEnergyCalibrations(m_splines);
  
  return;
}

void ZdcAnalysisTool::setTimeCalibrations()
{
  char name[128];
  sprintf(name,"%s/%s",m_zdcAnalysisConfigPath.c_str(),m_zdcTimeCalibFileName.c_str());
  ATH_MSG_INFO("Opening time calibration file " << name);
  TFile* fCalib = TFile::Open(name);
  std::array<std::array<TSpline*, 4>, 2> T0HGOffsetSplines;
  std::array<std::array<TSpline*, 4>, 2> T0LGOffsetSplines;
  TSpline3* spline;
  for (int iside=0;iside<2;iside++)
    {
      for(int imod=0;imod<4;imod++)
	{
	  sprintf(name,"ZDC_T0calib_run%d_s%d_m%d_HG",m_runNumber,iside,imod);
	  spline = (TSpline3*) fCalib->GetObjectChecked(name,"TSpline3");
	  T0HGOffsetSplines[iside][imod] = spline;
	  sprintf(name,"ZDC_T0calib_run%d_s%d_m%d_LG",m_runNumber,iside,imod);
	  spline = (TSpline3*) fCalib->GetObjectChecked(name,"TSpline3");
	  T0LGOffsetSplines[iside][imod] = spline;
	}
    }
  m_zdcDataAnalyzer->LoadT0Calibrations(T0HGOffsetSplines,T0LGOffsetSplines);
  fCalib->Close();
}

StatusCode ZdcAnalysisTool::reprocessZdc() 
{
  if (!m_init)
    {
      ATH_MSG_INFO("Tool not initialized!");
      return StatusCode::FAILURE;
    }
  m_eventReady = false;
  //std::cout << "Trying to retrieve " << m_zdcModuleContainerName << std::endl;
  m_zdcModules = 0;
  ATH_CHECK(evtStore()->retrieve(m_zdcModules,m_zdcModuleContainerName));
  m_eventReady = true;

  ATH_CHECK(recoZdcModules(*m_zdcModules));

  return StatusCode::SUCCESS;
}

  bool ZdcAnalysisTool::sigprocMaxFinder(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual)
  {
    size_t nsamp = adc.size();
    float presamp = adc.at(0);
    unsigned short max_adc = 0;
    int max_index = -1;
    for (size_t i = 0;i<nsamp;i++)
      {
	if (adc[i]>max_adc)
	  {
	    max_adc = adc[i];
	    max_index = i;
	  }
      }
    amp = max_adc - presamp;
    time = max_index*deltaT;
    qual = 1.;

    if(max_index==-1) 
      {
	qual=0.;
	return false;
      }

    return true;
  }

  bool ZdcAnalysisTool::sigprocSincInterp(const std::vector<unsigned short>& adc, float deltaT, float& amp, float& time, float& qual)
  {    
    size_t nsamp = adc.size();
    float presamp = adc.at(0);
    tf1SincInterp->SetParameter(0,deltaT);
    for (size_t i = 0;i<nsamp;i++)
      {
	tf1SincInterp->SetParameter(i+1,adc.at(i)-presamp);
      }
    amp = tf1SincInterp->GetMaximum();
    time = tf1SincInterp->GetMaximumX();
    qual = 1.;
    return true;
  }

  float ZdcAnalysisTool::getModuleSum(int side)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleSum(side);
  }

/*
  float ZdcAnalysisTool::getModuleAmplitude(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetPulseAnalyzer(side,mod)->GetAmplitude();
  }

  float ZdcAnalysisTool::getModuleFitAmplitude(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetPulseAnalyzer(side,mod)->GetFitAmplitude();
  }

  float ZdcAnalysisTool::getModuleFitT0(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetPulseAnalyzer(side,mod)->GetFitT0();
  }

  float ZdcAnalysisTool::getModuleTime(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleTime(side,mod);
  }

  float ZdcAnalysisTool::getModuleCalibAmplitude(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleCalibAmplitude(side,mod);
  }

  float ZdcAnalysisTool::getModuleCalibTime(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleCalibTime(side,mod);
  }

  float ZdcAnalysisTool::getModuleStatus(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleStatus(side,mod);
  }

  float ZdcAnalysisTool::getModuleChisq(int side, int mod)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleChisq(side,mod);
  }
*/

  float ZdcAnalysisTool::getCalibModuleSum(int side)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetCalibModuleSum(side);
  }

  float ZdcAnalysisTool::getAverageTime(int side)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetAverageTime(side);
  }

  bool ZdcAnalysisTool::sideFailed(int side)
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->SideFailed(side);
  }

  unsigned int ZdcAnalysisTool::getModuleMask()
  {
    if (!m_zdcDataAnalyzer) return 0;
    return m_zdcDataAnalyzer->GetModuleMask();
  }

  float ZdcAnalysisTool::getTriggerEfficiency(int side)
  {

    m_zdcTriggerEfficiency->UpdatelumiBlock(m_lumiBlock);
    float adcSum = getModuleSum(side);
    float eff = m_zdcTriggerEfficiency->GetEfficiency(side, adcSum);
    //if(eff<0) std::cout << "eff= " << eff << " --> Invalid lumiBlock input: m_lumiBlock = " << m_lumiBlock << std::endl; // looking for -1's
    return eff;
  }

} // namespace ZDC
