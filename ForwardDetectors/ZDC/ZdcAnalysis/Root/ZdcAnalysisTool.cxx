/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZdcAnalysisTool.h"
#include "TGraph.h"
#include "TEnv.h"
#include "TSystem.h"
#include "ZdcAnalysis/ZdcSincInterp.h"
#include "TFile.h"
#include <sstream>
#include <memory>
#include <xAODForward/ZdcModuleAuxContainer.h>
#include "PathResolver/PathResolver.h"

namespace ZDC
{

  ZdcAnalysisTool::ZdcAnalysisTool(const std::string& name) : asg::AsgTool(name), m_name(name),
							      m_writeAux(false),
							      m_runNumber(0), m_lumiBlock(0),
                                                              m_eventInfoKey("EventInfo"),
                                                              m_ZdcModuleWriteKey("ZdcSums"),
    m_splines{{{{0,0,0,0}},{{0,0,0,0}}}}, m_zdcTriggerEfficiency(0)
{

#ifndef XAOD_STANDALONE
   declareInterface<IZdcAnalysisTool>(this);
#endif

  declareProperty("EventInfoKey",          m_eventInfoKey,          "Location of the event info.");
  declareProperty("ZdcModuleWriteKey",     m_ZdcModuleWriteKey,     "Output location of ZDC reprocessed data");
  declareProperty("Configuration", m_configuration = "PbPb2015");
  declareProperty("FlipEMDelay",m_flipEMDelay=false);
  declareProperty("LowGainOnly",m_lowGainOnly=false);
  declareProperty("WriteAux",m_writeAux=true);
  declareProperty("AuxSuffix",m_auxSuffix="");

  // The following job properties enable/disable and affect the calibration of the ZDC energies
  //
  declareProperty("DoCalib",m_doCalib=true);
  declareProperty("ZdcAnalysisConfigPath",m_zdcAnalysisConfigPath="$ROOTCOREBIN/data/ZdcAnalysis","ZDC Analysis config file path");
  //declareProperty("ForceCalibRun",m_forceCalibRun=287931); // last run of Pb+Pb 2015
  declareProperty("ForceCalibRun",m_forceCalibRun=-1); // last run of Pb+Pb 2015
  declareProperty("ForceCalibLB",m_forceCalibLB=814); // last LB of Pb+Pb 2015
  
  // The following parameters are primarily used for the "default" configuration, but also may be
  //   use to modify/tailor other configurations
  //
  declareProperty("NumSampl", m_numSample = 7); 
  declareProperty("DeltaTSample", m_deltaTSample = 25); 
  declareProperty("Presample", m_presample = 0); 
  declareProperty("CombineDelay", m_combineDelay = false); 
  declareProperty("DelayDeltaT", m_delayDeltaT = -12.5); 

  declareProperty("PeakSample", m_peakSample = 2);
  declareProperty("Peak2ndDerivThresh", m_Peak2ndDerivThresh = 10);

  declareProperty("T0", m_t0 = 50);
  declareProperty("Tau1", m_tau1 = 5);
  declareProperty("Tau2", m_tau2 = 25);
  declareProperty("FixTau1", m_fixTau1 = false);
  declareProperty("FixTau2", m_fixTau2 = false);

  declareProperty("DeltaTCut", m_deltaTCut = 25);
  declareProperty("ChisqRatioCut", m_ChisqRatioCut = 10);

  //ATH_MSG_INFO("Creating ZdcAnalysisoTool named " << m_name);
  //ATH_MSG_INFO("ZDC config file path " << m_zdcAnalysisConfigPath);

}

ZdcAnalysisTool::~ZdcAnalysisTool()
{
  ATH_MSG_DEBUG("Deleting ZdcAnalysisTool named " << m_name);
  //SafeDelete( m_zdcDataAnalyzer );
}

void ZdcAnalysisTool::initializeTriggerEffs(unsigned int runNumber)
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

  //file->Print();

  std::stringstream Aalpha_name;
  Aalpha_name<<"A_alpha_"<<runNumber;
  TSpline3* par_A_alpha = (TSpline3*)file->GetObjectChecked(Aalpha_name.str().c_str(),"TSpline3");

  if (!par_A_alpha)
    {
      ATH_MSG_ERROR("No trigger efficiencies for run number " << runNumber);
      m_doCalib = false;
    }

