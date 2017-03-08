/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesScanTrigger.h"

RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    const MsgStream& log,
                                    const double thresholdMin, const double thresholdMax, const uint32_t thresholdBins,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, log, prescale, -1, seedName, seedPrescale, false, extrapolation),
  m_rateScanHist(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    m_rateScanHist = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Threshold;Rate [Hz]"), thresholdBins, thresholdMin, thresholdMax);
    m_rateScanHist->SetName("rateVsThreshold");
  }

RatesScanTrigger::RatesScanTrigger( const std::string& name, 
                                    const MsgStream& log,
                                    const std::vector<double>& thresholdBinEdged,  
                                    const TriggerBehaviour_t behaviour,
                                    const double prescale,
                                    const std::string& seedName, const double seedPrescale,
                                    const ExtrapStrat_t extrapolation) :
  RatesTrigger(name, log, prescale, -1, seedName, seedPrescale, false, extrapolation),
  m_rateScanHist(nullptr), m_thresholdPassed(0), m_behaviour(behaviour)
  {
    if (thresholdBinEdged.size() < 2) {
      m_log << MSG::ERROR << "Need more than one entry in thresholdBinEdged to define histogram binning." << endmsg;
      return;
    }
    size_t nBins = thresholdBinEdged.size() - 1;
    m_rateScanHist = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Threshold;Rate [Hz]"), nBins, thresholdBinEdged.data());
    m_rateScanHist->SetName("rateVsThreshold");
  }

RatesScanTrigger::~RatesScanTrigger() {}

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
