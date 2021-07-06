/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesAnalysis/RatesHistoBase.h"

#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

RatesHistoBase::RatesHistoBase(const std::string& name, const MsgStream& log, const bool doHistograms) : 
  m_name(name),
  m_doHistograms(doHistograms), m_rateVsMu(nullptr), m_rateVsTrain(nullptr), m_data(nullptr),
  m_rateVsMuCachedPtr(nullptr), m_rateVsTrainCachedPtr(nullptr), m_dataCachedPtr(nullptr),
  m_log(log)
{
  if (doHistograms) {
    m_rateVsMu = std::make_unique<TH1D>("",TString(name + ";#mu;Rate / Unit #mu [Hz]"),226,-.5,225.5);
    m_rateVsMu->Sumw2(true);
    m_rateVsMu->SetName("rateVsMu");
    m_rateVsMuCachedPtr = m_rateVsMu.get();

    m_rateVsTrain = std::make_unique<TH1D>("",TString(name + ";Distance Into Train;Rate / BCID [Hz]"),100,-0.5,99.5);
    m_rateVsTrain->Sumw2(true);
    m_rateVsTrain->SetName("rateVsTrain");
    m_rateVsTrainCachedPtr = m_rateVsTrain.get();

    m_data =  std::make_unique<TH1D>("",TString(";Variable;Raw Data"), RatesBinIdentifier_t::kNRATES_BINS, -.5, RatesBinIdentifier_t::kNRATES_BINS - .5);
    m_data->Sumw2(true);
    m_data->SetName("data");
    // Set bin labels based on RatesBinIdentifier_t
    m_data->GetXaxis()->SetBinLabel(1, "Actve Raw");
    m_data->GetXaxis()->SetBinLabel(2, "Active Weighted");
    m_data->GetXaxis()->SetBinLabel(3, "Pass Raw");
    m_data->GetXaxis()->SetBinLabel(4, "Pass Weighted OR");
    m_data->GetXaxis()->SetBinLabel(5, "Pass Weighted AND");
    m_data->GetXaxis()->SetBinLabel(6, "Express");
    m_data->GetXaxis()->SetBinLabel(7, "Unique");
    m_dataCachedPtr = m_data.get();

  }
}


RatesHistoBase::~RatesHistoBase() {
}

StatusCode RatesHistoBase::giveMuHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name) { 
  if (!m_rateVsMu.get()) { 
    m_log << MSG::ERROR << "RatesHistoBase::giveMuHist Warning requested histograms when histograming is OFF here." << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_CHECK( svc->regHist(name, std::move(m_rateVsMu), m_rateVsMuCachedPtr) );
  m_log << MSG::DEBUG << "For " << m_name << "(" << this << ") m_rateVsMuCachedPtr is updated to " << (uint64_t) m_rateVsMuCachedPtr << endmsg; 
  return StatusCode::SUCCESS;
}


StatusCode RatesHistoBase::giveTrainHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name) { 
  if (!m_rateVsTrain.get()) {
    m_log << MSG::ERROR << "RatesHistoBase::giveTrainHist Warning requested histograms when histograming is OFF here." << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_CHECK( svc->regHist(name, std::move(m_rateVsTrain), m_rateVsTrainCachedPtr) );
  return StatusCode::SUCCESS;
}


StatusCode RatesHistoBase::giveDataHist(const ServiceHandle<ITHistSvc>& svc, const std::string& name) { 
  if (!m_data.get()) {
    m_log << MSG::ERROR << "RatesHistoBase::giveDataHist Warning requested histograms when histograming is OFF here." << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_CHECK( svc->regHist(name, std::move(m_data), m_dataCachedPtr) );
  return StatusCode::SUCCESS;
}


void RatesHistoBase::clearTrainHist() {
  m_rateVsTrain.reset();
  m_rateVsTrainCachedPtr = nullptr;
}


TH1* RatesHistoBase::getDataHist() {
  return m_dataCachedPtr; // Caller must be able to deal with nullptr (e.g unique rates groups calling on their trigger)
}


const std::string& RatesHistoBase::getExtrapolationFactorString(ExtrapStrat_t strat) const {
  static const std::vector<std::string> values{"LINEAR_L","LINEAR_BUNCH_EXPO_MU","LINEAR_BUNCHES","LINEAR_MU","NONE"};
  return values.at(static_cast<size_t>(strat)); 
}


double RatesHistoBase::getExtrapolationFactor(const WeightingValuesSummary_t& weights, const ExtrapStrat_t strat) const {
  switch (strat) {
    case kLINEAR: return weights.m_linearLumiFactor;
    case kEXPO_MU: return weights.m_expoMuFactor;
    case kBUNCH_SCALING: return weights.m_bunchFactor;
    case kMU_SCALING: return weights.m_muFactor;
    case kNONE: return weights.m_noScaling;
    default: m_log << MSG::ERROR << "Error in getExtrapolationFactor. Unknown ExtrapStrat_t ENUM supplied " << strat << endmsg;
  }
  return 0.;
}
