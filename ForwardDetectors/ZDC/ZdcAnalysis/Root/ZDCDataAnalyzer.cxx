/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <ZdcAnalysis/ZDCDataAnalyzer.h>
#include <ZdcAnalysis/ZDCPulseAnalyzer.h>

#include <sstream>

ZDCDataAnalyzer::ZDCDataAnalyzer(ZDCMsg::MessageFunctionPtr msgFunc_p, int nSample, float deltaTSample, size_t preSampleIdx, std::string fitFunction,
                                 const ZDCModuleFloatArray& peak2ndDerivMinSamples,
                                 const ZDCModuleFloatArray& peak2ndDerivMinThresholdsHG,
                                 const ZDCModuleFloatArray& peak2ndDerivMinThresholdsLG,
                                 bool forceLG) :
  m_msgFunc_p(msgFunc_p),
  m_nSample(nSample), m_deltaTSample(deltaTSample), m_preSampleIdx(preSampleIdx),
  m_fitFunction(fitFunction),
  m_forceLG(forceLG),
  m_repassEnabled(false),
  m_eventCount(0),
  m_haveECalib(false),
  m_haveT0Calib(false),
  m_currentLB(-1),
  m_moduleMask(0),
  m_moduleSum({{0, 0}}),
  m_moduleSumErrSq({{0, 0}}),
  m_moduleSumPreSample({{0, 0}}),
  m_calibModuleSum({{0, 0}}),
  m_calibModuleSumErrSq({{0, 0}}),
  m_averageTime({{0, 0}}),
  m_fail({{false, false}})
{
  m_moduleDisabled[0] = {{false, false, false, false}};
  m_moduleDisabled[1] = {{false, false, false, false}};

  m_moduleAnalyzers[0] = {{0, 0, 0, 0}};
  m_moduleAnalyzers[1] = {{0, 0, 0, 0}};

  m_calibAmplitude[0] = {{0, 0, 0, 0}};
  m_calibAmplitude[1] = {{0, 0, 0, 0}};

  m_calibTime[0] = {{0, 0, 0, 0}};
  m_calibTime[1] = {{0, 0, 0, 0}};

  m_dataLoaded[0] = {{false, false, false, false}};
  m_dataLoaded[1] = {{false, false, false, false}};

  m_delayedOrder[0] = {0, 0, 0, 0};
  m_delayedOrder[1] = {0, 0, 0, 0};

  // For now we are using hard-coded gain factors and pedestals
  //
  m_HGGains[0] = {{10, 10, 10, 10}};
  m_HGGains[1] = {{10, 10, 10, 10}};

  m_pedestals[0] = {{100, 100, 100, 100}};
  m_pedestals[1] = {{100, 100, 100, 100}};

  // Default "calibrations"
  //
  m_currentECalibCoeff = {{{{1, 1, 1, 1}}, {{1, 1, 1, 1}}}};

  m_currentT0OffsetsHG = {{{{0, 0, 0, 0}}, {{0, 0, 0, 0}}}};
  m_currentT0OffsetsLG = {{{{0, 0, 0, 0}}, {{0, 0, 0, 0}}}};

  // Construct the per-module pulse analyzers
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      std::ostringstream moduleTag;
      moduleTag << "_s" << side << "_m" << module;

      m_moduleAnalyzers[side][module].reset (new ZDCPulseAnalyzer(m_msgFunc_p, moduleTag.str().c_str(), m_nSample, m_deltaTSample, m_preSampleIdx,
                                             m_pedestals[side][module], m_HGGains[side][module], m_fitFunction,
                                             peak2ndDerivMinSamples[side][module],
                                             peak2ndDerivMinThresholdsHG[side][module],
                                             peak2ndDerivMinThresholdsLG[side][module]));
      if (m_forceLG) m_moduleAnalyzers[side][module]->SetForceLG(true);
    }
  }
}

ZDCDataAnalyzer::~ZDCDataAnalyzer()
{
}

bool ZDCDataAnalyzer::DisableModule(size_t side, size_t module)
{
  if (side < 2 && module < 4) {
    //
    // Can't disable in the middle of analysis
    //
    if (m_dataLoaded[side][module]) return false;
    else {
      m_moduleDisabled[side][module] = true;
      return true;
    }
  }
  else {
    return false;
  }
}

void ZDCDataAnalyzer::EnableDelayed(float deltaT, const ZDCModuleFloatArray& undelayedDelayedPedestalDiff)
{
  int delayedOrder = deltaT < 0 ? -1 : 1;
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_delayedOrder[side][module] = delayedOrder;
      m_moduleAnalyzers[side][module]->EnableDelayed(std::abs(deltaT), undelayedDelayedPedestalDiff[side][module]);
    }
  }
}

