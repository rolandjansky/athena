/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesGroup.h"
#include "RatesAnalysis/RatesTrigger.h"

void RatesCPS::execute(const double prescale) {
  m_weight *= 1. - (1. / ( prescale / m_coherentFactor ) );
}

double RatesCPS::getWeight() const {
  return (1. - m_weight) / m_coherentFactor;
}

RatesGroup::RatesGroup(const std::string& name, const MsgStream& log, const double prescale, const bool doHistograms) :
  RatesHistoBase(name, log, doHistograms),
  m_name(name),
  m_nameHash(std::hash<std::string>{}(name)),
  m_groupPrescale(prescale < 1. ? 0 : prescale),
  m_rateAccumulatorOR(0.),
  m_rateAccumulatorAND(0.),
  m_rateAccumulatorOR2(0.),
  m_rateAccumulatorAND2(0.),
  m_doCachedWeights(false),
  m_cachedWeights(),
  m_useCachedWeights(false),
  m_masterGroup(nullptr),
  m_isExpressGroup(false),
  m_children()
  {}

RatesGroup::~RatesGroup() {}

const std::string RatesGroup::printConfig() const {
  std::stringstream ss;
  ss << m_name << " groupPS:" << m_groupPrescale << std::endl;
  for (const auto& child : m_children) {
    if (child.first != std::hash<std::string>{}("")) ss << "\t" << (**(child.second.begin())).getSeedName() << " ->" << std::endl;
    for (const auto& chain : child.second) {
      ss << "\t\t" << chain->getName() << std::endl;
    }
  }  
  return ss.str();
}

const std::string RatesGroup::printRate(const double ratesDenominator) const {
  std::stringstream ss;
  ss <<"RateOR:" << std::setw(11) << std::right << m_rateAccumulatorOR/ratesDenominator 
     << " +- " << std::setw(11) << std::left << sqrt(m_rateAccumulatorOR2)/ratesDenominator << " Hz, "
     << " RateAND:" << std::setw(11) << std::right << m_rateAccumulatorAND/ratesDenominator 
     << " +- " << std::setw(11) << std::left << sqrt(m_rateAccumulatorAND2)/ratesDenominator << " Hz"
     << " : " <<  m_name;
  return ss.str();
}

void RatesGroup::addToGroup(const RatesTrigger* toAdd) {
  if (m_children.count(toAdd->getSeedHash()) == 0) {
    m_children.insert( std::make_pair(toAdd->getSeedHash(), std::set<const RatesTrigger*>() ) );
    m_cachedWeights.insert( std::make_pair(toAdd->getSeedHash(), 1.) );
  } 
  m_children.at(toAdd->getSeedHash()).insert(toAdd);
}

void RatesGroup::removeFromGroup(const RatesTrigger* toRemove) { 
  if (m_children.count(toRemove->getSeedHash()) == 0) return;
  m_children.at(toRemove->getSeedHash()).erase(toRemove);
}

void RatesGroup::removeOtherL1(const RatesTrigger* toKeep) { 
  for (auto iterator = m_children.begin(); iterator != m_children.end(); /*noop*/) {
    if (iterator->first == toKeep->getSeedHash()) {
      ++iterator; // Keep
    } else {
      iterator = m_children.erase(iterator);
    }
  }
}

