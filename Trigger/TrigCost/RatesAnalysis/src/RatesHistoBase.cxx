/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesHistoBase.h"

uint32_t RatesHistoBase::m_histoID = 0;

RatesHistoBase::RatesHistoBase(const std::string& name, const MsgStream& log, const bool doHistograms) : 
  m_doHistograms(doHistograms), m_rateVsMu(nullptr), m_rateVsTrain(nullptr), m_data(nullptr), m_log(log) 
{
  if (m_doHistograms) {
    m_rateVsMu = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";#mu;Rate / Unit #mu [Hz]"),226,-.5,225.5) ;
    m_rateVsMu->SetName("rateVsMu");
    m_rateVsMu->Sumw2(true);

    m_rateVsTrain = new TH1D(TString(std::to_string(m_histoID++)),TString(name + ";Distance Into Train;Rate / BCID [Hz]"),100,-0.5,99.5) ;
    m_rateVsTrain->SetName("rateVsTrainPosition");
    m_rateVsTrain->Sumw2(true);

    m_data = new TH1D(TString(std::to_string(m_histoID++)),TString(";Variable;Raw Data"), RatesBinIdentifier_t::kNRATES_BINS, -.5, RatesBinIdentifier_t::kNRATES_BINS - .5) ;
    m_data->SetName("data");
    m_data->Sumw2(true);

    // std::cout << " For " << name << " I have made my two histograms " << (uint64_t) m_rateVsMu << " " << m_rateVsMu->GetName() << " and " << (uint64_t)m_rateVsTrain << " "  << m_rateVsTrain->GetName() << std::endl; 
  }
}

RatesHistoBase::~RatesHistoBase() {}

TH1D* RatesHistoBase::getMuHist() const { 
  if (!m_doHistograms) { 
    m_log << MSG::ERROR << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << endmsg;
    return nullptr;
  }
  return m_rateVsMu; 
}

TH1D* RatesHistoBase::getTrainHist() const { 
  if (!m_doHistograms) {
    m_log << MSG::ERROR << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << endmsg;
    return nullptr;
  }
  return m_rateVsTrain;
}

TH1D* RatesHistoBase::getDataHist() const { 
  if (!m_doHistograms) return nullptr; // Do not flag an error here as this is fetched by the unique rates groups on their trigger
  return m_data;
}


void RatesHistoBase::normaliseHist(const double ratesDenominator) {
  if (m_doHistograms) {
    m_rateVsMu->Scale(1. / ratesDenominator);
    m_rateVsTrain->Scale(1. / ratesDenominator);
    //m_data does not get scaled as it contains raw data
  }
}