void ZDCDataAnalyzer::EnableDelayed(const ZDCModuleFloatArray& delayDeltaTArray, const ZDCModuleFloatArray& undelayedDelayedPedestalDiff)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      if (delayDeltaTArray[side][module] < 0) m_delayedOrder[side][module] = -1;
      else m_delayedOrder[side][module] = 1;

      (*m_msgFunc_p)(ZDCMsg::Verbose, "Enabling use of delayed samples on side, module = " + std::to_string(side) + ", " +
                     std::to_string(module) + ", delta t = " + std::to_string(delayDeltaTArray[side][module]));

      m_moduleAnalyzers[side][module]->EnableDelayed(std::abs(delayDeltaTArray[side][module]), undelayedDelayedPedestalDiff[side][module]);
    }
  }
}

void ZDCDataAnalyzer::EnableRepass(const ZDCModuleFloatArray& peak2ndDerivMinRepassHG, const ZDCModuleFloatArray& peak2ndDerivMinRepassLG)
{
  m_repassEnabled = true;
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->EnableRepass(peak2ndDerivMinRepassHG[side][module], peak2ndDerivMinRepassLG[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetPeak2ndDerivMinTolerances(size_t tolerance) {
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetPeak2ndDerivMinTolerance(tolerance);
    }
  }
}


void ZDCDataAnalyzer::SetFitTimeMax(float tmax) {
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetFitTimeMax(tmax);
    }
  }
}


void ZDCDataAnalyzer::SetSaveFitFunc(bool save) {
  ZDCPulseAnalyzer::SetSaveFitFunc(save);
}


void ZDCDataAnalyzer::SetTauT0Values(const ZDCModuleBoolArray& fixTau1, const ZDCModuleBoolArray& fixTau2,
                                     const ZDCModuleFloatArray& tau1, const ZDCModuleFloatArray& tau2,
                                     const ZDCModuleFloatArray& t0HG, const ZDCModuleFloatArray& t0LG)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetTauT0Values(fixTau1[side][module], fixTau2[side][module],
          tau1[side][module], tau2[side][module], t0HG[side][module], t0LG[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetFitMinMaxAmpValues(const ZDCModuleFloatArray& minAmpHG, const ZDCModuleFloatArray& minAmpLG,
    const ZDCModuleFloatArray& maxAmpHG, const ZDCModuleFloatArray& maxAmpLG)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetFitMinMaxAmp(minAmpHG[side][module], minAmpLG[side][module],
          maxAmpHG[side][module], maxAmpLG[side][module]);

    }
  }
}


void ZDCDataAnalyzer::SetADCOverUnderflowValues(const ZDCModuleFloatArray& HGOverflowADC, const ZDCModuleFloatArray& HGUnderflowADC,
    const ZDCModuleFloatArray& LGOverflowADC)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetADCOverUnderflowValues(HGOverflowADC[side][module], HGUnderflowADC[side][module], LGOverflowADC[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetCutValues(const ZDCModuleFloatArray& chisqDivAmpCutHG, const ZDCModuleFloatArray& chisqDivAmpCutLG,
                                   const ZDCModuleFloatArray& deltaT0MinHG, const ZDCModuleFloatArray& deltaT0MaxHG,
                                   const ZDCModuleFloatArray&  deltaT0MinLG, const ZDCModuleFloatArray& deltaT0MaxLG)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetCutValues(chisqDivAmpCutHG[side][module], chisqDivAmpCutLG[side][module],
          deltaT0MinHG[side][module], deltaT0MaxHG[side][module],
          deltaT0MinLG[side][module], deltaT0MaxLG[side][module]);
    }
  }
}

void ZDCDataAnalyzer::SetTimingCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGParamArr,
    const std::array<std::array<std::vector<float>, 4>, 2>& LGParamArr)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetTimingCorrParams(HGParamArr.at(side).at(module), LGParamArr.at(side).at(module));
    }
  }

}

void ZDCDataAnalyzer::SetNonlinCorrParams(const std::array<std::array<std::vector<float>, 4>, 2>& HGNonlinCorrParams)
{
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_moduleAnalyzers[side][module]->SetNonlinCorrParams(HGNonlinCorrParams.at(side).at(module));
    }
  }
}

