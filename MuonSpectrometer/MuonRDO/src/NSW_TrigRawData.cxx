/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/NSW_TrigRawData.h"

Muon::NSW_TrigRawData::NSW_TrigRawData() :
  DataVector<Muon::NSW_TrigRawDataSegment>(),
  m_sectorId(0),
  m_sectorSide('-'),
  m_bcId(0)
{

}

Muon::NSW_TrigRawData::NSW_TrigRawData(uint16_t sectorId, uint16_t bcId) :
  DataVector<Muon::NSW_TrigRawDataSegment>(),
  m_sectorId(sectorId),
  m_sectorSide('-'),
  m_bcId(bcId)
{

}

Muon::NSW_TrigRawData::NSW_TrigRawData(uint16_t sectorId, char sectorSide, uint16_t bcId) :
  DataVector<Muon::NSW_TrigRawDataSegment>(),
  m_sectorId(sectorId),
  m_sectorSide(sectorSide),
  m_bcId(bcId)
{

}

Muon::NSW_TrigRawData::NSW_TrigRawData(const Muon::NSW_TrigRawData &trd) :
  DataVector<Muon::NSW_TrigRawDataSegment>()
{
  m_sectorId = trd.m_sectorId;
  m_sectorSide = trd.m_sectorSide;
  m_bcId = trd.m_bcId;
  for (const auto rawSegment : trd) {
    Muon::NSW_TrigRawDataSegment* trigRawDataSegment = new Muon::NSW_TrigRawDataSegment(*rawSegment);
    this->push_back(trigRawDataSegment);
  }
}