  std::stringstream Abeta_name;
  Abeta_name<<"A_beta_"<<runNumber;
  TSpline3* par_A_beta = (TSpline3*)file->GetObjectChecked(Abeta_name.str().c_str(),"TSpline3");
  std::stringstream Atheta_name;
  Atheta_name<<"A_theta_"<<runNumber;
  TSpline3* par_A_theta = (TSpline3*)file->GetObjectChecked(Atheta_name.str().c_str(),"TSpline3");
  
  std::stringstream Calpha_name;
  Calpha_name<<"C_alpha_"<<runNumber;
  TSpline3* par_C_alpha = (TSpline3*)file->GetObjectChecked(Calpha_name.str().c_str(),"TSpline3");
  std::stringstream Cbeta_name;
  Cbeta_name<<"C_beta_"<<runNumber;
  TSpline3* par_C_beta = (TSpline3*)file->GetObjectChecked(Cbeta_name.str().c_str(),"TSpline3");
  std::stringstream Ctheta_name;
  Ctheta_name<<"C_theta_"<<runNumber;
  TSpline3* par_C_theta = (TSpline3*)file->GetObjectChecked(Ctheta_name.str().c_str(),"TSpline3");
  
  std::stringstream Err_Aalpha_name;
  Err_Aalpha_name<<"A_alpha_error_"<<runNumber;
  TSpline3* parErr_A_alpha = (TSpline3*)file->GetObjectChecked(Err_Aalpha_name.str().c_str(),"TSpline3");
  std::stringstream Err_Abeta_name;
  Err_Abeta_name<<"A_beta_error_"<<runNumber;
  TSpline3* parErr_A_beta = (TSpline3*)file->GetObjectChecked(Err_Abeta_name.str().c_str(),"TSpline3");
  std::stringstream Err_Atheta_name;
  Err_Atheta_name<<"A_theta_error_"<<runNumber;
  TSpline3* parErr_A_theta = (TSpline3*)file->GetObjectChecked(Err_Atheta_name.str().c_str(),"TSpline3");
  
  std::stringstream Err_Calpha_name;
  Err_Calpha_name<<"C_alpha_error_"<<runNumber;
  TSpline3* parErr_C_alpha = (TSpline3*)file->GetObjectChecked(Err_Calpha_name.str().c_str(),"TSpline3");
  std::stringstream Err_Cbeta_name;
  Err_Cbeta_name<<"C_beta_error_"<<runNumber;
  TSpline3* parErr_C_beta = (TSpline3*)file->GetObjectChecked(Err_Cbeta_name.str().c_str(),"TSpline3");
  std::stringstream Err_Ctheta_name;
  Err_Ctheta_name<<"C_theta_error_"<<runNumber;
  TSpline3* parErr_C_theta = (TSpline3*)file->GetObjectChecked(Err_Ctheta_name.str().c_str(),"TSpline3");
  
  
  std::stringstream Cov_A_alpha_beta_name;
  Cov_A_alpha_beta_name<<"cov_A_alpha_beta_"<<runNumber;
  TSpline3* cov_A_alpha_beta = (TSpline3*)file->GetObjectChecked(Cov_A_alpha_beta_name.str().c_str(),"TSpline3");
  std::stringstream Cov_A_alpha_theta_name;
  Cov_A_alpha_theta_name<<"cov_A_alpha_theta_"<<runNumber;
  TSpline3* cov_A_alpha_theta = (TSpline3*)file->GetObjectChecked(Cov_A_alpha_theta_name.str().c_str(),"TSpline3");
  std::stringstream Cov_A_beta_theta_name;
  Cov_A_beta_theta_name<<"cov_A_beta_theta_"<<runNumber;
  TSpline3* cov_A_beta_theta = (TSpline3*)file->GetObjectChecked(Cov_A_beta_theta_name.str().c_str(),"TSpline3");
  
