/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RATESANALYSIS_RATESTGROUP_H
#define RATESANALYSIS_RATESTGROUP_H 1

#include "RatesTrigger.h"

/**
 * @brief Helper class to keep track of coherent prescales between chains in a group
 * This class knows about its group's common prescale factor and can hence apply it coherently at the end 
 */
class RatesCPS { 
 public:
  RatesCPS() : m_weight(1.), m_coherentFactor(0.) {}
  void setCoherentFactor(const double coherentFactor) { m_coherentFactor = coherentFactor; } //!< Set a common prescale factor for this group.
  void execute(const double prescale); //!< Keep track of a union product (OR of many chains) excluding the coherent part.
  double getWeight() const; //!< Return the union product (OR of many chains) coherently weighted by the coherent factor.

 private:
  double m_weight;
  double m_coherentFactor;
};

/**
 * @brief Used to calculate the rate for a group of RatesTrigger objects at L1 or the HLT 
 */
class RatesGroup : public RatesHistoBase {
 public: 

  /**
   * @brief Construct new RatesGroup to enumerate the combined union (OR) and intersection (AND) rate of a set of trigger at L1 or the HLT
   * @param name Name of the group
   * @param msgSvc Reference to message service
   * @param prescale Prescale to apply to the whole group
   * @param doHistograms Flag to mint histograms or not
   * @param doExtrapolation Flag to switch on or off extrapolation within the group
   */
  RatesGroup(const std::string& name, IMessageSvc* msgSvc, const bool doHistograms = true, const bool doExtrapolation = true);

  virtual ~RatesGroup();

  RatesGroup(const RatesGroup&) = delete;

  RatesGroup& operator=(const RatesGroup&) = delete;

  /**
   * @brief Add a trigger to this group. It will be stored in a set mapped to its L1 seed. 
   * 'All L1' items are mapped under an empty string.
   * @param toAdd Pointer to a trigger to add to this group.
   */
  void addToGroup(const RatesTrigger* toAdd);

  /**
   * @brief Remove a trigger from this group. It will be removed from the set mapped to its L1 seed. 
   * 'All L1' items are mapped under an empty string.
   * @param toAdd Pointer to a trigger to be removed.
   */
  void removeFromGroup(const RatesTrigger* toRemove);

  /**
   * @brief Remove from the groups mapping all triggers which have a dissimilar seed to the supplied trigger.
   * The group will be left with exactly one set of triggers with a common L1 seed.
   * This is used in unique rates groups which can use a lot of the calculations done by the master rates group
   * @param toKeep Pointer to a trigger. All other triggers which do not share toKeep's L1 seed will be removed.
   */
  void removeOtherL1(const RatesTrigger* toKeep);

  /**
   * @brief Perform group rates evaluation.
   * We force all HLT chains to only have a single L1 seed, however L1 items may seed many HLT chains.
   * This allows for a factorising algorithm to be utilised.
   * The union (OR) and intercept (AND) are calculated.
   * If this is the master group (the "Main" group of HLT chains) then the sub-weights from all L1 seeds is cached
   * If this is a chain's unique rate group then the cached info from the Main group is used to speed up the calculation
   * Coherent prescales are taken into consideration. 
   * @param weights Struct of all weighting data which may be required in the calculation
   */
  void execute(const WeightingValuesSummary_t& weights); 

  void setExpressGroup(const bool i); //!< Flag this group as the express group (modifies group trigger's prescales)
  void setDoCachedWeights(const bool i); //!< Flag group to cache weights. Called only on the Master group
  void setUseCachedWeights(const bool i); //!< Set to use cached weights from the Master group (need ptr to m_masterGroup)
  void duplicateChildren(const RatesGroup* toDuplicate); //!< Copy in triggers from another group. Sets my master
  double getCachedWeight(const size_t l1Hash) const; //!< Get cached weight from triggers seeding from a given L1 item.
  void setUniqueTrigger(RatesTrigger* trigger); //!< Set trigger I am doing unique rates for
  RatesTrigger* getUniqueTrigger(); //!< Get the trigger I am doing unique rates for

  /**
   * @brief Get the unique rate of a unique-rate group
   * For a group being used to get a unique rate, we need to subtract the rate of this
   * group of N-1 chains from the master group of N chains.
   * @return the unique rate of the trigger being calculated by this group
   */
  double getUniqueWeight(const double ratesDenominator) const;

  const std::unordered_map<size_t, std::set<const RatesTrigger*>>& getChildren() const; //<! Return all triggers in the group, ordered by common L1

  const std::string printConfig() const; //!< Prints the RatesGroup's configuration

  /**
   * @brief Prints the RatesGroup's rate
   * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
   */
  const std::string printRate(const double ratesDenominator) const;

 private:

  const std::string m_name; //!< Name of the group.
  const size_t m_nameHash; //!< Hash of the group's name
  public: //TODO remove this
  double m_rateAccumulatorOR; //!< Numerator for the rate of the OR of all triggers
  double m_rateAccumulatorAND; //!< Numerator for the rate of the AND of all triggers
  double m_rateAccumulatorOR2; //!< For stat error
  double m_rateAccumulatorAND2; //!< For stat error
  
  bool m_doCachedWeights; //!< Used in the global rates group. Cache extra information for the benefit of the unique rate groups
  std::unordered_map<size_t, double> m_cachedWeights; //!< Cached weight of the OR of all triggers *except* for the L1 seed-hash of the key here.
  bool m_useCachedWeights; //!< Efficiency. Required m_masterGroup to have been set.
  const ExtrapStrat_t m_extrapolationStrategy; //!< How this group is to scale with luminosity. Currently supported are linear and none.
  const RatesGroup* m_masterGroup; //!< If not nullptr, then use the cached weights info in this master group object
  RatesTrigger* m_uniqueTrigger; //!< If not nullptr, then a trigger this group is calculating the unique rate for. Needs non-const as fills a histo

  bool m_isExpressGroup; //!< If this group is calculating for the express stream - also include express prescales.

  std::unordered_map<size_t, std::set<const RatesTrigger*>> m_children; // Sets of triggers in this group, separated by their L1 seeds
};

#endif //> !RATESANALYSIS_RATESGROUP_H

