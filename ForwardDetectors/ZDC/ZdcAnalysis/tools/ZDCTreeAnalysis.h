/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Dec 16 10:23:25 2015 by ROOT version 6.04/12
// from TTree zdcTree/ZDC Tree
// found on file: data15_hi.00287259.calibration_zdcCalib.recon.AOD.c931.root
//////////////////////////////////////////////////////////

#ifndef ZDCTreeAnalysis_h
#define ZDCTreeAnalysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TSpline.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <iostream>
#include <sstream>

#include "ZDCDataAnalyzer.h"

class ZDCTreeAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

  TFile* _outTFile;
  TTree* _outTree;
  bool _doOutput;

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          runNumber;
   UInt_t          eventNumber;
   UInt_t          lumiBlock;
   UInt_t          bcid;
   ULong64_t       trigger;
   UInt_t          trigger_TBP;
   UInt_t          tbp[16];
   UInt_t          tav[16];

   UInt_t          passBits;
   UShort_t        zdc_raw[2][4][2][2][7];

   Float_t         fcalEt;
   Int_t           ntrk;
   Int_t           nvx;
   Float_t         vx[3];
   Int_t           vxntrk;
   Float_t         vxcov[6];
   UShort_t        mbts_countA;
   UShort_t        mbts_countC;
   Float_t         mbts_timeA;
   Float_t         mbts_timeC;
   Float_t         mbts_timeDiff;

   Bool_t          L1_ZDC_A;
   Float_t         ps_L1_ZDC_A;
   Bool_t          L1_ZDC_C;
   Float_t         ps_L1_ZDC_C;
   Bool_t          L1_ZDC_AND;
   Float_t         ps_L1_ZDC_AND;
   Bool_t          L1_ZDC_A_C;
   Float_t         ps_L1_ZDC_A_C;

  //  Data for branches that are to be added to the tree
  //
  int bcidGap;
  int bcidPosInTrain;

  // Data obtained from the analysis
  //
  // Summary data
  //
  int zdc_ModuleMask;
  float  zdc_SumAmp[2];
  float  zdc_SumCalibAmp[2];
  float  zdc_AvgTime[2];
  bool zdc_sideFailed[2];

  // Per-module data
  //
  std::vector<float>* zdc_samplesSub;
  std::vector<float>* zdc_samplesDeriv;
  std::vector<float>* zdc_samplesDeriv2nd;
  
  float  zdc_Presample[2][4];

  float  zdc_MaxADC[2][4];
  int    zdc_MaxADCSample[2][4];

  float  zdc_MinADC[2][4];
  int    zdc_MinADCSample[2][4];

  float  zdc_Min2ndDeriv[2][4];
  int    zdc_Min2ndDerivSample[2][4];

  int    zdc_Status[2][4];
  float  zdc_Amp[2][4];
  float  zdc_FitT0[2][4];
  float  zdc_FitChisq[2][4];
  float  zdc_T0Corr[2][4];

  float  zdc_CalibAmp[2][4];
  float  zdc_CalibTime[2][4];

  // The object responsible for the actual analysis
  //
  ZDCDataAnalyzer* _dataAnalyzer_p;

private:
  // Critical construction parameters
  //
  int _nSample;
  float _deltaTSample;
  int _preSampleIdx;
  std::string _fitFunction;
  
  ZDCDataAnalyzer::ZDCModuleFloatArray _peak2ndDerivMinSamples;
  ZDCDataAnalyzer::ZDCModuleFloatArray _peak2ndDerivMinThresholds;

  // Cut quantities
  //
  float _HGOverFlowADC[2][4];
  float _HGUnderFlowADC[2][4];
  float _DeltaT0CutLow[2][4];
  float _DeltaT0CutHigh[2][4];
  float _chisqDivAmpCutHG[2][4];
  float _chisqDivAmpCutLG[2][4];

  //  Per-module calibration factors
  //
  // Time-dependent 
  //
  bool _haveLBDepT0[2][4];
  TSpline* _moduleT0HGLB[2][4];
  TSpline* _moduleT0LGLB[2][4];

  bool _haveCalibrations;
  float _modECalib[2][4];

  bool _haveLBDepECalib[2][4];
  TSpline* _modECalibLB[2][4];

  // Allow per-module Tau1, tau2 settings
  //
  bool _haveModuleSettings[2][4];
  bool _fixTau1[2][4];
  bool _fixTau2[2][4];
  float _moduleTau1[2][4];
  float _moduleTau2[2][4];
  float _moduleT0LG[2][4];
  float _moduleT0HG[2][4];

  std::array<std::array<float, 4>, 2> _moduleHGGains; 

  //  Time slewing corrections
  //
  float _T0SlewCoeffHG[2][4][3];
  float _T0SlewCoeffLG[2][4][3];
  
  // Bunch information
  //
  std::vector<std::set<int> > _trains;
  std::vector<int> _BCIDGap;
  std::vector<int> _BCIDPosInTrain;

  // Which entry we're reading
  //
  bool _inLoop;
  int _currentEntry;
  unsigned int _runNumber;

