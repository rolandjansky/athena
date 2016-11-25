/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

bool ZDCPulseAnalyzer::_quietFits = true;
std::string ZDCPulseAnalyzer::_fitOptions = "";
TH1* ZDCPulseAnalyzer::_undelayedFitHist = 0;
TH1* ZDCPulseAnalyzer::_delayedFitHist = 0;
TF1* ZDCPulseAnalyzer::_combinedFitFunc = 0;

void ZDCPulseAnalyzer::CombinedPulsesFCN(int& numParam, double*, double& f, double* par, int flag)
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

  size_t nSamples = _undelayedFitHist->GetNbinsX();
  if (_delayedFitHist->GetNbinsX() != nSamples) throw;

  // undelayed 
  //
  for (size_t isample = 0; isample < nSamples; isample++) {
    double histValue = _undelayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(_undelayedFitHist->GetBinError(isample + 1), 1.0);
    double t = _undelayedFitHist->GetBinCenter(isample + 1);
    
    double funcVal = _combinedFitFunc->EvalPar(&t, &par[1]);

    //    std::cout << "Calculating chis^2, undelayed sample " << isample << "t = " << t << ", data = " << histValue << ", func = " << funcVal << std::endl;
    
    double pull = (histValue - funcVal)/histError;
    chiSquare += pull*pull;
  }

  // delayed 
  //
  for (size_t isample = 0; isample < nSamples; isample++) {
    double histValue = _delayedFitHist->GetBinContent(isample + 1);
    double histError = std::max(_delayedFitHist->GetBinError(isample + 1), 1.0);
    double t = _delayedFitHist->GetBinCenter(isample + 1);
    
    double funcVal = _combinedFitFunc->EvalPar(&t, &par[1]) + delayBaselineAdjust;
    double pull = (histValue - funcVal)/histError;

    //    std::cout << "Calculating chis^2, delayed sample " << isample << "t = " << t << ", data = " << histValue << ", func = " << funcVal << std::endl;

    chiSquare += pull*pull;
  }

  f = chiSquare;
}


ZDCPulseAnalyzer::ZDCPulseAnalyzer(std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, 
				   float gainHG, std::string fitFunction, int peak2ndDerivMinSample, 
				   float peak2ndDerivMinThreshHG, float peak2ndDerivMinThreshLG) :
  _tag(tag), _Nsample(Nsample), _deltaTSample(deltaTSample), _preSampleIdx(preSampleIdx), 
  _pedestal(pedestal), _gainHG(gainHG), _forceLG(false), _fitFunction(fitFunction),
  _peak2ndDerivMinSample(peak2ndDerivMinSample), 
  _peak2ndDerivMinTolerance(1),
  _peak2ndDerivMinThreshHG(peak2ndDerivMinThreshHG),
  _peak2ndDerivMinThreshLG(peak2ndDerivMinThreshLG),
  _haveTimingCorrections(false), _haveNonlinCorr(false), _initializedFits(false),
  _useDelayed(false), _delayedHist(0), _prePulseCombinedFitter(0), _defaultCombinedFitter(0),
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

