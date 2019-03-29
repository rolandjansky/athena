/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EtaEtCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

// return whether jet is in an abs(Eta) and Et window.

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <cmath>
#include <sstream>


EtaEtCleanerTool::EtaEtCleanerTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  base_class(type, name, parent){ 
}

bool EtaEtCleanerTool::select(const pHypoJet& jet) const{
  auto eta =  std::abs(jet->eta());
  auto et = jet->et();
  
  return eta >= m_etaMin and eta < m_etaMax and et >= m_etMin and et < m_etMax;
}

std::string EtaEtCleanerTool::getName() const noexcept {
  return "EtaEtCleanerTool";}

std::string EtaEtCleanerTool::toString() const noexcept {
  std::stringstream ss;
  ss << "EtaEtCleanerTool: etaMin: "
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


