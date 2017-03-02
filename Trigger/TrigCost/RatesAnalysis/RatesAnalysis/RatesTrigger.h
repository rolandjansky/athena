#ifndef RATESANALYSIS_RATESTRIGGER_H
#define RATESANALYSIS_RATESTRIGGER_H 1

#include "RatesHistoBase.h"

class RatesGroup; //fwd

/**
 * @brief Used to calculate the rate for a single trigger at L1 or the HLT 
 */
class RatesTrigger : public RatesHistoBase { 
 public: 

  /**
   * @brief Construct new RatesTrigger to enumerate the rate for a single L1 or HLT trigger
   * Provide pass/fail information to RatesGroups which this trigger is part of.
   * @param name Name of the trigger
   * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
   * @param expressPrescale If the trigger is HLT and in the express group, its express prescale. Otherwise set = 0
   * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
   * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
   * @param doHistograms Flag to mint histograms or not
   * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
   */
  RatesTrigger(const std::string& name,
    const double prescale = 1.,
    const double expressPrescale = -1,
    const std::string& seedName = "",
    const double seedPrecale = 1.,
    const bool doHistograms = true,
    const ExtrapStrat_t extrapolation = ExtrapStrat_t::kLINEAR);
  ~RatesTrigger();

  virtual void reset() { m_pass = false; } //!< If I was used in an event, reset me.

  void setSeedsFromRandom(const bool i) { m_seedsFromRandom = i; } //!< Set if this trigger is to behave as if it seeds from a random L1 item
  
  void setPassed(const bool i = true, const bool unbiasedEvent = false); //!< Set the pass/fail bool. Execute needs to be called separately afterwards.

  /**
   * @brief Set the pass/fail bool and immediately call execute. Should only be done once per event.
   * @param i If the trigger passed or failed the event
   * @weights Struct of weighting information for the event
   */
  void setPassedAndExecute(const bool i, const WeightingValuesSummary_t& weights);

  /**
   * @brief Execute trigger rate emulation. If the trigger passed, add to its rate the effective number of events it is accepting.
   * Also keep track of any express prescale rate
   * @param weights Summary of all event weights which may be used by this trigger
   */
  virtual void execute(const WeightingValuesSummary_t& weights);

  size_t getSeedHash() const { return m_seedHash; } //<! Get the hash of the name of the seed of this trigger.

  const std::string& getSeedName() const { return m_seed; } //!< Get the name of the seed of this trigger

  double getSeedPrescale() const { return m_seedPrescale; } //!< Get the prescale of the seed of this trigger

  size_t getHash() const { return m_nameHash; } //!< Get the hash of the name of this trigger

  const std::string& getName() const { return m_name; } //!< Get the name of this trigger

  /**
   * @brief Gets the triggers prescale
   * @param includeExpress If true, the items express prescale is added on top of its regular prescale
   */
  double getPrescale(const bool includeExpress = false) const;

  bool getPassed() const { return m_pass; } //!< If the trigger passed in the event

  bool getDisabled() const { return (isZero(m_prescale) || isZero(m_seedPrescale)); } //!< If I or my seed were prescaled out

  void setUniqueGroup(const RatesGroup* unique) { m_uniqueGroup = unique; } //!< If I have a group which is calculating my unique rate.

  void setCoherentFactor(const double lowestCommonPrescale) { m_coherentFactor = lowestCommonPrescale; } //!< If i'm in a CPS group, set the lowest commons PS factor of the group

  void setCPS(const std::string& group) { m_CPSID = std::hash<std::string>{}(group); } //!< If I'm in a CPS group, set the group name (I'll keep a copy of the hash)
  
  size_t getCPSID() const { return m_CPSID; } //!< Get the hash of my CPS group name
  
  double getCoherentFactor() const { return m_coherentFactor; } //!< Get the lowest common prescale factor of all triggers in my CPS group

  const std::string printConfig() const; //!< Prints the RatesTrigger's configuration

  /**
   * @brief Prints the RatesTrigger's rate
   * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
   * @return The information to be printed
   */
  virtual const std::string printRate(const double ratesDenominator) const;
 
  /**
   * @brief Prints the RatesTrigger's express rate
   * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
   * @return The information to be printed
   */
  const std::string printExpressRate(const double ratesDenominator) const;

  /**
   * @brief Use my m_extrapolationStrategy to select the correct weight from the supplied WeightingValuesSummary_t
   * @param weights Struct of weights from which to choose the correct one
   * @return The extrapolation weight
   */
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

