/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZDCPulseAnalyzer.h"

#include <numeric>
#include <algorithm>

#include "TFitResult.h"
#include "TFitResultPtr.h"

bool ZDCPulseAnalyzer::_quietFits = true;
std::string ZDCPulseAnalyzer::_fitOptions = "";

ZDCPulseAnalyzer::ZDCPulseAnalyzer(std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, 
				   float gainHG, std::string fitFunction, int peak2ndDerivMinSample, 
				   float peak2ndDerivMinThreshHG, float peak2ndDerivMinThreshLG) :
  _tag(tag), _Nsample(Nsample), _deltaTSample(deltaTSample), _preSampleIdx(preSampleIdx), 
  _pedestal(pedestal), _gainHG(gainHG), _forceLG(false), _fitFunction(fitFunction),
  _peak2ndDerivMinSample(peak2ndDerivMinSample), 
  _peak2ndDerivMinThreshHG(peak2ndDerivMinThreshHG),
  _peak2ndDerivMinThreshLG(peak2ndDerivMinThreshLG),
  _haveTimingCorrections(false), _haveNonlinCorr(false), _initializedFits(false),
  _ADCSamplesHGSub(Nsample, 0), _ADCSamplesLGSub(Nsample, 0), 
  _ADCSSampSigHG(Nsample, 1), _ADCSSampSigLG(Nsample, 1), // By default the ADC uncertainties are set to one
  _samplesSub(Nsample, 0),
  _defaultFitWrapper(0), _prePulseFitWrapper(0)
{
  // Create the histogram used for fitting
  //
  _tmin = -deltaTSample/2;
  _tmax = _tmin + ((float) Nsample)*deltaTSample;

  std::string histName = "ZDCFitHist" + tag;

  _fitHist = new TH1F(histName.c_str(), "", _Nsample, _tmin, _tmax);
  _fitHist->SetDirectory(0);

  SetDefaults();
  Reset();
}

ZDCPulseAnalyzer::~ZDCPulseAnalyzer()
{
  if (!_defaultFitWrapper) delete _defaultFitWrapper;
  if (!_prePulseFitWrapper) delete _prePulseFitWrapper;  
}

void ZDCPulseAnalyzer::SetDefaults()
{
  _nominalTau1 = 4;
  _nominalTau2 = 21;

  _fixTau1 = false;
  _fixTau2 = false;

  _HGOverflowADC = 900;
  _HGUnderflowADC = 20;
  _LGOverflowADC = 1000;

  _chisqDivAmpCutLG = 100;
  _chisqDivAmpCutHG = 100;
  
  _T0CutLowLG = _tmin;
  _T0CutLowHG = _tmin;

  _T0CutHighLG = _tmax;
  _T0CutHighHG = _tmax;

  _LGT0CorrParams.assign(4, 0);
  _HGT0CorrParams.assign(4, 0);

  _fitOptions = "s";
}

void ZDCPulseAnalyzer::Reset()
{
  _haveData = false;
  _havePulse = false;
  _fail = false;
  
  _HGOverflow = false;
  _HGUnderflow = false;
  
  _LGOverflow = false;
  _LGUnderflow = false;
  
  _PSHGOverUnderflow = false;
  
  _prePulse = false;
  _fitFailed = false;

  _useLowGain = false;

  _badT0 = false;
  _badChisq = false;

  _fitAmplitude = 0;
  _fitT0 = -100;
  _fitChisq = 0;
  
  _fitT0Corr = -100;
  _amplitude = 0;
  _ampError = 0;
  _preSampleAmp = 0;
  _bkgdMaxFraction = 0;

  _samplesSub.clear();
  _samplesDeriv.clear();
  _samplesDeriv2nd.clear();
}

void ZDCPulseAnalyzer::SetTauT0Values(bool fixTau1, bool fixTau2, float tau1, float tau2, float t0HG, float t0LG)
{
  _fixTau1 = fixTau1;
  _fixTau2 = fixTau2;
  _nominalTau1 = tau1;
  _nominalTau2 = tau2;

  _nominalT0HG = t0HG;
  _nominalT0LG = t0LG;

  _initializedFits = false;
}

void ZDCPulseAnalyzer::SetADCOverUnderflowValues(int HGOverflowADC, int HGUnderflowADC, int LGOverflowADC)
{
  _HGOverflowADC = HGOverflowADC;
  _LGOverflowADC = LGOverflowADC;
  _HGUnderflowADC = HGUnderflowADC;
}

