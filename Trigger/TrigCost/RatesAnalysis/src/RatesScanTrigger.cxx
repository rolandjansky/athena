#include "RatesAnalysis/RatesScanTrigger.h"

/**
 * Construct new RatesScanTrigger to enumerate the rate for a single L1 or HLT trigger as a function of some threshold
 * @param name Name of the trigger
 * @param thresholdMin The lower threshold of this trigger, rates will not be available below this threshold
 * @param thresholdMax The upper threshold of this trigger, rates will not be available above this threshold
 * @param thresholdBins Granularity 
 * @param behaviour If the trigger should activate above (kTriggerAboveThreshold) or below (kTriggerBelowThreshold) the threshold
 * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
 * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
 * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
 * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
 */
RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    const double thresholdMin, const double thresholdMax, const uint32_t thresholdBins,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, prescale, -1, seedName, seedPrescale, false, extrapolation),
  m_rateScanHist(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    m_rateScanHist = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Threshold;Rate [Hz]"), thresholdBins, thresholdMin, thresholdMax);
    m_rateScanHist->SetName("rateVsThreshold");
  }


/**
 * Construct new RatesScanTrigger to enumerate the rate for a single L1 or HLT trigger as a function of some threshold
 * @param name Name of the trigger
 * @param thresholdBinEdged Vector of bin edges to use for quantifying rate as a function of threshold
 * @param behaviour If the trigger should activate above (kTriggerAboveThreshold) or below (kTriggerBelowThreshold) the threshold
 * @param prescale The prescale of the trigger. Anything < 1 is considered disabled
 * @param seedName The name of any L1 seed the trigger has, leave blank if L1 item / no L1 seed.
 * @param seedPrescale The prescale of any L1 seed. Leave = 1 if no L1 seed.
 * @param extrapolation The luminosity extrapolation strategy to be applied to this trigger
 */
RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    const std::vector<double>& thresholdBinEdged,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, prescale, -1, seedName, seedPrescale, false, extrapolation),
  m_rateScanHist(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    if (thresholdBinEdged.size() < 2) {
      std::cerr << "RatesScanTrigger::RatesScanTrigger: ERROR Need more than one entry in thresholdBinEdged to define histogram binning." << std::endl;
      return;
    }
    size_t nBins = thresholdBinEdged.size() - 1;
    m_rateScanHist = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Threshold;Rate [Hz]"), nBins, thresholdBinEdged.data());
    m_rateScanHist->SetName("rateVsThreshold");
  }

/**
 * Letting ROOT handle the memory management of the histograms
 */
RatesScanTrigger::~RatesScanTrigger() {}

/**
 * Sets the threshold the event 
 */
void RatesScanTrigger::passThreshold(const double t, const bool unbiasedEvent) {
  if (m_seedsFromRandom == true && unbiasedEvent == false) return;
  m_thresholdPassed = t;
}

/**
 * Set the pass threshold and immediately call execute. Should only be done once per event.
 */
void RatesScanTrigger::setPassedAndExecute(const double t, const WeightingValuesSummary_t& weights) {
  if (m_seedsFromRandom == true && weights.m_isUnbiased == false) return;
  if (m_thresholdPassed != std::numeric_limits<double>::min()) return; // We've already done this event
  m_thresholdPassed = t;
  execute(weights);
}

/**
 * Execute trigger rate emulation. If the trigger passed threshold is within the defined range then fill the 
 * relevant histogram bins
 * @param weights Summary of all event weights which may be used by this trigger
 */
