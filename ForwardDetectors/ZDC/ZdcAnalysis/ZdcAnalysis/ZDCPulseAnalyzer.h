/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCPulseAnalyzer_h
#define _ZDCPulseAnalyzer_h

#include <vector>
#include <string>

#include <TF1.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TList.h>

// #include <AsgTools/MessageCheck.h>
#include "ZdcAnalysis/ZDCMsg.h"
#include "ZdcAnalysis/ZDCFitWrapper.h"

class TFitter;

class ZDCPulseAnalyzer
{
public:
  enum {PulseBit              = 0,  //  &1
        LowGainBit            = 1,  //  &2
        FailBit               = 2,  //  &4
        HGOverflowBit         = 3,  //  &8
        // -------------------------
        HGUnderflowBit        = 4,  //  &16
        PSHGOverUnderflowBit  = 5,  //  &32
        LGOverflowBit         = 6,  //  &64
        LGUnderflowBit        = 7,  //  &128
        // -------------------------
        PrePulseBit           = 8,  //  &256
        PostPulseBit          = 9,  //  &512
        FitFailedBit          = 10, //  &1024
        BadChisqBit           = 11, //  &2048
        // -------------------------
        BadT0Bit              = 12, //  &4096
        ExcludeEarlyLGBit     = 13, //  &8192
        ExcludeLateLGBit      = 14  //  &16384
       };

private:
  typedef std::vector<float>::const_iterator SampleCIter;

  //  Static data
  //
  static std::string s_fitOptions;
  static bool s_quietFits;
  static bool s_saveFitFunc;
  static TH1* s_undelayedFitHist;
  static TH1* s_delayedFitHist;
  static TF1* s_combinedFitFunc;
  static float s_combinedFitTMax;
  static float s_combinedFitTMin;
  static std::vector<float> pullValues;

  // Quantities provided/set in the constructor
  //
  ZDCMsg::MessageFunctionPtr m_msgFunc_p;
  std::string m_tag;
  size_t m_Nsample;
  size_t m_preSampleIdx;
  float m_deltaTSample;
  int m_pedestal;
  float m_gainHG;
  bool m_forceLG;
  float m_tmin;
  float m_tmax;

  std::string m_fitFunction;
  size_t m_peak2ndDerivMinSample;
  size_t m_peak2ndDerivMinTolerance;
  float m_peak2ndDerivMinThreshLG;
  float m_peak2ndDerivMinThreshHG;

  // Default fit values and cuts that can be set via modifier methods
  //
  bool m_adjTimeRangeEvent; // indicates whether we adjust the time range for this specific event

  int m_minSampleEvt;
  int m_maxSampleEvt;

  int m_HGOverflowADC;
  int m_HGUnderflowADC;
  int m_LGOverflowADC;

  float m_nominalT0HG;
  float m_nominalT0LG;

  float m_nominalTau1;
  float m_nominalTau2;

  bool m_fixTau1;
  bool m_fixTau2;

  float m_defaultFitTMax;   // user-provided upper limit on samples to be included in fit
  float m_defaultFitTMin;   // user-provided upper limit on samples to be included in fit

  float m_chisqDivAmpCutLG; // maximum good LG chisq / amplitude
  float m_chisqDivAmpCutHG; // maximum good HG chisq / amplitude

  float m_T0CutLowLG;  // minimum good corrected time for LG fits
  float m_T0CutHighLG; // maximum good corrected time for LG fits

  float m_T0CutLowHG;  // minimum good corrected time for HG fits
  float m_T0CutHighHG; // maximum good corrected time for HG fits

  float m_defaultT0Max;   // Upper limit on pulse t0
  float m_defaultT0Min;   // Lower limit on pulse t0

  bool m_haveTimingCorrections;
  std::vector<float> m_LGT0CorrParams; // Parameters used to correct the fit LG times
  std::vector<float> m_HGT0CorrParams; // Parameters used to correct the fit HG times

  bool m_haveNonlinCorr;
  std::vector<float> m_nonLinCorrParams;

  // Histogram used to perform the fits and function wrappers
  //
  mutable TH1* m_fitHist;