void ZDCPulseAnalyzer::SetCutValues(float chisqDivAmpCutHG, float chisqDivAmpCutLG,
				    float deltaT0MinHG, float deltaT0MaxHG, 
				    float deltaT0MinLG, float deltaT0MaxLG) 
{
  _chisqDivAmpCutHG = chisqDivAmpCutHG;
  _chisqDivAmpCutLG = chisqDivAmpCutLG;

  _T0CutLowHG = deltaT0MinHG;
  _T0CutLowLG = deltaT0MinLG;

  _T0CutHighHG = deltaT0MaxHG;
  _T0CutHighLG = deltaT0MaxLG;
}

void ZDCPulseAnalyzer::SetupFitFunctions()
{
  if (!_defaultFitWrapper) delete _defaultFitWrapper;
  if (!_prePulseFitWrapper) delete _prePulseFitWrapper;

  if (_fitFunction == "FermiExp") {
    if (!_fixTau1 || !_fixTau2) {
      //
      // Use the variable tau version of the expFermiFit
      //
      _defaultFitWrapper = new ZDCFitExpFermiVariableTaus(_tag, _tmin, _tmax, _fixTau1, _fixTau2, _nominalTau1, _nominalTau2);
    }
    else {
      _defaultFitWrapper = new ZDCFitExpFermiFixedTaus(_tag, _tmin, _tmax, _nominalTau1, _nominalTau2);
    }

    _prePulseFitWrapper = new ZDCFitExpFermiPrePulse(_tag, _tmin, _tmax, _nominalTau1, _nominalTau2); 
  }
  else {
    throw;
  }

  _initializedFits = true;
}

bool ZDCPulseAnalyzer::LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float>  ADCSamplesLG) 
{
  if (!_initializedFits) SetupFitFunctions();

  // Clear any transient data 
  //
  Reset();

  // Make sure we have the right number of samples. Should never fail. necessry?
  //
  if (ADCSamplesHG.size() != _Nsample || ADCSamplesLG.size() != _Nsample) {
    _fail = true;
    return false;
  }

  // Now do pedestal subtraction and check for overflows
  //
  for (size_t isample = 0; isample < _Nsample; isample++) {
    float ADCHG = ADCSamplesHG[isample];
    float ADCLG = ADCSamplesLG[isample];
    
    if (ADCHG > _HGOverflowADC) {
      _HGOverflow = true;
      
      if (isample == _preSampleIdx) _PSHGOverUnderflow = true;
    }
    else if (ADCHG < _HGUnderflowADC) {
      _HGUnderflow = true;
      
      if (isample == _preSampleIdx) _PSHGOverUnderflow  = true;
    }
    
    if (ADCLG > _LGOverflowADC) {
      _LGOverflow = true;
      _fail = true;
      _amplitude = 1024 * _gainHG; // Give a vale here even though we know it's wrong because
                                   //   the user may not check the return value and we know that 
                                   //   amplitude is bigger than this
    }
    
    if (ADCLG == 0) {
      _LGUnderflow = true;
      _fail = true;
    }
    
    _ADCSamplesHGSub[isample] = ADCHG - _pedestal;
    _ADCSamplesLGSub[isample] = ADCLG - _pedestal;
  }

  //  if (_fail) return false;

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  _useLowGain = _HGUnderflow || _HGOverflow || _forceLG;

  if (_useLowGain) {
    bool result = AnalyzeData(_ADCSamplesLGSub, _ADCSSampSigLG, _LGT0CorrParams, _chisqDivAmpCutLG, 
			      _T0CutLowLG, _T0CutHighLG, _peak2ndDerivMinThreshLG);
    if (result) {
      // 
      // Multiply amplitude by gain factor
      //
      _amplitude = _fitAmplitude * _gainHG;
      _ampError = _fitAmpError * _gainHG;
      _preSampleAmp = _preSample * _gainHG;
    }

    return result;
  }
  else {
    bool result = AnalyzeData(_ADCSamplesHGSub, _ADCSSampSigHG, _HGT0CorrParams, _chisqDivAmpCutHG, 
			      _T0CutLowHG, _T0CutHighHG, _peak2ndDerivMinThreshHG);
    if (result) {
      _preSampleAmp = _preSample;
      _amplitude = _fitAmplitude;
      _ampError = _fitAmpError;

      //  If we have a non-linear correction, apply it here
      //
      if (_haveNonlinCorr) {
	float ampCorrFact = _amplitude/1000. - 0.5;
	float corrFactor = 1./(1. + _nonLinCorrParams[0]*ampCorrFact + _nonLinCorrParams[1]*ampCorrFact*ampCorrFact);

	_amplitude *= corrFactor;
	_ampError *= corrFactor;
      }
    }

    return result;
  }
}