void RatesGroup::execute(const WeightingValuesSummary_t& weights) {
  double weightOR = 1., weightAND = 1.;
  if (m_doCachedWeights == true) { // Reset cache
    for (auto& cacheElement : m_cachedWeights) cacheElement.second = 1.;
  }

  // Need a check for an empty group. Otherwise the "AND" logic will
  if (m_children.size() == 0) weightAND = 0;

  for (const auto& element : m_children) { // Loop over all children, partitioned by L1 seed
    const std::set<const RatesTrigger*>& triggers = element.second; // I'm the set of triggers seeded off this item

    double weightL1 = 0;
    double weightHLT_OR = 1.;
    double weightHLT_AND = 1.;
    std::unordered_map<size_t, RatesCPS> weightHLT_CPS; // Map of CPS-group-hash to RatesCPS accumulators

    for (const auto& trigger : triggers) { // 
      if (trigger->getPassed()) {

        const double trigPrescaleReciprocal = 1. / trigger->getPrescale( m_isExpressGroup );

        weightL1 = 1. / trigger->getSeedPrescale();
        weightHLT_AND *= trigPrescaleReciprocal; // TODO - check that CPS doesn't affect AND

        const size_t CPSID = trigger->getCPSID(); 
        if (CPSID == 0) { // No CPS
          weightHLT_OR *= 1. - trigPrescaleReciprocal;
        } else { // Keep track for each CPS group
          RatesCPS& cps = weightHLT_CPS[CPSID];
          cps.setCoherentFactor( trigger->getCoherentFactor() );
          cps.execute( trigPrescaleReciprocal );
        }

      } else { // Trigger FAILED
        weightHLT_AND *= 0;
      }

      // Efficiency check. 
      // If any PS=1 chain passes, the weightHLT_OR = (1 - 1/1) = 0 [which gets inverted to 1 in weightOR below]
      // If any chain fails, weightHLT_AND = 0
      if (isZero(weightHLT_OR) && isZero(weightHLT_AND)) break;
    }

    // Include the CPS chain's contributions
    for (const auto& cps : weightHLT_CPS) weightHLT_OR *= 1. - cps.second.getWeight();

    weightOR  *= 1. - (weightL1 * (1. - weightHLT_OR));
    weightAND *= weightL1 * weightHLT_AND; 

    //if (m_name == "Main") std::cout << "|weightL1:" << weightL1 << ",weightHLT_OR:" << weightHLT_OR << ",weightOR:" << weightOR;

    // If we are caching this result for use by the Unique groups
    if (m_doCachedWeights) {
      for (auto& cacheElement : m_cachedWeights) {
        if (cacheElement.first == element.first) continue; // This is the one L1 seed in the combination we're excluding from this cache
        cacheElement.second *=  1. - (weightL1 * (1. - weightHLT_OR));
      }
    }

  }

  // If we are a unique group then we only actually have one element above - the set of L1 items which have
  // a common seed with our unique trigger. Everything else is identical to the global rate calc so we can fetch it from the cache
  if (m_useCachedWeights == true) {
    size_t myOneAndOnlyL1SeedHash = (m_children.begin())->first;
    weightOR *= m_masterGroup->getCachedWeight( myOneAndOnlyL1SeedHash );
  }

  //if (m_name == "Physics") std::cout << ">>>>>weightOR:" << weightOR << ",wOR:" << wOR << std::endl;

  // We don't apply any lumi extrapolation when filling the MU histogram
  // This is wOR but without weights.m_linearLumiFactor or any other scaling in L
  if (m_rateVsMu != nullptr) m_rateVsMu->Fill(weights.m_eventMu, (1./m_groupPrescale) * weights.m_enhancedBiasWeight * (1. - weightOR));

  //TODO - we currently only let groups scale linearly. Should change this.
  const double w = (1./m_groupPrescale) * weights.m_enhancedBiasWeight * weights.m_linearLumiFactor;
  const double wOR  = w * (1. - weightOR);
  const double wAND = w * weightAND;

  if (m_rateVsTrain != nullptr) m_rateVsTrain->Fill(weights.m_distanceInTrain, wOR);
  m_rateAccumulatorOR   += wOR;
  m_rateAccumulatorAND  += wAND;
  m_rateAccumulatorOR2  += wOR * wOR;
  m_rateAccumulatorAND2 += wAND * wAND;

}

double RatesGroup::getUniqueWeight(const double ratesDenominator) const {
  const double diff = m_masterGroup->m_rateAccumulatorOR - m_rateAccumulatorOR;
  if (isZero(diff)) return 0.;
  return diff / ratesDenominator; 
}
