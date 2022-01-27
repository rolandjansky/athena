/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesScanTrigger.h"

#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    IMessageSvc* msgSvc,
                                    const double thresholdMin, const double thresholdMax, const uint32_t thresholdBins,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, msgSvc, prescale, -1, seedName, seedPrescale, /*base histograms*/false, extrapolation),
  m_rateScanHist(nullptr), m_rateScanHistCachedPtr(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    m_rateScanHist = std::make_unique<TH1D>("", TString(name + ";Threshold;Rate [Hz]"), thresholdBins, thresholdMin, thresholdMax);
    m_rateScanHist->Sumw2(true);

    m_rateScanHistCachedPtr = m_rateScanHist.get();
  }

RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    IMessageSvc* msgSvc,
                                    const std::vector<double>& thresholdBinEdged,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, msgSvc, prescale, -1, seedName, seedPrescale, false, extrapolation),
  m_rateScanHist(nullptr), m_rateScanHistCachedPtr(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    if (thresholdBinEdged.size() < 2) {
      ATH_MSG_ERROR("Need more than one entry in thresholdBinEdged to define histogram binning.");
      return;
    }
    size_t nBins = thresholdBinEdged.size() - 1;
    m_rateScanHist = std::make_unique<TH1D>("", TString(name + ";Threshold;Rate [Hz]"), nBins, thresholdBinEdged.data());
    m_rateScanHist->Sumw2(true);

    m_rateScanHistCachedPtr = m_rateScanHist.get();
  }

RatesScanTrigger::~RatesScanTrigger() {
}

void RatesScanTrigger::passThreshold(const double t, const bool unbiasedEvent) {
  if (m_seedsFromRandom == true && unbiasedEvent == false) return;
  m_thresholdPassed = t;
}

void RatesScanTrigger::setPassedAndExecute(const double t, const WeightingValuesSummary_t& weights) {
  if (m_seedsFromRandom == true && weights.m_isUnbiased == false) return;
  if (m_thresholdPassed != std::numeric_limits<double>::min()) return; // We've already done this event
  m_thresholdPassed = t;
  execute(weights);
}


StatusCode RatesScanTrigger::giveThresholdHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name) { 
  ATH_CHECK( svc->regHist(name, std::move(m_rateScanHist), m_rateScanHistCachedPtr) );
  return StatusCode::SUCCESS;
}

void RatesScanTrigger::execute(const WeightingValuesSummary_t& weights) {
  if (m_thresholdPassed == std::numeric_limits<double>::min()) return; // Did not pass
  // This histogram we *do* include the extrapolation weight as we plot vs. some trigger property, not some event property
  double w = m_totalPrescaleWeight * weights.m_enhancedBiasWeight * getExtrapolationFactor(weights, m_extrapolationStrategy);
  // Fill the histogram cumulatively
  // We match exactly with the *lower* edge of all bins
  const int nBins = m_rateScanHistCachedPtr->GetNbinsX();
  for (int bin = 1; bin <= nBins; ++bin) {
    const double low = m_rateScanHistCachedPtr->GetBinLowEdge(bin);
    const double width = m_rateScanHistCachedPtr->GetBinWidth(bin);
    if ( (m_behaviour == kTriggerAboveThreshold && m_thresholdPassed < (low + width)) ||
         (m_behaviour == kTriggerBelowThreshold && m_thresholdPassed > low)) {
      m_rateScanHistCachedPtr->Fill(m_rateScanHistCachedPtr->GetBinCenter(bin), w);
    }
  }
  // Underflow && Overflow
  const double xMin = m_rateScanHistCachedPtr->GetXaxis()->GetXmin();
  const double xMax = m_rateScanHistCachedPtr->GetXaxis()->GetXmax();
  if ( (m_behaviour == kTriggerAboveThreshold && m_thresholdPassed < xMin) || m_behaviour == kTriggerBelowThreshold ) {
    m_rateScanHistCachedPtr->Fill(xMin - 1., w);
  }
  if ( m_behaviour == kTriggerAboveThreshold || (m_behaviour == kTriggerBelowThreshold && m_thresholdPassed >= xMax) ) {
    m_rateScanHistCachedPtr->Fill(xMax + 1, w);
  }
}

const std::string RatesScanTrigger::printRate(const double ratesDenominator) const {
  std::stringstream ss;
  const int nBins = m_rateScanHistCachedPtr->GetNbinsX();
  ss << std::setfill(' '); 
  ss << m_name << " [PS:" << m_prescale << "]";
  if (m_seed != "") ss << " <- " << m_seed << " [PS:" << m_seedPrescale << "]";
  ss << " (Extrap:"<< getExtrapolationFactorString(m_extrapolationStrategy) << ")" << std::endl;

  if (m_behaviour == kTriggerBelowThreshold) {

    ss << "    Threshold <= " << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetXaxis()->GetXmin();
    ss << " Rate :" << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(0)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(0)/ratesDenominator << " Hz" << std::endl;

    for (int bin = 1; bin <= nBins; ++bin) {
      ss << "    Threshold <= ";
      ss << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinLowEdge(bin) + m_rateScanHistCachedPtr->GetBinWidth(bin);
      ss << " Rate :" << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(bin)/ratesDenominator;
      ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(bin)/ratesDenominator << " Hz";
      ss << std::endl;
    }

    ss << "    Threshold >  " << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetXaxis()->GetXmax();
    ss << " Rate :" << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(nBins+1)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(nBins+1)/ratesDenominator << " Hz";
  
  } else if (m_behaviour == kTriggerAboveThreshold) {

    ss << "    Threshold <  " << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetXaxis()->GetXmin();
    ss << " Rate: " << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(0)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(0)/ratesDenominator << " Hz" << std::endl;
  
    for (int bin = 1; bin <= nBins; ++bin) {
      ss << "    Threshold >= ";
      ss << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinLowEdge(bin);
      ss << " Rate: " << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(bin)/ratesDenominator;
      ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(bin)/ratesDenominator << " Hz";
      ss << std::endl;
    }

    ss << "    Threshold >= " << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetXaxis()->GetXmax();
    ss << " Rate: " << std::setw(11) << std::right << m_rateScanHistCachedPtr->GetBinContent(nBins+1)/ratesDenominator;
    ss << " +- "   << std::setw(11) << std::left << m_rateScanHistCachedPtr->GetBinError(nBins+1)/ratesDenominator << " Hz";
  
  }

  return ss.str();
}
