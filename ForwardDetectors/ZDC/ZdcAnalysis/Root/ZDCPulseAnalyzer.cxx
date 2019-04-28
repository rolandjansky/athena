/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCPulseAnalyzer.h"

#include <numeric>
#include <algorithm>

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

void ZDCPulseAnalyzer::CombinedPulsesFCN(int& /*numParam*/, double*, double& f, double* par, int flag)
{
  //  The first parameter is a correction factor to account for decrease in beam intensity between x
  //    and y scan. It is applied here and not passed to the actual fit function
  //

  if (flag >=1 && flag <= 3) {
    f = 0;
    return;
  }

  double chiSquare = 0;

  float delayBaselineAdjust = par[0];

  int nSamples = s_undelayedFitHist->GetNbinsX();
  if (s_delayedFitHist->GetNbinsX() != nSamples) throw;

  // undelayed
  //
  for (int isample = 0; isample < nSamples; isample++) {
    double histValue = s_undelayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(s_undelayedFitHist->GetBinError(isample + 1), 1.0);
    double t = s_undelayedFitHist->GetBinCenter(isample + 1);

    if (t > s_combinedFitTMax) break;

    double funcVal = s_combinedFitFunc->EvalPar(&t, &par[1]);

    // ANA_MSG_VERBOSE ("Calculating chis^2, undelayed sample " << isample << "t = " << t << ", data = " << histValue << ", func = " << funcVal);

    double pull = (histValue - funcVal)/histError;
    chiSquare += pull*pull;
  }

  // delayed
  //
  for (int isample = 0; isample < nSamples; isample++) {
    double histValue = s_delayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(s_delayedFitHist->GetBinError(isample + 1), 1.0);
    double t = s_delayedFitHist->GetBinCenter(isample + 1);

    if (t > s_combinedFitTMax) break;

    double funcVal = s_combinedFitFunc->EvalPar(&t, &par[1]) + delayBaselineAdjust;
    double pull = (histValue - funcVal)/histError;

    // ANA_MSG_VERBOSE ("Calculating chis^2, delayed sample " << isample << "t = " << t << ", data = " << histValue << ", func = " << funcVal);

    chiSquare += pull*pull;
  }

  f = chiSquare;
}


ZDCPulseAnalyzer::ZDCPulseAnalyzer(MsgStream *val_msg, std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal,
				   float gainHG, std::string fitFunction, int peak2ndDerivMinSample,
				   float peak2ndDerivMinThreshHG, float peak2ndDerivMinThreshLG) :
  m_msg (val_msg),
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
  m_tmin = -deltaTSample/2;
  m_tmax = m_tmin + ((float) Nsample)*deltaTSample;

  m_fitTMax = m_tmax;

  std::string histName = "ZDCFitHist" + tag;

  m_fitHist = new TH1F(histName.c_str(), "", m_Nsample, m_tmin, m_tmax);
  m_fitHist->SetDirectory(0);

  SetDefaults();
  Reset();
}

ZDCPulseAnalyzer::~ZDCPulseAnalyzer()
{
  if (!m_defaultFitWrapper) delete m_defaultFitWrapper;
  if (!m_prePulseFitWrapper) delete m_prePulseFitWrapper;
}

void ZDCPulseAnalyzer::EnableDelayed(float deltaT, float pedestalShift)
{
  m_useDelayed = true;
  m_delayedDeltaT = deltaT;
  m_delayedPedestalDiff = pedestalShift;

  m_delayedHist = new TH1F((std::string(m_fitHist->GetName()) + "delayed").c_str(), "", m_Nsample, m_tmin + m_delayedDeltaT, m_tmax + m_delayedDeltaT);

  m_ADCSamplesHGSub.assign(2*m_Nsample, 0);
}

