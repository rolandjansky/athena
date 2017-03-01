#ifndef RATESANALYSIS_RATESTRIGGER_H
#define RATESANALYSIS_RATESTRIGGER_H 1

#include "RatesHistoBase.h"

class RatesGroup; //fwd

/**
 * Used to calculate the rate for a single trigger at L1 or the HLT 
 */
class RatesTrigger : public RatesHistoBase { 
 public: 

  RatesTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const bool doHistograms = true,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);
  ~RatesTrigger();

  virtual void reset() { m_pass = false; }

  void setSeedsFromRandom(const bool i) { m_seedsFromRandom = i; }
  void setPassed(const bool i = true, const bool unbiasedEvent = false);
  void setPassedAndExecute(const bool i, const WeightingValuesSummary_t& weights);
  virtual void execute(const WeightingValuesSummary_t& weights);

  size_t getSeedHash() const { return m_seedHash; } 
  const std::string& getSeedName() const { return m_seed; } 
  double getSeedPrescale() const { return m_seedPrescale; } 
  size_t getHash() const { return m_nameHash; }
  const std::string& getName() const { return m_name; } 
  double getPrescale(const bool includeExpress = false) const;
  bool getPassed() const { return m_pass; }
  bool getDisabled() const { return (isZero(m_prescale) || isZero(m_seedPrescale)); }

  void setUniqueGroup(const RatesGroup* unique) { m_uniqueGroup = unique; }

  void setCoherentFactor(const double lowestCommonPrescale) { m_coherentFactor = lowestCommonPrescale; }
  void setCPS(const std::string& group) { m_CPSID = std::hash<std::string>{}(group); }
  size_t getCPSID() const { return m_CPSID; }
  double getCoherentFactor() const { return m_coherentFactor; }

  const std::string printConfig() const;
  virtual const std::string printRate(const double ratesDenominator) const;
  const std::string printExpressRate(const double ratesDenominator) const;

  double getExtrapolationFactor(const WeightingValuesSummary_t& weights) const;

 protected: 

  bool m_pass; //!< Did the trigger pass or not?
  bool m_seedsFromRandom; //!< Does this trigger seed from a random trigger? If so it should only be exposed to unbiased events
  double m_rateAccumulator; //!< Weighted events passed
  double m_rateAccumulator2; //!< Weighted events passed squared
  double m_rateExpressAccumulator; //!< Weighted express stream events 
  double m_rateExpressAccumulator2; //!< Weighted express stream events squared

  size_t m_CPSID; //!< If I'm in a coherent prescale group, my group's ID (hash of the group name)
  double m_coherentFactor; //!< If I'm in a coherent prescale group, the prescale of the lowest non-disabled chain in the group 

  const RatesGroup* m_uniqueGroup; //!< Pointer to the group which is calculating my unique rate

  const std::string m_name; //!< My name
  const std::string m_seed; //!< My seed, "" if no seed
  const size_t m_nameHash; //!< Hash of my name
  const size_t m_seedHash; //!< Has of my seed name
  const double m_prescale; //!< My prescale factor
  const double m_expressPrescale; //!< My express stream prescale factor, gets applied on top of the regular prescale.
  const double m_seedPrescale; //<! My seed's prescale, 1 if no seed.
  const double m_totalPrescaleWeight; //!< Equal to 1/m_seedPrescale*m_prescale
  const double m_totalPrescaleWeightExpress; //!< Equal to 1/m_seedPrescale*m_prescale*m_expressPrescale

  const ExtrapStrat_t m_extrapolationStrategy; //!< How this trigger is to scale with luminosity

}; 


#endif //> !RATESANALYSIS_RATESTRIGGER_H

