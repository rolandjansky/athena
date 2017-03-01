#include "RatesAnalysis/RatesTrigger.h"
#include "RatesAnalysis/RatesGroup.h"

/**
 * Construct new RatesTrigger to enumerate the rate for a single L1 or HLT trigger, and provide pass/fail
 * information to RatesGroups which this trigger is part of.
 * @param name Name of the trigger
 * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
 * @param expressPrescale If the trigger is HLT and in the express group, its express prescale. Otherwise set = 0
 * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
 * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
 * @param doHistograms Flag to mint histograms or not
 * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
 */
RatesTrigger::RatesTrigger(const std::string& name, const double prescale, const double expressPrescale,
                           const std::string& seedName, const double seedPrescale, const bool doHistograms,
                           const ExtrapStrat_t extrapolation) :
  RatesHistoBase(name, (prescale < 1. || seedPrescale < 1. ? false : doHistograms)),
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

/**
 * Set the pass/fail bool. Execute needs to be called separately afterwards.
 */
void RatesTrigger::setPassed(const bool i, const bool unbiasedEvent) { 
  if (m_seedsFromRandom == true && unbiasedEvent == false) return;
  m_pass = i;
}

/**
 * Set the pass/fail bool and immediately call execute. Should only be done once per event.
 */
void RatesTrigger::setPassedAndExecute(const bool i, const WeightingValuesSummary_t& weights) { 
  if (m_seedsFromRandom == true && weights.m_isUnbiased == false) return;
  if (m_pass == false) { // Protect against two positive calls/event
    m_pass = i;
    execute(weights);
  }
}

/**
 * Execute trigger rate emulation. If the trigger passed, add to its rate the effective number of events it is accepting.
 * Also keep track of any express prescale rate
 * @param weights Summary of all event weights which may be used by this trigger
 */
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
  if (m_expressPrescale >= 1) {
    const double wExp = m_totalPrescaleWeightExpress * weights.m_enhancedBiasWeight * getExtrapolationFactor(weights);
    m_rateExpressAccumulator  += wExp;
    m_rateExpressAccumulator2 += wExp * wExp;
  }
}

/**
 * Gets the triggers prescale
 * @param includeExpress If true, the items express prescale is added on top of its regular prescale
 */
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
    default: std::cerr << "ERROR in getExtrapolationFactor. Unknown ExtrapStrat_t ENUM supplied " << m_extrapolationStrategy << std::endl;
  }
  return 0.;
}


/**
 * Prints the RatesTrigger's configuration
 */
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

/**
 * Prints the RatesTrigger's rate
 * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
 */
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

/**
 * Prints the RatesTrigger's express rate
 * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
 */
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

