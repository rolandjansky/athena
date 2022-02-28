/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef NSW_TRIGRAWDATA_H
#define NSW_TRIGRAWDATA_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "MuonRDO/NSW_TrigRawDataSegment.h"

namespace Muon {
class NSW_TrigRawData : public DataVector<Muon::NSW_TrigRawDataSegment>
{

 public:
  NSW_TrigRawData();
  NSW_TrigRawData(uint16_t sectorId, uint16_t bcId);
  NSW_TrigRawData(uint16_t sectorId, char sectorSide, uint16_t bcId);
  NSW_TrigRawData(const NSW_TrigRawData &trd);

  ~NSW_TrigRawData() { };

  uint16_t sectorId() const {return m_sectorId;}
  char sectorSide() const {return m_sectorSide;}
  uint16_t bcId() const {return m_bcId;}

 private:

  uint16_t m_sectorId;
  char m_sectorSide;
  uint16_t m_bcId;

};
}

CLASS_DEF(Muon::NSW_TrigRawData,140250087,1)

#endif   ///  NSW_TRIGRAWDATA_H
