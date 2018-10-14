/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/NSW_TrigRawData.h"

Muon::NSW_TrigRawData::NSW_TrigRawData() :
  DataVector<Muon::NSW_TrigRawDataSegment>(),
  m_sectorId(0),
  m_bcId(0)
{

}

Muon::NSW_TrigRawData::NSW_TrigRawData(uint16_t sectorId, uint16_t bcId) :
  DataVector<Muon::NSW_TrigRawDataSegment>(),
  m_sectorId(sectorId),
  m_bcId(bcId)
{

}