void ZDCDataAnalyzer::StartEvent(int lumiBlock)
{
  (*m_msgFunc_p)(ZDCMsg::Verbose, ("Starting new event, event index = " + std::to_string(m_eventCount)));

  // By default we perform quiet pulse fits
  //
  if ((*m_msgFunc_p)(ZDCMsg::Verbose, ""))
    ZDCPulseAnalyzer::SetQuietFits(false);
  else
    ZDCPulseAnalyzer::SetQuietFits(true);

  //  See if we have to load up new calibrations
  //
  if (lumiBlock != m_currentLB) {
    (*m_msgFunc_p)(ZDCMsg::Verbose,  ("Starting new luminosity block " + std::to_string(lumiBlock)));

    if (m_haveECalib) {
      (*m_msgFunc_p)(ZDCMsg::Verbose, ("Loading energy calibrations for event " + std::to_string(m_eventCount) + ", lumi block " +
                                       std::to_string(lumiBlock)));

      for (size_t side : {0, 1}) {
        for (size_t module : {0, 1, 2, 3}) {
          float splineLBMin = m_LBDepEcalibSplines[side][module]->GetXmin();
          float splineLBMax = m_LBDepEcalibSplines[side][module]->GetXmax();

          if (lumiBlock >= splineLBMin && lumiBlock <= splineLBMax) {
            m_currentECalibCoeff[side][module] = m_LBDepEcalibSplines[side][module]->Eval(lumiBlock);
          }
          else if (lumiBlock < splineLBMin) {
            m_currentECalibCoeff[side][module] = m_LBDepEcalibSplines[side][module]->Eval(splineLBMin);
          }
          else {
            m_currentECalibCoeff[side][module] = m_LBDepEcalibSplines[side][module]->Eval(splineLBMax);
          }
        }
      }
    } // end of if (_haveEcalib) {

    if (m_haveT0Calib) {
      (*m_msgFunc_p)(ZDCMsg::Verbose, ("Loading timing calibrations for event " + std::to_string(m_eventCount) + ", lumi block " + std::to_string(lumiBlock)));

      for (size_t side : {0, 1}) {
        for (size_t module : {0, 1, 2, 3}) {
          float splineLBMin = m_T0HGOffsetSplines[side][module]->GetXmin();
          float splineLBMax = m_T0HGOffsetSplines[side][module]->GetXmax();

          if (lumiBlock >= splineLBMin && lumiBlock <= splineLBMax) {
            m_currentT0OffsetsHG[side][module] = m_T0HGOffsetSplines[side][module]->Eval(lumiBlock);
            m_currentT0OffsetsLG[side][module] = m_T0LGOffsetSplines[side][module]->Eval(lumiBlock);
          }
          else if (lumiBlock < splineLBMin) {
            m_currentT0OffsetsHG[side][module] = m_T0HGOffsetSplines[side][module]->Eval(splineLBMin);
            m_currentT0OffsetsLG[side][module] = m_T0LGOffsetSplines[side][module]->Eval(splineLBMin);
          }
          else {
            m_currentT0OffsetsHG[side][module] = m_T0HGOffsetSplines[side][module]->Eval(splineLBMax);
            m_currentT0OffsetsLG[side][module] = m_T0LGOffsetSplines[side][module]->Eval(splineLBMax);
          }
        }
      }
    } // end of if (m_haveT0Calib)
  }

  // Initialize transient results
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      m_dataLoaded[side][module] = false;
      m_moduleStatus[side][module] = 0;
      m_calibAmplitude[side][module] = 0;
      m_calibTime[side][module] = 0;
      //      _moduleFail[side][module] = false;
    }

    m_moduleSum[side] = 0;
    m_moduleSumErrSq[side] = 0;
    m_moduleSumPreSample[side] = 0;

    m_calibModuleSum[side] = 0;
    m_calibModuleSumErrSq[side] = 0;

    m_averageTime[side] = 0;
    m_fail[side] = false;
  }

  m_moduleMask = 0;
  m_currentLB = lumiBlock;
}

void ZDCDataAnalyzer::LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float>& HGSamples, const std::vector<float>& LGSamples)
{
  // We immediately return if this module is disabled
  //
  if (m_moduleDisabled[side][module]) {
    (*m_msgFunc_p)(ZDCMsg::Verbose, ("Skipping analysis of disabled mofule for event index " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) + ", " + std::to_string(module)));

    return;
  }

  (*m_msgFunc_p)(ZDCMsg::Verbose, ("/n Loading data for event index " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) + ", " + std::to_string(module)));

  ZDCPulseAnalyzer* pulseAna_p = m_moduleAnalyzers[side][module].get();
  pulseAna_p->LoadAndAnalyzeData(HGSamples, LGSamples);
  m_dataLoaded[side][module] = true;

  if (pulseAna_p->Failed()) {
    (*m_msgFunc_p)(ZDCMsg::Debug, ("ZDCPulseAnalyzer::LoadData() returned fail for event " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) + ", " + std::to_string(module)));

    m_fail[side] = true;
  }

  m_moduleStatus[side][module] = pulseAna_p->GetStatusMask();
}

