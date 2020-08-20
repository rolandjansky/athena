/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
        ExcludeLateLGBit      = 14, //  &16384
        preExpTailBit         = 15,  // &32768
        //
        FitMinAmpBit          = 16,   // 0x10000
        RepassPulseBit        = 17,    // 0x20000
        ArmSumIncludeBit      = 18,
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

  bool m_useDelayed;

  bool m_enableRepass;
  float m_peak2ndDerivMinRepassLG;
  float m_peak2ndDerivMinRepassHG;

  // Default fit values and cuts that can be set via modifier methods
  //
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

  float m_fitAmpMinHG;      // Minimum amplitude in the fit
  float m_fitAmpMinLG;      // Minimum amplitude in the fit

  float m_fitAmpMaxHG;      // Minimum amplitude in the fit
  float m_fitAmpMaxLG;      // Minimum amplitude in the fit

  //
  bool m_haveTimingCorrections;
  std::vector<float> m_LGT0CorrParams; // Parameters used to correct the fit LG times
  std::vector<float> m_HGT0CorrParams; // Parameters used to correct the fit HG times

  bool m_haveNonlinCorr;
  std::vector<float> m_nonLinCorrParams;

  // Histogram used to perform the fits and function wrappers
  //
  mutable std::unique_ptr<TH1> m_fitHist;

  bool m_initializedFits;
  std::unique_ptr<ZDCFitWrapper> m_defaultFitWrapper;
  std::unique_ptr<ZDCPrePulseFitWrapper> m_prePulseFitWrapper;

  // Members to keep track of adjustments to time range used in analysis/fit
  //
  bool m_adjTimeRangeEvent; // indicates whether we adjust the time range for this specific event

  int m_minSampleEvt;
  int m_maxSampleEvt;

  // Delayed pulse members
  //
  bool  m_useFixedBaseline;
  float m_delayedDeltaT;
  float m_delayedPedestalDiff;
  mutable std::unique_ptr<TH1> m_delayedHist;

  std::unique_ptr<TFitter> m_prePulseCombinedFitter;
  std::unique_ptr<TFitter> m_defaultCombinedFitter;

  // Dynamic data loaded for each pulse (event)
  // ==========================================

  // -----------------------
  // Statuses
  //
  bool m_haveData;

  bool m_havePulse;
  bool m_useLowGain;
  bool m_fail;
  bool m_HGOverflow;

  bool m_HGUnderflow;
  bool m_PSHGOverUnderflow;
  bool m_LGOverflow;
  bool m_LGUnderflow;

  bool m_prePulse;
  bool m_postPulse;
  bool m_fitFailed;
  bool m_badChisq;

  bool m_badT0;
  bool m_ExcludeEarly;
  bool m_ExcludeLate;
  bool m_preExpTail;

  bool m_fixPrePulse;
  bool m_fixPostPulse;
  bool m_fitMinAmp;
  bool m_repassPulse;

  // -----------------------

  bool  m_backToHG_pre;
  float m_baselineCorr;

  // Pulse analysis
  //
  int m_usedPresampIdx;
  float m_preSample;

  float m_maxADCValue;
  float m_minADCValue;
  float m_maxDelta;
  float m_minDelta;

  int m_maxSampl;
  int m_minSampl;

  float m_initialExpAmp;
  float m_minDeriv2nd;
  int   m_minDeriv2ndIndex;

  float m_fitTMax;          // event-by-event specified fit tmax
  float m_fitTMin;          // event-by-event specified fit tmin

  float m_fitPostT0lo;      // use to assign lower bound of post pulse T0

  float m_initialPrePulseT0;
  float m_initialPrePulseAmp;

  float m_initialPostPulseT0;

  float m_fitAmplitude;
  float m_fitAmpError;
  float m_fitTime;
  float m_fitTimeSub;
  float m_fitTimeCorr;
  float m_fitTCorr2nd;
  float m_fitTau1;
  float m_fitTau2;
  float m_fitChisq;
  float m_fitPreT0;
  float m_fitPreAmp;
  float m_fitPostT0;
  float m_fitPostAmp;
  float m_fitExpAmp;
  float m_amplitude;
  float m_ampError;
  float m_preSampleAmp;
  float m_preAmplitude;
  float m_postAmplitude;
  float m_expAmplitude;
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
  void Reset(bool reanalyze = false);
  void SetDefaults();
  void SetupFitFunctions();

  bool DoAnalysis(bool repass);

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

  static std::unique_ptr<TFitter> MakeCombinedFitter(TF1* func);

  //  The minuit FCN used for fitting combined undelayed and delayed pulses
  //
  static void CombinedPulsesFCN(int& numParam, double*, double& f, double* par, int flag);

  void UpdateFitterTimeLimits(TFitter* fitter, ZDCFitWrapper* wrapper, bool prePulse);

