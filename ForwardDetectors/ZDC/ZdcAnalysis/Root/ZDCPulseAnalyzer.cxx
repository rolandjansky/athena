/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCPulseAnalyzer.h"

#include <numeric>
#include <algorithm>
#include <sstream>
#include <cmath>

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TVirtualFitter.h"
#include "TFitter.h"
#include "TList.h"
#include "TMinuit.h"


extern int gErrorIgnoreLevel;

bool ZDCPulseAnalyzer::s_quietFits = true;
std::string ZDCPulseAnalyzer::s_fitOptions = "";
TH1* ZDCPulseAnalyzer::s_undelayedFitHist = 0;
TH1* ZDCPulseAnalyzer::s_delayedFitHist = 0;
TF1* ZDCPulseAnalyzer::s_combinedFitFunc = 0;
float ZDCPulseAnalyzer::s_combinedFitTMax = 1000;
float ZDCPulseAnalyzer::s_combinedFitTMin = -0.5;   // add to allow switch to high gain by skipping early samples
std::vector<float> ZDCPulseAnalyzer::pullValues;

void ZDCPulseAnalyzer::CombinedPulsesFCN(int& /*numParam*/, double*, double& f, double* par, int flag)
{
  //  The first parameter is a correction factor to account for decrease in beam intensity between x
  //    and y scan. It is applied here and not passed to the actual fit function
  //
  int nSamples = s_undelayedFitHist->GetNbinsX();

  if (flag == 3) {
    pullValues.assign(nSamples * 2, 0);
  }

  double chiSquare = 0;

  float delayBaselineAdjust = par[0];

  // if (s_delayedFitHist->GetNbinsX() != nSamples) throw std::exception();

  // undelayed
  //
  for (int isample = 0; isample < nSamples; isample++) {
    double histValue = s_undelayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(s_undelayedFitHist->GetBinError(isample + 1), 1.0);
    double t = s_undelayedFitHist->GetBinCenter(isample + 1);

    if (t > s_combinedFitTMax) break;
    if (t < s_combinedFitTMin) continue;

    double funcVal = s_combinedFitFunc->EvalPar(&t, &par[1]);

    double pull = (histValue - funcVal) / histError;

    if (flag == 3) pullValues[2 * isample] = pull;
    chiSquare += pull * pull;
  }

  // delayed
  //
  for (int isample = 0; isample < nSamples; isample++) {
    double histValue = s_delayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(s_delayedFitHist->GetBinError(isample + 1), 1.0);
    double t = s_delayedFitHist->GetBinCenter(isample + 1);

    if (t > s_combinedFitTMax) break;
    if (t < s_combinedFitTMin) continue;

    double funcVal = s_combinedFitFunc->EvalPar(&t, &par[1]) + delayBaselineAdjust;
    double pull = (histValue - funcVal) / histError;

    if (flag == 3) pullValues[2 * isample + 1] = pull;
    chiSquare += pull * pull;
  }

  f = chiSquare;
}


ZDCPulseAnalyzer::ZDCPulseAnalyzer(ZDCMsg::MessageFunctionPtr msgFunc_p, std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal,
                                   float gainHG, std::string fitFunction, int peak2ndDerivMinSample,
                                   float peak2ndDerivMinThreshHG, float peak2ndDerivMinThreshLG) :
  m_msgFunc_p(msgFunc_p),
  m_tag(tag), m_Nsample(Nsample),
  m_preSampleIdx(preSampleIdx),
  m_deltaTSample(deltaTSample),
  m_pedestal(pedestal), m_gainHG(gainHG), m_forceLG(false), m_fitFunction(fitFunction),
  m_peak2ndDerivMinSample(peak2ndDerivMinSample),
  m_peak2ndDerivMinTolerance(1),
  m_peak2ndDerivMinThreshLG(peak2ndDerivMinThreshLG),
  m_peak2ndDerivMinThreshHG(peak2ndDerivMinThreshHG),
  m_haveTimingCorrections(false), m_haveNonlinCorr(false), m_initializedFits(false),
  m_defaultFitWrapper(0), m_prePulseFitWrapper(0),
  m_useDelayed(false), m_delayedHist(0), m_prePulseCombinedFitter(0), m_defaultCombinedFitter(0),
  m_ADCSamplesHGSub(Nsample, 0), m_ADCSamplesLGSub(Nsample, 0),
  m_ADCSSampSigHG(Nsample, 1), m_ADCSSampSigLG(Nsample, 1), // By default the ADC uncertainties are set to one
  m_samplesSub(Nsample, 0)
{
  // Create the histogram used for fitting
  //
  m_tmin = -deltaTSample / 2;
  m_tmax = m_tmin + ((float) Nsample) * deltaTSample;

  std::string histName = "ZDCFitHist" + tag;

  m_fitHist = new TH1F(histName.c_str(), "", m_Nsample, m_tmin, m_tmax);
  m_fitHist->SetDirectory(0);

  SetDefaults();
  Reset();
}

ZDCPulseAnalyzer::~ZDCPulseAnalyzer()
{
  if (m_defaultFitWrapper)  delete m_defaultFitWrapper;
  if (m_prePulseFitWrapper) delete m_prePulseFitWrapper;

  // if (m_fitHist)     delete m_fitHist;
  // if (m_delayedHist) delete m_delayedHist;
}

void ZDCPulseAnalyzer::EnableDelayed(float deltaT, float pedestalShift, bool fixedBaseline)
{
  m_useDelayed = true;
  m_useFixedBaseline = fixedBaseline;

  m_delayedDeltaT = deltaT;
  m_delayedPedestalDiff = pedestalShift;

  m_deltaTSample /= 2.;

  m_defaultT0Max = m_deltaTSample * (m_peak2ndDerivMinSample + m_peak2ndDerivMinTolerance + 0.5);
  m_defaultT0Min = m_deltaTSample * (m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance - 0.5);

  m_delayedHist = new TH1F((std::string(m_fitHist->GetName()) + "delayed").c_str(), "", m_Nsample, m_tmin + m_delayedDeltaT, m_tmax + m_delayedDeltaT);
  m_delayedHist->SetDirectory(0);

  m_ADCSamplesHGSub.assign(2 * m_Nsample, 0);
}