bool ZDCPulseAnalyzer::AnalyzeData(const std::vector<float>& samples,        // The samples used for this event
				   const std::vector<float>& samplesSig,     // The "resolution" on the ADC value
				   const std::vector<float>& t0CorrParams,   // The parameters used to correct the t0               
				   float maxChisqDivAmp,                     // The maximum chisq / amplitude ratio
				   float minT0Corr, float maxT0Corr,         // The minimum and maximum corrected T0 values
				   float peak2ndDerivMinThresh
				   )
{
  //  Do the presample subtraction using a lambda function
  //
  float preSample = samples[_preSampleIdx];
  _preSample = preSample;
  _samplesSub = samples;

  std::for_each(_samplesSub.begin(), _samplesSub.end(), [=] (float& adcUnsub) {return adcUnsub -= preSample;} );

  // Find maximum and minimum values
  //
  std::pair<SampleCIter, SampleCIter> minMaxIters = std::minmax_element(_samplesSub.begin(), _samplesSub.end());
  SampleCIter minIter = minMaxIters.first;
  SampleCIter maxIter = minMaxIters.second;

  _maxADCValue = *maxIter;
  _minADCValue = *minIter;

  float maxMinADCDiff = _maxADCValue - _minADCValue;

  _maxSampl = std::distance(_samplesSub.cbegin(), maxIter);
  _minSampl = std::distance(_samplesSub.cbegin(), minIter);

  // Calculate first and 2nd "derivatives" 
  //
  std::vector<float> deriv(_Nsample, 0);
  std::adjacent_difference(samples.begin(), samples.end(), deriv.begin());

  // Save the derivatives, dropping the useless first element
  //
  _samplesDeriv.insert(_samplesDeriv.begin(), deriv.begin() + 1, deriv.end());

  std::vector<float> deriv2nd(_Nsample, 0);
  std::adjacent_difference(_samplesDeriv.begin(), _samplesDeriv.end(), deriv2nd.begin());

  // Save the second derivatives, dropping the useless first element
  //
  _samplesDeriv2nd.insert(_samplesDeriv2nd.begin(), deriv2nd.begin() + 1, deriv2nd.end());

  // Find the sampl which has the lowest 2nd derivative
  //
  SampleCIter minDeriv2ndIter = std::min_element(_samplesDeriv2nd.begin(), _samplesDeriv2nd.end());

  _minDeriv2nd = *minDeriv2ndIter;
  _minDeriv2ndIndex = std::distance(_samplesDeriv2nd.cbegin(), minDeriv2ndIter);

  //  The ket test for presence of a pulse: is the minimum 2nd derivative in the right place and is it 
  //    large enough (sufficiently negative) 
  //
  if (std::abs(_minDeriv2ndIndex - _peak2ndDerivMinSample) <= 1  && _minDeriv2nd <= peak2ndDerivMinThresh) {
    _havePulse = true;
  }
  else {
    _havePulse = false;
  }

  // Now decide whether we have a preceeding pulse or not
  //   

  // Have to be careful with the derivative indices, but test up to minDeriv2ndIndex - 1
  //
  bool haveNegDeriv = false;

  for (int isampl = 0; isampl < _minDeriv2ndIndex; isampl++) {

    // If any of the derivatives prior to the peak (as determined by the 2nd derivative) is negative
    //   Then we are on the tailing edge of a previous pulse.
    //
    if (_samplesDeriv[isampl] < 0) {

      //
      //  Put a threshold on the derivative as a fraction of the difference between max and min so that
      //     we don't waste lots of time for insignificant prepulses.
      //
      if (std::abs(_samplesDeriv[isampl]) / (_maxADCValue - _minADCValue + 1) > 0.05) _prePulse = true;

      // Also, if we have two negative derivatives in a row, we have a prepulse
      //
      if (haveNegDeriv) _prePulse = true;
      else haveNegDeriv = true;
    }

    //  If any of the 2nd derivatives before the minium are negative, we have the possibility of positive
    //    preceeding pulse
    //
    if (_samplesDeriv2nd[isampl] < 0 && _samplesDeriv2nd[isampl + 1] > 0) _prePulse = true;
  }

  // Additional specific tests for pre pulses etc.
  //
  if (_deltaTSample < 20) {
    //  
    //  Don't do this check for the 40 MHz data as the pulse often drops below baseline 
    //    due to overshoot, not pre-pulses
    //
    if (_samplesSub[_Nsample - 1] <=  0 ||
	_samplesSub[_Nsample - 1] <=  _samplesSub[_preSampleIdx+1] ) _prePulse = true;

  }


  FillHistogram(_samplesSub, samplesSig);

  //  Stop now if we have no pulse
  //
  if (_fail || !_havePulse) return false;

  DoFit();

  if (!FitFailed()) {
    if (_haveTimingCorrections) {
      _fitT0Corr = _fitT0;

      //
      // We correct relative to the middle of the amplitude range, divided by 100 to make corrections smaller
      //
      float t0CorrFact = (_fitAmplitude - 500)/100.;

      float correction = (t0CorrParams[0] + t0CorrParams[1]*t0CorrFact + t0CorrParams[2]*t0CorrFact*t0CorrFact +
			  t0CorrParams[3]*t0CorrFact*t0CorrFact*t0CorrFact);

      _fitT0Corr -= correction;
    }

    // Now check for valid chisq and valid time
    //
    if (_fitChisq / _fitAmplitude > maxChisqDivAmp) _badChisq = true;
    if (_fitT0Corr < minT0Corr || _fitT0Corr > maxT0Corr) _badT0 = true;

    //    std::cout << "Testing t0 cut: t0corr = " << _fitT0Corr << ", min max cuts = " << minT0Corr << ", " << maxT0Corr << std::endl;
  }

  return !_fitFailed;
}

