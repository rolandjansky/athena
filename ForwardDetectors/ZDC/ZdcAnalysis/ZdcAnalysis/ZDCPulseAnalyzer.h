/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCPulseAnalyzer_h
#define _ZDCPulseAnalyzer_h

//#include <valarray>
#include <vector>
#include <string>

#include <TF1.h>
#include <TH1.h>

#include "ZdcAnalysis/ZDCFitWrapper.h"

class ZDCPulseAnalyzer
{
public:
  enum {PulseBit = 0, LowGainBit, FailBit, HGOverflowBit, 
	HGUnderflowBit, PSHGOverUnderflowBit, LGOverflowBit, LGUnderflowBit,
	PrePulseBit, PostPulseBit, FitFailedBit, BadChisqBit, BadT0Bit};

private:
  typedef std::vector<float>::const_iterator SampleCIter;

  //  Static data 
  //
  static std::string _fitOptions;
  static bool _quietFits;

  // Quantities provided/set in the constructor
  //
  std::string _tag;
  size_t _Nsample;
  size_t _preSampleIdx;
  float _deltaTSample;
  int _pedestal;
  float _gainHG;
  bool _forceLG;
  float _tmin;
  float _tmax;
  std::string _fitFunction;
  float _peak2ndDerivMinSample;
  float _peak2ndDerivMinThreshLG;
  float _peak2ndDerivMinThreshHG;

  // Default fit values and cuts that can be set via modifier methods
  //
  int _HGOverflowADC;
  int _HGUnderflowADC;
  int _LGOverflowADC;

  float _nominalT0HG;
  float _nominalT0LG;

  float _nominalTau1;
  float _nominalTau2;

  bool _fixTau1;
  bool _fixTau2;

  float _chisqDivAmpCutLG; // maximum good LG chisq / amplitude
  float _chisqDivAmpCutHG; // maximum good HG chisq / amplitude

  float _T0CutLowLG;  // minimum good corrected time for LG fits
  float _T0CutHighLG; // maximum good corrected time for LG fits

  float _T0CutLowHG;  // minimum good corrected time for HG fits
  float _T0CutHighHG; // maximum good corrected time for HG fits

  bool _haveTimingCorrections;
  std::vector<float> _LGT0CorrParams; // Parameters used to correct the fit LG times
  std::vector<float> _HGT0CorrParams; // Parameters used to correct the fit HG times

  bool _haveNonlinCorr;
  std::vector<float> _nonLinCorrParams;

  // Histogram used to perform the fits and function wrappers
  //
  mutable TH1* _fitHist;

  bool _initializedFits;
  ZDCFitWrapper* _defaultFitWrapper;
  ZDCFitWrapper* _prePulseFitWrapper;

  //  TH1* _LGFitHist;
  // TH1* _fitHist;
  // TF1* _FitFuncNoPrePulse;
  // TF1* _FitFuncPrePulse;

  // Dynamic data loaded for each pulse (event)
  // ==========================================

  // Statuses
  //
  bool _haveData;
  bool _havePulse;
  bool _fail;
  bool _useLowGain;
  bool _HGOverflow;
  bool _HGUnderflow;
  bool _LGOverflow;
  bool _LGUnderflow;
  bool _PSHGOverUnderflow;
  bool _prePulse;
  bool _postPulse;
  bool _fitFailed;
  bool _badT0;
  bool _badChisq;

  // Pulse analysis
  //
  float _preSample;

  float _maxADCValue;
  float _minADCValue;
  float _maxDelta;
  float _minDelta;

  int _maxSampl;
  int _minSampl;
  int _maxDeltaSampl;
  int _minDeltaSampl;

  float _minDeriv2nd;
  int _minDeriv2ndIndex;

  float _fitAmplitude;
  float _fitAmpError;
  float _fitT0;
  float _fitT0Corr;
  float _fitTau1;
  float _fitTau2;
  float _fitChisq;
  float _amplitude;
  float _ampError;
  float _preSampleAmp;
  float _bkgdMaxFraction;

  std::vector<float> _ADCSamplesHGSub;
  std::vector<float> _ADCSamplesLGSub;

  std::vector<float> _ADCSSampSigHG;
  std::vector<float> _ADCSSampSigLG;

  std::vector<float> _samplesSub;
  std::vector<float> _samplesDeriv;
  std::vector<float> _samplesDeriv2nd;
  
  // Private methods
  //
  void Reset();
  void SetDefaults();
  void SetupFitFunctions();

  bool AnalyzeData(const std::vector<float>& samples,        // The samples used for this event
		   const std::vector<float>& samplesSig,     // The "resolution" on the ADC value
		   const std::vector<float>& toCorrParams,   // The parameters used to correct the t0               
		   float maxChisqDivAmp,                     // The maximum chisq / amplitude ratio
		   float minT0Corr, float maxT0Corr,          // The minimum and maximum corrected T0 values
		   float peak2ndDerivMinThresh
		   );