void ZDCPulseAnalyzer::SetDefaults()
{
  m_nominalTau1 = 4;
  m_nominalTau2 = 21;

  m_fixTau1 = false;
  m_fixTau2 = false;

  m_HGOverflowADC  = 900;
  m_HGUnderflowADC = 20;
  m_LGOverflowADC  = 1000;

  m_chisqDivAmpCutLG = 100;
  m_chisqDivAmpCutHG = 100;

  m_T0CutLowLG = m_tmin;
  m_T0CutLowHG = m_tmin;

  m_T0CutHighLG = m_tmax;
  m_T0CutHighHG = m_tmax;

  m_LGT0CorrParams.assign(4, 0);
  m_HGT0CorrParams.assign(4, 0);

  m_defaultFitTMax = m_tmax;
  m_defaultFitTMin = m_tmin;

  m_defaultT0Max = m_deltaTSample * (m_peak2ndDerivMinSample + m_peak2ndDerivMinTolerance + 1);
  m_defaultT0Min = m_deltaTSample * (m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance + 1);

  m_initialPrePulseT0  = -10;
  m_initialPrePulseAmp = 5;

  s_fitOptions = "s";
}

void ZDCPulseAnalyzer::Reset()
{
  m_haveData  = false;
  m_havePulse = false;
  m_fail      = false;

  m_HGOverflow  = false;
  m_HGUnderflow = false;

  m_LGOverflow  = false;
  m_LGUnderflow = false;

  m_PSHGOverUnderflow = false;

  m_preExpTail = false;
  m_prePulse   = false;
  m_postPulse  = false;
  m_fitFailed  = false;

  m_useLowGain = false;

  m_badT0    = false;
  m_badChisq = false;

  m_delayedBaselineShift = 0;

  m_fitAmplitude = 0;
  m_fitTime      = -100;
  m_fitTimeSub   = -100;
  m_fitTimeCorr  = -100;

  m_fitChisq = 0;

  m_amplitude       = 0;
  m_ampError        = 0;
  m_preSampleAmp    = 0;
  m_bkgdMaxFraction = 0;

  m_minSampleEvt = 0;
  m_maxSampleEvt = (m_useDelayed ? 2 * m_Nsample - 1 : m_Nsample - 1);

  m_fitTMax = m_defaultFitTMax;
  m_fitTMin = m_defaultFitTMin;

  m_adjTimeRangeEvent = false;

  m_backToHG_pre  = false;
  m_lastHGOverFlowSample  = -999;
  m_firstHGOverFlowSample = 999;

  m_initialPrePulseT0  = -10;
  m_initialPrePulseAmp = 5;

  m_samplesSub.clear();
  m_samplesDeriv.clear();
  m_samplesDeriv2nd.clear();

  int sampleVecSize = m_Nsample;
  if (m_useDelayed) sampleVecSize *= 2;

  m_ADCSamplesHGSub.assign(sampleVecSize, 0);
  m_ADCSamplesLGSub.assign(sampleVecSize, 0);
}

void ZDCPulseAnalyzer::SetTauT0Values(bool fixTau1, bool fixTau2, float tau1, float tau2, float t0HG, float t0LG)
{
  m_fixTau1     = fixTau1;
  m_fixTau2     = fixTau2;
  m_nominalTau1 = tau1;
  m_nominalTau2 = tau2;

  m_nominalT0HG = t0HG;
  m_nominalT0LG = t0LG;

  std::ostringstream ostrm;
  ostrm << "ZDCPulseAnalyzer::SetTauT0Values:: m_fixTau1=" << m_fixTau1 << "  m_fixTau2=" << m_fixTau2 << "  m_nominalTau1=" << m_nominalTau1 << "  m_nominalTau2=" << m_nominalTau2 << "  m_nominalT0HG=" << m_nominalT0HG << "  m_nominalT0LG=" << m_nominalT0LG;

  (*m_msgFunc_p)(ZDCMsg::Info, ostrm.str());

  m_initializedFits = false;
}

void ZDCPulseAnalyzer::SetFitTimeMax(float tmax)
{
  if (tmax < m_tmin) {
    (*m_msgFunc_p)(ZDCMsg::Error, ("ZDCPulseAnalyzer::SetFitTimeMax:: invalid FitTimeMax: " + std::to_string(tmax)));
    return;
  }

  m_defaultFitTMax = std::min(tmax, m_defaultFitTMax);

  (*m_msgFunc_p)(ZDCMsg::Verbose, ("Setting FitTMax to " + std::to_string(m_defaultFitTMax)));

  if (m_initializedFits) SetupFitFunctions();
}

void ZDCPulseAnalyzer::SetADCOverUnderflowValues(int HGOverflowADC, int HGUnderflowADC, int LGOverflowADC)
{
  m_HGOverflowADC  = HGOverflowADC;
  m_LGOverflowADC  = LGOverflowADC;
  m_HGUnderflowADC = HGUnderflowADC;
}

void ZDCPulseAnalyzer::SetCutValues(float chisqDivAmpCutHG, float chisqDivAmpCutLG,
                                    float deltaT0MinHG, float deltaT0MaxHG,
                                    float deltaT0MinLG, float deltaT0MaxLG)
{
  m_chisqDivAmpCutHG = chisqDivAmpCutHG;
  m_chisqDivAmpCutLG = chisqDivAmpCutLG;

  m_T0CutLowHG = deltaT0MinHG;
  m_T0CutLowLG = deltaT0MinLG;

  m_T0CutHighHG = deltaT0MaxHG;
  m_T0CutHighLG = deltaT0MaxLG;
}

void ZDCPulseAnalyzer::SetupFitFunctions()
{
  if (m_defaultFitWrapper)  delete m_defaultFitWrapper;
  if (m_prePulseFitWrapper) delete m_prePulseFitWrapper;

  float prePulseTMin = 0;
  float prePulseTMax = prePulseTMin + m_deltaTSample * (m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance);

  if (m_fitFunction == "FermiExp") {
    if (!m_fixTau1 || !m_fixTau2) {
      //
      // Use the variable tau version of the expFermiFit
      //
      m_defaultFitWrapper = new ZDCFitExpFermiVariableTaus(m_tag, m_tmin, m_tmax, m_fixTau1, m_fixTau2, m_nominalTau1, m_nominalTau2);
    }
    else {
      m_defaultFitWrapper = new ZDCFitExpFermiFixedTaus(m_tag, m_tmin, m_tmax, m_nominalTau1, m_nominalTau2);
    }

    m_prePulseFitWrapper = new ZDCFitExpFermiPrePulse(m_tag, m_tmin, m_tmax, m_nominalTau1, m_nominalTau2);
  }
  else if (m_fitFunction == "FermiExpLinear") {
    if (!m_fixTau1 || !m_fixTau2) {
      //
      // Use the variable tau version of the expFermiFit
      //
      m_defaultFitWrapper = new ZDCFitExpFermiVariableTaus(m_tag, m_tmin, m_tmax, m_fixTau1, m_fixTau2, m_nominalTau1, m_nominalTau2);
    }
    else {
      m_defaultFitWrapper = new ZDCFitExpFermiLinearFixedTaus(m_tag, m_tmin, m_tmax, m_nominalTau1, m_nominalTau2);
    }

    m_prePulseFitWrapper = new ZDCFitExpFermiLinearPrePulse(m_tag, m_tmin, m_tmax, m_nominalTau1, m_nominalTau2);
  }
  else {
    (*m_msgFunc_p)(ZDCMsg::Fatal, "Wrong fit function type.");
  }

  m_defaultFitWrapper->SetT0Range(m_defaultT0Min, m_defaultT0Max);
  m_prePulseFitWrapper->SetT0Range(m_defaultT0Min, m_defaultT0Max);
  m_prePulseFitWrapper->SetPrePulseT0Range(prePulseTMin, prePulseTMax);

  m_initializedFits = true;
}

