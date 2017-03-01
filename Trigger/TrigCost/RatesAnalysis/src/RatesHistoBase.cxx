#include "RatesAnalysis/RatesHistoBase.h"

uint32_t RatesHistoBase::m_histoID = 0;

/**
 * Base class such that everyone can have histograms.
 */
RatesHistoBase::RatesHistoBase(const std::string& name, const bool doHistograms) : 
  m_doHistograms(doHistograms), m_rateVsMu(nullptr), m_rateVsTrain(nullptr) 
{
  if (m_doHistograms) {
    m_rateVsMu = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";#mu;Rate / Unit #mu [Hz]"),100,0.,100.) ;
    m_rateVsMu->SetName("rateVsMu");

    m_rateVsTrain = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Distance Into Train;Rate / BCID [Hz]"),100,-0.5,99.5) ;
    m_rateVsTrain->SetName("rateVsTrainPosition");

    // std::cout << " For " << name << " I have made my two histograms " << (uint64_t) m_rateVsMu << " " << m_rateVsMu->GetName() << " and " << (uint64_t)m_rateVsTrain << " "  << m_rateVsTrain->GetName() << std::endl; 
  }
}

/**
 * @return histogram pointer or nullptr and an error
 */
TH1D* RatesHistoBase::getMuHist() const { 
  if (!m_doHistograms) {
    std::cerr << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << std::endl;
    return nullptr;
  }
  // std::cout << " returning a pointer to mu " << m_rateVsMu << std::endl;
  // std::cout << m_rateVsMu->GetName() << std::endl;
  return m_rateVsMu; 
}

/**
 * @return histogram pointer or nullptr and an error
 */
TH1D* RatesHistoBase::getTrainHist() const { 
  if (!m_doHistograms) {
    std::cerr << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << std::endl;
    return nullptr;
  }
  // std::cout << " returning a pointer to train " << m_rateVsTrain << std::endl;
  // std::cout << m_rateVsTrain->GetName() << std::endl;
  return m_rateVsTrain;
}

/**
 * Letting ROOT handle the memory management of the histograms
 */
RatesHistoBase::~RatesHistoBase() {}

/**
 * Normalise to walltime to get rate.
 */
void RatesHistoBase::normaliseHist(const double ratesDenominator) {
  if (m_doHistograms) {
    m_rateVsMu->Scale(1. / ratesDenominator);
    m_rateVsTrain->Scale(1. / ratesDenominator);
  }
}
