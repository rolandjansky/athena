/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EtaEtCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

// return whether jet is in an abs(Eta) and Et window.

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <cmath>

EtaEtCleaner::EtaEtCleaner(float etaMin,
                           float etaMax,
                           float etMin,
                           float etMax):
  m_etaMin(etaMin), m_etaMax(etaMax), m_etMin(etMin), m_etMax(etMax){
  }

bool EtaEtCleaner::operator()(const pHypoJet& jet) const{
  auto eta =  std::abs(jet->eta());
  auto et = jet->et();
  
  return eta >= m_etaMin and eta < m_etaMin and et >= m_etMin and et < m_etaMin;
}

std::string EtaEtCleaner::getName() const noexcept {return "EtaEtCleaner";}