public:

  ZDCPulseAnalyzer(ZDCMsg::MessageFunctionPtr msgFunc_p, std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, float gainHG,
                   std::string fitFunction, int peak2ndDerivMinSample, float peak2DerivMinThreshHG, float peak2DerivMinThreshLG);

  ~ZDCPulseAnalyzer(){}

  static void SetFitOPtions(std::string fitOptions) { s_fitOptions = fitOptions;}
  static void SetQuietFits  (bool quiet) {s_quietFits = quiet;}
  static void SetSaveFitFunc(bool save ) {s_saveFitFunc = save;}
  static bool QuietFits() {return s_quietFits;}

  void EnableDelayed(float deltaT, float pedestalShift, bool fixedBaseline = false);

  void EnableRepass(float peak2ndDerivMinRepassHG, float peak2ndDerivMinRepassLG);

  void SetPeak2ndDerivMinTolerance(size_t tolerance) {m_peak2ndDerivMinTolerance = tolerance;}

  void SetForceLG(bool forceLG) {m_forceLG = forceLG;}
  bool ForceLG() const {return m_forceLG;}

  void SetCutValues(float chisqDivAmpCutHG, float chisqDivAmpCutLG,
                    float deltaT0MinHG, float deltaT0MaxHG,
                    float deltaT0MinLG, float deltaT0MaxLG) ;

  void SetFitMinMaxAmp(float minAmpHG, float minAmpLG, float maxAmpHG, float maxAmpLG);

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

  bool LoadAndAnalyzeData(const std::vector<float>& ADCSamplesHG, const std::vector<float>& ADCSamplesLG);

  bool LoadAndAnalyzeData(const std::vector<float>& ADCSamplesHG, const std::vector<float>& ADCSamplesLG,
                          const std::vector<float>& ADCSamplesHGDelayed, const std::vector<float>& ADCSamplesLGDelayed);

  bool ReanalyzeData();

  bool HaveData() const {return m_haveData;}

  // ------------------------------------------------------------
  // Status bit setting functions
  //
  bool HavePulse()  const {return m_havePulse;}
  bool UseLowGain() const {return m_useLowGain;}
  bool Failed()     const {return m_fail;}
  bool HGOverflow() const {return m_HGOverflow;}

  bool HGUnderflow()       const {return m_HGUnderflow;}
  bool PSHGOverUnderflow() const {return m_PSHGOverUnderflow;}
  bool LGOverflow()        const {return m_LGOverflow;}
  bool LGUnderflow()       const {return m_LGUnderflow;}

  bool PrePulse()  const {return m_prePulse;}
  bool PostPulse() const {return m_postPulse;}
  bool FitFailed() const {return m_fitFailed;}
  bool BadChisq()  const {return m_badChisq;}

  bool BadT0()          const {return m_badT0;}
  bool ExcludeEarlyLG() const {return m_ExcludeEarly;}
  bool ExcludeLateLG()  const {return m_ExcludeLate;}
  bool preExpTail()     const {return m_preExpTail;}
  bool fitMinimumAmplitude() const {return m_fitMinAmp;}
  bool repassPulse() const {return m_repassPulse;}
  bool ArmSumInclude() const {return HavePulse() && !(FitFailed() || BadChisq() || BadT0() || fitMinimumAmplitude());}

  // ------------------------------------------------------------


  // ---------------------------
  // Get fit parameters
  //
  float GetFitAmplitude() const {return m_fitAmplitude;}
  float GetFitT0()        const {return m_fitTime;}
  float GetT0Sub()        const {return m_fitTimeSub;}
  float GetT0Corr()       const {return m_fitTimeCorr;}
  float GetChisq()        const {return m_fitChisq;}
  float GetFitTau1()      const {return m_fitTau1;}
  float GetFitTau2()      const {return m_fitTau2;}
  float GetFitPreT0()     const {return m_fitPreT0;}
  float GetFitPreAmp()    const {return m_preAmplitude;}
  float GetFitPostT0()    const {return m_fitPostT0;}
  float GetFitPostAmp()   const {return m_postAmplitude;}
  float GetFitExpAmp()    const {return m_expAmplitude;}
  // ---------------------------

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

    return m_fitHist.get();
  }

  std::shared_ptr<TGraphErrors> GetCombinedGraph() const;
  std::shared_ptr<TGraphErrors> GetGraph() const;


  std::unique_ptr<TH1> GetFitPulls() const
  {
    int nbins = (m_useDelayed ? 2 * m_Nsample : m_Nsample);
    std::unique_ptr<TH1> hist {new TH1F((std::string("FitPulls") + m_tag).c_str(), "", nbins, m_tmin, m_tmax)};

    for (size_t ibin = 0; ibin < (size_t) nbins; ibin++) {
      hist->SetBinContent(ibin + 1, m_fitPulls[ibin]);
    }

    return hist;
  }

  std::shared_ptr<TGraphErrors> GetUndelayedGraph() const;
  std::shared_ptr<TGraphErrors> GetDelayedGraph() const;

  void Dump() const;
  void Dump_setting() const;

  const std::vector<float>& GetSamplesSub() const {return m_samplesSub;}
  const std::vector<float>& GetSamplesDeriv() const {return m_samplesDeriv;}
  const std::vector<float>& GetSamplesDeriv2nd() const {return m_samplesDeriv2nd;}
};


#endif
