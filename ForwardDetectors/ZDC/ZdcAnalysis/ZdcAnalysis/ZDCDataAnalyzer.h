/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCDataAnalyzer_h
#define _ZDCDataAnalyzer_h

#include <ZdcAnalysis/ZDCPulseAnalyzer.h>
#include <TSpline.h>

#include <array>
#include <string>

#include "ZdcAnalysis/ZDCMsg.h"

class ZDCDataAnalyzer
{
public:
  typedef std::array<std::array<float, 4>, 2> ZDCModuleFloatArray;
  typedef std::array<std::array<bool, 4>, 2> ZDCModuleBoolArray;

private:
  ZDCMsg::MessageFunctionPtr m_msgFunc_p;
  size_t m_nSample;
  float m_deltaTSample;
  size_t m_preSampleIdx;
  std::string m_fitFunction;
  bool m_forceLG;

  bool m_repassEnabled;

  std::array<std::array<int, 4>, 2> m_delayedOrder;

  ZDCModuleBoolArray m_moduleDisabled;
  std::array<std::array<std::unique_ptr<ZDCPulseAnalyzer>, 4>, 2> m_moduleAnalyzers;

  int m_eventCount;

  ZDCModuleFloatArray m_HGGains;
  ZDCModuleFloatArray m_pedestals;

  bool m_haveECalib;
  std::array<std::array<std::unique_ptr<TSpline>, 4>, 2> m_LBDepEcalibSplines;

  bool m_haveT0Calib;
  std::array<std::array<std::unique_ptr<TSpline>, 4>, 2> m_T0HGOffsetSplines;
  std::array<std::array<std::unique_ptr<TSpline>, 4>, 2> m_T0LGOffsetSplines;

  // Transient data that is updated each LB or each event
  //
  int m_currentLB;
  ZDCModuleFloatArray m_currentECalibCoeff;
  ZDCModuleFloatArray m_currentT0OffsetsHG;
  ZDCModuleFloatArray m_currentT0OffsetsLG;

  std::array<std::array<bool, 4>, 2> m_dataLoaded;
  // std::array<std::array<bool, 4>, 2> _moduleFail;

  unsigned int m_moduleMask;

  std::array<std::array<unsigned int, 4>, 2> m_moduleStatus;
  std::array<std::array<float, 4>, 2> m_calibAmplitude;
  std::array<std::array<float, 4>, 2> m_calibTime;

  std::array<float, 2> m_moduleSum;
  std::array<float, 2> m_moduleSumErrSq;
  std::array<float, 2> m_moduleSumPreSample;

  std::array<float, 2> m_calibModuleSum;
  std::array<float, 2> m_calibModuleSumErrSq;

  std::array<float, 2> m_averageTime;
  std::array<bool, 2> m_fail;


public:

  ZDCDataAnalyzer(ZDCMsg::MessageFunctionPtr messageFunc_p, int nSample, float deltaTSample,
                  size_t preSampleIdx, std::string fitFunction,
                  const ZDCModuleFloatArray& peak2ndDerivMinSamples,
                  const ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG,
                  const ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
                  bool forceLG = false);

  ~ZDCDataAnalyzer();

  void EnableDelayed(float deltaT, const ZDCModuleFloatArray& undelayedDelayedPedestalDiff);
  void EnableDelayed(const ZDCModuleFloatArray& delayDeltaT, const ZDCModuleFloatArray& undelayedDelayedPedestalDiff);

  void EnableRepass(const ZDCModuleFloatArray& peak2ndDerivMinRepassHG, const ZDCModuleFloatArray& peak2ndDerivMinRepassLG);

  unsigned int GetModuleMask() const {return m_moduleMask;}

  float GetModuleSum(size_t side) const {return m_moduleSum.at(side);}
  float GetModuleSumErr(size_t side) const {return std::sqrt(m_moduleSumErrSq.at(side));}

  float GetCalibModuleSum(size_t side) const {return m_calibModuleSum.at(side);}
  float GetCalibModuleSumErr(size_t side) const {return std::sqrt(m_calibModuleSumErrSq.at(side));}