  std::stringstream Cov_C_alpha_beta_name;
  Cov_C_alpha_beta_name<<"cov_C_alpha_beta_"<<runNumber;
  TSpline3* cov_C_alpha_beta = (TSpline3*)file->GetObjectChecked(Cov_C_alpha_beta_name.str().c_str(),"TSpline3");
  std::stringstream Cov_C_alpha_theta_name;
  Cov_C_alpha_theta_name<<"cov_C_alpha_theta_"<<runNumber;
  TSpline3* cov_C_alpha_theta = (TSpline3*)file->GetObjectChecked(Cov_C_alpha_theta_name.str().c_str(),"TSpline3");
  std::stringstream Cov_C_beta_theta_name;
  Cov_C_beta_theta_name<<"cov_C_beta_theta_"<<runNumber;
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

ZDCDataAnalyzer* ZdcAnalysisTool::initializeDefault()
{
  // We rely completely on the default parameters specified in the job properties to control:
  //   # samples
  //   frequency (more precisely, time/sample)
  //   which sample to use as the pre-sample
  //   where to expact the maxim of the peak (min 2nd derivative)
  //   thresholds on the 2nd derivative for valid pulses 
  //   whether to fix the tau values in the pulse fitting
  //   the default tau values
  //   the nominal T0
  //   delta T and chisq/amp cuts
  //
  //   For now, we continue to use hard-coded values for the maximum and minimum ADC values
  //   For now we also use the FermiExp pulse model.

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1, tau2, peak2ndDerivMinSamples, t0;
  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG;
  ZDCDataAnalyzer::ZDCModuleFloatArray deltaT0CutLow, deltaT0CutHigh, chisqDivAmpCut;
  ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;
    
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      fixTau1Arr[side][module] = m_fixTau1;
      fixTau2Arr[side][module] = m_fixTau2;
      tau1[side][module] = m_tau1;
      tau2[side][module] = m_tau2;

      peak2ndDerivMinSamples[side][module] = m_peakSample;
      peak2ndDerivMinThresholdsHG[side][module] = -m_Peak2ndDerivThresh;
      peak2ndDerivMinThresholdsLG[side][module] = -m_Peak2ndDerivThresh/2;

      t0[side][module] = m_t0;
      deltaT0CutLow[side][module] = -m_deltaTCut;
      deltaT0CutHigh[side][module] = m_deltaTCut;
      chisqDivAmpCut[side][module] = m_ChisqRatioCut;
    }
  }

  ATH_MSG_INFO( "Default: delta t cut, value low = " << deltaT0CutLow[0][0] << ", high = " << deltaT0CutHigh[0][0] );

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {{{{800, 800, 800, 800}}, {{800, 800, 800, 800}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {{{{1020, 1020, 1020, 1020}}, {{1020, 1020, 1020, 1020}}}};

  //  Construct the data analyzer
  //
  ZDCDataAnalyzer* zdcDataAnalyzer = new ZDCDataAnalyzer(m_numSample, m_deltaTSample, m_presample, "FermiExp", peak2ndDerivMinSamples,
							 peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG, m_lowGainOnly);

  zdcDataAnalyzer->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  zdcDataAnalyzer->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0, t0);
  zdcDataAnalyzer->SetCutValues(chisqDivAmpCut, chisqDivAmpCut, deltaT0CutLow, deltaT0CutHigh, deltaT0CutLow, deltaT0CutHigh);

  if (m_combineDelay) {
    ZDCDataAnalyzer::ZDCModuleFloatArray defaultPedestalShifts = {{{{0, 0, 0, 0}}, {{0, 0, 0, 0}}}};

    zdcDataAnalyzer->EnableDelayed(m_delayDeltaT, defaultPedestalShifts);
  }

  return zdcDataAnalyzer;
}

ZDCDataAnalyzer* ZdcAnalysisTool::initializepPb2016()
{
  //
  //   For now, we continue to use hard-coded values for the maximum and minimum ADC values
  //   For now we also use the FermiExp pulse model.

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1Arr, tau2Arr, peak2ndDerivMinSamples, t0;
  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG;
  ZDCDataAnalyzer::ZDCModuleFloatArray deltaT0CutLow, deltaT0CutHigh, chisqDivAmpCut;
  ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;
    
  //  For now we allow the tau values to be controlled by the job properties until they are better determined
  //
  const int peakSample = 5;
  const float peak2ndDerivThreshHG = -12;
  const float peak2ndDerivThreshLG = -10;
  const float tau1 = 4.5;
  const float tau2 = 22.;


  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      fixTau1Arr[side][module] = m_fixTau1;
      fixTau2Arr[side][module] = m_fixTau2;
      tau1Arr[side][module] = tau1;
      tau2Arr[side][module] = tau2;

      peak2ndDerivMinSamples[side][module] = peakSample;
      peak2ndDerivMinThresholdsHG[side][module] = peak2ndDerivThreshHG;
      peak2ndDerivMinThresholdsLG[side][module] = peak2ndDerivThreshLG;

      t0[side][module] = m_t0;
      deltaT0CutLow[side][module] = -m_deltaTCut;
      deltaT0CutHigh[side][module] = m_deltaTCut;
      chisqDivAmpCut[side][module] = m_ChisqRatioCut;
    }
  }

  //  ATH_MSG_INFO( "Default: delta t cut, value low = " << deltaT0CutLow[0][0] << ", high = " << deltaT0CutHigh[0][0] );

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {{{{800, 800, 800, 800}}, {{800, 800, 800, 800}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {{{{1020, 1020, 1020, 1020}}, {{1020, 1020, 1020, 1020}}}};

  //  Construct the data analyzer
  //
  //  We adopt hard-coded values for the number of samples and the frequency which we kept fixed for all physics data
  //
  ZDCDataAnalyzer* zdcDataAnalyzer = new ZDCDataAnalyzer(7, 25, 1, "FermiExp", peak2ndDerivMinSamples,
							 peak2ndDerivMinThresholdsHG, peak2ndDerivMinThresholdsLG, m_lowGainOnly);

  // Open up tolerances on the position of the peak for now
  //
  zdcDataAnalyzer->SetPeak2ndDerivMinTolerances(2);

  // We alwyas disable the 12EM (sideC) module which was not present (LHCf)
  //
  zdcDataAnalyzer->DisableModule(0,0);

  zdcDataAnalyzer->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  zdcDataAnalyzer->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1Arr, tau2Arr, t0, t0);
  zdcDataAnalyzer->SetCutValues(chisqDivAmpCut, chisqDivAmpCut, deltaT0CutLow, deltaT0CutHigh, deltaT0CutLow, deltaT0CutHigh);

  // We allow the combineDelay to be controlled by the properties
  //
  //  if (m_combineDelay) {
    m_combineDelay = true;
    ZDCDataAnalyzer::ZDCModuleFloatArray defaultPedestalShifts = {{{{0, 0, 0, 0}}, {{0, 0, 0, 0}}}};

    zdcDataAnalyzer->EnableDelayed(-12.5, defaultPedestalShifts);
    //  }

  return zdcDataAnalyzer;
}

void ZdcAnalysisTool::initialize40MHz()
{
  // We have a complete configuration and so we override all of the default parameters
  //

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1 = {{{{4.2, 3.8, 5.2, 5.0}},
                                                {{5.0, 3.7, 3.5, 3.5}}}};

  // identical to 80 MHz -- is this right
  ZDCDataAnalyzer::ZDCModuleFloatArray tau2 = {{{{20.0, 20.4, 18.9, 20.8}},
                                                {{19.1, 21.9, 22.6, 23.4}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinSamples = {{{{1, 1, 2, 1}},
                                                                  {{1, 1, 1, 1}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsHG = {{{{-8, -8, -8, -8}},
                                                                       {{-8, -8, -8, -8}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray peak2ndDerivMinThresholdsLG = {{{{-4, -4, -4, -4}},
                                                                       {{-4, -4, -4, -4}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {{{{800, 800, 800, 800}}, {{800, 800, 800, 800}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {{{{1020, 1020, 1020, 1020}}, {{1020, 1020, 1020, 1020}}}};

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

  ZDCDataAnalyzer::ZDCModuleFloatArray t0HG = {{{{53.942, 49.887, 59.633, 46.497}},
                                                {{46.314, 42.267, 50.327, 41.605}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray t0LG = {{{{51.771, 47.936, 57.438, 44.191}},
                                                {{44.295, 41.755, 48.081, 40.175}}}};


  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG = {{{{-6, -5, -5, -5}}, {{-5, -5, -5, -5}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {{{{8, 8, 8, 11}}, {{8, 10, 8, 12}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG = {{{{-6, -5, -5, -5}}, {{-5, -5, -5, -5}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {{{{8, 8, 8, 11}}, {{8, 10, 8, 12}}}};

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
  // We have a complete configuration and so we override all of the default parameters
  //

  m_peak2ndDerivMinSamples = {{{{3, 2, 3, 2}},
                               {{2, 2, 2, 2}}}};

  m_peak2ndDerivMinThresholdsHG = {{{{-8, -8, -8, -8}},
                                    {{-8, -8, -8, -8}}}};

  m_peak2ndDerivMinThresholdsLG = {{{{-4, -4, -4, -4}},
                                    {{-4, -4, -4, -4}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {{{{800, 800, 800, 800}}, {{800, 800, 800, 800}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {{{{950, 950, 950, 950}}, {{950, 950, 950, 950}}}};

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

  ZDCDataAnalyzer::ZDCModuleFloatArray tau1 = {{{{3.9, 3.4, 4.1, 4.2}},
                                                {{4.2, 3.6, 3.3, 3.4}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray tau2 = {{{{20.0, 20.4, 18.9, 20.8}},
                                                {{19.1, 21.9, 22.6, 23.4}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray t0HG = {{{{44.24, 40.35, 49.3, 36.0}},
                                                {{36.0, 31.1, 40.75, 30.5}}}};
					       
  ZDCDataAnalyzer::ZDCModuleFloatArray t0LG = {{{{42.65, 38.5, 47.4, 34}},
                                                {{33.7, 29.9, 39.0, 29.3}}}};

  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {{{{10, 10, 10, 10}}, {{10, 10, 10, 10}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG = {{{{-6, -5, -5, -5}}, {{-5, -5, -5, -5}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {{{{8, 8, 8, 11}}, {{8, 10, 8, 12}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG = {{{{-6, -5, -5, -5}}, {{-5, -5, -5, -5}}}};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {{{{8, 8, 8, 11}}, {{8, 10, 8, 12}}}};

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

  m_zdcDataAnalyzer_80MHz = new ZDCDataAnalyzer(7 , 12.5, 0, "FermiExp",m_peak2ndDerivMinSamples,
						m_peak2ndDerivMinThresholdsHG, m_peak2ndDerivMinThresholdsLG,m_lowGainOnly);

  m_zdcDataAnalyzer_80MHz->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  m_zdcDataAnalyzer_80MHz->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0HG, t0LG);
  m_zdcDataAnalyzer_80MHz->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);
  m_zdcDataAnalyzer_80MHz->SetTimingCorrParams(slewingParamsHG, slewingParamsLG);
  m_zdcDataAnalyzer_80MHz->SetNonlinCorrParams(moduleHGNonLinCorr);
}

StatusCode ZdcAnalysisTool::initializeTool()
{
  m_tf1SincInterp = new TF1("SincInterp",ZDC::SincInterp,-5.,160.,8);
  m_tf1SincInterp->SetNpx(300);

  // Set up calibrations
  //
  std::string filename = PathResolverFindCalibFile( "ZdcAnalysis/ZdcAnalysisConfig.conf" );
  TEnv env(filename.c_str());

  m_zdcEnergyCalibFileName = std::string(env.GetValue("ZdcEnergyCalibFileName","ZdcCalibrations_v1.root"));
  ATH_MSG_INFO("ZDC energy calibration filename " << m_zdcEnergyCalibFileName);
  m_zdcTimeCalibFileName = std::string(env.GetValue("ZdcTimeCalibFileName","ZdcTimeCalibrations_v1.root"));
  ATH_MSG_INFO("ZDC time calibration filename " << m_zdcTimeCalibFileName);
  m_zdcTriggerEffParamsFileName = std::string(env.GetValue("ZdcTriggerEffFileName", "ZdcTriggerEffParameters_v4.root"));
  ATH_MSG_INFO("ZDC trigger efficiencies filename " << m_zdcTriggerEffParamsFileName);


  if (m_forceCalibRun>-1) {
    ATH_MSG_INFO("CAREFUL: forcing calibration run/LB =" << m_forceCalibRun << "/" << m_forceCalibLB);
    
    if (m_forceCalibLB < 0) {
      ATH_MSG_ERROR("Invalid settings: Forced run > 0 but lumi block < 0");
      return StatusCode::FAILURE;
    }
  }

  // Use configuration to direct initialization
  //
  if (m_configuration == "default") {
    m_zdcDataAnalyzer = initializeDefault();
  }
  else if (m_configuration == "PbPb2015") {
    initialize80MHz();
    initialize40MHz();
    
    m_zdcDataAnalyzer = m_zdcDataAnalyzer_80MHz; // default
  }
  else if (m_configuration == "pPb2016") {
    m_zdcDataAnalyzer = initializepPb2016();
  }
  else {
    ATH_MSG_ERROR("Unknown configuration: "  << m_configuration);
    return StatusCode::FAILURE;
  }

  // If an aux suffix is provided, prepend it with "_" so we don't have to do so at each use
  //
  if (m_writeAux && m_auxSuffix != "") {
    m_auxSuffix = "_" + m_auxSuffix;
  }

  ATH_MSG_INFO("Configuration: "<<m_configuration);
  ATH_MSG_INFO("FlipEMDelay: "<<m_flipEMDelay);
  ATH_MSG_INFO("LowGainOnly: "<<m_lowGainOnly);

  ATH_MSG_INFO("Using Combined delayed and undelayed samples: "<< m_combineDelay);

  ATH_MSG_INFO("WriteAux: "<<m_writeAux);
  ATH_MSG_INFO("AuxSuffix: "<<m_auxSuffix);
  ATH_MSG_INFO("DoCalib: "<<m_doCalib);
  ATH_MSG_INFO("ForceCalibRun: "<<m_forceCalibRun);
  ATH_MSG_INFO("ForceCalibLB: "<<m_forceCalibLB);
  ATH_MSG_INFO("NumSampl: "<<m_numSample); 
  ATH_MSG_INFO("DeltaTSample: "<<m_deltaTSample); 
  ATH_MSG_INFO("Presample: "<<m_presample); 
  ATH_MSG_INFO("PeakSample: "<<m_peakSample);
  ATH_MSG_INFO("Peak2ndDerivThresh: "<<m_Peak2ndDerivThresh);

  if (m_combineDelay)  ATH_MSG_INFO("DelayDeltaT: "<< m_delayDeltaT);

  ATH_MSG_INFO("T0: "<<m_t0);
  ATH_MSG_INFO("Tau1: "<<m_tau1);
  ATH_MSG_INFO("Tau2: "<<m_tau2);
  ATH_MSG_INFO("FixTau1: "<<m_fixTau1);
  ATH_MSG_INFO("FixTau2: "<<m_fixTau2);
  ATH_MSG_INFO("DeltaTCut: "<<m_deltaTCut);
  ATH_MSG_INFO("ChisqRatioCut: "<<m_ChisqRatioCut);

  ATH_CHECK( m_eventInfoKey.initialize());
  ATH_CHECK( m_ZdcModuleWriteKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode ZdcAnalysisTool::configureNewRun(unsigned int runNumber)
{
  ATH_MSG_INFO("Setting up new run " << runNumber);

  // We do nothing for the default configuration
  //
  if (m_configuration != "default") {
    if (m_configuration == "PbPb2015") {
      //
      // Two periods, 40 MHz and 80 MHz readout
      //
      if (runNumber < 287222) m_zdcDataAnalyzer = m_zdcDataAnalyzer_40MHz;
      else m_zdcDataAnalyzer = m_zdcDataAnalyzer_80MHz;
    }
  }

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

  float deltaT = m_deltaTSample;

  sigprocMaxFinder(*adc0,deltaT,amp,time,qual);
  if (m_writeAux) {
    module.auxdecor<float>("amplitudeG0_mf" + m_auxSuffix)=amp;
    module.auxdecor<float>("timeG0_mf" + m_auxSuffix)=time;
  }

  sigprocMaxFinder(*adc1,deltaT,amp,time,qual);
  module.auxdecor<float>("amplitudeG1_mf" + m_auxSuffix)=amp;
  module.auxdecor<float>("timeG1_mf" + m_auxSuffix)=time;

  if (module.type()==0)
    {
      sigprocSincInterp(*adc0,deltaT,amp,time,qual);
      module.auxdecor<float>("amplitudeG0_si" + m_auxSuffix)=amp;
      module.auxdecor<float>("timeG0_si" + m_auxSuffix)=time;
    }

  return StatusCode::SUCCESS;
}

StatusCode ZdcAnalysisTool::recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) 
{
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  if (!eventInfo.isValid()) return StatusCode::FAILURE;

  // check for new run number, if new, possibly update configuration and/or calibrations
  //
  unsigned int thisRunNumber = eventInfo->runNumber();
  if (thisRunNumber != m_runNumber) {
    ATH_MSG_DEBUG("ZDC analysis tool will be configured for run " << thisRunNumber);
      
    ATH_CHECK(configureNewRun(thisRunNumber)); // ALWAYS check methods that return StatusCode

    ATH_MSG_DEBUG("Setting up calibrations");

    if (m_doCalib) {
      // 
      // Check for calibration override
      //
      unsigned int calibRunNumber = thisRunNumber;
      if (m_forceCalibRun > -1) calibRunNumber = m_forceCalibRun;

      setEnergyCalibrations(calibRunNumber);
      initializeTriggerEffs(calibRunNumber); // if energy calibrations fail to load, then so will trigger efficiencies
      //setTimeCalibrations();      
    }  

    m_runNumber = thisRunNumber;
  }

  m_lumiBlock = eventInfo->lumiBlock();

  unsigned int calibLumiBlock = m_lumiBlock;
  if (m_doCalib) {
    if (m_forceCalibRun > 0) calibLumiBlock = m_forceCalibLB;
  }

  ATH_MSG_DEBUG("Starting event processing");
  m_zdcDataAnalyzer->StartEvent(calibLumiBlock);

  const std::vector<unsigned short>* adcUndelayLG = 0;
  const std::vector<unsigned short>* adcUndelayHG = 0;

  const std::vector<unsigned short>* adcDelayLG = 0;
  const std::vector<unsigned short>* adcDelayHG = 0;

  ATH_MSG_DEBUG("Processing modules");
  for (const auto zdcModule : moduleContainer)
    {
      
      if (zdcModule->type()!=0) continue;

      if (zdcModule->zdcModule()==0 && m_flipEMDelay) // flip delay/non-delay for EM big tube
	{
	  adcUndelayLG = &(*(zdcModule->TTg0d1Link()))->adc();
	  adcUndelayHG = &(*(zdcModule->TTg1d1Link()))->adc();

	  adcDelayLG = &(*(zdcModule->TTg0d0Link()))->adc();
	  adcDelayHG = &(*(zdcModule->TTg1d0Link()))->adc();
	}
      else
	{
	  adcUndelayLG = &(*(zdcModule->TTg0d0Link()))->adc();
	  adcUndelayHG = &(*(zdcModule->TTg1d0Link()))->adc();

	  adcDelayLG = &(*(zdcModule->TTg0d1Link()))->adc();
	  adcDelayHG = &(*(zdcModule->TTg1d1Link()))->adc();
	}
      
      static std::vector<float> HGUndelADCSamples(m_numSample);
      static std::vector<float> LGUndelADCSamples(m_numSample);

      std::copy(adcUndelayLG->begin(), adcUndelayLG->end(), LGUndelADCSamples.begin());
      std::copy(adcUndelayHG->begin(), adcUndelayHG->end(), HGUndelADCSamples.begin());
      
      int side = (zdcModule->side()==-1) ? 0 : 1 ;

      //std::cout << "LG: ";for (int i = 0;i<7;i++){std::cout<<LGADCSamples.at(i)<< " ";};std::cout<<std::endl;
      //std::cout << "HG: ";for (int i = 0;i<7;i++){std::cout<<HGADCSamples.at(i)<< " ";};std::cout<<std::endl;

      if (!m_combineDelay) {
	m_zdcDataAnalyzer->LoadAndAnalyzeData(side,zdcModule->zdcModule(), HGUndelADCSamples, LGUndelADCSamples);
      }
      else {
	std::vector<float> HGDelayADCSamples(m_numSample);
	std::vector<float> LGDelayADCSamples(m_numSample);

	std::copy(adcDelayLG->begin(),adcDelayLG->end(), LGDelayADCSamples.begin());
	std::copy(adcDelayHG->begin(),adcDelayHG->end(), HGDelayADCSamples.begin());

	// If the delayed channels actually come earlier (as in the pPb in 2016), we invert the meaning of delayed and undelayed
	//   see the initialization sections for similar inversion on the sign of the pedestal difference
	//
	if (m_delayDeltaT > 0) {
	  m_zdcDataAnalyzer->LoadAndAnalyzeData(side,zdcModule->zdcModule(), 
						HGUndelADCSamples, LGUndelADCSamples,
						HGDelayADCSamples, LGDelayADCSamples);
	}
	else {
	  m_zdcDataAnalyzer->LoadAndAnalyzeData(side,zdcModule->zdcModule(), 
						HGDelayADCSamples, LGDelayADCSamples,
						HGUndelADCSamples, LGUndelADCSamples);
	}
      }
    }

  ATH_MSG_DEBUG("Finishing event processing");
  
  m_zdcDataAnalyzer->FinishEvent();

  ATH_MSG_DEBUG("Adding variables");
  
  for (const auto zdcModule : moduleContainer)
    {

      if (zdcModule->type()!=0) continue;

      int side = (zdcModule->side()==-1) ? 0 : 1 ;
      int mod = zdcModule->zdcModule();

      if (m_writeAux) {
	if (m_doCalib) {
	  float calibEnergy = m_zdcDataAnalyzer->GetModuleCalibAmplitude(side, mod);
	  zdcModule->auxdecor<float>("CalibEnergy" + m_auxSuffix) = calibEnergy;
	  zdcModule->auxdecor<float>("CalibTime" + m_auxSuffix) = m_zdcDataAnalyzer->GetModuleCalibTime(side, mod);
	}
	else
	  {
	    zdcModule->auxdecor<float>("CalibEnergy" + m_auxSuffix) = -1000;
	    zdcModule->auxdecor<float>("CalibTime" + m_auxSuffix) = -1000;
	  }

	zdcModule->auxdecor<unsigned int>("Status" + m_auxSuffix) = m_zdcDataAnalyzer->GetModuleStatus(side, mod);
	zdcModule->auxdecor<float>("Amplitude" + m_auxSuffix) = m_zdcDataAnalyzer->GetModuleAmplitude(side, mod);
	zdcModule->auxdecor<float>("Time" + m_auxSuffix) = m_zdcDataAnalyzer->GetModuleTime(side, mod);
	
	const ZDCPulseAnalyzer* pulseAna_p = m_zdcDataAnalyzer->GetPulseAnalyzer(side, mod);
	zdcModule->auxdecor<float>("Chisq" + m_auxSuffix) = pulseAna_p->GetChisq();      
	zdcModule->auxdecor<float>("FitAmp" + m_auxSuffix) = pulseAna_p->GetFitAmplitude();
	zdcModule->auxdecor<float>("FitAmpError" + m_auxSuffix) = pulseAna_p->GetAmpError();
	zdcModule->auxdecor<float>("FitT0" + m_auxSuffix) = pulseAna_p->GetFitT0();
	zdcModule->auxdecor<float>("BkgdMaxFraction" + m_auxSuffix) = pulseAna_p->GetBkgdMaxFraction();
	zdcModule->auxdecor<float>("PreSampleAmp" + m_auxSuffix) = pulseAna_p->GetPreSampleAmp();
	//zdcModule->auxdecor<float>("Presample" + m_auxSuffix) = pulseAna_p->GetPresample();
      }
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

  ATH_CHECK( SG::WriteHandle<xAOD::ZdcModuleContainer>(m_ZdcModuleWriteKey).record( std::move(newModuleContainer),
                                                                                    std::move(newModuleAuxContainer)));
  return StatusCode::SUCCESS;
}

void ZdcAnalysisTool::setEnergyCalibrations(unsigned int runNumber)
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
	  //std::cout << "m_splines[iside][imod] = " << m_splines[iside][imod] << std::endl;
	  SafeDelete( m_splines[iside][imod] );

	  sprintf(name,"ZDC_Gcalib_run%d_s%d_m%d",runNumber,iside,imod);
	  ATH_MSG_INFO("Searching for graph " << name);
	  TGraph* g = (TGraph*) fCalib->GetObjectChecked(name,"TGraph");
	  if (!g && m_doCalib)
	    {
	      ATH_MSG_ERROR("No calibrations for run " << runNumber);
	      m_doCalib = false;
	    }

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

void ZdcAnalysisTool::setTimeCalibrations(unsigned int runNumber)
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
	  sprintf(name,"ZDC_T0calib_run%d_s%d_m%d_HG",runNumber,iside,imod);
	  spline = (TSpline3*) fCalib->GetObjectChecked(name,"TSpline3");
	  T0HGOffsetSplines[iside][imod] = spline;
	  sprintf(name,"ZDC_T0calib_run%d_s%d_m%d_LG",runNumber,iside,imod);
	  spline = (TSpline3*) fCalib->GetObjectChecked(name,"TSpline3");
	  T0LGOffsetSplines[iside][imod] = spline;
	}
    }
  m_zdcDataAnalyzer->LoadT0Calibrations(T0HGOffsetSplines,T0LGOffsetSplines);
  fCalib->Close();
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
    m_tf1SincInterp->SetParameter(0,deltaT);
    for (size_t i = 0;i<nsamp;i++)
      {
	m_tf1SincInterp->SetParameter(i+1,adc.at(i)-presamp);
      }
    amp = m_tf1SincInterp->GetMaximum();
    time = m_tf1SincInterp->GetMaximumX();
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
    if (!m_doCalib) return -1;

    m_zdcTriggerEfficiency->UpdatelumiBlock(m_lumiBlock);
    float adcSum = getModuleSum(side);
    float eff = m_zdcTriggerEfficiency->GetEfficiency(side, adcSum);
    //if(eff<0) std::cout << "eff= " << eff << " --> Invalid lumiBlock input: m_lumiBlock = " << m_lumiBlock << std::endl; // looking for -1's
    return eff;
  }

} // namespace ZDC
