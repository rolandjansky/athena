/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesTrigger.h"
#include "RatesAnalysis/RatesGroup.h"

RatesTrigger::RatesTrigger(const std::string& name, IMessageSvc* msgSvc, const double prescale, const double expressPrescale,
                           const std::string& seedName, const double seedPrescale, const bool doHistograms,
                           const ExtrapStrat_t extrapolation) :
  RatesHistoBase(name, msgSvc, doHistograms),
  m_pass(false),
  m_active(false),
  m_seedsFromRandom(false),
  m_rateAccumulator(0.),
  m_rateAccumulator2(0.),
  m_rateExpressAccumulator(0.),
  m_rateExpressAccumulator2(0.),
  m_CPSID(0),
  m_coherentFactor(0.),
  m_uniqueGroup(nullptr),
  m_name(name),
  m_seed(seedName),
  m_nameHash(std::hash<std::string>{}(name)),
  m_seedHash(std::hash<std::string>{}(seedName)),
  m_prescale(prescale < 1. ? 0 : prescale),
  m_expressPrescale(expressPrescale < 1. ? 0 : expressPrescale),
  m_seedPrescale(seedPrescale < 1. ? 0 : seedPrescale),
  m_totalPrescaleWeight(1. / (m_prescale * m_seedPrescale)),
  m_totalPrescaleWeightExpress(1. / (m_prescale * m_seedPrescale * m_expressPrescale)),
  m_extrapolationStrategy(extrapolation)
  {}

RatesTrigger::~RatesTrigger() {}

void RatesTrigger::setPassed(const bool passed, const bool active, const bool unbiasedEvent) { 
  if (m_seedsFromRandom == true && unbiasedEvent == false) return;
  if (m_active && !m_pass) throw std::runtime_error("Cannot pass if not active");
  m_pass = passed;
  m_active = active;
}

void RatesTrigger::setPassedAndExecute(const bool passed, const bool active, const WeightingValuesSummary_t& weights) { 
  if (m_seedsFromRandom == true && weights.m_isUnbiased == false) return;
  if (m_pass == false && m_active == false) { // Protect against two positive calls/event
    if (m_active && !m_pass) throw std::runtime_error("Cannot pass if not active");
    m_pass = passed;
    m_active = active;
    execute(weights);
  }
}


void RatesTrigger::execute(const WeightingValuesSummary_t& weights) {
  // Efficiency
  if (getDisabled()) {
    return;
  }

  const double w_noLScale = m_totalPrescaleWeight * weights.m_enhancedBiasWeight;
  const double w = w_noLScale * getExtrapolationFactor(weights, m_extrapolationStrategy);
  const double wExp = m_totalPrescaleWeightExpress * weights.m_enhancedBiasWeight * getExtrapolationFactor(weights, m_extrapolationStrategy);

  if (m_active) {
    m_ratesActive  += w;
    m_ratesActive2 += w * w;
    if (m_dataCachedPtr != nullptr) {
      m_dataCachedPtr->Fill(RatesBinIdentifier_t::kACTIVE_RAW_BIN, 1.);
      m_dataCachedPtr->Fill(RatesBinIdentifier_t::kACTIVE_WEIGHTED_BIN, w);
    }
  }

  if (m_pass) {
    // The vs. mu histogram is a property of the INPUT event so we don't apply any L scaling here
    if (m_rateVsMuCachedPtr != nullptr) m_rateVsMuCachedPtr->Fill(weights.m_eventMu, w_noLScale);
    // The vs. position in train is agnostic to INPUT event & TARGET conditions - i.e. the bunch train structure is not
    // re-weighted in any way. Hence we can apply whatever extrapolation strategy we want here.
    if (m_rateVsTrainCachedPtr != nullptr) m_rateVsTrainCachedPtr->Fill(weights.m_distanceInTrain, w);
    m_rateAccumulator  += w;
    m_rateAccumulator2 += w * w;
    if (m_dataCachedPtr != nullptr) {
      m_dataCachedPtr->Fill(RatesBinIdentifier_t::kPASS_RAW_BIN, 1.);
      m_dataCachedPtr->Fill(RatesBinIdentifier_t::kPASS_WEIGHTED_OR_BIN, w);
    }
    if (m_expressPrescale >= 1) {
      m_rateExpressAccumulator  += wExp;
      m_rateExpressAccumulator2 += wExp * wExp;
      if (m_dataCachedPtr != nullptr) m_dataCachedPtr->Fill(RatesBinIdentifier_t::kEXPRESS_BIN, wExp);
    }
  }
}

