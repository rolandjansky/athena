/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_RATESSCANTRIGGER_H
#define RATESANALYSIS_RATESSCAMTRIGGER_H 1

#include "RatesTrigger.h"
#include <limits> 

/**
 * Used to calculate a rate scan as a function of some threshold value. Typically a pT cut
 */
class RatesScanTrigger : public RatesTrigger { 
 public: 

  /**
   * @brief enum to describe if a trigger should activate for values >= or <= than the thresold
   */
  enum TriggerBehaviour_t {
    kTriggerAboveThreshold, //!< Trigger for >= threshold
    kTriggerBelowThreshold  //!< Trigger for <= threshold
  };

  /**
   * @brief Construct new RatesScanTrigger to enumerate the rate for a single L1 or HLT trigger as a function of some threshold
   * @param name Name of the trigger
   * @param msgSvc Reference to message service
   * @param thresholdMin The lower threshold of this trigger, rates will not be available below this threshold
   * @param thresholdMax The upper threshold of this trigger, rates will not be available above this threshold
   * @param thresholdBins Granularity 
   * @param behaviour If the trigger should activate above (kTriggerAboveThreshold) or below (kTriggerBelowThreshold) the threshold
   * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
   * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
   * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
   * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
   */
  RatesScanTrigger(const std::string& name,
    IMessageSvc* msgSvc,
    const double thresholdMin,
    const double thresholdMax,
    const uint32_t thresholdBins = 100,
    const TriggerBehaviour_t behaviour = kTriggerBelowThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);

  /**
   * @brief Construct new RatesScanTrigger to enumerate the rate for a single L1 or HLT trigger as a function of some threshold
   * @param name Name of the trigger
   * @param log Reference to message service
   * @param thresholdBinEdged Vector of bin edges to use for quantifying rate as a function of threshold
   * @param behaviour If the trigger should activate above (kTriggerAboveThreshold) or below (kTriggerBelowThreshold) the threshold
   * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
   * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
   * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
   * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
   */
  RatesScanTrigger(const std::string& name,
    IMessageSvc* msgSvc,
    const std::vector<double>& thresholdBinEdges,
    const TriggerBehaviour_t behaviour = kTriggerBelowThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);

  virtual ~RatesScanTrigger();

  RatesScanTrigger(const RatesScanTrigger&) = delete;

  RatesScanTrigger& operator=(const RatesScanTrigger&) = delete;

  void reset() override { m_thresholdPassed = std::numeric_limits<double>::min(); } //!< If I was used in an event, reset me

  /**
   * @brief Sets the threshold the event
   * @param t The threshold reached by the event
   * @param unbiasedEvent If the event was triggered at random online 
   */
  void passThreshold(const double t, const bool unbiasedEvent = false);

  /**
   * @brief Set the pass threshold and immediately call execute. Should only be done once per event.
   * @param t The threshold reached by the event
   * @param weights Set of weighting values which may be needed.
   */
  void setPassedAndExecute(const double t, const WeightingValuesSummary_t& weights);

  /**
   * @brief Execute trigger rate emulation. 
   * If the trigger passed threshold is within the defined range then fill the 
   * relevant histogram bins
   * @param weights Set of weighting values which may be needed.
   */
  void execute(const WeightingValuesSummary_t& weights) override;

  /**
   * @brief Prints the RatesScanTrigger's rate (different output to a regular trigger)
   * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
   */
  const std::string printRate(const double ratesDenominator) const override;

  StatusCode giveThresholdHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name); 

 private: 

  std::unique_ptr<TH1> m_rateScanHist; //!< Even if we are not exporting it - we still need this histo 
  TH1* m_rateScanHistCachedPtr;
  double m_thresholdPassed; //!< Analogous to m_pass. This is the threshold that the trigger passed in the event
  TriggerBehaviour_t m_behaviour; //!< If we need to be above or below the threshold to cause the trigger to fire

}; 


#endif //> !RATESANALYSIS_RATESSCANTRIGGER_H