  void FillHistogram(const std::vector<float>& samples, const std::vector<float>& samplesSig) const
  {
    // Set the data and errors in the histogram object
    //
    for (size_t isample = 0; isample < _Nsample; isample++) {
      _fitHist->SetBinContent(isample + 1, samples[isample]);
      _fitHist->SetBinError(isample + 1, samplesSig[isample]);
    }
  }

public:

  ZDCPulseAnalyzer(std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, float gainHG,
		   std::string fitFunction, int peak2ndDerivMinSample, float peak2DerivMinThreshHG, float peak2DerivMinThreshLG);

  ~ZDCPulseAnalyzer();

  static void SetFitOPtions(std::string fitOptions) { _fitOptions = fitOptions;}
  static void SetQuietFits(bool quiet) {_quietFits = quiet;}
  static bool QuietFits() {return _quietFits;}

  void SetForceLG(bool forceLG) {_forceLG = forceLG;}
  bool ForceLG() const {return _forceLG;}

  void SetCutValues(float chisqDivAmpCutHG, float chisqDivAmpCutLG,
		    float deltaT0MinHG, float deltaT0MaxHG, 
		    float deltaT0MinLG, float deltaT0MaxLG) ;

  void SetTauT0Values(bool fixTau1, bool fixTau2, float tau1, float tau2, float t0HG, float t0LG);

  void SetADCOverUnderflowValues(int HGOverflowADC, int HGUnderflowADC, int LGOverflowADC);

  void SetTimingCorrParams(std::vector<float> HGT0CorrParams, std::vector<float> LGT0CorrParams)
  {
    if (HGT0CorrParams.size() == 4 && LGT0CorrParams.size() == 4) {
      _HGT0CorrParams = HGT0CorrParams;
      _LGT0CorrParams = LGT0CorrParams;
      _haveTimingCorrections = true;
    }
    else throw;
  }

  void SetNonlinCorrParams(const std::vector<float>& params) 
  {
    //  Check for valid length
    //
    if (params.size() != 2) throw;

    std::cout << "Setting non-linear parameters for module: " << _tag << ", vlues = " 
	      << params[0] << ", " << params[1] << std::endl;

    _nonLinCorrParams = params;
  }

  bool LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float> ADCSamplesLG);

  bool HaveData() const {return _haveData;}
  bool HavePulse() const {return _havePulse;}
  bool Failed() const {return _fail;}
  bool BadChisq() const {return _badChisq;}
  bool BadT0() const {return _badT0;}
  bool FitFailed() const {return _fitFailed;}
  bool PrePulse() const {return _prePulse;}
  bool UseLowGain() const {return _useLowGain;}

  bool HGOverflow() const {return _HGOverflow;}
  bool HGUnderflow() const {return _HGOverflow;}
  bool LGOverflow() const {return _LGOverflow;}
  bool LGUnderflow() const {return _LGUnderflow;}

  bool PSHGOverUnderflow() const {return _PSHGOverUnderflow;}

  float GetFitAmplitude() const {return _fitAmplitude;}
  float GetFitT0() const {return _fitT0;}
  float GetT0Corr() const {return _fitT0Corr;}
  float GetChisq() const {return _fitChisq;}

  float GetFitTau1() const {return _fitTau1;}
  float GetFitTau2() const {return _fitTau2;}

  float GetAmplitude() const {return _amplitude;}
  float GetAmpError() const {return _ampError;}

  float GetPresample() const {return _preSample;}

  float GetMaxADC() const {return _maxADCValue;}
  float GetMinADC() const {return _minADCValue;}

  int GetMaxADCSample() const {return _maxSampl;}
  int GetMinADCSample() const {return _minSampl;}

  float GetMaxDelta() const {return _maxDelta;}
  float GetMinDelta() const {return _minDelta;}

  float GetMinDeriv2nd() const {return _minDeriv2nd;}
  float GetMinDeriv2ndIndex() const {return _minDeriv2ndIndex;}

  unsigned int GetStatusMask() const;

  float GetPreSampleAmp() const {return _preSampleAmp;}
  float GetBkgdMaxFraction() const {return _bkgdMaxFraction;}

  const TH1* GetHistogramPtr() const {
    //
    // We defer filling the histogram if we don't have a pulse until the histogram is requested
    // 
    if (!_havePulse) {
      if (UseLowGain()) FillHistogram(_samplesSub, _ADCSSampSigLG);
      else FillHistogram(_samplesSub, _ADCSSampSigHG);
    }

    return _fitHist;
  }

  void DoFit();

  void Dump() const;

  const std::vector<float>& GetSamplesSub() const {return _samplesSub;}
  const std::vector<float>& GetSamplesDeriv() const {return _samplesDeriv;}
  const std::vector<float>& GetSamplesDeriv2nd() const {return _samplesDeriv2nd;}
};


#endif