private:
   // List of branches
   TBranch        *b_runNumber;   //!
   TBranch        *b_eventNumber;   //!
   TBranch        *b_lumiBlock;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_trigger;   //!
   TBranch        *b_trigger_TBP;   //!
   TBranch        *b_tbp;   //!
   TBranch        *b_tav;   //!
   TBranch        *b_decisions;   //!
   TBranch        *b_prescales;   //!
   TBranch        *b_passBits;   //!

   TBranch        *b_zdc_raw;   //!
  //   TBranch        *b_fcalEt;   //!
  //   TBranch        *b_ntrk;   //!
  //   TBranch        *b_nvx;   //!
   // TBranch        *b_vx;   //!
   // TBranch        *b_vxntrk;   //!
   // TBranch        *b_vxcov;   //!
   // TBranch        *b_mbts_countA;   //!
   // TBranch        *b_mbts_countC;   //!
   // TBranch        *b_mbts_timeA;   //!
   // TBranch        *b_mbts_timeC;   //!
   // TBranch        *b_mbts_timeDiff;   //!

   TBranch        *b_L1_ZDC_A;   //!
   TBranch        *b_ps_L1_ZDC_A;   //!
   TBranch        *b_L1_ZDC_C;   //!
   TBranch        *b_ps_L1_ZDC_C;   //!
   TBranch        *b_L1_ZDC_AND;   //!
   TBranch        *b_ps_L1_ZDC_AND;   //!
   TBranch        *b_L1_ZDC_A_C;   //!
   TBranch        *b_ps_L1_ZDC_A_C;   //!

  void InitInternal();
  void SetupBunchTrains();

public:

  ZDCTreeAnalysis(std::string filename, int nSample = 7, double deltaT = 12.5, int preSamplIdx = 1, std::string fitFunction = "FermiExp");

  void SetADCOverUnderflowValues(const ZDCDataAnalyzer::ZDCModuleFloatArray& HGOverFlowADC,
				 const ZDCDataAnalyzer::ZDCModuleFloatArray& HGUnderFlowADC,
				 const ZDCDataAnalyzer::ZDCModuleFloatArray& LGOverFlowADC)
  {
    _dataAnalyzer_p->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  }

  void SetCutValues(const ZDCDataAnalyzer::ZDCModuleFloatArray& chisqDivAmpCutHG, 
		    const ZDCDataAnalyzer::ZDCModuleFloatArray& chisqDivAmpCutLG,
		    const ZDCDataAnalyzer::ZDCModuleFloatArray& DeltaT0CutLowHG, 
		    const ZDCDataAnalyzer::ZDCModuleFloatArray& DeltaT0CutHighHG,
		    const ZDCDataAnalyzer::ZDCModuleFloatArray& DeltaT0CutLowLG, 
		    const ZDCDataAnalyzer::ZDCModuleFloatArray& DeltaT0CutHighLG)

  {
    _dataAnalyzer_p->SetCutValues(chisqDivAmpCutHG, chisqDivAmpCutLG, 
				  DeltaT0CutLowHG, DeltaT0CutHighHG, 
				  DeltaT0CutLowLG, DeltaT0CutHighLG);
  }

  void SetTauT0Values(const ZDCDataAnalyzer::ZDCModuleBoolArray& fixTau1, 
		      const ZDCDataAnalyzer::ZDCModuleBoolArray& fixTau2, 
		      const ZDCDataAnalyzer::ZDCModuleFloatArray& tau1, 
		      const ZDCDataAnalyzer::ZDCModuleFloatArray& tau2, 
		      const ZDCDataAnalyzer::ZDCModuleFloatArray& t0HG, 
		      const ZDCDataAnalyzer::ZDCModuleFloatArray& t0LG) 
  {
    _dataAnalyzer_p->SetTauT0Values(fixTau1, fixTau2, tau1, tau2, t0HG, t0LG);
  }


  void SetDebugLevel(int debugLevel = 0) 
  {
    _dataAnalyzer_p->SetDebugLevel(debugLevel);
  }

  void LoadEnergyCalibrations(const std::array<std::array<TSpline*, 4>, 2>&  calibSplines)
  {
    _dataAnalyzer_p->LoadEnergyCalibrations(calibSplines);
  }

  void LoadT0Calibrations(const std::array<std::array<TSpline*, 4>, 2>&  calibSplinesHG,
			   const std::array<std::array<TSpline*, 4>, 2>&  calibSplinesLG)
  {
    _dataAnalyzer_p->LoadtT0Calibrations(calibSplinesHG, calibSplinesLG);
  }

  void SetLBDepT0(int iside, int imod, TSpline* t0SplineLG, TSpline* t0SplineHG);

  void SetSlewingCoeff(const std::array<std::array<std::vector<float>, 4>, 2>& HGParamArr,
		       const std::array<std::array<std::vector<float>, 4>, 2>& LGParamArr)
  {
    _dataAnalyzer_p->SetTimingCorrParams(HGParamArr, LGParamArr);
  }

  void OpenOutputTree(const std::string & file);
  void CloseOutputTree();

  void PlotFits(const std::string & canvasSavePath = "");

  virtual ~ZDCTreeAnalysis();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual void     Loop(int numEntries = -1, int startEntry = 0);
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);

  void LoadEntry(int entry) 
  {
    GetEntry(entry);
    DoAnalysis();
  }
  
  void LoadNextEntry() 
  {
    GetEntry(_currentEntry + 1);
    DoAnalysis();
  }

  unsigned int GetRunNumber() const {return _runNumber;}

  void DoAnalysis();
};


