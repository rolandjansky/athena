/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <iostream>
#include <sstream>

void SetT0Values(double LGt0Array[2][4], double HGt0Array[2][4])
{

  HGt0Array[0][0] = 44.0 + 0.24;
  HGt0Array[0][1] = 40.7 - 0.34;
  HGt0Array[0][2] = 48.2 + 0.13;
  HGt0Array[0][3] = 35.5 + 0.53;

  HGt0Array[1][0] = 35.2 + 0.04;
  HGt0Array[1][1] = 31.3 - 0.22;
  HGt0Array[1][2] = 40.9 - 0.15;
  HGt0Array[1][3] = 30.7 - 0.17;

  LGt0Array[0][0] = 42.5 + 0.15; 
  LGt0Array[0][1] = 39.0 - 0.52 + 0.027;
  LGt0Array[0][2] = 46.3 - 0.06 - 0.06;
  LGt0Array[0][3] = 33.7 - 0.37 - 0.10;

  LGt0Array[1][0] = 33.4 + 0.33 - 0.66;
  LGt0Array[1][1] = 30.1 - 0.21 - 0.04;
  LGt0Array[1][2] = 39.5 - 0.37 - 0.05;
  LGt0Array[1][3] = 29.6 - 0.28 - 0.058;

} 

void LoadCalibrations(ZDCTreeAnalysis* ana, std::string filename, int runNumber, bool eCalib = true, bool t0Calib = true)
{
  TFile* file = TFile::Open(filename.c_str());
  if (!file->IsOpen()) {
    std::cout << "Error opening calibration file, quitting" << std::endl;
  }

  std::ostringstream runString;
  runString << "run" << runNumber;

  if (eCalib) {

    std::string calibNamePrefix = "ZDC_Ecalib_" + runString.str();

    std::array<std::array<TSpline*, 4>, 2>  ECalibSplinePtrs;
  
    for (size_t side : {0, 1}) {
      for (size_t module : {0, 1, 2, 3}) {
	std::ostringstream sideModStr;
	sideModStr << "_s" << side << "_m" << module;

	TSpline3* spline_p = (TSpline3*) file->GetObjectChecked((calibNamePrefix + sideModStr.str()).c_str(), "TSpline3");

	ECalibSplinePtrs[side][module] = spline_p;
      }
    }

    ana->LoadEnergyCalibrations(ECalibSplinePtrs);
  }

  if (t0Calib) {
    std::string calibNamePrefix = "ZDC_T0calib_" + runString.str();

    std::array<std::array<TSpline*, 4>, 2> t0CalibSplinePtrsHG, t0CalibSplinePtrsLG;

    for (size_t side : {0, 1}) {
      for (size_t module : {0, 1, 2, 3}) {
	std::ostringstream sideModStr;
	sideModStr << "_s" << side << "_m" << module;
	
	std::string HGName = calibNamePrefix + "_HG" + sideModStr.str();
	std::string LGName = calibNamePrefix + "_LG" + sideModStr.str();

	TSpline3* HGSpline_p = (TSpline3*) file->GetObjectChecked(HGName.c_str(), "TSpline3");
	TSpline3* LGSpline_p = (TSpline3*) file->GetObjectChecked(LGName.c_str(), "TSpline3");

	if (!HGSpline_p || !LGSpline_p) {
	  std::cout << "Error reading t0 calibrations for side, mod = " << side << ", " << module << " -- quitting" << std::endl;
	  return;
	}
	
	t0CalibSplinePtrsHG[side][module] = HGSpline_p;
	t0CalibSplinePtrsLG[side][module] = LGSpline_p;
      }
    }

    ana->LoadT0Calibrations(t0CalibSplinePtrsHG, t0CalibSplinePtrsLG);
  }

  file->Close();
}

