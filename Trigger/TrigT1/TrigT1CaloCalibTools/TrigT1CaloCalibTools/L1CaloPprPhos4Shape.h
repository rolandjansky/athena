/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************
// Name: L1CaloPprPhos4Shape.h
// Package: TrigT1CaloCalibConditions
//
// Author: Taylor Childers (childers@mail.cern.ch)
//
// This class can be used to store a PPM Signal Shape
// in the form of a TProfile acquired from a PHOS4 scan.
// Use one of the Fill functions in order order to fill the
// TProfile:
// 1. Fill(ns_step,adc): ns_step should be the nanosecond
//    step in the phos4 scan where the nanosecond step runs
//    from 0 to the number of 25 nanosecond time slices read
//    out times 25 nanoseconds, i.e. in 15+1 readout mode
//    ns_step would run from 0 to 25*15. adc is just the value
//    of the ADC read out for that step.
// 2. Fill(timeSlice,rodHeaderStep,adc): timeSlice is the
//    time slice of the ADC value to be stored. rodHeaderStep
//    is the step number from the DSS board, which is read out
//    via the ROD Headers. adc is just the value
//    of the ADC read out for that step.
// 3. Fill(rodHeaderStep,adc): rodHeaderStep is again the step
//    number from the DSS board, which is read out via the ROD
//    Headers. This time adc is a vector of the ADC values
//    with each entry being a different time slice.
//
// In the Finalize step of the algorithm you should call the
// Finalize() function. It is used to process the raw signal
// and remove the pedestal variation which comes from the
// varrying PHOS4 values. This produces the TH1F histogram
// which can be used as a processed or cleaned up signal shape.
// It also checks the signal and produces a ChanCalibErroCode
// which can be used in the PprChanCalib folder. This function
// produces many files:
//
// pprPhos4SignalShapes,<run number>.root:
//   This contains a TTree with all the values used in this
//   analysis, such as the raw and processed signal histograms
//   and their corresponding peak values and peak bin location.
//   It also contains the pedestal mean and sigma as caluclated
//   from the first 50 ns of the pulse shape histogram.
//   Also the rising slope mean and sigma are included. It also
//   includes the current and newly calculated FullDelayData.
// pprPhos4FullDelayData.<run number>.txt:
//   This contains the file needed by the database in order
//   to update the timing offsets for the PPMs using the
//   FullDelayData attribute.
// pprPhos4RawSignalShapes.<run number>.ps:
//   Contains all the raw signal shapes for viewing.
// pprPhos4ProcessedSignalShapes.<run number>.ps:
//   Contains all the processed signal shapes for viewing.
//
// The pedestal correction uses the first 2 time slices, which
// are assumed to be at the pedestal. This typically means you
// need to be in 15+1 mode with the peak at 7. So if you are
// analyzing in 5+1 mode the processed histo will not be
// correct.
//
// The Get()-er functions are self explanitory I hope.
//
// *********************************************************

#ifndef L1CALOPPRPHOS4SHAPE_H
#define L1CALOPPRPHOS4SHAPE_H

#include <string>
#include <vector>
#include <cmath>

// ROOT includes
#include "TProfile.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TH1.h"

// Get the message service and StoreGate
#include "AthenaKernel/MsgStreamMember.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"

class L1CaloPprPhos4Shape {

public:
   L1CaloPprPhos4Shape(const L1CaloCoolChannelId& coolId);
   ~L1CaloPprPhos4Shape();

   enum CaloLayer {
      LAYER_NOT_SET = 0,
      EM = 1,
      HAD = 2
   };

   void SetMinimumSignalHeight(const unsigned int min) {m_minSignalHeight = min;};
   unsigned int GetMinimumSignalHeight(void) const {return m_minSignalHeight;};

   void SetEta(const float eta) {m_eta = eta;};
   float GetEta(void) const {return m_eta;};

   void SetPhi(const float phi) {m_phi = phi;};
   float GetPhi(void) const {return m_phi;};