bool ZDCPulseAnalyzer::LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float>  ADCSamplesLG)
{
  if (m_useDelayed) {
    (*m_msgFunc_p)(ZDCMsg::Fatal, "ZDCPulseAnalyzer::LoadAndAnalyzeData:: Wrong LoadAndAnalyzeData called -- expecting both delayed and undelayed samples");
  }

  if (!m_initializedFits) SetupFitFunctions();

  // Clear any transient data
  //
  Reset();

  // Make sure we have the right number of samples. Should never fail. necessry?
  //
  if (ADCSamplesHG.size() != m_Nsample || ADCSamplesLG.size() != m_Nsample) {
    m_fail = true;
    return false;
  }

  // Now do pedestal subtraction and check for overflows
  //
  for (size_t isample = 0; isample < m_Nsample; isample++) {
    float ADCHG = ADCSamplesHG[isample];
    float ADCLG = ADCSamplesLG[isample];

    if (ADCHG > m_HGOverflowADC) {
      m_HGOverflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow = true;
    }
    else if (ADCHG < m_HGUnderflowADC) {
      m_HGUnderflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow  = true;
    }

    if (ADCLG > m_LGOverflowADC) {
      m_LGOverflow = true;
      m_fail = true;
      m_amplitude = 1024 * m_gainHG; // Give a vale here even though we know it's wrong because
      //   the user may not check the return value and we know that
      //   amplitude is bigger than this
    }

    if (ADCLG == 0) {
      m_LGUnderflow = true;
      m_fail = true;
    }

    m_ADCSamplesHGSub[isample] = ADCHG - m_pedestal;
    m_ADCSamplesLGSub[isample] = ADCLG - m_pedestal;
  }

  //  if (m_fail) return false;

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  m_useLowGain = m_HGUnderflow || m_HGOverflow || m_forceLG;

  if (m_useLowGain) {
    bool result = AnalyzeData(m_Nsample, m_preSampleIdx, m_ADCSamplesLGSub, m_ADCSSampSigLG, m_LGT0CorrParams, m_chisqDivAmpCutLG,
                              m_T0CutLowLG, m_T0CutHighLG, m_peak2ndDerivMinThreshLG);
    if (result) {
      //
      // Multiply amplitude by gain factor
      //
      m_amplitude = m_fitAmplitude * m_gainHG;
      m_ampError = m_fitAmpError * m_gainHG;
      m_preSampleAmp = m_preSample * m_gainHG;
    }

    return result;
  }
  else {
    bool result = AnalyzeData(m_Nsample, m_preSampleIdx, m_ADCSamplesHGSub, m_ADCSSampSigHG, m_HGT0CorrParams, m_chisqDivAmpCutHG,
                              m_T0CutLowHG, m_T0CutHighHG, m_peak2ndDerivMinThreshHG);
    if (result) {
      m_preSampleAmp = m_preSample;
      m_amplitude = m_fitAmplitude;
      m_ampError = m_fitAmpError;

      //  If we have a non-linear correction, apply it here
      //
      if (m_haveNonlinCorr) {
        float ampCorrFact = m_amplitude / 1000. - 0.5;
        float corrFactor = 1. / (1. + m_nonLinCorrParams[0] * ampCorrFact + m_nonLinCorrParams[1] * ampCorrFact * ampCorrFact);

        m_amplitude *= corrFactor;
        m_ampError *= corrFactor;
      }
    }

    return result;
  }
}

