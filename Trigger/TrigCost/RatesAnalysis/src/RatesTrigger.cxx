/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesTrigger.h"
#include "RatesAnalysis/RatesGroup.h"

RatesTrigger::RatesTrigger(const std::string& name, const MsgStream& log, const double prescale, const double expressPrescale,
                           const std::string& seedName, const double seedPrescale, const bool doHistograms,
                           const ExtrapStrat_t extrapolation) :
  RatesHistoBase(name, log, (prescale < 1. || seedPrescale < 1. ? false : doHistograms)),
  m_pass(false),
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

void RatesTrigger::setPassed(const bool i, const bool unbiasedEvent) { 
  if (m_seedsFromRandom == true && unbiasedEvent == false) return;
  m_pass = i;
}

void RatesTrigger::setPassedAndExecute(const bool i, const WeightingValuesSummary_t& weights) { 
  if (m_seedsFromRandom == true && weights.m_isUnbiased == false) return;
  if (m_pass == false) { // Protect against two positive calls/event
    m_pass = i;
    execute(weights);
  }
}

void RatesTrigger::execute(const WeightingValuesSummary_t& weights) {
  if (m_pass == false) return; 
  double w =  m_totalPrescaleWeight * weights.m_enhancedBiasWeight;
  // The vs. mu histogram is a property of the INPUT event so we don't apply any L scaling here
  if (m_rateVsMu != nullptr) m_rateVsMu->Fill(weights.m_eventMu, w);
  w *= getExtrapolationFactor(weights);
  // The vs. position in train is agnostic to INPUT event & TARGET conditions - i.e. the bunch train structure is not
  // re-weighted in any way. Hence we can apply whatever extrapolation strategy we want here.
  if (m_rateVsTrain != nullptr) m_rateVsTrain->Fill(weights.m_distanceInTrain, w);
  m_rateAccumulator  += w;
  m_rateAccumulator2 += w * w;
  if (m_data != nullptr) m_data->Fill(RatesBinIdentifier_t::kRATE_BIN_OR, w);
  if (m_expressPrescale >= 1) {
    const double wExp = m_totalPrescaleWeightExpress * weights.m_enhancedBiasWeight * getExtrapolationFactor(weights);
    m_rateExpressAccumulator  += wExp;
    m_rateExpressAccumulator2 += wExp * wExp;
    if (m_data != nullptr) m_data->Fill(RatesBinIdentifier_t::kEXPRESS_BIN, wExp);
  }
}

double RatesTrigger::getPrescale(const bool includeExpress) const { 
  if (includeExpress) return m_prescale * m_expressPrescale;
  return m_prescale;
}

double RatesTrigger::getExtrapolationFactor(const WeightingValuesSummary_t& weights) const {
  switch (m_extrapolationStrategy) {
    case kLINEAR: return weights.m_linearLumiFactor;
    case kEXPO_MU: return weights.m_expoMuFactor;
    case kBUNCH_SCALING: return weights.m_bunchFactor;
    case kMU_SCALING: return weights.m_muFactor;
    case kNONE: return weights.m_noScaling;
    default: m_log << MSG::ERROR << "Error in getExtrapolationFactor. Unknown ExtrapStrat_t ENUM supplied " << m_extrapolationStrategy << endmsg;
  }
  return 0.;
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
  ss << "Rate:" << std::setw(11) << std::right << m_rateAccumulator/ratesDenominator 
     << " +- "    << std::setw(11) << std::left << sqrt(m_rateAccumulator2)/ratesDenominator << " Hz";
  if (m_uniqueGroup != nullptr) {
    const double unique = (getDisabled() == true ? 0. : m_uniqueGroup->getUniqueWeight(ratesDenominator));
    //const double unique = m_uniqueGroup->m_rateAccumulatorOR / ratesDenominator; // For dbg - this is the rate of N-1 
    // Getting the fractional error of refgular rate and applying it to the unique rate
    const double uniqueErr = (isZero(m_rateAccumulator) ? 0. : (sqrt(m_rateAccumulator2)/m_rateAccumulator) * unique);
    ss << ", Unique Rate:" << std::setw(11) << std::right << unique
       << " +- "           << std::setw(11) << std::left << uniqueErr << " Hz";
  }
  ss << " : ";
  if (m_seed != "") ss << m_seed << " [PS:" << m_seedPrescale << "] -> ";
  ss << m_name << " [PS:" << m_prescale << "]";
  return ss.str();
}

const std::string RatesTrigger::printExpressRate(const double ratesDenominator) const {
  std::stringstream ss;
  ss << std::setfill(' '); 
  ss << "Express Rate:" << std::setw(11) << std::right << m_rateExpressAccumulator/ratesDenominator 
     << " +- "    << std::setw(11) << std::left << sqrt(m_rateExpressAccumulator2)/ratesDenominator << " Hz";
  ss << " : ";
  if (m_seed != "") ss << m_seed << " [PS:" << m_seedPrescale << "] -> ";
  ss << m_name << " [PS:" << m_prescale << "] [EXPRESS PS:" << m_expressPrescale << "]";
  return ss.str();
}