   void SetLayer(const CaloLayer layer) {m_layer = layer;};
   CaloLayer GetLayer(void) const {return m_layer;};

   StatusCode Fill(const unsigned int ns_step, const unsigned int adc);
   StatusCode Fill(const unsigned int timeSlice, const unsigned int rodHeaderStep, const int adc);
   StatusCode Fill(const unsigned int rodHeaderStep, const std::vector<int> adc);

   StatusCode Finalize(void);

   TProfile* GetRawSignalShape(void) {return m_raw_signalShape;};
   unsigned int GetRawMaxPeakBin(void) {return m_raw_maxPeakBin;};
   float GetRawMaxPeakValue(void) {return m_raw_maxPeakValue;};
   unsigned int GetRawMaxFullDelayData(void) {return m_raw_maxFullDelayData;};
   unsigned int GetRawFitPeakBin(void) {return m_raw_fitPeakBin;};
   float GetRawFitPeakValue(void) {return m_raw_fitPeakValue;};
   unsigned int GetRawFitFullDelayData(void) {return m_raw_fitFullDelayData;};
   bool GetRawFitPerformed(void) {return m_raw_fitPerformed;};

   TF1* GetRawFit(void) {return m_raw_fit;};

   TH1F* GetProcessedSignalShape(void) {return m_processed_signalShape;};
   unsigned int GetProcessedMaxPeakBin(void) {return m_processed_maxPeakBin;};
   float GetProcessedMaxPeakValue(void) {return m_processed_maxPeakValue;};
   unsigned int GetProcessedMaxFullDelayData(void) {return m_processed_maxFullDelayData;};
   unsigned int GetProcessedFitPeakBin(void) {return m_processed_fitPeakBin;};
   float GetProcessedFitPeakValue(void) {return m_processed_fitPeakValue;};
   unsigned int GetProcessedFitFullDelayData(void) {return m_processed_fitFullDelayData;};
   bool GetProcessedFitPerformed(void) {return m_processed_fitPerformed;};

   TF1* GetProcessedFit(void) {return m_processed_fit;};

   unsigned int GetRawPhos4Setting(void) {return ((m_processed_maxPeakBin - 1) % 25);};

   unsigned int GetCoolId(void) {return m_coolId.id();};

   void SetPedValue(unsigned int value) {m_pedValue = value;};
   void SetL1aFadcSlice(unsigned int slice) {m_l1aFadcSlice = slice;};

   unsigned int GetPedValue(void) {return m_pedValue;};
   unsigned int GetL1aFadcSlice(void) {return m_l1aFadcSlice;};

   float GetRisingSlopeMean(void) {return m_risingSlopeMean;};
   float GetRisingSlopeSigma(void) {return m_risingSlopeSigma;};

   unsigned int GetErrorCode(void) {return m_errorCode.errorCode();};

   float GetPedestalMean(void) {return m_rawPedestalMean;};
   float GetPedestalSigma(void) {return m_rawPedestalSigma;};

   void SetCurrentFullDelayData(unsigned int delay) {m_currentFullDelayData = delay;};
   unsigned int GetCurrentFullDelayData(void) {return m_currentFullDelayData;};

   bool IsValidChannel(void) {return m_isValid;};
   void SetValidChannel(bool validity) {m_isValid = validity;};

   bool IsEnabled(void) {return m_isEnabled;};
   void SetEnabled(bool enabled) {m_isEnabled = enabled;};

   bool IsProcessed(void) {return m_processSignalShape;};


private:
   // don't want C++ implementing these automatically
   L1CaloPprPhos4Shape();
   L1CaloPprPhos4Shape(const L1CaloPprPhos4Shape& rhs);
   void operator=(const L1CaloPprPhos4Shape& rhs);

   Athena::MsgStreamMember* m_log;

   unsigned int m_minSignalHeight;

   L1CaloCoolChannelId m_coolId;
   float m_eta;
   float m_phi;
   CaloLayer m_layer;

   bool m_isValid; // is this a valid channel (used or unused)
   bool m_isEnabled; // is this channel enabled