void RatesScanTrigger::execute(const WeightingValuesSummary_t& weights) {
  if (m_thresholdPassed == std::numeric_limits<double>::min()) return; // Did not pass
  // This histogram we *do* include the extrapolation weight as we plot vs. some trigger property, not some event property
  double w = m_totalPrescaleWeight * weights.m_enhancedBiasWeight * getExtrapolationFactor(weights);
  // Fill the histogram cumulatively
  // We match exactly with the *lower* edge of all bins
  const int nBins = m_rateScanHist->GetNbinsX();
  for (int bin = 1; bin <= nBins; ++bin) {
    const double low = m_rateScanHist->GetBinLowEdge(bin);
    const double width = m_rateScanHist->GetBinWidth(bin);
    if ( (m_behaviour == kTriggerAboveThreshold && m_thresholdPassed < (low + width)) ||
         (m_behaviour == kTriggerBelowThreshold && m_thresholdPassed > low)) {
      m_rateScanHist->Fill(m_rateScanHist->GetBinCenter(bin), w);
    }
  }
  // Underflow && Overflow
  const double xMin = m_rateScanHist->GetXaxis()->GetXmin();
  const double xMax = m_rateScanHist->GetXaxis()->GetXmax();
  if ( (m_behaviour == kTriggerAboveThreshold && m_thresholdPassed < xMin) || m_behaviour == kTriggerBelowThreshold ) {
    m_rateScanHist->Fill(xMin - 1., w);
  }
  if ( m_behaviour == kTriggerAboveThreshold || (m_behaviour == kTriggerBelowThreshold && m_thresholdPassed >= xMax) ) {
    m_rateScanHist->Fill(xMax + 1, w);
  }
}

void RatesScanTrigger::normaliseHist(const double ratesDenominator) {
  RatesHistoBase::normaliseHist(ratesDenominator);
  m_rateScanHist->Scale(1. / ratesDenominator);
}




/**
 * Prints the RatesScanTrigger's rate
 * @param ratesDenominator The walltime for the run, needed to normalise from integrated weighted counts to a rate.
 */
const std::string RatesScanTrigger::printRate(const double ratesDenominator) const {
  std::stringstream ss;
  const int nBins = m_rateScanHist->GetNbinsX();
  ss << std::setfill(' '); 
  if (m_seed != "") ss << m_seed << " [PS:" << m_seedPrescale << "] -> ";
  ss << m_name << " [PS:" << m_prescale << "]" << std::endl;

  if (m_behaviour == kTriggerBelowThreshold) {

    ss << "    Threshold <= " << std::setw(11) << std::left << m_rateScanHist->GetXaxis()->GetXmin();
    ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(0)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(0)/ratesDenominator << " Hz" << std::endl;

    for (int bin = 1; bin <= nBins; ++bin) {
      ss << "    Threshold <= ";
      ss << std::setw(11) << std::left << m_rateScanHist->GetBinLowEdge(bin) + m_rateScanHist->GetBinWidth(bin);
      ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(bin)/ratesDenominator;
      ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(bin)/ratesDenominator << " Hz";
      ss << std::endl;
    }

    ss << "    Threshold >  " << std::setw(11) << std::left << m_rateScanHist->GetXaxis()->GetXmax();
    ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(nBins+1)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(nBins+1)/ratesDenominator << " Hz";
  
  } else if (m_behaviour == kTriggerAboveThreshold) {

    ss << "    Threshold <  " << std::setw(11) << std::left << m_rateScanHist->GetXaxis()->GetXmin();
    ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(0)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(0)/ratesDenominator << " Hz" << std::endl;
  
    for (int bin = 1; bin <= nBins; ++bin) {
      ss << "    Threshold >= ";
      ss << std::setw(11) << std::left << m_rateScanHist->GetBinLowEdge(bin);
      ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(bin)/ratesDenominator;
      ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(bin)/ratesDenominator << " Hz";
      ss << std::endl;
    }

    ss << "    Threshold >= " << std::setw(11) << std::left << m_rateScanHist->GetXaxis()->GetXmax();
    ss << " Rate:" << std::setw(11) << std::right << m_rateScanHist->GetBinContent(nBins+1)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHist->GetBinError(nBins+1)/ratesDenominator << " Hz";
  
  }

  return ss.str();
}