void ZDCPulseAnalyzer::SetDefaults()
{
  m_nominalTau1 = 4;
  m_nominalTau2 = 21;

  m_fixTau1 = false;
  m_fixTau2 = false;

  m_HGOverflowADC = 900;
  m_HGUnderflowADC = 20;
  m_LGOverflowADC = 1000;

  m_chisqDivAmpCutLG = 100;
  m_chisqDivAmpCutHG = 100;

  m_T0CutLowLG = m_tmin;
  m_T0CutLowHG = m_tmin;

  m_T0CutHighLG = m_tmax;
  m_T0CutHighHG = m_tmax;

  m_LGT0CorrParams.assign(4, 0);
  m_HGT0CorrParams.assign(4, 0);

  s_fitOptions = "s";
}

void ZDCPulseAnalyzer::Reset()
{
  m_haveData = false;
  m_havePulse = false;
  m_fail = false;

  m_HGOverflow = false;
  m_HGUnderflow = false;

  m_LGOverflow = false;
  m_LGUnderflow = false;

  m_PSHGOverUnderflow = false;

  m_prePulse = false;
  m_fitFailed = false;

  m_useLowGain = false;

  m_badT0 = false;
  m_badChisq = false;

  m_fitAmplitude = 0;
  m_fitTime = -100;
  m_fitTimeSub = -100;
  m_fitTimeCorr = -100;

  m_fitChisq = 0;

  m_amplitude = 0;
  m_ampError = 0;
  m_preSampleAmp = 0;
  m_bkgdMaxFraction = 0;

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
  m_fixTau1 = fixTau1;
  m_fixTau2 = fixTau2;
  m_nominalTau1 = tau1;
  m_nominalTau2 = tau2;

  m_nominalT0HG = t0HG;
  m_nominalT0LG = t0LG;

  ANA_MSG_INFO ("m_fixTau1=" << m_fixTau1 << "  m_fixTau2=" << m_fixTau2 << "  m_nominalTau1=" << m_nominalTau1 << "  m_nominalTau2=" << m_nominalTau2 << "  m_nominalT0HG=" << m_nominalT0HG << "  m_nominalT0LG=" << m_nominalT0LG);

  m_initializedFits = false;
}

void ZDCPulseAnalyzer::SetFitTimeMax(float tmax)
{
  if (tmax < m_tmin) {
    ANA_MSG_INFO ("ZDCPulseAnalyzer::SetFitTimeMax:: invalid FitTimeMax: " << tmax);
    return;
  }

  ANA_MSG_VERBOSE ("Setting FitTMax to " << tmax);

  m_fitTMax = tmax;

  if (m_initializedFits) SetupFitFunctions();
}

void ZDCPulseAnalyzer::SetADCOverUnderflowValues(int HGOverflowADC, int HGUnderflowADC, int LGOverflowADC)
{
  m_HGOverflowADC = HGOverflowADC;
  m_LGOverflowADC = LGOverflowADC;
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
  if (!m_defaultFitWrapper) delete m_defaultFitWrapper;
  if (!m_prePulseFitWrapper) delete m_prePulseFitWrapper;

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

    if (m_fitTMax < m_tmax) {
      m_defaultFitWrapper->SetFitT0Max(m_tmin, m_fitTMax);
      m_prePulseFitWrapper->SetFitT0Max(m_tmin, m_fitTMax);
    }
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

    if (m_fitTMax < m_tmax) {
      m_defaultFitWrapper->SetFitT0Max(m_tmin, m_fitTMax);
      m_prePulseFitWrapper->SetFitT0Max(m_tmin, m_fitTMax);
    }
  }
  else {
    throw;
  }

  m_initializedFits = true;
}