double RatesTrigger::getPrescale(const bool includeExpress) const { 
  if (includeExpress) return m_prescale * m_expressPrescale;
  return m_prescale;
}

const std::string RatesTrigger::printConfig() const {
  std::stringstream ss;
  ss << std::setfill(' ') 
     << "Prescale:" << std::setw(11) << std::left << m_prescale 
     << " xpress:" << std::setw(11) << std::left << m_expressPrescale 
     << " CPSID:" << std::setw(11) << std::left << m_CPSID 
     << " seedPS:" << std::setw(11) << std::left << m_seedPrescale 
     << " : " << (m_seed != "" ? m_seed : "") << (m_seed != "" ? " -> " : "") << m_name;
  return ss.str();
}

const std::string RatesTrigger::printRate(const double ratesDenominator) const {
  std::stringstream ss;
  ss << std::setfill(' '); 
  ss << "Rate: " << std::setw(11) << std::right << m_rateAccumulator/ratesDenominator 
     << " +- "    << std::setw(11) << std::left << sqrt(m_rateAccumulator2)/ratesDenominator << " Hz";
  if (m_uniqueGroup != nullptr) {
    const double unique = (getDisabled() == true ? 0. : m_uniqueGroup->getUniqueWeight(ratesDenominator));
    //const double unique = m_uniqueGroup->m_rateAccumulatorOR / ratesDenominator; // For dbg - this is the rate of N-1 
    // Getting the fractional error of refgular rate and applying it to the unique rate
    const double uniqueErr = (isZero(m_rateAccumulator) ? 0. : (sqrt(m_rateAccumulator2)/m_rateAccumulator) * unique);
    ss << ", Unique Rate: " << std::setw(11) << std::right << unique
       << " +- "           << std::setw(11) << std::left << uniqueErr << " Hz";
  }
  ss << " : ";
  ss << m_name << " [PS:" << m_prescale << "]";
  if (m_seed != "") ss << " <- " << m_seed << " [PS:" << m_seedPrescale << "]";
  ss << " (Extrap:"<< getExtrapolationFactorString(m_extrapolationStrategy) <<")";
  return ss.str();
}

const std::string RatesTrigger::printExpressRate(const double ratesDenominator) const {
  std::stringstream ss;
  ss << std::setfill(' '); 
  ss << "Express Rate: " << std::setw(11) << std::right << m_rateExpressAccumulator/ratesDenominator 
     << " +- "    << std::setw(11) << std::left << sqrt(m_rateExpressAccumulator2)/ratesDenominator << " Hz";
  ss << " : ";
  ss << m_name << " [PS:" << m_prescale << "] [EXPRESS PS:" << m_expressPrescale << "]";
  if (m_seed != "") ss << " <- " << m_seed << " [PS:" << m_seedPrescale << "]";
  ss << " (Extrap:"<< getExtrapolationFactorString(m_extrapolationStrategy) <<")";
  return ss.str();
}

void RatesTrigger::reset() { m_pass = false; m_active = false; }

void RatesTrigger::setSeedsFromRandom(const bool i) { m_seedsFromRandom = i; }

size_t RatesTrigger::getSeedHash() const { return m_seedHash; }

const std::string& RatesTrigger::getSeedName() const { return m_seed; }

double RatesTrigger::getSeedPrescale() const { return m_seedPrescale; }

size_t RatesTrigger::getHash() const { return m_nameHash; }

const std::string& RatesTrigger::getName() const { return m_name; }

bool RatesTrigger::getPassed() const { return m_pass; }

bool RatesTrigger::getActive() const { return m_active; }

bool RatesTrigger::getDisabled() const { return (isZero(m_prescale) || isZero(m_seedPrescale)); }

void RatesTrigger::setUniqueGroup(const RatesGroup* unique) { m_uniqueGroup = unique; } 

void RatesTrigger::setCoherentFactor(const double lowestCommonPrescale) { m_coherentFactor = lowestCommonPrescale; } 

void RatesTrigger::setCPS(const std::string& group) { m_CPSID = std::hash<std::string>{}(group); }

size_t RatesTrigger::getCPSID() const { return m_CPSID; }

double RatesTrigger::getCoherentFactor() const { return m_coherentFactor; } 
