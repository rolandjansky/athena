/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "Variable.h"
#include "TH1F.h"
#include "TH2F.h"

Variable::Variable(const std::string& name, TH1* cacheHistoPtr, VariableType type) :
  m_name(name),
  m_variableType(type),
  m_cacheHistoPtr(cacheHistoPtr),
  m_calls(0),
  m_xaccumulator(0.),
  m_yaccumulator(0.),
  m_weight(0.),
  m_oneOverDenominator(1.) {
}


const std::string& Variable::getName() const {
  return m_name;
}


size_t Variable::getCalls() const {
  return m_calls;
}


float Variable::getAccumulator() const {
  return m_xaccumulator;
}


void Variable::setDenominator(float value) {
  // Stored as reciprocal so as to be able to check for /0 here
  if (fabs(value) < 1e-10) {
    m_oneOverDenominator = 0.0;
  } else {
    m_oneOverDenominator = 1. / value;
  }
}


StatusCode Variable::fill(float value, float weight) {
  switch(m_variableType) {
    case kPerCall:
      ATH_CHECK(m_cacheHistoPtr != nullptr);
      m_cacheHistoPtr->Fill(value * m_oneOverDenominator, weight);
      break;
    case kPerEvent:
      ++m_calls;
      m_xaccumulator += value;
      m_weight = weight;
      break;
    default:
      return StatusCode::FAILURE;
      break;
  }
  return StatusCode::SUCCESS;
}


StatusCode Variable::fill(float xvalue, float yvalue, float weight) {
  switch(m_variableType) {
    case kPerCall:
      ATH_CHECK(m_cacheHistoPtr != nullptr);
      dynamic_cast<TH2F*>(m_cacheHistoPtr)->Fill(xvalue * m_oneOverDenominator, yvalue * m_oneOverDenominator, weight);
      break;
    case kPerEvent:
      ++m_calls;
      m_xaccumulator += xvalue;
      m_yaccumulator += yvalue;
      m_weight = weight;
      break;
    default:
      return StatusCode::FAILURE;
      break;
  }
  return StatusCode::SUCCESS;
}


StatusCode Variable::fill(const std::string& label, float weight) {
  m_cacheHistoPtr->Fill(label.c_str(), weight);
  return StatusCode::SUCCESS;
}


StatusCode Variable::increment(float weight) {
  ATH_CHECK(fill(1.0, weight));
  return StatusCode::SUCCESS;
}


StatusCode Variable::setBinLabel(int bin, const std::string& label) {
  m_cacheHistoPtr->GetXaxis()->SetBinLabel(bin, label.c_str());
  return StatusCode::SUCCESS;
}


StatusCode Variable::endEvent() {
  if (m_variableType == kPerEvent && m_calls > 0) {
    ATH_CHECK(m_cacheHistoPtr != nullptr);
    // 2D histogram
    if (m_yaccumulator > 0){
      dynamic_cast<TH2F*>(m_cacheHistoPtr)->Fill(m_xaccumulator * m_oneOverDenominator, m_yaccumulator, m_weight);
    } else {
      m_cacheHistoPtr->Fill(m_xaccumulator * m_oneOverDenominator, m_weight);
    }
    
  }
  m_calls = 0;
  m_xaccumulator = 0.0;
  m_yaccumulator = 0.0;
  m_weight = 0.0;
  m_oneOverDenominator = 1.0;
  return StatusCode::SUCCESS;
}
