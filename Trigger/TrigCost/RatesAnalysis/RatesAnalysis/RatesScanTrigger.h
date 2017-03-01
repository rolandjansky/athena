#ifndef RATESANALYSIS_RATESSCANTRIGGER_H
#define RATESANALYSIS_RATESSCAMTRIGGER_H 1

#include "RatesTrigger.h"
#include <limits> 

/**
 * Used to calculate a rate scan as a function of some threshold value. Typically a pT cut
 */
class RatesScanTrigger : public RatesTrigger { 
 public: 

  enum TriggerBehaviour_t {kTriggerAboveThreshold, kTriggerBelowThreshold};

  RatesScanTrigger(const std::string& name,
    const double thresholdMin,
    const double thresholdMax,
    const uint32_t thresholdBins = 100,
    const TriggerBehaviour_t behaviour = kTriggerBelowThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);

  RatesScanTrigger(const std::string& name,
    const std::vector<double>& thresholdBinEdges,
    const TriggerBehaviour_t behaviour = kTriggerBelowThreshold,
    const double prescale = 1.,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);

  ~RatesScanTrigger();

  void reset() override { m_thresholdPassed = std::numeric_limits<double>::min(); }

  void passThreshold(const double t, const bool unbiasedEvent = false);
  void setPassedAndExecute(const double t, const WeightingValuesSummary_t& weights);
  void execute(const WeightingValuesSummary_t& weights) override;
  void normaliseHist(const double ratesDenominator) override;

  const std::string printRate(const double ratesDenominator) const override;

  TH1D* getThresholdHist() const { return m_rateScanHist; }

 private: 

  TH1D* m_rateScanHist; //!< Even if we are not exporting - we still need this histo 
  double m_thresholdPassed; //!< Analogous to m_pass. This is the threshold that the trigger passed in the event
  TriggerBehaviour_t m_behaviour; //!< If we need to be above or below the threshold to cause the trigger to fire

}; 


#endif //> !RATESANALYSIS_RATESSCANTRIGGER_H