  bool m_initializedFits;
  ZDCFitWrapper* m_defaultFitWrapper;
  ZDCPrePulseFitWrapper* m_prePulseFitWrapper;

  // Delayed pulse members
  //
  bool  m_useDelayed;
  bool  m_useFixedBaseline;
  float m_delayedDeltaT;
  float m_delayedPedestalDiff;
  mutable TH1* m_delayedHist;

  TFitter* m_prePulseCombinedFitter;
  TFitter* m_defaultCombinedFitter;

  // Dynamic data loaded for each pulse (event)
  // ==========================================

  // Statuses
  //
  bool m_haveData;
  bool m_havePulse;
  bool m_fail;
  bool m_useLowGain;
  bool m_HGOverflow;
  bool m_HGUnderflow;
  bool m_LGOverflow;
  bool m_LGUnderflow;
  bool m_PSHGOverUnderflow;
  bool m_preExpTail;
  bool m_prePulse;
  bool m_postPulse;
  bool m_fitFailed;
  bool m_badT0;
  bool m_badChisq;

  bool  m_backToHG_pre;
  float m_baselineCorr;

  // Pulse analysis
  //
  float m_preSample;

  float m_maxADCValue;
  float m_minADCValue;
  float m_maxDelta;
  float m_minDelta;

  int m_maxSampl;
  int m_minSampl;

  float m_minDeriv2nd;
  int   m_minDeriv2ndIndex;

  float m_fitTMax;          // event-by-event specified fit tmax
  float m_fitTMin;          // event-by-event specified fit tmin

  float m_initialPrePulseT0;
  float m_initialPrePulseAmp;

  float m_fitAmplitude;
  float m_fitAmpError;
  float m_fitTime;
  float m_fitTimeSub;
  float m_fitTimeCorr;
  float m_fitTau1;
  float m_fitTau2;
  float m_fitChisq;
  float m_amplitude;
  float m_ampError;
  float m_preSampleAmp;
  float m_bkgdMaxFraction;
  float m_delayedBaselineShift;

  int m_lastHGOverFlowSample  = -999;
  int m_firstHGOverFlowSample = 999;

  std::vector<float> m_ADCSamplesHGSub;
  std::vector<float> m_ADCSamplesLGSub;

  std::vector<float> m_ADCSSampSigHG;
  std::vector<float> m_ADCSSampSigLG;

  std::vector<float> m_samplesSub;
  std::vector<float> m_samplesDeriv;
  std::vector<float> m_samplesDeriv2nd;

  std::vector<float> m_fitPulls;

  // Private methods
  //
  void Reset();
  void SetDefaults();
  void SetupFitFunctions();

  bool AnalyzeData(size_t nSamples, size_t preSample,
                   const std::vector<float>& samples,        // The samples used for this event
                   const std::vector<float>& samplesSig,     // The "resolution" on the ADC value
                   const std::vector<float>& toCorrParams,   // The parameters used to correct the t0
                   float maxChisqDivAmp,                     // The maximum chisq / amplitude ratio
                   float minT0Corr, float maxT0Corr,          // The minimum and maximum corrected T0 values
                   float peak2ndDerivMinThresh
                  );


  void FillHistogram(const std::vector<float>& samples, const std::vector<float>& samplesSig) const
  {
    if (!m_useDelayed) {
      // Set the data and errors in the histogram object
      //
      for (size_t isample = 0; isample < m_Nsample; isample++) {
        m_fitHist->SetBinContent(isample + 1, samples[isample]);
        m_fitHist->SetBinError(isample + 1, samplesSig[isample]);
      }
    }
    else {
      // Set the data and errors in the histogram object
      //
      for (size_t isample = 0; isample < m_Nsample; isample++) {
        m_fitHist->SetBinContent(isample + 1, samples[isample * 2]);
        m_delayedHist->SetBinContent(isample + 1, samples[isample * 2 + 1]);

        m_fitHist->SetBinError(isample + 1, samplesSig[isample]); // ***** horrible hack: fix ASAP
        m_delayedHist->SetBinError(isample + 1, samplesSig[isample]);
      }

    }
  }

