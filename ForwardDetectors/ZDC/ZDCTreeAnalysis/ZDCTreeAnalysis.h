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
#include <TEntryList.h>
#include <TCut.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <iostream>
#include <sstream>

#include "ZdcAnalysis/ZDCDataAnalyzer.h"
#include "ZdcAnalysis/ZDCMsg.h"

class ZDCTreeAnalysis {
public :
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

  // Processing state members
  //
  bool _inLoop;
  int _currentEntry;
  unsigned int _runNumber;

  bool _haveEntryList;
  TEntryList* _entryList;
  unsigned int _currentSelected;

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
  UShort_t*       zdc_raw;

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
  int    zdc_ModuleMask;
  float  zdc_SumAmp[2];
  float  zdc_SumAmpErr[2];

  float  zdc_SumCalibAmp[2];
  float  zdc_SumCalibAmpErr[2];

  float  zdc_SumPreSampleAmp[2];

  float  zdc_AvgTime[2];
  bool   zdc_sideFailed[2];

  // Per-module data
  //
  std::vector<int>* zdc_side;
  std::vector<int>* zdc_module;
  std::vector<std::vector<float> >* zdc_samplesSub;
  std::vector<std::vector<float> >* zdc_samplesDeriv;
  std::vector<std::vector<float> >* zdc_samplesDeriv2nd;

  //  float zdc_samplesDeriv[2][4][6];
  //  float zdc_samplesDeriv2nd[2][4][5];

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
  float  zdc_FitTau1[2][4];
  float  zdc_FitTau2[2][4];
  float  zdc_FitChisq[2][4];
  float  zdc_T0Corr[2][4];
  float  zdc_FitPreAmp[2][4];  // bill
  float  zdc_FitPreT0[2][4];   // bill
  float  zdc_FitPostAmp[2][4]; // bill
  float  zdc_FitPostT0[2][4];  // bill
  float  zdc_FitExpAmp[2][4];  // bill
  float  zdc_ModuleQuality[2][4];   // bill
  float  zdc_Quality[2];            // bill

  float  zdc_AmpError[2][4];
  float  zdc_BkgdMaxFraction[2][4];

  float  zdc_CalibAmp[2][4];
  float  zdc_CalibTime[2][4];

  float  zdc_delayedBS[2][4];  // Bill

  // The object responsible for the actual analysis
  //
  ZDCDataAnalyzer* _dataAnalyzer_p;

  float _fitTMax = 0;

  int  nSave       = 0;
  int  index       = 0;
  int  eventIndex  = 0;
  bool _outputplot = false;

private:
  // Critical construction parameters
  //
  int   _nSample;
  float _deltaTSample;
  int   _preSampleIdx;

  std::string _fitFunction;

  static int _debugLevel;

  bool saveEvent = false;

  ZDCDataAnalyzer::ZDCModuleFloatArray _peak2ndDerivMinSamples;
  ZDCDataAnalyzer::ZDCModuleFloatArray _peak2ndDerivMinThresholdsHG;
  ZDCDataAnalyzer::ZDCModuleFloatArray _peak2ndDerivMinThresholdsLG;

  // Use delayed samples?
  //
  bool _useDelayed;
  float _delayDeltaT;

  // Members that help with indexing into the zdc_raw array
  //
  const int _zdcrawModuleSize;

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
  std::array<std::array<std::vector<float>, 4>, 2> _moduleHGNonLinCorr;

  //  Time slewing corrections
  //
  float _T0SlewCoeffHG[2][4][3];
  float _T0SlewCoeffLG[2][4][3];

  // Bunch information
  //
  std::vector<std::set<int> > _trains;
  std::vector<int> _BCIDGap;
  std::vector<int> _BCIDPosInTrain;

private:
  void InitInternal();
  void SetupBunchTrains();

public:

  ZDCTreeAnalysis(std::string filename, int nSample, double deltaT, int preSamplIdx, std::string fitFunction,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinSamples,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
                  bool forceLG = false);

  ZDCTreeAnalysis(TChain *chain, int nSample, double deltaT, int preSamplIdx, std::string fitFunction,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinSamples,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG,
                  const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
                  bool forceLG = false);

  void SetADCOverUnderflowValues(const ZDCDataAnalyzer::ZDCModuleFloatArray& HGOverFlowADC,
                                 const ZDCDataAnalyzer::ZDCModuleFloatArray& HGUnderFlowADC,
                                 const ZDCDataAnalyzer::ZDCModuleFloatArray& LGOverFlowADC)
  {
    _dataAnalyzer_p->SetADCOverUnderflowValues(HGOverFlowADC, HGUnderFlowADC, LGOverFlowADC);
  }

