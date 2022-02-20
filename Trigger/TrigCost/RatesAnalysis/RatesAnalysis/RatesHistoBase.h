/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_RATESHISTOBASE_H
#define RATESANALYSIS_RATESHISTOBASE_H 1

#include "TH1.h"
#include "TString.h"

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthMessaging.h"

#include <string>
#include <unordered_map>
#include <set>
#include <sstream>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <memory>

class ITHistSvc; // Forward

/**
 * Extrapolation strategy to apply to each emulated trigger.
 * @see RatesAnalysisAlg::setTargetLumi
 */
enum ExtrapStrat_t{
  kLINEAR, //!< Scale trigger linearly with luminosity
  kEXPO_MU, //!< Scale trigger linearly in bunches and exponentially in mu. Exponential modifier factor is to be provided.
  kBUNCH_SCALING, //!< Scale trigger linearly but only in the number of bunches
  kMU_SCALING, //!< Scale trigger linearly but only in the change in \<mu\>
  kNONE //!< Do not scale this trigger for changes in luminosity
};

/**
 * Lables a bin in a histogram.
 */
enum RatesBinIdentifier_t{
  kACTIVE_RAW_BIN, //!< Bin used to store the raw total events in which the trigger was active
  kACTIVE_WEIGHTED_BIN, //!< Bin used to store the weighted events in which the trigger was active
  kPASS_RAW_BIN, //!< Bin used to store the raw total events in which the trigger passed
  kPASS_WEIGHTED_OR_BIN, //!< Bin used to store the total rate (OR)
  kPASS_WEIGHTED_AND_BIN, //!< Bin used to store the total rate (AND)
  kEXPRESS_BIN, //!< Bin used to store the express rate.
  kUNIQUE_BIN, //!< Bin used to store data needed to get the unique rate
  kNRATES_BINS //!< Must always come last
};

/**
 * @brief Structure to hold per-event weights for distribution
 */
struct WeightingValuesSummary_t {
  // Event property weights
  double   m_enhancedBiasWeight; //!< A property of the event derived from online enhanced bias prescales
  double   m_eventMu; //!< The actual number of interactions in the event
  double   m_eventLumi; //!< The instantaneous lumi in cm-2s-1
  bool     m_isUnbiased; //!< If the event was taken online with a RD trigger
  uint32_t m_distanceInTrain; //!< How far into the bunch train the event was, in bunch crossings 
  double   m_eventLiveTime; //!< How much wall-time at P1 did this event represent
  // Lumi weights
  double   m_bunchFactor; //!< What weight needs to be applied to extrapolate rates linear in number of bunches
  double   m_muFactor; //!< What weight needs to be applied to extrapolate rates linear in mu
  double   m_linearLumiFactor; //!< What weight needs to be applied to extrapolate rates linear in mu and bunches 
  double   m_expoMuFactor; //!< What weight needs to be applied to extrapolate rates linear in bunches and exponential in mu.
  const double m_noScaling = 1.; //!< Weight for no scaling.
  const std::string print() {
    std::stringstream ss;
    ss << "WeightSummary: wEB:" << m_enhancedBiasWeight << " mu:" << m_eventMu << " lumi:" << m_eventLumi;
    ss << " isUnbiased:" << m_isUnbiased << " distanceInTrain:" << m_distanceInTrain;
    ss << " liveTime:" << m_eventLiveTime;
    ss << " lwBunch:" << m_bunchFactor << " lwMu:" << m_muFactor;
    ss << " lwExpoMu:" << m_expoMuFactor << " lwLin:" << m_linearLumiFactor;
    return ss.str();
  } 
};

/**
 * @brief Basic base class for any common functionality between RatesTrigger and RatesGroup
 * This means that everyone has access to the same histograms
 */
class RatesHistoBase : public AthMessaging {
 public:

  RatesHistoBase(const std::string& name, IMessageSvc* msgSvc, const bool doHistograms = true);
  virtual ~RatesHistoBase();

  RatesHistoBase(const RatesHistoBase&) = delete;

  RatesHistoBase& operator=(const RatesHistoBase&) = delete;

  StatusCode giveMuHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name); 
  StatusCode giveTrainHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name); 
  StatusCode giveDataHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name); 
  void clearTrainHist(); //!< In some jobs we don't want to do the rates vs. position in train
  TH1* getDataHist(); //!< @return cached, non-owning, histogram pointer or nullptr. Does not cause error
  bool doHistograms() const { return m_doHistograms; } //!< If histogramming was enabled in this rates object
  const std::string& getExtrapolationFactorString(ExtrapStrat_t strat) const;
  double getExtrapolationFactor(const WeightingValuesSummary_t& weights, const ExtrapStrat_t strat) const;

  static bool isZero(double v) { return fabs(v) < 1e-10; } //<! Helper fn

 protected:

  std::string m_name; //!< My name
  bool m_doHistograms; //!< If histogramming is switched on
  std::unique_ptr<TH1> m_rateVsMu; //!< Histogram of rate as a fn. of the input event's mu
  std::unique_ptr<TH1> m_rateVsTrain; //!< Histogram of rate as a fn. of position in bunch train
  std::unique_ptr<TH1> m_data;  //!< Histogram of raw rates quantites, for when we need to normalise offline (e.g. grid processing)
  TH1* m_rateVsMuCachedPtr; //!< Cached, non-owning pointer
  TH1* m_rateVsTrainCachedPtr;  //!< Cached, non-owning pointer
  TH1* m_dataCachedPtr; //!< Cached, non-owning pointer
  bool  m_givenRateVsMu; //!< m_rateVsMu has been given to the THistSvc and should not be deleted
  bool  m_givenRateVsTrain; //!< m_rateVsTrain has been given to the THistSvc and should not be deleted
  bool  m_givenData; //!< m_data has been given to the THistSvc and should not be deleted
};

#endif //> !RATESANALYSIS_RATESHISTOBASE_H