void ZDCPulseAnalyzer::EnableDelayed(float deltaT, float pedestalShift)
{
  _useDelayed = true;
  _delayedDeltaT = deltaT;
  _delayedPedestalDiff = pedestalShift;

  _delayedHist = new TH1F((std::string(_fitHist->GetName()) + "delayed").c_str(), "", _Nsample, _tmin + _delayedDeltaT, _tmax + _delayedDeltaT);

  _ADCSamplesHGSub.assign(2*_Nsample, 0);
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
  _fitTime = -100;
  _fitTimeSub = -100;
  _fitTimeCorr = -100;

  _fitChisq = 0;
  
  _amplitude = 0;
  _ampError = 0;
  _preSampleAmp = 0;
  _bkgdMaxFraction = 0;

  _samplesSub.clear();
  _samplesDeriv.clear();
  _samplesDeriv2nd.clear();

  int sampleVecSize = _Nsample;
  if (_useDelayed) sampleVecSize *= 2;

  _ADCSamplesHGSub.assign(sampleVecSize, 0);
  _ADCSamplesLGSub.assign(sampleVecSize, 0);
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
  //  std::cout << "Use delayed = " << _useDelayed << std::endl;

  if (_useDelayed) throw;
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

  //  std::cout << "Loaded data" << std::endl;

  //  if (_fail) return false;

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  _useLowGain = _HGUnderflow || _HGOverflow || _forceLG;

  if (_useLowGain) {
    bool result = AnalyzeData(_Nsample, _preSampleIdx, _ADCSamplesLGSub, _ADCSSampSigLG, _LGT0CorrParams, _chisqDivAmpCutLG, 
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
    bool result = AnalyzeData(_Nsample, _preSampleIdx, _ADCSamplesHGSub, _ADCSSampSigHG, _HGT0CorrParams, _chisqDivAmpCutHG, 
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

bool ZDCPulseAnalyzer::LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float>  ADCSamplesLG,
					  std::vector<float> ADCSamplesHGDelayed, std::vector<float> ADCSamplesLGDelayed)
{
  if (!_useDelayed) throw;
  if (!_initializedFits) SetupFitFunctions();

  // Clear any transient data 
  //
  Reset();

  // Make sure we have the right number of samples. Should never fail. necessry?
  //
  if (ADCSamplesHG.size() != _Nsample || ADCSamplesLG.size() != _Nsample ||
      ADCSamplesHGDelayed.size() != _Nsample || ADCSamplesLGDelayed.size() != _Nsample) {
    _fail = true;
    return false;
  }


  // Now do pedestal subtraction and check for overflows
  //
  for (size_t isample = 0; isample < _Nsample; isample++) {
    float ADCHG = ADCSamplesHG[isample];
    float ADCLG = ADCSamplesLG[isample];

    float ADCHGDelay = ADCSamplesHGDelayed[isample];
    float ADCLGDelay = ADCSamplesLGDelayed[isample];
    
    if (ADCHG > _HGOverflowADC) {
      _HGOverflow = true;
      
      if (isample == _preSampleIdx) _PSHGOverUnderflow = true;
    }
    else if (ADCHG < _HGUnderflowADC) {
      _HGUnderflow = true;
      
      if (isample == _preSampleIdx) _PSHGOverUnderflow  = true;
    }
    
    if (ADCHGDelay > _HGOverflowADC) {
      _HGOverflow = true;
    }
    else if (ADCHGDelay < _HGUnderflowADC) {
      _HGUnderflow = true;
    }

    if (ADCLG > _LGOverflowADC) {
      _LGOverflow = true;
      _fail = true;
      _amplitude = 1024 * _gainHG; // Give a value here even though we know it's wrong because
                                   //   the user may not check the return value and we know that 
                                   //   amplitude is bigger than this
    }
    else if (ADCLG == 0) {
      _LGUnderflow = true;
      _fail = true;
    }
    
    if (ADCLGDelay > _LGOverflowADC) {
      _LGOverflow = true;
      _fail = true;
      _amplitude = 1024 * _gainHG; // Give a value here even though we know it's wrong because
                                   //   the user may not check the return value and we know that 
                                   //   amplitude is bigger than this
    }
    else if (ADCLGDelay == 0) {
      _LGUnderflow = true;
      _fail = true;
    }

    _ADCSamplesHGSub[isample*2] = ADCHG - _pedestal;
    _ADCSamplesLGSub[isample*2] = ADCLG - _pedestal;

    _ADCSamplesHGSub[isample*2 + 1] = ADCHGDelay - _pedestal - _delayedPedestalDiff;
    _ADCSamplesLGSub[isample*2 + 1] = ADCLGDelay - _pedestal - _delayedPedestalDiff;
  }

  // if (!_PSHGOverUnderflow) {

  //   std::cout << "_ADCSamplesHGSub = ";
  //   for (size_t sample = 0; sample < _ADCSamplesHGSub.size(); sample++) {
  //     std::cout << ", [" << sample << "] = " << _ADCSamplesHGSub[sample];
  //   }

  //   //  Attempt to address up front cases where we have significant offsets between the delayed and undelayed
  //   //
  //   float delta10 = _ADCSamplesHGSub[1] - _ADCSamplesHGSub[0];
  //   float delta21 = _ADCSamplesHGSub[2] - _ADCSamplesHGSub[1];
  //   float delta32 = _ADCSamplesHGSub[3] - _ADCSamplesHGSub[2];

  //   std::cout << "testing delayed shift: delta10 = " << delta10 << ", delta21 = " << delta21 << ", delta32 = " << delta32 << std::endl;

  //   if (std::abs(delta10 + delta21) <= 2 && std::abs(delta10 - delta32) <= 2) {
  //     //  Very likely we have an offset between the delayed and undelayed
  //     //
      
  //     std::cout << "correcting HG delayed ssamples down by " << 0.5*(delta10 + delta32) << std::endl;

  //     for (int isample = 0; isample < _Nsample; isample++) {
  // 	_ADCSamplesHGSub[isample*2 + 1] -= 0.5*(delta10 + delta32);
  //     }

  //     std::cout << " corrected _ADCSamplesHGSub = ";
  //     for (size_t sample = 0; sample < _ADCSamplesHGSub.size(); sample++) {
  // 	std::cout << ", [" << sample << "] = " << _ADCSamplesHGSub[sample];
  //     }

  //   }
  // }

  //  if (_fail) return false;

  // Now we actually do the analysis using the high or low gain samples as determined
  //  on the basis of the above analysis
  //
  _useLowGain = _HGUnderflow || _HGOverflow || _forceLG;

  if (_useLowGain) {

    bool result = AnalyzeData(_Nsample*2, _preSampleIdx*2, _ADCSamplesLGSub, _ADCSSampSigLG, _LGT0CorrParams, _chisqDivAmpCutLG, 
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
    bool result = AnalyzeData(_Nsample*2, _preSampleIdx*2, _ADCSamplesHGSub, _ADCSSampSigHG, _HGT0CorrParams, _chisqDivAmpCutHG, 
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
  _preSample = preSample;
  _samplesSub = samples;


  // std::cout << "_samplesSub = ";
  // for (size_t sample = 0; sample < _samplesSub.size(); sample++) {
  //   std::cout << ", [" << sample << "] = " << _samplesSub[sample];
  // }
  
  if (_useDelayed) {
    //  Attempt to address up front cases where we have significant offsets between the delayed and undelayed
    //
    // float delta10 = _samplesSub[1] - _samplesSub[0];
    // float delta21 = _samplesSub[2] - _samplesSub[1];
    // float delta32 = _samplesSub[3] - _samplesSub[2];
    
    // //    std::cout << "testing delayed shift: delta10 = " << delta10 << ", delta21 = " << delta21 << ", delta32 = " << delta32 << std::endl;
    
    // if (std::abs(delta10 + delta21) <= 0.1*std::abs(delta10 - delta21)/2 && std::abs(delta10 - delta32) <= 0.1*std::abs(delta10 + delta32)/2) {
    //   //  Very likely we have an offset between the delayed and undelayed
    //   //
      
      //      std::cout << "correcting HG delayed ssamples down by " << 0.5*(delta10 + delta32) << std::endl;

    
    double baselineCorr = 0.5*(_samplesSub[1] - _samplesSub[0] + _samplesSub[3] - _samplesSub[4]); 
      
    for (int isample = 0; isample < nSamples; isample++) {
      if (isample%2) _samplesSub[isample] -= baselineCorr;
    }
      
      // std::cout << " corrected _samplesSub = ";
      // for (size_t sample = 0; sample < _samplesSub.size(); sample++) {
      // 	std::cout << ", [" << sample << "] = " << _samplesSub[sample];
      // } 
  }

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
  std::vector<float> deriv(nSamples, 0);
  std::adjacent_difference(_samplesSub.begin(), _samplesSub.end(), deriv.begin());

  // Save the derivatives, dropping the useless first element
  //
  _samplesDeriv.insert(_samplesDeriv.begin(), deriv.begin() + 1, deriv.end());

  std::vector<float> deriv2nd(nSamples, 0);
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
  if (std::abs(_minDeriv2ndIndex - (int) _peak2ndDerivMinSample) <= _peak2ndDerivMinTolerance  && _minDeriv2nd <= peak2ndDerivMinThresh) {
    _havePulse = true;
  }
  else {
    _havePulse = false;
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

  for (int isampl = 0; isampl < _minDeriv2ndIndex; isampl++) {

    // If any of the derivatives prior to the peak (as determined by the 2nd derivative) is negative
    //   Then we are on the tailing edge of a previous pulse.
    //
    if (_samplesDeriv[isampl] < -0.5) {

      //
      //  Put a threshold on the derivative as a fraction of the difference between max and min so that
      //     we don't waste lots of time for insignificant prepulses.
      //
      if (std::abs(_samplesDeriv[isampl]) / (_maxADCValue - _minADCValue + 1) > 0.05) {

	// std:: cout << "Setting prepulse due to large negative derivative: sample = " << isampl << ", value = " << _samplesDeriv[isampl]  
	// 	   << ", threshold = " << (_maxADCValue - _minADCValue + 1)*0.05  << std::endl;
	_prePulse = true;
      }

      // Also, if we have two negative derivatives in a row, we have a prepulse
      //
      if (lastNegDeriv) {
	_prePulse = true;
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
    if (_samplesDeriv2nd[isampl] < 0 && _samplesDeriv2nd[isampl + 1] > 0) {
      if  (_samplesDeriv2nd[isampl] < 0.1*_minDeriv2nd) {
	_prePulse = true;
	// std:: cout << "Setting prepulse due to negative 2nd derivative: sample = " << isampl << ", value =  " << _samplesDeriv2nd[isampl] << ", threshold = " << 0.1*_minDeriv2nd << std::endl;
      }
    }
  }

  // Additional specific tests for pre pulses etc.
  //
  if (_deltaTSample < 20) {
    //  
    //  Don't do this check for the 40 MHz data as the pulse often drops below baseline 
    //    due to overshoot, not pre-pulses
    //
    //if //_samplesSub[_Nsample - 1] <=  -_maxADCValue*0.05 ||
    //    if	(_samplesSub[_Nsample - 1] - _samplesSub[_preSampleIdx+1] < -_maxADCValue*0.05) _prePulse = true;

  }


  FillHistogram(_samplesSub, samplesSig);

  //  Stop now if we have no pulse
  //
  if (_fail || !_havePulse) return false;

  if (!_useDelayed) DoFit();
  else DoFitCombined();

  if (!FitFailed()) {
    _fitTimeCorr = _fitTimeSub;

    if (_haveTimingCorrections) {

      //
      // We correct relative to the middle of the amplitude range, divided by 100 to make corrections smaller
      //
      float t0CorrFact = (_fitAmplitude - 500)/100.;

      float correction = (t0CorrParams[0] + t0CorrParams[1]*t0CorrFact + t0CorrParams[2]*t0CorrFact*t0CorrFact +
			  t0CorrParams[3]*t0CorrFact*t0CorrFact*t0CorrFact);

      _fitTimeCorr -= correction;
    }


    // Now check for valid chisq and valid time
    //
    if (_fitChisq / (_fitAmplitude + 1.0e-6) > maxChisqDivAmp) _badChisq = true;
    if (_fitTimeCorr < minT0Corr || _fitTimeCorr > maxT0Corr) _badT0 = true;

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
    std::string tempOptions = options + "e";
    _fitHist->Fit(fitWrapper->GetWrapperTF1(), tempOptions.c_str());
    _bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  }

  _fitAmplitude = fitWrapper->GetAmplitude();
  _fitTime = fitWrapper->GetTime();
  _fitTimeSub = _fitTime - t0Initial;
  _fitChisq = result_ptr->Chi2();

  _fitTau1 = fitWrapper->GetTau1();
  _fitTau2 = fitWrapper->GetTau2();

  _fitAmpError = fitWrapper->GetAmpError();
  //  }
}

void ZDCPulseAnalyzer::DoFitCombined()
{
  // Set the initial values
  //
  float ampInitial = _maxADCValue - _minADCValue;
  float t0Initial = (_useLowGain ? _nominalT0LG : _nominalT0HG);

  ZDCFitWrapper* fitWrapper = _defaultFitWrapper;
  if (PrePulse()) fitWrapper = _prePulseFitWrapper;

  fitWrapper->Initialize(ampInitial, t0Initial);
  if (PrePulse()) {
    if (_samplesDeriv[0] < 0) {
      (static_cast<ZDCPrePulseFitWrapper*>(_prePulseFitWrapper))->SetInitialPrePulseT0(-10);
    }
    else {
      (static_cast<ZDCPrePulseFitWrapper*>(_prePulseFitWrapper))->SetInitialPrePulseT0(0);
    }
  }

  // Set up the virtual fitter
  //
  TFitter* theFitter = 0;

  if (PrePulse()) {
    if (!_prePulseCombinedFitter) _prePulseCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());
    theFitter = _prePulseCombinedFitter;

    //    std::cout << "Testing prepulse wrapper, npar = " << theFitter->GetNumberTotalParameters() << std::endl;
  }
  else {
    if (!_defaultCombinedFitter) {
      _defaultCombinedFitter = MakeCombinedFitter(fitWrapper->GetWrapperTF1());
    }
    theFitter = _defaultCombinedFitter;

    //    std::cout << "Testing default wrapper, npar = " << theFitter->GetNumberTotalParameters() << std::endl;
  }
  
  // Set the static pointers to histograms and function for use in FCN
  //
  _undelayedFitHist = _fitHist;
  _delayedFitHist = _delayedHist;
  _combinedFitFunc = fitWrapper->GetWrapperTF1();

  size_t numFitPar = theFitter->GetNumberTotalParameters();

  theFitter->GetMinuit()->fISW[4] = -1;

  double arglist[100];
  // arglist[0] = 1;
  // arglist[1] = 0;

  // theFitter->ExecuteCommand("SET PAR ", arglist, 2);
  
  theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, 0, 0);

  for (size_t ipar = 0; ipar < numFitPar - 1; ipar++) {
    double parLimitLow, parLimitHigh;

    _combinedFitFunc->GetParLimits(ipar, parLimitLow, parLimitHigh);

    // std::cout << "Setting minuit parameter " << ipar + 1 << " to value " << _combinedFitFunc->GetParameter(ipar) << ", limits = " 
    // 	      << parLimitLow << " to " << parLimitHigh << std::endl;

    theFitter->SetParameter(ipar + 1, _combinedFitFunc->GetParName(ipar), _combinedFitFunc->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh);

    // arglist[0] = ipar + 2;;
    // arglist[1] = _combinedFitFunc->GetParameter(ipar);
    // theFitter->ExecuteCommand("SET PAR ", arglist, 2);

    //    theFitter->GetMinuit()->Command(("SET PAR " + std::to_string(ipar + 2) + " " + std::to_string(_combinedFitFunc->GetParameter(ipar))).c_str() );
  }

  // Now perform the fit
  //
  //  double arglist[100];

  if (_quietFits) {
    theFitter->GetMinuit()->fISW[4] = -1;

    int  ierr= 0; 
    theFitter->GetMinuit()->mnexcm("SET NOWarnings",0,0,ierr);
  }
  else theFitter->GetMinuit()->fISW[4] = 0;

  arglist[0] = 2000; // number of function calls
  arglist[1] = 0.01; // tolerance
  int status = theFitter->ExecuteCommand("MIGRAD",arglist,2);

  double fitAmp = theFitter->GetParameter(1);
  if (status || fitAmp < 1) {
    
    // We first fit with no baseline adjust
    //
    theFitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, 0, 0);
    theFitter->FixParameter(0);

    for (size_t ipar = 0; ipar < numFitPar - 1; ipar++) {
      double parLimitLow, parLimitHigh;
      
      _combinedFitFunc->GetParLimits(ipar, parLimitLow, parLimitHigh);
      theFitter->SetParameter(ipar + 1, _combinedFitFunc->GetParName(ipar), _combinedFitFunc->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh); 
    }

    arglist[0] = 2000; // number of function calls
    arglist[1] = 0.01; // tolerance
    status = theFitter->ExecuteCommand("MIGRAD",arglist,2);
    if (!status) {
      theFitter->ReleaseParameter(0);
      _fitFailed = true;
    }
    else {
      theFitter->ReleaseParameter(0);
      status = theFitter->ExecuteCommand("MIGRAD",arglist,2);

      if (status || fitAmp < 1) {_fitFailed = false;}
    }
  }
  else _fitFailed = false;

  if (!_quietFits) theFitter->GetMinuit()->fISW[4] = -1;

  std::vector<double> funcParams(numFitPar - 1);
  std::vector<double> funcParamErrs(numFitPar - 1);

  // Save the baseline shift between delayed and undelayed samples
  //
  _delayedBaselineShift = theFitter->GetParameter(0);

  // Capture and store the fit function parameteds and errors
  //
  for (size_t ipar = 1; ipar < numFitPar; ipar++) {
    funcParams[ipar - 1] = theFitter->GetParameter(ipar);
    funcParamErrs[ipar - 1] = theFitter->GetParError(ipar);
  }

  _combinedFitFunc->SetParameters(&funcParams[0]);
  _combinedFitFunc->SetParErrors(&funcParamErrs[0]);

  // Cpature the chi-square etc.
  //
  double chi2, edm, errdef;
  int nvpar, nparx;

  theFitter->GetStats(chi2, edm, errdef, nvpar, nparx);

  int ndf = 2*_Nsample - nvpar;

  _combinedFitFunc->SetChisquare(chi2);
  _combinedFitFunc->SetNDF(ndf);

    // add to list of functions
  _undelayedFitHist->GetListOfFunctions()->Add(_combinedFitFunc);
  _delayedFitHist->GetListOfFunctions()->Add(_combinedFitFunc);

  //  if (QuietFits()) options += "Q0";

  // TFitResultPtr result_ptr = _fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());

  // int fitStatus = result_ptr;
  // if (fitStatus != 0) {
  //   TFitResultPtr try2Result_ptr = _fitHist->Fit(fitWrapper->GetWrapperTF1(), options.c_str());
  //   if ((int) try2Result_ptr != 0) _fitFailed = true;
  // }
  // else _fitFailed = false;

  // //  if (!_fitFailed) {
  // _bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  // if (std::abs(_bkgdMaxFraction) > 0.25) {
  //   //    std::string tempOptions = options + "e";
  //   _fitHist->Fit(fitWrapper->GetWrapperTF1(), tempOptions.c_str());
  //   _bkgdMaxFraction = fitWrapper->GetBkgdMaxFraction();
  // }

  _fitAmplitude = fitWrapper->GetAmplitude();
  _fitTime = fitWrapper->GetTime();
  _fitTimeSub = _fitTime - t0Initial;
  _fitChisq = chi2;

  _fitTau1 = fitWrapper->GetTau1();
  _fitTau2 = fitWrapper->GetTau2();

  _fitAmpError = fitWrapper->GetAmpError();
  //  }
}


TFitter* ZDCPulseAnalyzer::MakeCombinedFitter(TF1* func) 
{
  //  gErrorIgnoreLevel = 1001;
  TVirtualFitter::SetDefaultFitter("Minuit");

  size_t nFitParams = func->GetNpar() + 1;
  TFitter* fitter = new TFitter(nFitParams);
  
  fitter->GetMinuit()->fISW[4] = -1;
  fitter->SetParameter(0, "delayBaselineAdjust", 0, 0.01, 0, 0);
  
  for (size_t ipar = 0; ipar < nFitParams - 1; ipar++) {
    double parLimitLow, parLimitHigh;

    func->GetParLimits(ipar, parLimitLow, parLimitHigh);
    fitter->SetParameter(ipar + 1, func->GetParName(ipar), func->GetParameter(ipar), 0.01, parLimitLow, parLimitHigh);
  }
  
  fitter->SetFCN(ZDCPulseAnalyzer::CombinedPulsesFCN);

  double arglist[100];
  arglist[0] = -1;

  return fitter;
}

void ZDCPulseAnalyzer::Dump() const
{
  std::cout << "ZDCPulseAnalyzer dump for tag = " << _tag << std::endl;

  std::cout << "Presample index, value = " << _preSampleIdx << ", " << _preSample << std::endl;

  if (_useDelayed) {
    std::cout << "using delayed samples with delta T = " << _delayedDeltaT << ", and pedestalDiff == " << _delayedPedestalDiff << std::endl;
  }

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

  if (PrePulse()) statusMask |= 1 << PrePulseBit;
  if (FitFailed()) statusMask |= 1 << FitFailedBit;
  if (BadChisq()) statusMask |= 1 << BadChisqBit;
  if (BadT0()) statusMask |= 1 << BadT0Bit;

  return statusMask;
}