bool ZDCPulseAnalyzer::LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float>  ADCSamplesLG,
    std::vector<float> ADCSamplesHGDelayed, std::vector<float> ADCSamplesLGDelayed)
{
  if (!m_useDelayed) {
    (*m_msgFunc_p)(ZDCMsg::Fatal, "ZDCPulseAnalyzer::LoadAndAnalyzeData:: Wrong LoadAndAnalyzeData called -- expecting only undelayed samples");
  }

  if (!m_initializedFits) SetupFitFunctions();

  // Clear any transient data
  //
  Reset();

  // Make sure we have the right number of samples. Should never fail. necessry?
  //
  if (ADCSamplesHG.size() != m_Nsample || ADCSamplesLG.size() != m_Nsample ||
      ADCSamplesHGDelayed.size() != m_Nsample || ADCSamplesLGDelayed.size() != m_Nsample) {
    m_fail = true;
    return false;
  }

  // Now do pedestal subtraction and check for overflows
  //
  for (size_t isample = 0; isample < m_Nsample; isample++) {
    float ADCHG = ADCSamplesHG[isample];
    float ADCLG = ADCSamplesLG[isample];

    float ADCHGDelay = ADCSamplesHGDelayed[isample];
    float ADCLGDelay = ADCSamplesLGDelayed[isample];

    int undelSampIdx = 2 * isample;
    int delSampIdx = 2 * isample + 1;

    if (ADCHG > m_HGOverflowADC) {
      m_HGOverflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow = true;
      if (undelSampIdx > m_lastHGOverFlowSample)  m_lastHGOverFlowSample  = undelSampIdx;
      if (undelSampIdx < m_firstHGOverFlowSample) m_firstHGOverFlowSample = undelSampIdx;
    }
    else if (ADCHG < m_HGUnderflowADC) {
      m_HGUnderflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow  = true;
    }

    if (ADCHGDelay > m_HGOverflowADC) {
      if (delSampIdx > m_lastHGOverFlowSample)  m_lastHGOverFlowSample  = delSampIdx;
      if (delSampIdx < m_firstHGOverFlowSample) m_firstHGOverFlowSample = delSampIdx;

      m_HGOverflow = true;
    }
    else if (ADCHGDelay < m_HGUnderflowADC) {
      m_HGUnderflow = true;
    }

    if (ADCLG > m_LGOverflowADC) {
      m_LGOverflow = true;
      m_fail = true;
      m_amplitude = 1024 * m_gainHG; // Give a value here even though we know it's wrong because
      //   the user may not check the return value and we know that
      //   amplitude is bigger than this
    }
    else if (ADCLG == 0) {
      m_LGUnderflow = true;
      m_fail = true;
    }

    if (ADCLGDelay > m_LGOverflowADC) {
      m_LGOverflow = true;
      m_fail = true;
      m_amplitude = 1024 * m_gainHG; // Give a value here even though we know it's wrong because
      //   the user may not check the return value and we know that
      //   amplitude is bigger than this
    }
    else if (ADCLGDelay == 0) {
      m_LGUnderflow = true;
      m_fail = true;
    }

    m_ADCSamplesHGSub[isample * 2] = ADCHG - m_pedestal;
    m_ADCSamplesLGSub[isample * 2] = ADCLG - m_pedestal;

    m_ADCSamplesHGSub[isample * 2 + 1] = ADCHGDelay - m_pedestal - m_delayedPedestalDiff;
    m_ADCSamplesLGSub[isample * 2 + 1] = ADCLGDelay - m_pedestal - m_delayedPedestalDiff;
  }

  // ------------------------------------------------------
  // Determine whether we can avoid the use of low gain by excluding early or late samples
  //
  if (m_HGOverflow) {
    if (m_lastHGOverFlowSample < 2 && m_lastHGOverFlowSample > -1) {
      m_HGOverflow = false;
      m_minSampleEvt = m_lastHGOverFlowSample + 1;
      m_fitTMin = std::max(m_fitTMin, m_deltaTSample * (m_lastHGOverFlowSample + 1) + m_deltaTSample / 2);
      m_adjTimeRangeEvent = true;
      m_backToHG_pre = true;
    }
  }
  else {
    //
    // Test to see with the sample just before we measure could have overflowed
    //  ??? why does backExtrapolate work
    float backExtrapolate = m_ADCSamplesHGSub[0] + m_pedestal + m_ADCSamplesHGSub[0] - m_ADCSamplesHGSub[1];
    if (backExtrapolate > m_HGOverflowADC) {
      m_minSampleEvt = 1;
      m_fitTMin = std::max(m_fitTMin, m_deltaTSample - m_deltaTSample / 2);
      m_adjTimeRangeEvent = true;
    }
  }
  if (m_firstHGOverFlowSample > 11 && m_firstHGOverFlowSample < 14) {
    m_maxSampleEvt = m_firstHGOverFlowSample;
    m_HGOverflow = false;
    m_fitTMax = std::min(m_fitTMax, m_deltaTSample * m_firstHGOverFlowSample - m_deltaTSample / 2);
    m_adjTimeRangeEvent = true;
  }
  // ------------------------------------------------------

  // Even if none of the samples overflows the HG range, if there is a preceeding pulse with a steep slope,
  //   the ADC could still have overflowed immediately preceeding the first sample, and we know that the next
  //   sample after an overflow is distorted.
  //
  //

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  m_useLowGain = m_HGUnderflow || m_HGOverflow || m_forceLG;

  if (m_useLowGain) {

    bool result = AnalyzeData(m_Nsample * 2, m_preSampleIdx * 2, m_ADCSamplesLGSub, m_ADCSSampSigLG, m_LGT0CorrParams, m_chisqDivAmpCutLG,
                              m_T0CutLowLG, m_T0CutHighLG, m_peak2ndDerivMinThreshLG);
    if (result) {
      //
      // Multiply amplitude by gain factor
      //
      m_amplitude = m_fitAmplitude * m_gainHG;
      m_ampError = m_fitAmpError * m_gainHG;
      m_preSampleAmp = m_preSample * m_gainHG;
    }

    return result;
  }
  else {
    bool result = AnalyzeData(m_Nsample * 2, m_preSampleIdx * 2, m_ADCSamplesHGSub, m_ADCSSampSigHG, m_HGT0CorrParams, m_chisqDivAmpCutHG,
                              m_T0CutLowHG, m_T0CutHighHG, m_peak2ndDerivMinThreshHG);
    if (result) {
      m_preSampleAmp = m_preSample;
      m_amplitude = m_fitAmplitude;
      m_ampError = m_fitAmpError;

      //  If we have a non-linear correction, apply it here
      //
      if (m_haveNonlinCorr) {
        float ampCorrFact = m_amplitude / 1000. - 0.5;
        float corrFactor = 1. / (1. + m_nonLinCorrParams[0] * ampCorrFact + m_nonLinCorrParams[1] * ampCorrFact * ampCorrFact);

        m_amplitude *= corrFactor;
        m_ampError *= corrFactor;
      }
    }

    return result;
  }
}