  void EnableDelayed(float deltaT, const ZDCDataAnalyzer::ZDCModuleFloatArray& undelayedDelayedPedestalDiff)
  {
    _useDelayed = true;
    _dataAnalyzer_p->EnableDelayed(deltaT, undelayedDelayedPedestalDiff);
  }

  void EnableDelayed(const ZDCDataAnalyzer::ZDCModuleFloatArray& delayDeltaTArray, const ZDCDataAnalyzer::ZDCModuleFloatArray& undelayedDelayedPedestalDiff)
  {
    _useDelayed = true;
    _dataAnalyzer_p->EnableDelayed(delayDeltaTArray, undelayedDelayedPedestalDiff);
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

  static void SetDebugLevel(int debugLevel = 0)
  {
    _debugLevel = debugLevel;
  }

  static bool msgFunction(unsigned int level, std::string message)
  {
    if (level == ZDCMsg::Fatal) {
      std::cout << message << std::endl;
      throw;
    }

    if (level >= (unsigned int) _debugLevel) {
      if (message != "") std::cout << message << std::endl;
      return true;
    }
    else return false;
  }

  void DisableModule(size_t side, size_t module)
  {
    _dataAnalyzer_p->DisableModule(side, module);
  }

  void SetPeak2ndDerivMinTolerances(size_t tolerance)
  {
    _dataAnalyzer_p->SetPeak2ndDerivMinTolerances(tolerance);
  }

  void LoadEnergyCalibrations(std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>&  calibSplines)
  {
    _dataAnalyzer_p->LoadEnergyCalibrations(calibSplines);
  }

  void LoadT0Calibrations(std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>&  calibSplinesHG,
                          std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>&  calibSplinesLG)
  {
    _dataAnalyzer_p->LoadT0Calibrations(calibSplinesHG, calibSplinesLG);
  }

  void SetLBDepT0(int iside, int imod, TSpline* t0SplineLG, TSpline* t0SplineHG);

  void SetSlewingCoeff(const std::array<std::array<std::vector<float>, 4>, 2>& HGParamArr,
                       const std::array<std::array<std::vector<float>, 4>, 2>& LGParamArr)
  {
    _dataAnalyzer_p->SetTimingCorrParams(HGParamArr, LGParamArr);
  }

  void SetNonlinCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGNonlinCorrParams)
  {
    _dataAnalyzer_p->SetNonlinCorrParams(HGNonlinCorrParams);
  }

  void SetFitTimeMax (float tmax) {_fitTMax = tmax; _dataAnalyzer_p->SetFitTimeMax(tmax);}
  void SetSaveFitFunc(bool  save) {_dataAnalyzer_p->SetSaveFitFunc(save);}
  void OutputPlot(bool x = false, int y = 0) {_outputplot = x; nSave = y;}

  void Dump_setting() {
    if (_debugLevel < 4) {
      std::cout << "========================================================================================================================" << std::endl;
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 4; j++) {
          std::cout << "-------------------------------------------------------------------------------------------------------------------" << std::endl;
          std::cout << "Side: " << i << ", Module: " << j << std::endl;
          _dataAnalyzer_p->GetPulseAnalyzer(i, j)->Dump_setting();
        }
      }
      std::cout << "========================================================================================================================" << std::endl;
    }
  }


  void OpenOutputTree(std::string file);
  void CloseOutputTree();

  void PlotFits(int side);

  virtual ~ZDCTreeAnalysis();
  // virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  // virtual void     Init(TTree *tree);
  virtual void     Init(TChain *i_chain);
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

  int SetSelection(TCut selection);
  int LoadSelected(unsigned int selEntry);
  int LoadNextSelected();

  unsigned int GetRunNumber() const {return _runNumber;}

  void DoAnalysis();
};


#endif

#ifdef ZDCTreeAnalysis_cxx