#endif

#ifdef ZDCTreeAnalysis_cxx

ZDCTreeAnalysis::ZDCTreeAnalysis(const std::string & filename, int nSample, double deltaT, int preSamplIdx, const std::string & fitFunction) : 
  fChain(0), _outTFile(0), _outTree(0), 
  _nSample(nSample), _deltaTSample(deltaT),  _preSampleIdx(preSamplIdx),
  _doOutput(false), _currentEntry(-1), _inLoop(false),
  _haveCalibrations(false)
{

  // Open the fiel, extract and initialize the tree 
  //
  TTree* tree;
  TFile* f = new TFile(filename.c_str());
  f->GetObject("zdcTree",tree);
  Init(tree);

  // Capture the run number
  //
  GetEntry(1);
  _runNumber = runNumber;

  InitInternal();

  // Figure out the bunch spacing
  //
  SetupBunchTrains();

  // Set the HV gains
  //
  _moduleHGGains = {9.51122, 9.51980, 9.51122, 9.51122,
		    9.51415, 9.5049, 9.51659, 9.51415};

  _peak2ndDerivMinSamples = {3, 3, 3, 2, 
			     2, 2, 2, 2};

  _peak2ndDerivMinThresholds = {-7, -7, -7, -7,
				-7, -7, -7, -7};

  _dataAnalyzer_p = new ZDCDataAnalyzer(_nSample, _deltaTSample, _preSampleIdx, fitFunction,
					_peak2ndDerivMinSamples, _peak2ndDerivMinThresholds);

/* 81Undelayed */
/* 81H1: 9.5049 */
/* 81H2: 9.51659 */
/* 81H3: 9.50119 */
/* 81EM: 9.51415 */
/* 81Delayed */
/* 81H1: 9.52632 */
/* 81H2: 9.49662 */
/* 81H3: 9.50853 */
/* 81EM: 9.50842 */
/* 12Undelayed */
/* 12H1: 9.5198  */
/* 12H2: 9.51122 */
/* 12H3: 9.51122 */
/* 12EM: 9.51122 */
/* 12Delayed */
/* 12H1: 9.51237 */
/* 12H2: 9.50178 */
/* 12H3: 9.50178 */
/* 12EM: 9.50178 */
}