bool ZDCPulseAnalyzer::AnalyzeData(size_t nSamples, size_t preSampleIdx,
                                   const std::vector<float>& samples,        // The samples used for this event
                                   const std::vector<float>& samplesSig,     // The "resolution" on the ADC value
                                   const std::vector<float>& t0CorrParams,   // The parameters used to correct the t0
                                   float maxChisqDivAmp,                     // The maximum chisq / amplitude ratio
                                   float minT0Corr, float maxT0Corr,         // The minimum and maximum corrected T0 values
                                   float peak2ndDerivMinThresh
                                  )
{
  //  Do the presample subtraction using a lambda function
  //
  int usedPresample = preSampleIdx;

  if (m_adjTimeRangeEvent) {
    usedPresample = m_minSampleEvt + 1;
  }

  m_preSample = samples[usedPresample];

  m_samplesSub = samples;

  if (m_useDelayed) {
    if (m_useFixedBaseline) {
      m_baselineCorr = m_delayedPedestalDiff;
    }
    else {
      //  Attempt to address up front cases where we have significant offsets between the delayed and undelayed
      //
      if (m_peak2ndDerivMinSample > 4) {
        //
        // If we have enough samples to do a proper interpolation do so
        //

        // Check the slope in the first two samples form delayed and from undelayed
        //
        float slope1 = m_samplesSub[2] - m_samplesSub[0] + 1e-3;
        float slope2 = m_samplesSub[3] - m_samplesSub[1] + 1e-3;

        // We see inconsistent slopes in the initial samples, revert to nominal pedestal diff
        //
        if ((std::abs(slope1) > 5 || std::abs(slope2) > 5) && (slope1 / slope2 < 0 || std::abs(slope1 / slope2 - 1) > 1))  {
          m_baselineCorr = m_delayedPedestalDiff;
        }
        else {
          if (m_backToHG_pre) {
            m_baselineCorr =  m_samplesSub[3] - exp((log(m_samplesSub[2]) + log(m_samplesSub[4])) * 0.5);
          }
          else {
            if (m_preSample < 100) {
              // If we don't have a large pre-pulse use a linear interpolation
              //
              m_baselineCorr = (0.5 * (m_samplesSub[1] - m_samplesSub[0] + m_samplesSub[3] - m_samplesSub[2]) - 0.25 * (m_samplesSub[3] - m_samplesSub[1] + m_samplesSub[2] - m_samplesSub[0]));
            }
            else {
              // We have a large pre-pulse, use exponential interpolation
              //
              m_baselineCorr = m_samplesSub[1] - exp((log(m_samplesSub[0]) + log(m_samplesSub[2])) * 0.5);
            }
          }
        }
      }
      else {
        //
        // Otherwise do the simplest thing possible
        //
        m_baselineCorr = m_samplesSub[1] - m_samplesSub[0];
      }
    }

    // Now apply the baseline correction to align ADC values for delayed and undelayed samples
    //
    for (size_t isample = 0; isample < nSamples; isample++) {
      if (isample % 2) m_samplesSub[isample] -= m_baselineCorr;
    }
  }

  std::for_each(m_samplesSub.begin(), m_samplesSub.end(), [ = ] (float & adcUnsub) {return adcUnsub -= m_preSample;} );

  // Find maximum and minimum values
  //
  std::pair<SampleCIter, SampleCIter> minMaxIters = std::minmax_element(m_samplesSub.begin(), m_samplesSub.end());
  SampleCIter minIter = minMaxIters.first;
  SampleCIter maxIter = minMaxIters.second;

  m_maxADCValue = *maxIter;
  m_minADCValue = *minIter;

  m_maxSampl = std::distance(m_samplesSub.cbegin(), maxIter);
  m_minSampl = std::distance(m_samplesSub.cbegin(), minIter);

  // Calculate first and 2nd "derivatives"
  //
  std::vector<float> deriv(nSamples, 0);
  std::adjacent_difference(m_samplesSub.begin(), m_samplesSub.end(), deriv.begin());

  // Save the derivatives, dropping the useless first element
  //
  m_samplesDeriv.insert(m_samplesDeriv.begin(), deriv.begin() + 1, deriv.end());

  std::vector<float> deriv2nd(nSamples, 0);
  std::adjacent_difference(m_samplesDeriv.begin(), m_samplesDeriv.end(), deriv2nd.begin());

  // Save the second derivatives, dropping the useless first element
  //
  m_samplesDeriv2nd.insert(m_samplesDeriv2nd.begin(), deriv2nd.begin() + 1, deriv2nd.end());

  // Find the sample which has the lowest 2nd derivative. We loop over the range defined by the
  //  tolerance on the position of the minimum second derivative. Note: the +1 in the upper iterator is because
  //  that's where the loop terminates, not the last element.
  //
  SampleCIter minDeriv2ndIter = std::min_element(m_samplesDeriv2nd.begin() + m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance, m_samplesDeriv2nd.begin() + m_peak2ndDerivMinSample + m_peak2ndDerivMinTolerance + 1);

  m_minDeriv2nd = *minDeriv2ndIter;
  m_minDeriv2ndIndex = std::distance(m_samplesDeriv2nd.cbegin(), minDeriv2ndIter);

  //  The ket test for presence of a pulse: is the minimum 2nd derivative in the right place and is it
  //    large enough (sufficiently negative)
  //
  if (m_minDeriv2nd <= peak2ndDerivMinThresh) {
    m_havePulse = true;
  }
  else {
    m_havePulse = false;
  }

  // Now decide whether we have a preceeding pulse or not. There are two possible kinds of preceeding pulses:
  //   1) exponential tail from a preceeding pulse
  //   2) peaked pulse before the main pulse
  //
  //   we can, of course, have both
  //

  // To check for exponential tail, test the slope determined by the minimum ADC value (and pre-sample)
  // **beware** this can cause trouble in 2015 data where the pulses had overshoot due to the transformers
  //
  float expSlopeTest = m_minADCValue / std::max((float) m_maxADCValue - m_minADCValue, (float) 1.0);
  if (expSlopeTest < -0.05) {
    m_preExpTail = true;
  }

  bool lastPosDeriv = false;

  int loopLimit = (m_havePulse ? m_minDeriv2ndIndex - 2 : m_peak2ndDerivMinSample - 2);
  for (int isampl = m_minSampleEvt; isampl < loopLimit; isampl++) {

    // If any of the derivatives prior to the peak (as determined by the 2nd derivative) are significantly positive
    //   then we have a peaked pre-pulse
    //
    if (m_samplesDeriv[isampl] > (m_maxADCValue - m_minADCValue + 1) * 0.02) {

      //
      //  Put a threshold on the derivative as a fraction of the difference between max and min so that
      //     we don't waste lots of time for insignificant prepulses.
      //
      if (m_samplesDeriv[isampl] / (m_maxADCValue - m_minADCValue + 1) > 0.05) {  // ??? why no abs()
        m_prePulse = true;
      }

      // Also, if we have two positive derivatives in a row, we have a prepulse
      //
      if (lastPosDeriv) {
        m_prePulse = true;
      }
      else lastPosDeriv = true;
    }
    else {
      lastPosDeriv = false;
    }

    //  If any of the 2nd derivatives before the minium are negative, we have the possibility of positive
    //    preceeding pulse
    //
    if (m_samplesDeriv2nd[isampl] < 0.25 * m_minDeriv2nd && m_samplesDeriv2nd[isampl + 1] > 1) {
      m_prePulse = true;
    }
  }

  if (m_preExpTail) {
    m_initialPrePulseT0 = -20;
    m_initialPrePulseAmp = m_samplesSub[m_minSampleEvt];
  }

  if (m_prePulse) {
    // If we have a prepulse, find the smallest 2nd derivative
    //
    SampleCIter minPreDeriv2ndIter = std::min_element(m_samplesDeriv2nd.begin() + m_minSampleEvt,
                                     m_samplesDeriv2nd.begin() + loopLimit + 1);

    int minPreDeriv2ndIndex = std::distance(m_samplesDeriv2nd.cbegin(), minPreDeriv2ndIter);
    float minPreDeriv2nd = *minPreDeriv2ndIter;

    SampleCIter minSamplePreIter = std::min_element(m_samplesSub.begin() + std::max(m_minSampleEvt, minPreDeriv2ndIndex), m_samplesSub.begin() + m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance + 1);

    float minSamplePre = *minSamplePreIter;
    int minSamplePreIndex = std::distance(m_samplesSub.cbegin(), minSamplePreIter);

    float avgSlopePre = (m_samplesSub[minSamplePreIndex] - m_samplesSub[m_minSampleEvt]) / ((float) (minSamplePreIndex - m_minSampleEvt + 1e-3));

    if (!m_preExpTail || !(expSlopeTest / minPreDeriv2nd > 0.25 || avgSlopePre / minPreDeriv2nd > 0.75)) {
      m_initialPrePulseT0 = m_deltaTSample * (minPreDeriv2ndIndex + 1);

      if (minPreDeriv2nd == m_samplesDeriv2nd[minPreDeriv2ndIndex + 1]) m_initialPrePulseT0 +=  m_deltaTSample;

      m_initialPrePulseAmp = m_samplesSub[minPreDeriv2ndIndex + 1] - minSamplePre;
    }
  }

  if (m_preExpTail) m_prePulse = true;


  // -----------------------------------------------------
  // Post pulse detection
  //
  for (int isampl = m_minDeriv2ndIndex + 2; isampl < (int) m_samplesDeriv2nd.size(); isampl++) {

    // add small 1e-3 in division to avoid floating overflow
    //

    float deriv2ndTest = 2 * m_samplesDeriv2nd[isampl] / (m_samplesDeriv[isampl] + m_samplesDeriv[isampl + 1] + 1e-3);

    // The place to apply the cut on samples depends on whether we have found a minimum or a maximum
    //   The +1 for the minimum accounts for the shift between 2nd derivative and the samples
    //   if we find a maximum we cut one sample lower
    //
    if (m_samplesDeriv2nd[isampl] > 0 && std::abs(deriv2ndTest) > 1.5) {
      m_postPulse = true;
      m_maxSampleEvt = std::min(isampl + 1, m_maxSampleEvt);
      m_fitTMax = std::min(m_fitTMax, m_deltaTSample * (isampl + 1) + m_deltaTSample / 2);
      m_adjTimeRangeEvent = true;
      break;
    }
    else if ((m_samplesDeriv2nd[isampl] < 0 && std::abs(deriv2ndTest) > 0.5)) {
      m_postPulse = true;
      m_maxSampleEvt = std::min(isampl, m_maxSampleEvt);
      m_fitTMax = std::min(m_fitTMax, m_deltaTSample * (isampl) + m_deltaTSample / 2);
      m_adjTimeRangeEvent = true;
      break;
    }
  }
  // -----------------------------------------------------

  FillHistogram(m_samplesSub, samplesSig);

  //  Stop now if we have no pulse or we've detected a failure
  //
  if (m_fail || !m_havePulse) return false;

  if (!m_useDelayed) DoFit();
  else DoFitCombined();

  if (FitFailed()) {
    m_fail = true;
  }
  else {
    m_fitTimeCorr = m_fitTimeSub;

    if (m_haveTimingCorrections) {

      //
      // We correct relative to the middle of the amplitude range, divided by 100 to make corrections smaller
      //
      float t0CorrFact = (m_fitAmplitude - 500) / 100.;

      float correction = (t0CorrParams[0] + t0CorrParams[1] * t0CorrFact + t0CorrParams[2] * t0CorrFact * t0CorrFact + t0CorrParams[3] * t0CorrFact * t0CorrFact * t0CorrFact);

      m_fitTimeCorr -= correction;
    }


    // Now check for valid chisq and valid time
    //
    if (m_fitChisq / (m_fitAmplitude + 1.0e-6) > maxChisqDivAmp) m_badChisq = true;
    if (m_fitTimeCorr < minT0Corr || m_fitTimeCorr > maxT0Corr) m_badT0 = true;
  }

  return !m_fitFailed;
}