  float GetModuleSumPreSample(size_t side) const {return m_moduleSumPreSample.at(side);}

  float GetAverageTime(size_t side) const {return m_averageTime.at(side);}
  bool SideFailed(size_t side) const {return m_fail.at(side);}

  float GetModuleAmplitude(size_t side, size_t module) const {return m_moduleAnalyzers.at(side).at(module)->GetAmplitude();}
  float GetModuleTime(size_t side, size_t module) const {return m_moduleAnalyzers.at(side).at(module)->GetT0Corr();}
  float GetModuleChisq(size_t side, size_t module) const {return m_moduleAnalyzers.at(side).at(module)->GetChisq();}

  float GetModuleCalibAmplitude(size_t side, size_t module) const {return m_calibAmplitude.at(side).at(module);}
  float GetModuleCalibTime(size_t side, size_t module) const {return m_calibTime.at(side).at(module);}
  float GetModuleStatus(size_t side, size_t module) const {return m_moduleStatus.at(side).at(module);}

  float GetdelayedBS(size_t side, size_t module) const {return m_moduleAnalyzers.at(side).at(module)->GetdelayBS();}

  const ZDCPulseAnalyzer* GetPulseAnalyzer(size_t side, size_t module) const {return m_moduleAnalyzers.at(side).at(module).get();}

  bool DisableModule(size_t side, size_t module);

  void SetPeak2ndDerivMinTolerances(size_t tolerance);

  void SetFitTimeMax(float tmax);

  void SetSaveFitFunc(bool save);

  void SetADCOverUnderflowValues(const ZDCModuleFloatArray& HGOverflowADC, const ZDCModuleFloatArray& HGUnderflowADC,
                                 const ZDCModuleFloatArray& LGOverflowADC);

  void SetTauT0Values(const ZDCModuleBoolArray& fxiTau1, const ZDCModuleBoolArray& fxiTau2,
                      const ZDCModuleFloatArray& tau1, const ZDCModuleFloatArray& tau2,
                      const ZDCModuleFloatArray& t0HG, const ZDCModuleFloatArray& t0LG);

  void SetFitMinMaxAmpValues(const ZDCModuleFloatArray& minAmpHG, const ZDCModuleFloatArray& minAmpLG,
                             const ZDCModuleFloatArray& maxAmpHG, const ZDCModuleFloatArray& maxAmpLG);

  void SetCutValues(const ZDCModuleFloatArray& chisqDivAmpCutHG, const ZDCModuleFloatArray& chisqDivAmpCutLG,
                    const ZDCModuleFloatArray& deltaT0MinHG, const ZDCModuleFloatArray& deltaT0MaxHG,
                    const ZDCModuleFloatArray&  deltaT0MinLG, const ZDCModuleFloatArray& deltaT0MaxLG);


  void SetTimingCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGParamArr,
                           const std::array<std::array<std::vector<float>, 4>, 2>& LGParamArr);

  void SetNonlinCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGNonlinCorrParams);

  void LoadEnergyCalibrations(std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>& calibSplines)
  {
    (*m_msgFunc_p)(ZDCMsg::Verbose, "Loading energy calibrations");

    m_LBDepEcalibSplines = std::move (calibSplines);
    m_haveECalib = true;
  }

  void LoadT0Calibrations(std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>& T0HGOffsetSplines,
                          std::array<std::array<std::unique_ptr<TSpline>, 4>, 2>& T0LGOffsetSplines)
  {
    (*m_msgFunc_p)(ZDCMsg::Verbose, "Loading timing calibrations");

    m_T0HGOffsetSplines = std::move (T0HGOffsetSplines);
    m_T0LGOffsetSplines = std::move (T0LGOffsetSplines);

    m_haveT0Calib = true;
  }

  void StartEvent(int lumiBlock);

  void LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float>& HGSamples, const std::vector<float>& LGSamples);

  void LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float>& HGSamples, const std::vector<float>& LGSamples,
                          const std::vector<float>& HGSamplesDelayed, const std::vector<float>& LGSamplesDelayed);

  bool FinishEvent();

};
#endif