   unsigned int m_currentFullDelayData; // from PprChanCalib folder
   unsigned int m_pedValue; // from PprChanCalib folder
   unsigned int m_l1aFadcSlice; // from ReadoutConfig folder

   ChanCalibErrorCode m_errorCode; // holds the error bits for the database

   unsigned int CalculateFullDelayData(const int truePeakPosition);

   TProfile* m_raw_signalShape;
   float m_raw_maxPeakValue;
   unsigned int m_raw_maxPeakBin;
   unsigned int m_raw_maxFullDelayData;
   float m_raw_fitPeakValue;
   unsigned int m_raw_fitPeakBin;
   unsigned int m_raw_fitFullDelayData;
   bool m_raw_fitPerformed;

   TF1* m_raw_fit;

   float m_rawPedestalMean;
   float m_rawPedestalSigma;

   std::vector<unsigned int> m_pedCorrectionSum;
   std::vector<unsigned int> m_pedCorrectionSum2;
   std::vector<unsigned int> m_pedCorrectionN;
   std::vector<float> m_pedCorrectionMean;
   std::vector<float> m_pedCorrectionSigma;

   TH1F* m_processed_signalShape;
   float m_processed_maxPeakValue;
   unsigned int m_processed_maxPeakBin;
   unsigned int m_processed_maxFullDelayData;
   float m_processed_fitPeakValue;
   unsigned int m_processed_fitPeakBin;
   unsigned int m_processed_fitFullDelayData;
   bool m_processed_fitPerformed;

   TF1* m_processed_fit;

   float m_risingSlopeMean;
   float m_risingSlopeSigma;

   bool m_processSignalShape;

   // The maximum number of ADC Time Slices read out
   static const unsigned int MAX_ADC_TIME_SLICES = 15;
   static const unsigned int NANOSEC_PER_LHC_CLOCK_TICK = 25;

   // The cut used to check the pedestal is flat
   static const unsigned int MAX_PEDESTAL_SIGMA = 4;
   // The cut used to check if the signal is saturated
   static const unsigned int SATURATED_ADC = 1000;

   void CorrectRawPedestal(void);
   void CalculatePedCorrection(void);
   void CalculatePedestal(TH1* histo, float& pedMean, float& pedSigma);
   void FindBinWithMaxValue(TH1* histo, float& peakValue, unsigned int& peakBin);
   TF1* FitSignalPeak(TH1* histo, const float maxPeakValue, const unsigned int maxPeakBin, const unsigned int pedestal, float& fitPeakValue, unsigned int& fitPeakBin);

   void FillErrorCode(bool signalIsPresent, bool signalIsSaturated);
   bool IsSignalPresent(TH1* histo, const unsigned int threshold) const;
   bool IsSignalSaturated(TH1* histo, const unsigned int saturationPoint) const;
   void CheckRisingSlope(TH1* histo, unsigned int peakBin, float peakValue);

   std::string GetPlotName(const std::string base, const L1CaloCoolChannelId& coolId) const;
   std::string GetPlotTitle(const std::string base, const L1CaloCoolChannelId& coolId) const;
   std::string CoolIdToString(const L1CaloCoolChannelId& coolId) const;

};

class LandGaussFunc {
public:
   double operator() (double *x, double *par) {
      double x_max   =  par[0];
      double ampl    =  par[1];
      double sigma_g =  par[2];
      double sigma_l =  par[3];
      double pedestal = par[4];

      double output = 0;
      if ( x[0] <= par[0])
      {
         output = ampl * exp(((-0.5) * (x[0] - x_max) * (x[0] - x_max) / ((sigma_g) * (sigma_g) ) ) - 0.5) + pedestal;
      }
      else if (x[0] > par[0] )
      {
         const double inv_sigma_l = 1. / sigma_l;
         output = ampl * exp((-0.5) * (((x[0] - x_max) *inv_sigma_l) + (exp(-(x[0] - x_max) * inv_sigma_l) ) ) ) + pedestal;
      }

      return output;
   }
};



#endif