  void DoFit();
  void DoFitCombined();

  static TFitter* MakeCombinedFitter(TF1* func);

  //  The minuit FCN used for fitting combined undelayed and delayed pulses
  //
  static void CombinedPulsesFCN(int& numParam, double*, double& f, double* par, int flag);

  void UpdateFitterTimeLimits(TFitter* fitter, ZDCFitWrapper* wrapper, bool prePulse);

public:

  ZDCPulseAnalyzer(ZDCMsg::MessageFunctionPtr msgFunc_p, std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, float gainHG,
                   std::string fitFunction, int peak2ndDerivMinSample, float peak2DerivMinThreshHG, float peak2DerivMinThreshLG);

  ~ZDCPulseAnalyzer();

  static void SetFitOPtions(std::string fitOptions) { s_fitOptions = fitOptions;}
  static void SetQuietFits  (bool quiet) {s_quietFits = quiet;}
  static void SetSaveFitFunc(bool save ) {s_saveFitFunc = save;}
  static bool QuietFits() {return s_quietFits;}

  void EnableDelayed(float deltaT, float pedestalShift, bool fixedBaseline = false);

  void SetPeak2ndDerivMinTolerance(size_t tolerance) {m_peak2ndDerivMinTolerance = tolerance;}

  void SetForceLG(bool forceLG) {m_forceLG = forceLG;}
  bool ForceLG() const {return m_forceLG;}

  void SetCutValues(float chisqDivAmpCutHG, float chisqDivAmpCutLG,
                    float deltaT0MinHG, float deltaT0MaxHG,
                    float deltaT0MinLG, float deltaT0MaxLG) ;

  void SetTauT0Values(bool fixTau1, bool fixTau2, float tau1, float tau2, float t0HG, float t0LG);

  void SetADCOverUnderflowValues(int HGOverflowADC, int HGUnderflowADC, int LGOverflowADC);

  void SetTimingCorrParams(std::vector<float> HGT0CorrParams, std::vector<float> LGT0CorrParams)
  {
    if (HGT0CorrParams.size() == 4 && LGT0CorrParams.size() == 4) {
      m_HGT0CorrParams = HGT0CorrParams;
      m_LGT0CorrParams = LGT0CorrParams;
      m_haveTimingCorrections = true;
    }
    else throw;
  }

  void SetFitTimeMax(float tmax);

  void SetNonlinCorrParams(const std::vector<float>& params)
  {
    //  Check for valid length
    //
    if (params.size() != 2) throw;

    (*m_msgFunc_p)(ZDCMsg::Info, ("Setting non-linear parameters for module: " + m_tag + ", vlues = " + std::to_string(params[0]) + ", " + std::to_string(params[1])));

    m_nonLinCorrParams = params;
    m_haveNonlinCorr = true;
  }