ZDCTreeAnalysis::ZDCTreeAnalysis(TChain *chain, int nSample, double deltaT, int preSamplIdx, std::string fitFunction,
                                 const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinSamples,
                                 const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG,
                                 const ZDCDataAnalyzer::ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
                                 bool forceLG) :
  fChain(0), _outTFile(0), _outTree(0),
  _nSample(nSample), _deltaTSample(deltaT),  _preSampleIdx(preSamplIdx),
  _peak2ndDerivMinSamples(peak2ndDerivMinSamples),
  _peak2ndDerivMinThresholdsHG(peak2ndDerivMinThresholdsHG),
  _peak2ndDerivMinThresholdsLG(peak2ndDerivMinThresholdsLG),
  _useDelayed(false),
  _doOutput(false), _currentEntry(-1), _inLoop(false),
  _haveCalibrations(false),
  _zdcrawModuleSize(_nSample * 2 * 2) // High/low gain x delayed/undelayed x # samples
{

  // Open the fiel, extract and initialize the tree
  //
  Init(chain);

  // Capture the run number
  //
  GetEntry(1);
  _runNumber = runNumber;

  InitInternal();

  // Figure out the bunch spacing
  //
  SetupBunchTrains();

  ZDCMsg::MessageFunctionPtr msgPtr(new ZDCMsg::MessageFunction(msgFunction));

  _dataAnalyzer_p = new ZDCDataAnalyzer(msgPtr, _nSample, _deltaTSample, _preSampleIdx, fitFunction,
                                        _peak2ndDerivMinSamples,
                                        _peak2ndDerivMinThresholdsHG, _peak2ndDerivMinThresholdsLG,
                                        forceLG);
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

void ZDCTreeAnalysis::Init(TChain *i_chain)
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
  if (!i_chain) return;
  fChain = i_chain;
  fCurrent = -1;

  int zdcRawSize = 2 * 4 * 2 * 2 * _nSample;
  zdc_raw = new UShort_t[zdcRawSize];

  fChain->SetBranchAddress("runNumber"  , &runNumber  );
  fChain->SetBranchAddress("eventNumber", &eventNumber);
  fChain->SetBranchAddress("lumiBlock"  , &lumiBlock  );
  fChain->SetBranchAddress("bcid"       , &bcid       );
  fChain->SetBranchAddress("tbp"        , tbp         );//
  fChain->SetBranchAddress("passBits"   , &passBits   );//
  fChain->SetBranchAddress("zdc_raw"    , zdc_raw     );
  fChain->SetBranchAddress("L1_ZDC_A"   , &L1_ZDC_A   );
  // fChain->SetBranchAddress("ps_L1_ZDC_A", &ps_L1_ZDC_A);
  fChain->SetBranchAddress("L1_ZDC_C"   , &L1_ZDC_C   );//
  // fChain->SetBranchAddress("ps_L1_ZDC_C", &ps_L1_ZDC_C);
  fChain->SetBranchAddress("L1_ZDC_AND" , &L1_ZDC_AND );//
  // fChain->SetBranchAddress("ps_L1_ZDC_AND", &ps_L1_ZDC_AND);
  fChain->SetBranchAddress("L1_ZDC_A_C" , &L1_ZDC_A_C );//
  // fChain->SetBranchAddress("ps_L1_ZDC_A_C", &ps_L1_ZDC_A_C);

  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("runNumber"  , 1);
  fChain->SetBranchStatus("eventNumber", 1);
  fChain->SetBranchStatus("lumiBlock"  , 1);
  fChain->SetBranchStatus("bcid"       , 1);
  fChain->SetBranchStatus("zdc_raw"    , 1);
  fChain->SetBranchStatus("tbp"        , 1);//

  fChain->SetBranchStatus("passBits"   , 1);//

  fChain->SetBranchStatus("L1_ZDC_A"   , 1);
  fChain->SetBranchStatus("L1_ZDC_C"   , 1);//
  fChain->SetBranchStatus("L1_ZDC_AND" , 1);//
  fChain->SetBranchStatus("L1_ZDC_A_C" , 1);//
  // fChain->SetBranchStatus("uncalibC"   , 1);
  // fChain->SetBranchStatus("calibC"     , 1);
  // fChain->SetBranchStatus("zdc_ZdcModuleMask"  , 1);
  // fChain->SetBranchStatus("zdc_ZdcModuleAmp"   , 1);
  // fChain->SetBranchStatus("zdc_ZdcModuleStatus", 1);

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
// Int_t ZDCTreeAnalysis::Cut(Long64_t entry)
// {
// // This function may be called from Loop.
// // returns  1 if entry is accepted.
// // returns -1 otherwise.
//     return 1;
// }
#endif // #ifdef ZDCTreeAnalysis_cxx
