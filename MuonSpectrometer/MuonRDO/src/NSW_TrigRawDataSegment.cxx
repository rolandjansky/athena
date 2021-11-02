/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/  

#include "MuonRDO/NSW_TrigRawDataSegment.h"

Muon::NSW_TrigRawDataSegment::NSW_TrigRawDataSegment() :
  m_deltaTheta(0),
  m_phiIndex(0),
  m_rIndex(0),
  m_spare(0),
  m_lowRes(false),
  m_phiRes(false),
  m_monitor(false)
{ }

Muon::NSW_TrigRawDataSegment::NSW_TrigRawDataSegment(uint8_t deltaTheta, uint8_t phiIndex, uint8_t rIndex, uint8_t spare,
						     bool lowRes, bool phiRes, bool monitor ) :
  m_deltaTheta(deltaTheta),
  m_phiIndex(phiIndex),
  m_rIndex(rIndex),
  m_spare(spare),
  m_lowRes(lowRes),
  m_phiRes(phiRes),
  m_monitor(monitor)
{ }

Muon::NSW_TrigRawDataSegment::NSW_TrigRawDataSegment(uint8_t deltaTheta, uint8_t phiIndex, uint8_t rIndex,
						     bool lowRes, bool phiRes) :
  m_deltaTheta(deltaTheta),
  m_phiIndex(phiIndex),
  m_rIndex(rIndex),
  m_spare(0),
  m_lowRes(lowRes),
  m_phiRes(phiRes),
  m_monitor(0)
{ }

