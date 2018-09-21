/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesHistoBase.h"

uint32_t RatesHistoBase::m_histoID = 0;

RatesHistoBase::RatesHistoBase(const std::string& name, const MsgStream& log, const bool doHistograms) : 
  m_name(name),
  m_doHistograms(doHistograms), m_rateVsMu(nullptr), m_rateVsTrain(nullptr), m_data(nullptr),
  m_givenRateVsMu(false), m_givenRateVsTrain(false), m_givenData(false),
  m_log(log)
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

    m_log << MSG::DEBUG << " For " << name << "(" << this << ") I have made histograms " 
      << (uint64_t) m_rateVsMu << " " << m_rateVsMu->GetName() << " and " 
      << (uint64_t) m_rateVsTrain << " " << m_rateVsTrain->GetName() << " and " 
      << (uint64_t) m_data << " "  << m_data->GetName() << endmsg; 
  }
}

RatesHistoBase::~RatesHistoBase() {
  m_log << MSG::DEBUG << "Deleting " << m_name << " (" << this << ")" << endmsg;
  if (m_doHistograms) {
    if (!m_givenData) {
      delete m_data;
      m_data = nullptr;
    }
    if (!m_givenRateVsMu) {
      delete m_rateVsMu;
      m_rateVsMu = nullptr;
    }
    if (!m_givenRateVsTrain) {
      delete m_rateVsTrain;
      m_rateVsTrain = nullptr;
    }
  }
}

TH1D* RatesHistoBase::getMuHist(bool clientIsTHistSvc) { 
  if (!m_doHistograms) { 
    m_log << MSG::ERROR << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << endmsg;
    return nullptr;
  }
  if (clientIsTHistSvc) m_givenRateVsMu = true;
  return m_rateVsMu; 
}

TH1D* RatesHistoBase::getTrainHist(bool clientIsTHistSvc) { 
  if (!m_doHistograms) {
    m_log << MSG::ERROR << "RatesHistoBase::getTrainHist Warning requested histograms when histograming is OFF here." << endmsg;
    return nullptr;
  }
  if (clientIsTHistSvc) m_givenRateVsTrain = true;
  return m_rateVsTrain;
}

TH1D* RatesHistoBase::getDataHist(bool clientIsTHistSvc) { 
  if (clientIsTHistSvc) m_givenData = true;
  return m_data; // Do not flag an errors about nullptr here as this is fetched by the unique rates groups on their trigger
}


void RatesHistoBase::normaliseHist(const double ratesDenominator) {
  if (m_doHistograms) {
    m_rateVsMu->Scale(1. / ratesDenominator);
    m_rateVsTrain->Scale(1. / ratesDenominator);
    //m_data does not get scaled as it contains raw data
  }
}
