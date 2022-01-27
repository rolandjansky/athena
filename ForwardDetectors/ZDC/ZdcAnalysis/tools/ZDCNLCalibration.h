/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Aug 26 05:51:09 2016 by ROOT version 6.06/02
// from TTree zdcTree/ZDC Tree
// found on file: ../user.steinber.data15_hi.00287931.calibration_zdcCalib.merge.AOD.c932_m1533.42testNL3_output.root
//////////////////////////////////////////////////////////

#ifndef ZDCNLCalibration_h
#define ZDCNLCalibration_h

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1.h>

#include <array>
#include <map>
#include <algorithm>
#include <iostream>
#include <vector>

#include <TVectorD.h>
#include <TMatrixD.h>
#include <TDecompLU.h>


struct CalibData 
{
  std::array<std::vector<float>, 4> weights; 
  unsigned int LBStart;
  unsigned int LBEnd;
  
  CalibData()
  {
    std::vector<float> unity(1, 1);
    weights = {unity, unity, unity, unity};
    LBStart = 0;
    LBEnd = 10000;
  };

CalibData(int inLBStart, int inLBEnd, const std::array<std::vector<float>, 4>& inWeights) : 
  LBStart(inLBStart), LBEnd(inLBEnd), weights(inWeights)
  {};
};

// Header file for the classes stored in the TTree if any.

class ZDCNLCalibration 
{
  TFile* m_TFile;
  TTree*  m_tree;

  size_t m_maxNLPower;
  //  float  m_HEDeweight;
  bool m_useGRL;
  int m_debugLevel;

  // size_t m_numWeights;
  // size_t m_numFitParams;

  const float m_SNEnergy;
  const std::vector<float> m_HEFraction;

  // Declaration of leaf types
  UInt_t          runNumber;
  UInt_t          eventNumber;
  UInt_t          lumiBlock;
  UInt_t          bcid;
  UInt_t          passBits;
  
  Float_t         zdc_ZdcAmp[2];
  //   Float_t         zdc_ZdcEnergy[2];
  //   Float_t         zdc_ZdcTime[2];
  //   Short_t         zdc_ZdcStatus[2];
  //   Float_t         zdc_ZdcTrigEff[2];
  UInt_t          zdc_ZdcModuleMask;
  Float_t         zdc_ZdcModuleAmp[2][4];
  // Float_t         zdc_ZdcModuleTime[2][4];
  // Float_t         zdc_ZdcModuleFitAmp[2][4];
  // Float_t         zdc_ZdcModuleFitT0[2][4];
  // UInt_t          zdc_ZdcModuleStatus[2][4];
  // Float_t         zdc_ZdcModuleChisq[2][4];
  // Float_t         zdc_ZdcModuleCalibAmp[2][4];
  // Float_t         zdc_ZdcModuleCalibTime[2][4];
  // Float_t         zdc_ZdcModuleBkgdMaxFraction[2][4];
  // Float_t         zdc_ZdcModuleAmpError[2][4];
  Bool_t          L1_ZDC_A;
  Bool_t          L1_ZDC_C;
  Bool_t          L1_ZDC_AND;
  Bool_t          L1_ZDC_A_C;
  
  // List of branches
  TBranch        *b_runNumber;   //!
  TBranch        *b_eventNumber;   //!
  TBranch        *b_lumiBlock;   //!
  TBranch        *b_bcid;   //!
  
  TBranch        *b_passBits;   //!
  