void ZDCPulseAnalyzer::DoFit()
{
  // Set the initial values
  //
  float ampInitial = m_maxADCValue - m_minADCValue;
  float t0Initial = (m_useLowGain ? m_nominalT0LG : m_nominalT0HG);

  if (ampInitial < 5) ampInitial = 5.1;

  ZDCFitWrapper* fitWrapper = m_defaultFitWrapper;
  if (PrePulse()) fitWrapper = m_prePulseFitWrapper;

  fitWrapper->Initialize(ampInitial, t0Initial);
  if (PrePulse()) {
    //
    //
    (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetInitialPrePulse(m_initialPrePulseAmp, m_initialPrePulseT0);

    if (m_initialPrePulseT0 < 0) {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetPrePulseT0Range(-25, 0);
    }
    else {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetPrePulseT0Range(-m_deltaTSample / 2,
          (m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance)*m_deltaTSample);
    }
  }

  // Now perform the fit
  //
  std::string options = s_fitOptions;
  if (QuietFits()) options += "Q0";
  options += "s";

  TFitResultPtr result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str(), "", m_fitTMin, m_fitTMax);

  int fitStatus = result_ptr;
  if (fitStatus != 0) {
    TFitResultPtr try2Result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str(), "", m_fitTMin, m_fitTMax);
    if ((int) try2Result_ptr != 0) m_fitFailed = true;
  }
  else m_fitFailed = false;

  //  if (!m_fitFailed) {
  m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  if (std::abs(m_bkgdMaxFraction) > 0.25) {
    std::string tempOptions = options + "e";
    m_fitHist->Fit(fitWrapper->GetWrapperTF1(), tempOptions.c_str(), "", m_fitTMin, m_fitTMax);
    m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  }

  m_fitAmplitude = fitWrapper->GetAmplitude();
  m_fitTime = fitWrapper->GetTime();
  m_fitTimeSub = m_fitTime - t0Initial;
  m_fitChisq = result_ptr->Chi2();

  m_fitTau1 = fitWrapper->GetTau1();
  m_fitTau2 = fitWrapper->GetTau2();

  m_fitAmpError = fitWrapper->GetAmpError();
}