ZDCTreeAnalysis::~ZDCTreeAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ZDCTreeAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   int result = fChain->GetEntry(entry);
   if (result > 0) _currentEntry = entry;
   return result;
}
Long64_t ZDCTreeAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ZDCTreeAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   //   decisions = 0;
   // prescales = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   //   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   // fChain->SetBranchAddress("trigger", &trigger, &b_trigger);
   // fChain->SetBranchAddress("trigger_TBP", &trigger_TBP, &b_trigger_TBP);
   fChain->SetBranchAddress("tbp", tbp, &b_tbp);

   // fChain->SetBranchAddress("tav", tav, &b_tav);
   // //  fChain->SetBranchAddress("decisions", &decisions, &b_decisions);
   // // fChain->SetBranchAddress("prescales", &prescales, &b_prescales);
   fChain->SetBranchAddress("passBits", &passBits, &b_passBits);

   // fChain->SetBranchAddress("zdc_amp", zdc_amp, &b_zdc_amp);
   // fChain->SetBranchAddress("zdc_amp_rp", zdc_amp_rp, &b_zdc_amp_rp);
   // fChain->SetBranchAddress("zdc_time", zdc_time, &b_zdc_time);
   fChain->SetBranchAddress("zdc_raw", zdc_raw, &b_zdc_raw);
   // fChain->SetBranchAddress("zdc_ampHG", zdc_ampHG, &b_zdc_ampHG);
   // fChain->SetBranchAddress("zdc_ampLG", zdc_ampLG, &b_zdc_ampLG);
   // fChain->SetBranchAddress("zdc_sumHG", zdc_sumHG, &b_zdc_sumHG);
   // fChain->SetBranchAddress("zdc_sumLG", zdc_sumLG, &b_zdc_sumLG);
   // fChain->SetBranchAddress("zdc_ampHG_rp", zdc_ampHG_rp, &b_zdc_ampHG_rp);
   // fChain->SetBranchAddress("zdc_ampLG_rp", zdc_ampLG_rp, &b_zdc_ampLG_rp);
   // fChain->SetBranchAddress("zdc_sumHG_rp", zdc_sumHG_rp, &b_zdc_sumHG_rp);
   // fChain->SetBranchAddress("zdc_sumLG_rp", zdc_sumLG_rp, &b_zdc_sumLG_rp);
   // fChain->SetBranchAddress("fcalEt", &fcalEt, &b_fcalEt);
   //   fChain->SetBranchAddress("ntrk", &ntrk, &b_ntrk);
   // fChain->SetBranchAddress("nvx", &nvx, &b_nvx);
   // fChain->SetBranchAddress("vx", vx, &b_vx);
   // fChain->SetBranchAddress("vxntrk", &vxntrk, &b_vxntrk);
   // fChain->SetBranchAddress("vxcov", vxcov, &b_vxcov);
   // fChain->SetBranchAddress("mbts_countA", &mbts_countA, &b_mbts_countA);
   // fChain->SetBranchAddress("mbts_countC", &mbts_countC, &b_mbts_countC);
   // fChain->SetBranchAddress("mbts_timeA", &mbts_timeA, &b_mbts_timeA);
   // fChain->SetBranchAddress("mbts_timeC", &mbts_timeC, &b_mbts_timeC);
   // fChain->SetBranchAddress("mbts_timeDiff", &mbts_timeDiff, &b_mbts_timeDiff);

   fChain->SetBranchAddress("L1_ZDC_A", &L1_ZDC_A, &b_L1_ZDC_A);
   //   fChain->SetBranchAddress("ps_L1_ZDC_A", &ps_L1_ZDC_A, &b_ps_L1_ZDC_A);
   fChain->SetBranchAddress("L1_ZDC_C", &L1_ZDC_C, &b_L1_ZDC_C);
   //   fChain->SetBranchAddress("ps_L1_ZDC_C", &ps_L1_ZDC_C, &b_ps_L1_ZDC_C);
   fChain->SetBranchAddress("L1_ZDC_AND", &L1_ZDC_AND, &b_L1_ZDC_AND);
   //   fChain->SetBranchAddress("ps_L1_ZDC_AND", &ps_L1_ZDC_AND, &b_ps_L1_ZDC_AND);
   fChain->SetBranchAddress("L1_ZDC_A_C", &L1_ZDC_A_C, &b_L1_ZDC_A_C);
   //   fChain->SetBranchAddress("ps_L1_ZDC_A_C", &ps_L1_ZDC_A_C, &b_ps_L1_ZDC_A_C);

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("runNumber", 1); 
   fChain->SetBranchStatus("eventNumber", 1); 
   fChain->SetBranchStatus("lumiBlock", 1); 
   fChain->SetBranchStatus("bcid", 1); 
   fChain->SetBranchStatus("zdc_raw", 1); 
   fChain->SetBranchStatus("tbp", 1);

   fChain->SetBranchStatus("passBits", 1); 

   fChain->SetBranchStatus("L1_ZDC_A", 1); 
   fChain->SetBranchStatus("L1_ZDC_C", 1); 
   fChain->SetBranchStatus("L1_ZDC_AND", 1); 
   fChain->SetBranchStatus("L1_ZDC_A_C", 1); 

   Notify();
}

Bool_t ZDCTreeAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ZDCTreeAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ZDCTreeAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ZDCTreeAnalysis_cxx