  TBranch        *b_zdc_ZdcAmp;   //!
  //   TBranch        *b_zdc_ZdcEnergy;   //!
  //   TBranch        *b_zdc_ZdcTime;   //!
  //  TBranch        *b_zdc_ZdcStatus;   //!
  // TBranch        *b_zdc_ZdcTrigEff;   //!
  TBranch        *b_zdc_ZdcModuleMask;   //!
  TBranch        *b_zdc_ZdcModuleAmp;   //!
  // TBranch        *b_zdc_ZdcModuleTime;   //!
  // TBranch        *b_zdc_ZdcModuleFitAmp;   //!
  // TBranch        *b_zdc_ZdcModuleFitT0;   //!
  // TBranch        *b_zdc_ZdcModuleStatus;   //!
  // TBranch        *b_zdc_ZdcModuleChisq;   //!
  // TBranch        *b_zdc_ZdcModuleCalibAmp;   //!
  // TBranch        *b_zdc_ZdcModuleCalibTime;   //!
  // TBranch        *b_zdc_ZdcModuleBkgdMaxFraction;   //!
  // TBranch        *b_zdc_ZdcModuleAmpError;   //!
  TBranch        *b_L1_ZDC_A;   //!
  TBranch        *b_L1_ZDC_C;   //!
  TBranch        *b_L1_ZDC_AND;   //!
  TBranch        *b_L1_ZDC_A_C;   //!

  //  Map that keeps lumi block and event associations to optimize
  //    processing of lumi block ranges
  //
  typedef std::multimap<unsigned int, std::pair<unsigned int, unsigned int> > LBEvtMap;
  LBEvtMap m_LumiBlockEvtMap;

public:

  std::array<std::map<std::string, CalibData>, 2> m_calibrations;

  bool m_haveTest{};
  TH1D* m_testCalibSNHist{};
  std::array<TH1D*, 4> m_testCalibHEFracHist;
  std::array<TH1D*, 4> m_testCalibEnergyHist;
  TTree* m_testTree{};


  ZDCNLCalibration(std::string file, int maxNLPower = 3, bool useGRL = true, int debugLevel = 0);
  virtual ~ZDCNLCalibration() {}

  std::array<float, 4> FindSNPeaks(size_t LBLow, size_t LBHigh, size_t side);

  std::pair<float, float> FindSNRange(size_t LBLow, size_t LBHigh, size_t side);

  std::pair<std::pair<float, float>,std::pair<float, float>  > FindSNTwoNRanges(size_t LBLow, size_t LBHigh, size_t side);

  void SetDefaultCalibration(size_t side, const CalibData& calib) {
    AddCalibration(side, "default", calib);
  }

  void Calibrate(size_t side, const std::string & calibInput, const std::string & calibOutput, 
		 size_t LBLow, size_t LBHigh, std::array<int, 4> maxPowerModule,
		 std::vector<std::pair<double, double> >  nNeutERange, 
		 bool excludeHE, float heSumThresh, float HEDeweight);
  
  void TestCalibration(int side, std::string calibName);

  TH1* GetTestSNHist() {return (m_haveTest ? m_testCalibSNHist : 0);}
  TH1* GetTestFracHist(size_t module) {return (m_haveTest ? m_testCalibHEFracHist.at(module) : 0);}

  TTree* GetTestTree() {return m_testTree;}

private:

  void FillLumiBlockEvtMap();

  void FillMinimizationData(TMatrixD& minimMatrix, TVectorD& minimVector, 
			    std::array<int, 4> maxPowerModule, float HEDeweight, 
			    const std::vector<std::vector<double> >& sums1DVec, const std::vector<double>& sumsHE,
			    const std::vector<std::vector<double> >& sums2DVec, const std::vector<double>& sumsHE2D);
 
  void AddCalibration(size_t side, std::string tag, const CalibData& calib);
  CalibData GetCalibration(size_t side, std::string tag);

  //  Add the four amplitudes from a given event to the set of sums used in the NL weight fitting
  //    we need to make all combinations of amplitude products and for all combinations of powers
  //
  void AddToSums(std::vector<double>& sums1D, std::vector<double>& sums2D, float* amps) 
  {
    size_t index1D = 0;
    size_t index2D = 0;

    for (size_t module1 : {0, 1, 2, 3}) {
      double amp1 = amps[module1];
      double amp1Pow = amp1;

      for (size_t power1 = 0; power1 < m_maxNLPower; power1++) {
	for (size_t module2 : {0, 1, 2, 3}) {
	  double amp2 = amps[module2];
	  double amp2Pow = amp2;

	  for (size_t power2 = 0; power2 < m_maxNLPower; power2++) {
	    sums2D.at(index2D++) += amp1Pow*amp2Pow;
	    amp2Pow *= amp2;
	  }
	}

	sums1D.at(index1D++) += amp1Pow;
	amp1Pow *= amp1;
      }
    }
  }