  bool LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float> ADCSamplesLG);

  bool LoadAndAnalyzeData(std::vector<float> ADCSamplesHG, std::vector<float> ADCSamplesLG,
                          std::vector<float> ADCSamplesHGDelayed, std::vector<float> ADCSamplesLGDelayed);

  bool HaveData() const {return m_haveData;}
  bool HavePulse() const {return m_havePulse;}
  bool Failed() const {return m_fail;}
  bool BadChisq() const {return m_badChisq;}
  bool BadT0() const {return m_badT0;}
  bool FitFailed() const {return m_fitFailed;}
  bool PrePulse() const {return m_prePulse;}
  bool PostPulse() const {return m_postPulse;}
  bool UseLowGain() const {return m_useLowGain;}

  bool HGOverflow() const {return m_HGOverflow;}
  bool HGUnderflow() const {return m_HGUnderflow;}
  bool LGOverflow() const {return m_LGOverflow;}
  bool LGUnderflow() const {return m_LGUnderflow;}

  bool PSHGOverUnderflow() const {return m_PSHGOverUnderflow;}

  float GetFitAmplitude() const {return m_fitAmplitude;}
  float GetFitT0() const {return m_fitTime;}
  float GetT0Sub() const {return m_fitTimeSub;}
  float GetT0Corr() const {return m_fitTimeCorr;}
  float GetChisq() const {return m_fitChisq;}

  float GetFitTau1() const {return m_fitTau1;}
  float GetFitTau2() const {return m_fitTau2;}

  float GetAmplitude() const {return m_amplitude;}
  float GetAmpError() const {return m_ampError;}

  float GetPresample() const {return m_preSample;}

  float GetMaxADC() const {return m_maxADCValue;}
  float GetMinADC() const {return m_minADCValue;}

  int GetMaxADCSample() const {return m_maxSampl;}
  int GetMinADCSample() const {return m_minSampl;}

  float GetMaxDelta() const {return m_maxDelta;}
  float GetMinDelta() const {return m_minDelta;}

  float GetFitTMax() const {return m_fitTMax;}
  float GetFitTMin() const {return m_fitTMin;}

  float GetdelayBS() const {return m_delayedBaselineShift;}

  float GetMinDeriv2nd() const {return m_minDeriv2nd;}
  float GetMinDeriv2ndIndex() const {return m_minDeriv2ndIndex;}

  unsigned int GetStatusMask() const;

  float GetPreSampleAmp() const {return m_preSampleAmp;}
  float GetBkgdMaxFraction() const {return m_bkgdMaxFraction;}

  float GetDelayedBaselineShiftFit() const {return m_delayedBaselineShift;}
  float GetDelayedBaselineCorr() const {return m_baselineCorr;}

  const TH1* GetHistogramPtr() const {
    //
    // We defer filling the histogram if we don't have a pulse until the histogram is requested
    //
    if (!m_havePulse) {
      if (UseLowGain()) FillHistogram(m_samplesSub, m_ADCSSampSigLG);
      else FillHistogram(m_samplesSub, m_ADCSSampSigHG);
    }

    return m_fitHist;
  }

  TGraphErrors* GetCombinedGraph() const {
    //
    // We defer filling the histogram if we don't have a pulse until the histogram is requested
    //
    GetHistogramPtr();

    TGraphErrors* theGraph = new TGraphErrors(2 * m_Nsample);
    size_t npts = 0;

    for (int ipt = 0; ipt < m_fitHist->GetNbinsX(); ipt++) {
      theGraph->SetPoint(npts, m_fitHist->GetBinCenter(ipt + 1), m_fitHist->GetBinContent(ipt + 1));
      theGraph->SetPointError(npts++, 0, m_fitHist->GetBinError(ipt + 1));
    }

    for (int iDelayPt = 0; iDelayPt < m_delayedHist->GetNbinsX(); iDelayPt++) {
      theGraph->SetPoint(npts, m_delayedHist->GetBinCenter(iDelayPt + 1), m_delayedHist->GetBinContent(iDelayPt + 1) - m_delayedBaselineShift);
      theGraph->SetPointError(npts++, 0, m_delayedHist->GetBinError(iDelayPt + 1));
    }

    TF1* func_p = (TF1*) m_fitHist->GetListOfFunctions()->Last();
    theGraph->GetListOfFunctions()->Add(func_p);
    theGraph->SetName(( std::string(m_fitHist->GetName()) + "combinaed").c_str());

    theGraph->SetMarkerStyle(20);
    theGraph->SetMarkerColor(1);

    return theGraph;
  }

  std::unique_ptr<TH1> GetFitPulls() const
  {
    int nbins = (m_useDelayed ? 2 * m_Nsample : m_Nsample);
    std::unique_ptr<TH1> hist {new TH1F((std::string("FitPulls") + m_tag).c_str(), "", nbins, m_tmin, m_tmax)};

    for (size_t ibin = 0; ibin < (size_t) nbins; ibin++) {
      hist->SetBinContent(ibin + 1, m_fitPulls[ibin]);
    }

    return hist;
  }

  void Dump() const;
  void Dump_setting() const;

  const std::vector<float>& GetSamplesSub() const {return m_samplesSub;}
  const std::vector<float>& GetSamplesDeriv() const {return m_samplesDeriv;}
  const std::vector<float>& GetSamplesDeriv2nd() const {return m_samplesDeriv2nd;}
};


#endif