void ZDCPulseAnalyzer::DoFit()
{
  // Set the initial values
  //
  float ampInitial = _maxADCValue - _minADCValue;
  float t0Initial = (_useLowGain ? _nominalT0LG : _nominalT0HG);

  ZDCFitWrapper* fitWrapper = _defaultFitWrapper;
  if (PrePulse()) fitWrapper = _prePulseFitWrapper;

  fitWrapper->Initialize(ampInitial, t0Initial);
  if (PrePulse() && _samplesDeriv[0] < 0) {
    (static_cast<ZDCPrePulseFitWrapper*>(_prePulseFitWrapper))->SetInitialPrePulseT0(-10);
  }

  // Now perform the fit
  //
  std::string options = _fitOptions;
  if (QuietFits()) options += "Q0";
  options += "s";

  TFitResultPtr result_ptr = _fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());

  int fitStatus = result_ptr;
  if (fitStatus != 0) {
    TFitResultPtr try2Result_ptr = _fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());
    if ((int) try2Result_ptr != 0) _fitFailed = true;
  }
  else _fitFailed = false;

  //  if (!_fitFailed) {
  _bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  if (std::abs(_bkgdMaxFraction) > 0.25) {
    options += "e";
    _fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());
    _bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  }

  _fitAmplitude = fitWrapper->GetAmplitude();
  _fitT0 = fitWrapper->GetTime() - t0Initial;
  _fitChisq = result_ptr->Chi2();

  _fitTau1 = fitWrapper->GetTau1();
  _fitTau2 = fitWrapper->GetTau2();

  _fitAmpError = fitWrapper->GetAmpError();
  //  }
}

void ZDCPulseAnalyzer::Dump() const
{
  std::cout << "ZDCPulseAnalyzer dump for tag = " << _tag << std::endl;

  std::cout << "Presample index, value = " << _preSampleIdx << ", " << _preSample << std::endl;

  std::cout << "samplesSub ";
  for (size_t sample = 0; sample < _samplesSub.size(); sample++) {
    std::cout << ", [" << sample << "] = " << _samplesSub[sample];
  }
  std::cout << std::endl;

  std::cout << "samplesDeriv ";
  for (size_t sample = 0; sample < _samplesDeriv.size(); sample++) {
    std::cout << ", [" << sample << "] = " << _samplesDeriv[sample];
  }
  std::cout << std::endl;

  std::cout << "samplesDeriv2nd ";
  for (size_t sample = 0; sample < _samplesDeriv2nd.size(); sample++) {
    std::cout << ", [" << sample << "] = " << _samplesDeriv2nd[sample];
  }
  std::cout << std::endl;

  std::cout << "minimum 2nd deriv sample, value = " << _minDeriv2ndIndex << ", " << _minDeriv2nd << std::endl;
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

  if (FitFailed()) statusMask |= 1 << FitFailedBit;
  if (PrePulse()) statusMask |= 1 << PrePulseBit;
  if (BadChisq()) statusMask |= 1 << BadChisqBit;
  if (BadT0()) statusMask |= 1 << BadT0Bit;

  return statusMask;
}