void ZDCDataAnalyzer::LoadAndAnalyzeData(size_t side, size_t module, const std::vector<float>& HGSamples, const std::vector<float>& LGSamples,
    const std::vector<float>& HGSamplesDelayed, const std::vector<float>& LGSamplesDelayed)
{
  // We immediately return if this module is disabled
  //
  if (m_moduleDisabled[side][module]) {
    (*m_msgFunc_p)(ZDCMsg::Debug,  ("Skipping analysis of disabled mofule for event index " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) + ", " + std::to_string(module)));

    return;
  }

  if (m_delayedOrder[side][module] == 0) {
    (*m_msgFunc_p)(ZDCMsg::Error, ("Handling of delayed pulses not enabled, on side, module = " + std::to_string(side) +  ", " + std::to_string(module) + ", skipping processing for event index " + std::to_string(m_eventCount)));
    return;
  }

  (*m_msgFunc_p)(ZDCMsg::Verbose, ("Loading undelayed and delayed data for event index " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) +  ", " + std::to_string(module)));

  ZDCPulseAnalyzer* pulseAna_p = m_moduleAnalyzers[side][module].get();
  if (m_delayedOrder[side][module] > 0) {
    pulseAna_p->LoadAndAnalyzeData(HGSamples, LGSamples, HGSamplesDelayed, LGSamplesDelayed);
  }
  else {
    pulseAna_p->LoadAndAnalyzeData(HGSamplesDelayed, LGSamplesDelayed, HGSamples, LGSamples);
  }
  m_dataLoaded[side][module] = true;

  if (pulseAna_p->Failed()) {
    (*m_msgFunc_p)(ZDCMsg::Debug, ("ZDCPulseAnalyzer::LoadData() returned fail for event " + std::to_string(m_eventCount) + ", side, module = " + std::to_string(side) + ", " + std::to_string(module)));

    m_fail[side] = true;
  }

  m_moduleStatus[side][module] = pulseAna_p->GetStatusMask();
}

bool ZDCDataAnalyzer::FinishEvent()
{
  // First make sure that all data is loaded. while we're at it, count how many modules on each side have a pulse
  //
  unsigned int sideNPulsesMod[2] = {0, 0};

  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      if (!m_dataLoaded[side][module] && !m_moduleDisabled[side][module]) return false;
      if (m_moduleAnalyzers[side][module]->ArmSumInclude()) sideNPulsesMod[side]++;
    }
  }

  // Are we doing a repass? If so, reanalyze modules for which no pulse was found the first time
  //   as long as we have one module with a pulse on the given side
  //
  if (m_repassEnabled) {
    for (size_t side : {0, 1}) {
      if (sideNPulsesMod[side] == 0) continue;

      for (size_t module : {0, 1, 2, 3}) {
        ZDCPulseAnalyzer* pulseAna_p = m_moduleAnalyzers[side][module].get();

        // If this module had no pulse the first time, reanalyze it (with a lower 2nd derivative threshold)
        //
        if (!pulseAna_p->HavePulse()) {
          (*m_msgFunc_p)(ZDCMsg::Debug, ("ZDCPulseAnalyzer:: performing a repass on data for side, module = " + std::to_string(side) + ", " + std::to_string(module)));
          pulseAna_p->ReanalyzeData();
        }
      }
    }
  }

  // Now sum up amplitudes etc
  //
  for (size_t side : {0, 1}) {
    for (size_t module : {0, 1, 2, 3}) {
      ZDCPulseAnalyzer* pulseAna_p = m_moduleAnalyzers[side][module].get();

      if (pulseAna_p->ArmSumInclude()) {
        int moduleMaskBit = 4 * side + module;
        m_moduleMask |= 1 << moduleMaskBit;

        float amplitude = pulseAna_p->GetAmplitude();
        float ampError = pulseAna_p->GetAmpError();

        m_calibAmplitude[side][module] = amplitude * m_currentECalibCoeff[side][module];

        float calibAmpError = ampError * m_currentECalibCoeff[side][module];

        float timeCalib = pulseAna_p->GetT0Corr();
        if (pulseAna_p->UseLowGain()) timeCalib -= m_currentT0OffsetsLG[side][module];
        else timeCalib -= m_currentT0OffsetsHG[side][module];

        m_calibTime[side][module] = timeCalib;

        m_moduleSum[side] += amplitude;
        m_moduleSumErrSq[side] += ampError * ampError;

        m_moduleSumPreSample[side] += pulseAna_p->GetPreSampleAmp();

        m_calibModuleSum[side] += m_calibAmplitude[side][module];
        m_calibModuleSumErrSq[side] += calibAmpError * calibAmpError;

        m_averageTime[side] += m_calibTime[side][module] * m_calibAmplitude[side][module];
      }
    }
  }

  // Finish calculation of energy-weighted times
  //
  for (size_t side : {0, 1}) {
    if (m_calibModuleSum[side] > 1e-6) {
      m_averageTime[side] /= m_calibModuleSum[side];
    }
    else {
      m_averageTime[side] = 0;
    }
  }

  m_eventCount++;
  return true;
}