void ZDCPulseAnalyzer::DoFitCombined()
{
  // Set the initial values
  //
  float ampInitial = m_maxADCValue - m_minADCValue;
  float t0Initial = (m_useLowGain ? m_nominalT0LG : m_nominalT0HG);

  ZDCFitWrapper* fitWrapper = m_defaultFitWrapper;
  if (PrePulse()) fitWrapper = m_prePulseFitWrapper;

  // Initialize the fit wrapper for this eventm, specifying a
  //   per-event fit range if necessary
  //
  if (m_adjTimeRangeEvent) {
    fitWrapper->Initialize(ampInitial, t0Initial, m_fitTMin, m_fitTMax);
  }
  else {
    fitWrapper->Initialize(ampInitial, t0Initial);
  }

  if (PrePulse()) {
    //
    //
    (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetInitialPrePulse(m_initialPrePulseAmp, m_initialPrePulseT0);

    if (m_initialPrePulseT0 < 0) {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetPrePulseT0Range(-25, 0);
    }
    else {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetPrePulseT0Range(-m_deltaTSample / 2,
          (m_peak2ndDerivMinSample - m_peak2ndDerivMinTolerance)*m_deltaTSample);
    }
  }

  // Set up the virtual fitter
  //
  TFitter* theFitter = 0;

  if (PrePulse()) {
    if (m_prePulseCombinedFitter) delete m_prePulseCombinedFitter;
    m_prePulseCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());

    theFitter = m_prePulseCombinedFitter;
  }
  else {
    if (m_defaultCombinedFitter) delete m_defaultCombinedFitter;
    m_defaultCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());

    theFitter = m_defaultCombinedFitter;
  }

  // Set the static pointers to histograms and function for use in FCN
  //
  s_undelayedFitHist = m_fitHist;
  s_delayedFitHist = m_delayedHist;
  s_combinedFitFunc = fitWrapper->GetWrapperTF1();
  s_combinedFitTMax = m_fitTMax;
  s_combinedFitTMin = m_fitTMin;

  size_t numFitPar = theFitter->GetNumberTotalParameters();

  theFitter->GetMinuit()->fISW[4] = -1;

  // Now perform the fit
  //
  if (s_quietFits) {
    theFitter->GetMinuit()->fISW[4] = -1;

    int  ierr = 0;
    theFitter->GetMinuit()->mnexcm("SET NOWarnings", 0, 0, ierr);
  }
  else theFitter->GetMinuit()->fISW[4] = 0;

  // Only include baseline shift in fit for pre-pulses. Otherwise baseline matching should work
  //
  if (PrePulse()) {
    theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);
    theFitter->ReleaseParameter(0);
  }
  else {
    theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);
    theFitter->FixParameter(0);
  }

  double arglist[100];
  arglist[0] = 5000; // number of function calls
  arglist[1] = 0.01; // tolerance
  int status = theFitter->ExecuteCommand("MIGRAD", arglist, 2);

  double fitAmp = theFitter->GetParameter(1);
  if (status || fitAmp < 1) {

    // We first fit with no baseline adjust
    //
    theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);
    theFitter->FixParameter(0);

    if (fitAmp < 1) {
      if (m_adjTimeRangeEvent) {
        fitWrapper->Initialize(ampInitial, t0Initial, m_fitTMin, m_fitTMax);
      }
      else {
        fitWrapper->Initialize(ampInitial, t0Initial);
      }
    }

    status = theFitter->ExecuteCommand("MIGRAD", arglist, 2);
    if (status != 0) {
      theFitter->ReleaseParameter(0);
      m_fitFailed = true;
    }
    else {
      theFitter->ReleaseParameter(0);
      status = theFitter->ExecuteCommand("MIGRAD", arglist, 2);
      fitAmp = theFitter->GetParameter(1);

      if (status || fitAmp < 1) m_fitFailed = true;
      else m_fitFailed = false;
    }
  }
  else m_fitFailed = false;

  if (!s_quietFits) theFitter->GetMinuit()->fISW[4] = -1;

  std::vector<double> funcParams(numFitPar - 1);
  std::vector<double> funcParamErrs(numFitPar - 1);

  // Save the baseline shift between delayed and undelayed samples
  //
  m_delayedBaselineShift = theFitter->GetParameter(0);

  // Capture and store the fit function parameteds and errors
  //
  for (size_t ipar = 1; ipar < numFitPar; ipar++) {
    funcParams[ipar - 1] = theFitter->GetParameter(ipar);
    funcParamErrs[ipar - 1] = theFitter->GetParError(ipar);
  }

  s_combinedFitFunc->SetParameters(&funcParams[0]);
  s_combinedFitFunc->SetParErrors(&funcParamErrs[0]);

  // Capture the chi-square etc.
  //
  double chi2, edm, errdef;
  int nvpar, nparx;

  theFitter->GetStats(chi2, edm, errdef, nvpar, nparx);

  int ndf = 2 * m_Nsample - nvpar;

  s_combinedFitFunc->SetChisquare(chi2);
  s_combinedFitFunc->SetNDF(ndf);

  // add to list of functions
  s_undelayedFitHist->GetListOfFunctions()->Add(s_combinedFitFunc);
  s_delayedFitHist->GetListOfFunctions()->Add(s_combinedFitFunc);

  // Save the pull values from the last call to FCN
  //
  arglist[0] = 3; // number of function calls
  theFitter->ExecuteCommand("Cal1fcn", arglist, 1);
  m_fitPulls = pullValues;

  m_fitAmplitude = fitWrapper->GetAmplitude();
  m_fitTime = fitWrapper->GetTime();
  m_fitTimeSub = m_fitTime - t0Initial;
  m_fitChisq = chi2;

  m_fitTau1 = fitWrapper->GetTau1();
  m_fitTau2 = fitWrapper->GetTau2();

  m_fitAmpError = fitWrapper->GetAmpError();
}


TFitter* ZDCPulseAnalyzer::MakeCombinedFitter(TF1* func)
{
  TVirtualFitter::SetDefaultFitter("Minuit");

  size_t nFitParams = func->GetNpar() + 1;
  TFitter* fitter = new TFitter(nFitParams);

  fitter->GetMinuit()->fISW[4] = -1;
  fitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);

  for (size_t ipar = 0; ipar < nFitParams - 1; ipar++) {
    double parLimitLow, parLimitHigh;

    func->GetParLimits(ipar, parLimitLow, parLimitHigh);
    if (fabs(parLimitHigh / parLimitLow - 1) < 1e-6) {
      double value   = func->GetParameter(ipar);
      double lowLim  = std::min(value * 0.99, value * 1.01);
      double highLim = std::max(value * 0.99, value * 1.01);

      fitter->SetParameter(ipar + 1, func->GetParName(ipar), func->GetParameter(ipar), 0.01, lowLim, highLim);
      fitter->FixParameter(ipar + 1);
    }
    else {
      double value = func->GetParameter(ipar);
      if (value >= parLimitHigh)     value = parLimitHigh * 0.99;
      else if (value <= parLimitLow) value = parLimitLow * 1.01;

      fitter->SetParameter(ipar + 1, func->GetParName(ipar), value, 0.01, parLimitLow, parLimitHigh);
    }
  }

  fitter->SetFCN(ZDCPulseAnalyzer::CombinedPulsesFCN);

  return fitter;
}

