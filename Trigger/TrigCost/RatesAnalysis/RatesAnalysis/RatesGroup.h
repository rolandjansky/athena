#ifndef RATESANALYSIS_RATESTGROUP_H
#define RATESANALYSIS_RATESTGROUP_H 1

#include "RatesTrigger.h"

/**
 * Helper class to keep track of coherent prescales between chains in a group 
 */
class RatesCPS { 
 public:
  RatesCPS() : m_weight(1.), m_coherentFactor(0.) {}
  void setCoherentFactor(const double coherentFactor) { m_coherentFactor = coherentFactor; }
  void execute(const double prescale);
  double getWeight() const;

 private:
  double m_weight;
  double m_coherentFactor;
};

/**
 * Used to calculate the rate for a group of RatesTrigger objects at L1 or the HLT 
 */
class RatesGroup : public RatesHistoBase {
 public: 

  RatesGroup(const std::string& name, const double prescale = 1., const bool doHistograms = true);
  ~RatesGroup();

  void addToGroup(const RatesTrigger* toAdd);
  void removeFromGroup(const RatesTrigger* toRemove);
  void removeOtherL1(const RatesTrigger* toKeep);

  void execute(const WeightingValuesSummary_t& weights);

  void setExpressGroup(const bool i) { m_isExpressGroup = i; }
  void setDoCachedWeights(const bool i) { m_doCachedWeights = i; }
  void setUseCachedWeights(const bool i) { m_useCachedWeights = i; }
  void duplicateChildren(const RatesGroup* toDuplicate) { m_children = toDuplicate->getChildren(); m_masterGroup = toDuplicate; }
  double getCachedWeight(const size_t l1Hash) const { return m_cachedWeights.at(l1Hash); }
  double getUniqueWeight(const double ratesDenominator) const;
  const std::unordered_map<size_t, std::set<const RatesTrigger*>>& getChildren() const { return m_children; }

  const std::string printConfig() const;
  const std::string printRate(const double ratesDenominator) const;


 private:

  const std::string m_name; //!< Name of the group.
  size_t m_nameHash; //!< Hash of the group's name
  double m_groupPrescale; //!< Prescale to apply to the group as a whole
  public: //TODO remove this
  double m_rateAccumulatorOR; //!< Numeroator for the rate of the OR of all triggers
  double m_rateAccumulatorAND; //!< Numerator for the rate of the AND of all triggers
  double m_rateAccumulatorOR2; //!< For stat error
  double m_rateAccumulatorAND2; //!< For stat error
  
  bool m_doCachedWeights; //!< Used in the global rates group. Cache extra information for the benifit of the unique rate groups
  std::unordered_map<size_t, double> m_cachedWeights; //!< Cached weight of the OR of all triggers *except* for the L1 seed-hash of the key here.
  bool m_useCachedWeights; //!< Efficiency. Required m_masterGroup to have been set.
  const RatesGroup* m_masterGroup; //!< If not nullptr, then use the cached weights info in this master group object

  bool m_isExpressGroup; //!< If this group is calculating for the express stream - also include express prescales.

  std::unordered_map<size_t, std::set<const RatesTrigger*>> m_children; // Sets of triggers in this group, separated by their L1 seeds
};

#endif //> !RATESANALYSIS_RATESGROUP_H

