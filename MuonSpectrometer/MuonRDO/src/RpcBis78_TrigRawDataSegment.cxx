/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  

#include "MuonRDO/RpcBis78_TrigRawDataSegment.h"

Muon::RpcBis78_TrigRawDataSegment::RpcBis78_TrigRawDataSegment() :
  m_etaIndex(0),
  m_phiIndex(0),
  m_deltaEta(0),
  m_deltaPhi(0),
  m_flag3over3Eta(0),
  m_flag3over3Phi(0),
  m_trigword(0)
{ }

Muon::RpcBis78_TrigRawDataSegment::RpcBis78_TrigRawDataSegment(uint8_t etaIndex, uint8_t phiIndex, 
                                                               uint8_t deltaEta, uint8_t deltaPhi, 
                                                               uint8_t flag3over3Eta, uint8_t flag3over3Phi, 
                                                               uint32_t trigword ) :
  m_etaIndex(etaIndex),
  m_phiIndex(phiIndex),
  m_deltaEta(deltaEta),
  m_deltaPhi(deltaPhi),
  m_flag3over3Eta(flag3over3Eta),
  m_flag3over3Phi(flag3over3Phi),
  m_trigword(trigword)
{ }