void ZDCPulseAnalyzer::UpdateFitterTimeLimits(TFitter* fitter, ZDCFitWrapper* wrapper, bool prePulse)
{
  double parLimitLow, parLimitHigh;

  TF1* func_p = wrapper->GetWrapperTF1();
  func_p->GetParLimits(1, parLimitLow, parLimitHigh);

  fitter->SetParameter(2, func_p->GetParName(1), func_p->GetParameter(1), 0.01, parLimitLow, parLimitHigh);

  if (prePulse) {
    unsigned int parIndex = static_cast<ZDCPrePulseFitWrapper*>(wrapper)->GetPreT0ParIndex();

    func_p->GetParLimits(parIndex, parLimitLow, parLimitHigh);
    fitter->SetParameter(parIndex + 1, func_p->GetParName(parIndex), func_p->GetParameter(parIndex), 0.01, parLimitLow, parLimitHigh);
  }
}

void ZDCPulseAnalyzer::Dump() const
{
  (*m_msgFunc_p)(ZDCMsg::Info, ("ZDCPulseAnalyzer dump for tag = " + m_tag));

  (*m_msgFunc_p)(ZDCMsg::Info, ("Presample index, value = " + std::to_string(m_preSampleIdx) +  ", " + std::to_string(m_preSample)));

  if (m_useDelayed) {
    (*m_msgFunc_p)(ZDCMsg::Info, ("using delayed samples with delta T = " + std::to_string(m_delayedDeltaT) + ", and pedestalDiff == " +
                                  std::to_string(m_delayedPedestalDiff)));
  }

  std::ostringstream message1;
  message1 << "samplesSub ";
  for (size_t sample = 0; sample < m_samplesSub.size(); sample++) {
    message1 << ", [" << sample << "] = " << m_samplesSub[sample];
  }
  (*m_msgFunc_p)(ZDCMsg::Info, message1.str());


  std::ostringstream message2;
  message2 << "samplesDeriv ";
  for (size_t sample = 0; sample < m_samplesDeriv.size(); sample++) {
    message2 << ", [" << sample << "] = " << m_samplesDeriv[sample];
  }
  (*m_msgFunc_p)(ZDCMsg::Info, message2.str());


  std::ostringstream message3;
  message3 << "samplesDeriv2nd ";
  for (size_t sample = 0; sample < m_samplesDeriv2nd.size(); sample++) {
    message3 << ", [" << sample << "] = " << m_samplesDeriv2nd[sample];
  }
  (*m_msgFunc_p)(ZDCMsg::Info, message3.str());

  (*m_msgFunc_p)(ZDCMsg::Info, ("minimum 2nd deriv sample, value = " + std::to_string(m_minDeriv2ndIndex) + ", " + std::to_string(m_minDeriv2nd)));
}

void ZDCPulseAnalyzer::Dump_setting() const    // setting
{
  if (m_useDelayed) {
    (*m_msgFunc_p)(ZDCMsg::Info, ("using delayed samples with delta T = " + std::to_string(m_delayedDeltaT) + ", and pedestalDiff == " + std::to_string(m_delayedPedestalDiff)));
  }

  (*m_msgFunc_p)(ZDCMsg::Info, ("m_fixTau1 = " + std::to_string(m_fixTau1) + "  m_fixTau2=" + std::to_string(m_fixTau2) + "  m_nominalTau1=" + std::to_string(m_nominalTau1) + "  m_nominalTau2=" + std::to_string(m_nominalTau2) + "  m_nominalT0HG=" + std::to_string(m_nominalT0HG) + "  m_nominalT0LG=" + std::to_string(m_nominalT0LG)));

  (*m_msgFunc_p)(ZDCMsg::Info, ("m_defaultFitTMax = " + std::to_string(m_defaultFitTMax)));

  (*m_msgFunc_p)(ZDCMsg::Info, ("m_HGOverflowADC = " + std::to_string(m_HGOverflowADC) + "  m_HGUnderflowADC=" + std::to_string(m_HGUnderflowADC) + "  m_LGOverflowADC=" + std::to_string(m_LGOverflowADC)));

  (*m_msgFunc_p)(ZDCMsg::Info, ("m_chisqDivAmpCutLG = " + std::to_string(m_chisqDivAmpCutLG) + "  m_chisqDivAmpCutHG=" + std::to_string(m_chisqDivAmpCutHG)));

  (*m_msgFunc_p)(ZDCMsg::Info, ("m_T0CutLowLG = " + std::to_string(m_T0CutLowLG) + "  m_T0CutHighLG=" + std::to_string(m_T0CutHighLG) + "  m_T0CutLowHG=" + std::to_string(m_T0CutLowHG) + "  m_T0CutHighHG=" + std::to_string(m_T0CutHighHG)));
}

unsigned int ZDCPulseAnalyzer::GetStatusMask() const
{
  unsigned int statusMask = 0;

  if (HavePulse())  statusMask |= 1 << PulseBit;
  if (UseLowGain()) statusMask |= 1 << LowGainBit;
  if (Failed())     statusMask |= 1 << FailBit;
  if (HGOverflow()) statusMask |= 1 << HGOverflowBit;

  if (HGUnderflow())       statusMask |= 1 << HGUnderflowBit;
  if (PSHGOverUnderflow()) statusMask |= 1 << PSHGOverUnderflowBit;
  if (LGOverflow())        statusMask |= 1 << LGOverflowBit;
  if (LGUnderflow())       statusMask |= 1 << LGUnderflowBit;

  if (PrePulse())  statusMask |= 1 << PrePulseBit;
  if (PostPulse()) statusMask |= 1 << PostPulseBit;
  if (FitFailed()) statusMask |= 1 << FitFailedBit;
  if (BadChisq())  statusMask |= 1 << BadChisqBit;

  if (BadT0()) statusMask |= 1 << BadT0Bit;

  return statusMask;
}