ZDCTreeAnalysis* InitZDCAnalysis(const std::string & inputFile, const std::string & calibFile, bool fixTau1, bool fixTau2,
				 bool eCalib = true, bool t0Calib = true, bool doSlew = false)
{

  ZDCTreeAnalysis* ana = new ZDCTreeAnalysis(inputFile, 7, 12.5, 0);

  ana->SetDebugLevel(0);

  //  ADC over/underflow thresholds
  //
  ZDCDataAnalyzer::ZDCModuleFloatArray HGOverFlowADC = {800, 800, 800, 800, 800, 800, 800, 800};
  ZDCDataAnalyzer::ZDCModuleFloatArray HGUnderFlowADC = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray LGOverFlowADC = {950, 950, 950, 950, 950, 950, 950, 950};

  ana->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);

  // Set Tau and nominal timing offsets
  //
  ZDCDataAnalyzer::ZDCModuleBoolArray fixTau1Arr, fixTau2Arr;

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


  ZDCDataAnalyzer::ZDCModuleFloatArray t0HG = {44.0 + 0.24, 40.7 - 0.34, 48.2 + 0.13, 35.5 + 0.53,            
					       35.2 + 0.04, 31.3 - 0.22, 40.9 - 0.15, 30.7 - 0.17};

  ZDCDataAnalyzer::ZDCModuleFloatArray t0LG = {42.5 + 0.15, 39.0 - 0.52 + 0.027, 46.3 - 0.06 - 0.06, 33.7 - 0.37 - 0.10,                    
					       33.4 + 0.33 - 0.66, 30.1 - 0.21 - 0.04, 39.5 - 0.37 - 0.05, 29.6 - 0.28 - 0.058};


  ana->SetTauT0Values(fixTau1Arr, fixTau2Arr, tau1, tau2, t0HG, t0LG);

  int runNumber = ana->GetRunNumber();

  if (doSlew) {
  //   float HGSlewCoeff[2][4][3] = {-6.5e-2, 2.85e-2, -2.83e-3,
  // 				  -5.5e-2, 5.13e-2, 5.6e-3,
  // 				  -1.45e-3, 9.3e-2, 3.9e-3,
  // 				  -2.36e-2, 8.3e-2, 1.1e-3,
  // 				  -6.5e-2, 4.84e-2, -3.7e-3,
  // 				  1.34e-2, 6.57e-2, 5.37e-3,
  // 				  -5.37e-2, 3.49e-2, 3.8e-3,
  // 				  -3.3e-2,3.9e-2,2.2e-3};   

  //   float LGSlewCoeff[2][4][3] = {-9.6e-2, 4.39e-2, 2.93e-3,  //[0][0]
  // 				     -5.0e-2, 14.9e-2, 20.6e-3,  //[0][1]
  // 				     -4.4e-2, 5.3e-2, 0,         //[0][2]
  // 				     -9.90e-2, 4.08e-2, 0,       //[0][3]
  // 				     -8.7e-2, 4.2e-2, -3.2e-3,   //[1][0]
  // 				     -3.26e-2, 3.84e-2, -2.32e-3, //[1][1]
  // 				     -26.8e-2, -2.64e-2, -5.3e-3,//[1][2]
  // 				     -13.2e-2, 0.45e-2, -2.4e-3};    //[1][3]
  //				  
  //
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
    
    ana->SetSlewingCoeff(slewingParamsHG, slewingParamsLG);
  }

  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutHG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray chisqDivAmpCutLG = {10, 10, 10, 10, 10, 10, 10, 10};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowHG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighHG = {8, 8, 8, 11, 8, 10, 8, 12};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutLowLG = {-6, -5, -5, -5, -5, -5, -5, -5};
  ZDCDataAnalyzer::ZDCModuleFloatArray DeltaT0CutHighLG = {8, 8, 8, 11, 8, 10, 8, 12};

  ana->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, DeltaT0CutLowHG, DeltaT0CutHighHG, DeltaT0CutLowLG, DeltaT0CutHighLG);


  // float moduleECalib[2][4] = {1., 1, 1, 1, 1., 1, 1, 1}; 

  // if (eCalib) ana->EnableECalib(moduleECalib);

  if (eCalib || t0Calib) LoadCalibrations(ana, calibFile, runNumber, eCalib, t0Calib);

  return ana;
}

void RunZDCAnalysis(const std::string & inputFile, const std::string & outputFile, const std::string & calibFile, int nevent = -1, 
		    bool fixTau1 = true, bool fixTau2 = true, bool eCalib = false, bool t0Calib = false, bool doSlew = false)
{

  ZDCTreeAnalysis* ana = InitZDCAnalysis(inputFile, calibFile, fixTau1, fixTau2, eCalib, t0Calib, doSlew);

  ana->OpenOutputTree(outputFile);
  ana->Loop(nevent);
  ana->CloseOutputTree();
}