  static double CalculateEnergy(const float* moduleAmps, const CalibData& calib)
  {
    double energy = 0;

    for (size_t module : {0, 1, 2, 3}) {
      float amp = moduleAmps[module];
      float ampPow = amp;

      for (size_t power = 0; power < calib.weights[module].size(); power++) {
	energy += calib.weights[module][power]*ampPow;
	ampPow *= amp;
      }
    }

    return energy;
  }

};

#endif

#ifdef ZDCNLCalibration_cxx
ZDCNLCalibration::ZDCNLCalibration(std::string file, int maxNLPower,  bool useGRL, int debugLevel) : 
  m_TFile(0), m_tree(0), m_maxNLPower(maxNLPower), //m_HEDeweight(heDeweight),
  m_useGRL(useGRL), m_debugLevel(debugLevel),
  //  m_numWeights(m_maxNLPower*4), m_numFitParams(m_maxNLPower *4),
  m_SNEnergy(2510), m_HEFraction({0.31, 0.27, 0.21, 0.21}),
  m_haveTest(false)
{
  std::cout << "Initializing ZDCNLCalibration with debug level " << m_debugLevel << std::endl;

  TFile* temp_p = new TFile(file.c_str());
  if (!temp_p->IsOpen()) {
    std::cout << "Error opening input file " << file << std::endl;
    return;
  }

  m_TFile = temp_p;
  m_tree = static_cast<TTree*>(m_TFile->GetObjectChecked("zdcTree","TTree"));
  if (!m_tree) {
    std::cout << "Error reading tree from input file " << file << std::endl;
    return;
  }

  m_tree->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
  m_tree->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
  m_tree->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
  m_tree->SetBranchAddress("bcid", &bcid, &b_bcid);
  m_tree->SetBranchAddress("passBits", &passBits, &b_passBits);

  if (m_tree->FindBranch("zdc_ZdcModuleMask")) {
    m_tree->SetBranchAddress("zdc_ZdcAmp", zdc_ZdcAmp, &b_zdc_ZdcAmp);
    m_tree->SetBranchAddress("zdc_ZdcModuleMask", &zdc_ZdcModuleMask, &b_zdc_ZdcModuleMask);
    m_tree->SetBranchAddress("zdc_ZdcModuleAmp", zdc_ZdcModuleAmp, &b_zdc_ZdcModuleAmp);
  }
  else if (m_tree->FindBranch("zdc_ModuleMask")) {
    m_tree->SetBranchAddress("zdc_OptSumAmp", zdc_ZdcAmp, &b_zdc_ZdcAmp);
    m_tree->SetBranchAddress("zdc_ModuleMask", (int*) &zdc_ZdcModuleMask, &b_zdc_ZdcModuleMask);
    m_tree->SetBranchAddress("zdc_OptAmp", zdc_ZdcModuleAmp, &b_zdc_ZdcModuleAmp);
  }
  else throw std::runtime_error("ZDCNLCalibration::ZDCNLCalibration valid branch not found");

  m_tree->SetBranchAddress("L1_ZDC_A", &L1_ZDC_A, &b_L1_ZDC_A);
  m_tree->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C, &b_L1_ZDC_C);
  m_tree->SetBranchAddress("L1_ZDC_AND", &L1_ZDC_AND, &b_L1_ZDC_AND);
  m_tree->SetBranchAddress("L1_ZDC_A_C", &L1_ZDC_A_C, &b_L1_ZDC_A_C);

  if (m_debugLevel > 0) {
    std::cout << "Filling LumiBlock-event map" << std::endl;
  }

  FillLumiBlockEvtMap();

  // Provide "null" default calibration
  //
  AddCalibration(0, "default", CalibData());
  AddCalibration(1, "default", CalibData());
}

#endif // #ifdef ZDCNLCalibration_cxx
