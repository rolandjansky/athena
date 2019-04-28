/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ZDCPulseAnalyzer_h
#define _ZDCPulseAnalyzer_h

//#include <valarray>
#include <vector>
#include <string>

#include <TF1.h>
#include <TH1.h>
#include <TGraph.h>
#include <TList.h>

#include <AsgTools/MessageCheck.h>
#include "ZdcAnalysis/ZDCFitWrapper.h"

class TFitter;

class ZDCPulseAnalyzer
{
public:
  enum {PulseBit = 0, LowGainBit = 1, FailBit = 2, HGOverflowBit = 3,
	HGUnderflowBit = 4, PSHGOverUnderflowBit = 5, LGOverflowBit = 6, LGUnderflowBit = 7,
	PrePulseBit = 8, PostPulseBit = 9, FitFailedBit = 10, BadChisqBit = 11, BadT0Bit = 12};

private:
  typedef std::vector<float>::const_iterator SampleCIter;

  //  Static data
  //
  static std::string s_fitOptions;
  static bool s_quietFits;
  static TH1* s_undelayedFitHist;
  static TH1* s_delayedFitHist;
  static TF1* s_combinedFitFunc;
  static float s_combinedFitTMax;


  /// @name Functions providing the same interface as AthMessaging
  /// @{

  /// Test the output level of the object
  ///
  /// @param lvl The message level to test against
  /// @return boolean Indicting if messages at given level will be printed
  /// @returns <code>true</code> If messages at level "lvl" will be printed
  ///
  bool msgLvl( const MSG::Level lvl ) const {
    return m_msg->level() <= lvl;};

  /// The standard message stream.
  ///
  /// @param lvl The message level to set the stream to
  /// @returns A reference to the default message stream, set to level "lvl"
  ///
  MsgStream& msg( const MSG::Level lvl ) const {
    (*m_msg) << lvl;return *m_msg;};

  /// The standard message stream.
  ///
  /// @returns A reference to the default message stream of this object.
  ///
  MsgStream& msg() const {
    return *m_msg;};


  /// the message stream we use
  MsgStream *m_msg;

  /// @}

  // Quantities provided/set in the constructor
  //
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

  float m_fitTMax;
  int m_HGOverflowADC;
  int m_HGUnderflowADC;
  int m_LGOverflowADC;

  float m_nominalT0HG;
  float m_nominalT0LG;

  float m_nominalTau1;
  float m_nominalTau2;

  bool m_fixTau1;
  bool m_fixTau2;

  float m_chisqDivAmpCutLG; // maximum good LG chisq / amplitude
  float m_chisqDivAmpCutHG; // maximum good HG chisq / amplitude

  float m_T0CutLowLG;  // minimum good corrected time for LG fits
  float m_T0CutHighLG; // maximum good corrected time for LG fits

  float m_T0CutLowHG;  // minimum good corrected time for HG fits
  float m_T0CutHighHG; // maximum good corrected time for HG fits

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
  ZDCFitWrapper* m_prePulseFitWrapper;

  // Delayed pulse members
  //
  bool m_useDelayed;
  float m_delayedDeltaT;
  float m_delayedPedestalDiff;
  float m_baselineCorr;
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
  bool m_prePulse;
  bool m_fitFailed;
  bool m_badT0;
  bool m_badChisq;

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
  int m_minDeriv2ndIndex;

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

  std::vector<float> m_ADCSamplesHGSub;
  std::vector<float> m_ADCSamplesLGSub;

  std::vector<float> m_ADCSSampSigHG;
  std::vector<float> m_ADCSSampSigLG;

  std::vector<float> m_samplesSub;
  std::vector<float> m_samplesDeriv;
  std::vector<float> m_samplesDeriv2nd;

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
	      m_fitHist->SetBinContent(isample + 1, samples[isample*2]);
	      m_delayedHist->SetBinContent(isample + 1, samples[isample*2 + 1]);

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

public:

  ZDCPulseAnalyzer(MsgStream *val_msg, std::string tag, int Nsample, float deltaTSample, size_t preSampleIdx, int pedestal, float gainHG,
		   std::string fitFunction, int peak2ndDerivMinSample, float peak2DerivMinThreshHG, float peak2DerivMinThreshLG);

  ~ZDCPulseAnalyzer();

  static void SetFitOPtions(std::string fitOptions) { s_fitOptions = fitOptions;}
  static void SetQuietFits(bool quiet) {s_quietFits = quiet;}
  static bool QuietFits() {return s_quietFits;}

  void EnableDelayed(float deltaT, float pedestalShift);

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

    ANA_MSG_INFO ("Setting non-linear parameters for module: " << m_tag << ", vlues = "
                  << params[0] << ", " << params[1]);

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

  float GetMinDeriv2nd() const {return m_minDeriv2nd;}
  float GetMinDeriv2ndIndex() const {return m_minDeriv2ndIndex;}

  unsigned int GetStatusMask() const;

  float GetPreSampleAmp() const {return m_preSampleAmp;}
  float GetBkgdMaxFraction() const {return m_bkgdMaxFraction;}

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

  TGraph* GetCombinedGraph() const {
    //
    // We defer filling the histogram if we don't have a pulse until the histogram is requested
    //
    GetHistogramPtr();

    TGraph* theGraph = new TGraph(2*m_Nsample);
    size_t npts = 0;

    for (int ipt = 0; ipt < m_fitHist->GetNbinsX(); ipt++) {
      theGraph->SetPoint(npts++, m_fitHist->GetBinCenter(ipt + 1), m_fitHist->GetBinContent(ipt + 1));
    }

    for (int iDelayPt = 0; iDelayPt < m_delayedHist->GetNbinsX(); iDelayPt++) {
      theGraph->SetPoint(npts++, m_delayedHist->GetBinCenter(iDelayPt + 1), m_delayedHist->GetBinContent(iDelayPt + 1) - m_delayedBaselineShift);
    }

    TF1* func_p = (TF1*) m_fitHist->GetListOfFunctions()->Last();
    theGraph->GetListOfFunctions()->Add(func_p);
    theGraph->SetName(( std::string(m_fitHist->GetName()) + "combinaed").c_str());

    theGraph->SetMarkerStyle(20);
    theGraph->SetMarkerColor(1);

    return theGraph;
  }

  void Dump() const;

  const std::vector<float>& GetSamplesSub() const {return m_samplesSub;}
  const std::vector<float>& GetSamplesDeriv() const {return m_samplesDeriv;}
  const std::vector<float>& GetSamplesDeriv2nd() const {return m_samplesDeriv2nd;}
};


#endif
