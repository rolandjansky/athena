/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcBis78_TrigRawData.h"

Muon::RpcBis78_TrigRawData::RpcBis78_TrigRawData(uint16_t sectorId, uint16_t sideId, uint16_t bcId) :
  DataVector<Muon::RpcBis78_TrigRawDataSegment>(),
  m_sectorId(sectorId),
  m_sideId(sideId),
  m_bcId(bcId)
{

}