bool ZDCPulseAnalyzer::LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float>  ADCSamplesLG)
{
  ANA_MSG_VERBOSE ("Use delayed = " << m_useDelayed);

  if (m_useDelayed) throw;
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

  ANA_MSG_VERBOSE ("Loaded data");

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
	float ampCorrFact = m_amplitude/1000. - 0.5;
	float corrFactor = 1./(1. + m_nonLinCorrParams[0]*ampCorrFact + m_nonLinCorrParams[1]*ampCorrFact*ampCorrFact);

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
  if (!m_useDelayed) throw;
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

    if (ADCHG > m_HGOverflowADC) {
      m_HGOverflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow = true;
    }
    else if (ADCHG < m_HGUnderflowADC) {
      m_HGUnderflow = true;

      if (isample == m_preSampleIdx) m_PSHGOverUnderflow  = true;
    }

    if (ADCHGDelay > m_HGOverflowADC) {
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

    m_ADCSamplesHGSub[isample*2] = ADCHG - m_pedestal;
    m_ADCSamplesLGSub[isample*2] = ADCLG - m_pedestal;

    m_ADCSamplesHGSub[isample*2 + 1] = ADCHGDelay - m_pedestal - m_delayedPedestalDiff;
    m_ADCSamplesLGSub[isample*2 + 1] = ADCLGDelay - m_pedestal - m_delayedPedestalDiff;
  }

  // if (!m_PSHGOverUnderflow) {

  //   std::cout << "m_ADCSamplesHGSub = ";
  //   for (size_t sample = 0; sample < m_ADCSamplesHGSub.size(); sample++) {
  //     std::cout << ", [" << sample << "] = " << m_ADCSamplesHGSub[sample];
  //   }

  //   //  Attempt to address up front cases where we have significant offsets between the delayed and undelayed
  //   //
  //   float delta10 = m_ADCSamplesHGSub[1] - m_ADCSamplesHGSub[0];
  //   float delta21 = m_ADCSamplesHGSub[2] - m_ADCSamplesHGSub[1];
  //   float delta32 = m_ADCSamplesHGSub[3] - m_ADCSamplesHGSub[2];

  //   std::cout << "testing delayed shift: delta10 = " << delta10 << ", delta21 = " << delta21 << ", delta32 = " << delta32 << std::endl;

  //   if (std::abs(delta10 + delta21) <= 2 && std::abs(delta10 - delta32) <= 2) {
  //     //  Very likely we have an offset between the delayed and undelayed
  //     //

  //     std::cout << "correcting HG delayed ssamples down by " << 0.5*(delta10 + delta32) << std::endl;

  //     for (int isample = 0; isample < m_Nsample; isample++) {
  // 	m_ADCSamplesHGSub[isample*2 + 1] -= 0.5*(delta10 + delta32);
  //     }

  //     std::cout << " corrected m_ADCSamplesHGSub = ";
  //     for (size_t sample = 0; sample < m_ADCSamplesHGSub.size(); sample++) {
  // 	std::cout << ", [" << sample << "] = " << m_ADCSamplesHGSub[sample];
  //     }

  //   }
  // }

  //  if (m_fail) return false;

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  m_useLowGain = m_HGUnderflow || m_HGOverflow || m_forceLG;

  if (m_useLowGain) {

    bool result = AnalyzeData(m_Nsample*2, m_preSampleIdx*2, m_ADCSamplesLGSub, m_ADCSSampSigLG, m_LGT0CorrParams, m_chisqDivAmpCutLG,
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
    bool result = AnalyzeData(m_Nsample*2, m_preSampleIdx*2, m_ADCSamplesHGSub, m_ADCSSampSigHG, m_HGT0CorrParams, m_chisqDivAmpCutHG,
			      m_T0CutLowHG, m_T0CutHighHG, m_peak2ndDerivMinThreshHG);
    if (result) {
      m_preSampleAmp = m_preSample;
      m_amplitude = m_fitAmplitude;
      m_ampError = m_fitAmpError;

      //  If we have a non-linear correction, apply it here
      //
      if (m_haveNonlinCorr) {
	float ampCorrFact = m_amplitude/1000. - 0.5;
	float corrFactor = 1./(1. + m_nonLinCorrParams[0]*ampCorrFact + m_nonLinCorrParams[1]*ampCorrFact*ampCorrFact);

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
  float preSample = samples[preSampleIdx];
  m_preSample = preSample;
  m_samplesSub = samples;

  if (m_useDelayed) {
    //  Attempt to address up front cases where we have significant offsets between the delayed and undelayed
    //
    if (m_peak2ndDerivMinSample > 4) {
      m_baselineCorr = 0.5*(m_samplesSub[1] - m_samplesSub[0] + m_samplesSub[3] - m_samplesSub[2]) - 0.25 * (m_samplesSub[3] - m_samplesSub[1] + m_samplesSub[2] - m_samplesSub[0]);;
    }
    else {
      m_baselineCorr = m_samplesSub[1] - m_samplesSub[0];
    }

    for (size_t isample = 0; isample < nSamples; isample++) {
      if (isample%2) m_samplesSub[isample] -= m_baselineCorr;
    }
  }

  std::for_each(m_samplesSub.begin(), m_samplesSub.end(), [=] (float& adcUnsub) {return adcUnsub -= preSample;} );

  // Find maximum and minimum values
  //
  std::pair<SampleCIter, SampleCIter> minMaxIters = std::minmax_element(m_samplesSub.begin(), m_samplesSub.end());
  SampleCIter minIter = minMaxIters.first;
  SampleCIter maxIter = minMaxIters.second;

  m_maxADCValue = *maxIter;
  m_minADCValue = *minIter;

  // float maxMinADCDiff = m_maxADCValue - m_minADCValue;

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

  // Find the sample which has the lowest 2nd derivative
  //
  SampleCIter minDeriv2ndIter = std::min_element(m_samplesDeriv2nd.begin(), m_samplesDeriv2nd.end());

  m_minDeriv2nd = *minDeriv2ndIter;
  m_minDeriv2ndIndex = std::distance(m_samplesDeriv2nd.cbegin(), minDeriv2ndIter);

  //  The ket test for presence of a pulse: is the minimum 2nd derivative in the right place and is it
  //    large enough (sufficiently negative)
  //
  if (std::abs(m_minDeriv2ndIndex - (int) m_peak2ndDerivMinSample) <= (int)m_peak2ndDerivMinTolerance  && m_minDeriv2nd <= peak2ndDerivMinThresh) {
    m_havePulse = true;
  }
  else {
    m_havePulse = false;
  }

  // Now decide whether we have a preceeding pulse or not
  //
  // std::cout << "_samplesSDeriv = ";
  // for (size_t sample = 0; sample < _samplesDeriv.size(); sample++) {
  //   std::cout << ", [" << sample << "] = " << _samplesDeriv[sample];
  // }


  // Have to be careful with the derivative indices, but test up to minDeriv2ndIndex - 1
  //
  bool lastNegDeriv = false;

  for (int isampl = 0; isampl < m_minDeriv2ndIndex; isampl++) {

    // If any of the derivatives prior to the peak (as determined by the 2nd derivative) is negative
    //   Then we are on the tailing edge of a previous pulse.
    //
    if (m_samplesDeriv[isampl] < -0.5) {

      //
      //  Put a threshold on the derivative as a fraction of the difference between max and min so that
      //     we don't waste lots of time for insignificant prepulses.
      //
      if (std::abs(m_samplesDeriv[isampl]) / (m_maxADCValue - m_minADCValue + 1) > 0.05) {

	// std:: cout << "Setting prepulse due to large negative derivative: sample = " << isampl << ", value = " << m_samplesDeriv[isampl]
	// 	   << ", threshold = " << (m_maxADCValue - m_minADCValue + 1)*0.05  << std::endl;
	m_prePulse = true;
      }

      // Also, if we have two negative derivatives in a row, we have a prepulse
      //
      if (lastNegDeriv) {
	m_prePulse = true;
	//	std:: cout << "Setting prepulse due to successive negative derivatives" << std::endl;
      }
      else lastNegDeriv = true;
    }
    else {
      lastNegDeriv = false;
    }

    //  If any of the 2nd derivatives before the minium are negative, we have the possibility of positive
    //    preceeding pulse
    //
    if (m_samplesDeriv2nd[isampl] < 0 && m_samplesDeriv2nd[isampl + 1] > 0) {
      if  (m_samplesDeriv2nd[isampl] < 0.1*m_minDeriv2nd) {
	m_prePulse = true;
	// std:: cout << "Setting prepulse due to negative 2nd derivative: sample = " << isampl << ", value =  " << m_samplesDeriv2nd[isampl] << ", threshold = " << 0.1*m_minDeriv2nd << std::endl;
      }
    }
  }

  // Additional specific tests for pre pulses etc.
  //
  if (m_deltaTSample < 20) {
    //
    //  Don't do this check for the 40 MHz data as the pulse often drops below baseline
    //    due to overshoot, not pre-pulses
    //
    //if //_samplesSub[m_Nsample - 1] <=  -m_maxADCValue*0.05 ||
    //    if	(_samplesSub[m_Nsample - 1] - _samplesSub[m_preSampleIdx+1] < -m_maxADCValue*0.05) m_prePulse = true;

  }


  FillHistogram(m_samplesSub, samplesSig);

  //  Stop now if we have no pulse
  //
  if (m_fail || !m_havePulse) return false;

  if (!m_useDelayed) DoFit();
  else DoFitCombined();

  if (!FitFailed()) {
    m_fitTimeCorr = m_fitTimeSub;

    if (m_haveTimingCorrections) {

      //
      // We correct relative to the middle of the amplitude range, divided by 100 to make corrections smaller
      //
      float t0CorrFact = (m_fitAmplitude - 500)/100.;

      float correction = (t0CorrParams[0] + t0CorrParams[1]*t0CorrFact + t0CorrParams[2]*t0CorrFact*t0CorrFact +
			  t0CorrParams[3]*t0CorrFact*t0CorrFact*t0CorrFact);

      m_fitTimeCorr -= correction;
    }


    // Now check for valid chisq and valid time
    //
    if (m_fitChisq / (m_fitAmplitude + 1.0e-6) > maxChisqDivAmp) m_badChisq = true;
    if (m_fitTimeCorr < minT0Corr || m_fitTimeCorr > maxT0Corr) m_badT0 = true;

    //    std::cout << "Testing t0 cut: t0corr = " << m_fitT0Corr << ", min max cuts = " << minT0Corr << ", " << maxT0Corr << std::endl;
  }

  return !m_fitFailed;
}

void ZDCPulseAnalyzer::DoFit()
{
  // Set the initial values
  //
  float ampInitial = m_maxADCValue - m_minADCValue;
  float t0Initial = (m_useLowGain ? m_nominalT0LG : m_nominalT0HG);

  ZDCFitWrapper* fitWrapper = m_defaultFitWrapper;
  if (PrePulse()) fitWrapper = m_prePulseFitWrapper;

  fitWrapper->Initialize(ampInitial, t0Initial);
  if (PrePulse() && m_samplesDeriv[0] < 0) {
    (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetInitialPrePulseT0(-10);
  }

  // Now perform the fit
  //
  std::string options = s_fitOptions;
  if (QuietFits()) options += "Q0";
  options += "s";

  TFitResultPtr result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str(), "", m_tmin, m_fitTMax);

  int fitStatus = result_ptr;
  if (fitStatus != 0) {
    TFitResultPtr try2Result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str(), "", m_tmin, m_fitTMax);
    if ((int) try2Result_ptr != 0) m_fitFailed = true;
  }
  else m_fitFailed = false;

  //  if (!m_fitFailed) {
  m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  if (std::abs(m_bkgdMaxFraction) > 0.25) {
    std::string tempOptions = options + "e";
    m_fitHist->Fit(fitWrapper->GetWrapperTF1(), tempOptions.c_str(), "", m_tmin, m_fitTMax);
    m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  }

  m_fitAmplitude = fitWrapper->GetAmplitude();
  m_fitTime = fitWrapper->GetTime();
  m_fitTimeSub = m_fitTime - t0Initial;
  m_fitChisq = result_ptr->Chi2();

  m_fitTau1 = fitWrapper->GetTau1();
  m_fitTau2 = fitWrapper->GetTau2();

  m_fitAmpError = fitWrapper->GetAmpError();
  //  }
}

void ZDCPulseAnalyzer::DoFitCombined()
{
  // Set the initial values
  //
  float ampInitial = m_maxADCValue - m_minADCValue;
  float t0Initial = (m_useLowGain ? m_nominalT0LG : m_nominalT0HG);

  ZDCFitWrapper* fitWrapper = m_defaultFitWrapper;
  if (PrePulse()) fitWrapper = m_prePulseFitWrapper;

  fitWrapper->Initialize(ampInitial, t0Initial);
  if (PrePulse()) {
    if (m_samplesDeriv[0] < 0) {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetInitialPrePulseT0(-10);
    }
    else {
      (static_cast<ZDCPrePulseFitWrapper*>(m_prePulseFitWrapper))->SetInitialPrePulseT0(0);
    }
  }

  // Set up the virtual fitter
  //
  TFitter* theFitter = 0;

  if (PrePulse()) {
    if (!m_prePulseCombinedFitter) m_prePulseCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());
    theFitter = m_prePulseCombinedFitter;

    //    std::cout << "Testing prepulse wrapper, npar = " << theFitter->GetNumberTotalParameters() << std::endl;
  }
  else {
    if (!m_defaultCombinedFitter) {
      m_defaultCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());
    }
    theFitter = m_defaultCombinedFitter;

    //    std::cout << "Testing default wrapper, npar = " << theFitter->GetNumberTotalParameters() << std::endl;
  }

  // Set the static pointers to histograms and function for use in FCN
  //
  s_undelayedFitHist = m_fitHist;
  s_delayedFitHist = m_delayedHist;
  s_combinedFitFunc = fitWrapper->GetWrapperTF1();
  s_combinedFitTMax = m_fitTMax;

  size_t numFitPar = theFitter->GetNumberTotalParameters();

  theFitter->GetMinuit()->fISW[4] = -1;

  // Now perform the fit
  //
  if (s_quietFits) {
    theFitter->GetMinuit()->fISW[4] = -1;

    int  ierr= 0;
    theFitter->GetMinuit()->mnexcm("SET NOWarnings",0,0,ierr);
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
  arglist[0] = 2000; // number of function calls
  arglist[1] = 0.01; // tolerance
  int status = theFitter->ExecuteCommand("MIGRAD",arglist,2);

  double fitAmp = theFitter->GetParameter(1);
  if (status || fitAmp < 1) {

    // We first fit with no baseline adjust
    //
    theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);
    theFitter->FixParameter(0);

    status = theFitter->ExecuteCommand("MIGRAD",arglist,2);
    if (status != 0) {
      theFitter->ReleaseParameter(0);
      m_fitFailed = true;
    }
    else {
      theFitter->ReleaseParameter(0);
      status = theFitter->ExecuteCommand("MIGRAD",arglist,2);
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

  // Cpature the chi-square etc.
  //
  double chi2, edm, errdef;
  int nvpar, nparx;

  theFitter->GetStats(chi2, edm, errdef, nvpar, nparx);

  int ndf = 2*m_Nsample - nvpar;

  s_combinedFitFunc->SetChisquare(chi2);
  s_combinedFitFunc->SetNDF(ndf);

    // add to list of functions
  s_undelayedFitHist->GetListOfFunctions()->Add(s_combinedFitFunc);
  s_delayedFitHist->GetListOfFunctions()->Add(s_combinedFitFunc);

  //  if (QuietFits()) options += "Q0";

  // TFitResultPtr result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());

  // int fitStatus = result_ptr;
  // if (fitStatus != 0) {
  //   TFitResultPtr try2Result_ptr = m_fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());
  //   if ((int) try2Result_ptr != 0) m_fitFailed = true;
  // }
  // else m_fitFailed = false;

  // //  if (!m_fitFailed) {
  // m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  // if (std::abs(m_bkgdMaxFraction) > 0.25) {
  //   //    std::string tempOptions = options + "e";
  //   m_fitHist->Fit(fitWrapper->GetWrapperTF1(), tempOptions.c_str());
  //   m_bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  // }

  m_fitAmplitude = fitWrapper->GetAmplitude();
  m_fitTime = fitWrapper->GetTime();
  m_fitTimeSub = m_fitTime - t0Initial;
  m_fitChisq = chi2;

  m_fitTau1 = fitWrapper->GetTau1();
  m_fitTau2 = fitWrapper->GetTau2();

  m_fitAmpError = fitWrapper->GetAmpError();
  //  }
}


TFitter* ZDCPulseAnalyzer::MakeCombinedFitter(TF1* func)
{
  //  gErrorIgnoreLevel = 1001;
  TVirtualFitter::SetDefaultFitter("Minuit");

  size_t nFitParams = func->GetNpar() + 1;
  TFitter* fitter = new TFitter(nFitParams);

  fitter->GetMinuit()->fISW[4] = -1;
  fitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, -100, 100);

  for (size_t ipar = 0; ipar < nFitParams - 1; ipar++) {
    double parLimitLow, parLimitHigh;

    func->GetParLimits(ipar, parLimitLow, parLimitHigh);

    if (fabs(parLimitHigh/parLimitLow - 1) < 1e-6) {
      fitter->SetParameter(ipar + 1, func->GetParName(ipar), func->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh*1.1);
      fitter->FixParameter(ipar + 1);
    }
    else
      {
	fitter->SetParameter(ipar + 1, func->GetParName(ipar), func->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh);
      }
    //    fitter->SetParameter(ipar + 1, func->GetParName(ipar), func->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh);


  }

  fitter->SetFCN(ZDCPulseAnalyzer::CombinedPulsesFCN);

  return fitter;
}

void ZDCPulseAnalyzer::Dump() const
{
  ANA_MSG_INFO ("ZDCPulseAnalyzer dump for tag = " << m_tag);

  ANA_MSG_INFO ("Presample index, value = " << m_preSampleIdx << ", " << m_preSample);

  if (m_useDelayed) {
    ANA_MSG_INFO ("using delayed samples with delta T = " << m_delayedDeltaT << ", and pedestalDiff == " << m_delayedPedestalDiff);
  }

  if (msgLvl (MSG::INFO))
  {
    std::ostringstream message;
    message << "samplesSub ";
    for (size_t sample = 0; sample < m_samplesSub.size(); sample++) {
      message << ", [" << sample << "] = " << m_samplesSub[sample];
    }
    ANA_MSG_INFO (message.str());
  }

  if (msgLvl (MSG::INFO))
  {
    std::ostringstream message;
    message << "samplesDeriv ";
    for (size_t sample = 0; sample < m_samplesDeriv.size(); sample++) {
      message << ", [" << sample << "] = " << m_samplesDeriv[sample];
    }
    ANA_MSG_INFO (message.str());
  }

  if (msgLvl (MSG::INFO))
  {
    std::ostringstream message;
    message << "samplesDeriv2nd ";
    for (size_t sample = 0; sample < m_samplesDeriv2nd.size(); sample++) {
      message << ", [" << sample << "] = " << m_samplesDeriv2nd[sample];
    }
    ANA_MSG_INFO (message.str());
  }

  ANA_MSG_INFO ("minimum 2nd deriv sample, value = " << m_minDeriv2ndIndex << ", " << m_minDeriv2nd);
}

unsigned int ZDCPulseAnalyzer::GetStatusMask() const
{
  unsigned int statusMask = 0;

  if (HavePulse()) statusMask |= 1 << PulseBit;
  if (UseLowGain()) statusMask |= 1 << LowGainBit;
  if (Failed()) statusMask |= 1 << FailBit;
  if (HGOverflow()) statusMask |= 1 << HGOverflowBit;

  if (HGUnderflow()) statusMask |= 1 << HGUnderflowBit;
  if (PSHGOverUnderflow()) statusMask |= 1 << PSHGOverUnderflowBit;
  if (LGOverflow()) statusMask |= 1 << LGOverflowBit;
  if (LGUnderflow()) statusMask |= 1 << LGUnderflowBit;

  if (PrePulse()) statusMask |= 1 << PrePulseBit;
  if (FitFailed()) statusMask |= 1 << FitFailedBit;
  if (BadChisq()) statusMask |= 1 << BadChisqBit;
  if (BadT0()) statusMask |= 1 << BadT0Bit;

  return statusMask;
}
