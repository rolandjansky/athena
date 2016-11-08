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
#include <sstream>

EtaEtCleaner::EtaEtCleaner(double etaMin,
                           double etaMax,
                           double etMin,
                           double etMax):
  m_etaMin(etaMin), m_etaMax(etaMax), m_etMin(etMin), m_etMax(etMax){
  }

bool EtaEtCleaner::operator()(const pHypoJet& jet) const{
  auto eta =  std::abs(jet->eta());
  auto et = jet->et();
  
  return eta >= m_etaMin and eta < m_etaMax and et >= m_etMin and et < m_etMax;
}

std::string EtaEtCleaner::getName() const noexcept {return "EtaEtCleaner";}

std::string EtaEtCleaner::toString() const noexcept {
  std::stringstream ss;
  ss << "EtaEtCleaner: etaMin: "
     << m_etaMin
     << " etaMax "
     << m_etaMax
     << " etMin "
     << m_etMin
     << " etMax "
     << m_etMax
     <<'\n';

  return ss.str();
}


