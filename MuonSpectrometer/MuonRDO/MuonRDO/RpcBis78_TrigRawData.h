/*
  Copyright (C) 2021-2020 CERN for the benefit of the ATLAS collaboration
*/  

#ifndef RpcBis78_TRIGRAWDATA_H
#define RpcBis78_TRIGRAWDATA_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "MuonRDO/RpcBis78_TrigRawDataSegment.h"

/*
  This class is for BIS78 trigger simulation.
  BIS78 trigger goes to TGC sector logic.
  This is different from other BIS chambers of phase 2, in this case the
  trigger is in the barrel logic. A different class may be needed.
*/

namespace Muon {
class RpcBis78_TrigRawData : public DataVector<Muon::RpcBis78_TrigRawDataSegment>
{

 public:
  RpcBis78_TrigRawData() = default;
  RpcBis78_TrigRawData(uint16_t sectorId, uint16_t sideId, uint16_t bcId);
  ~RpcBis78_TrigRawData() = default;

  uint16_t sectorId() const {return m_sectorId;}
  uint16_t sideId() const {return m_sideId;}
  uint16_t bcId() const {return m_bcId;}

 private:

  uint16_t m_sectorId{0};  // sector
  uint16_t m_sideId{0};  // side
  uint16_t m_bcId{0};  // BC-id

};
}

CLASS_DEF(Muon::RpcBis78_TrigRawData,168084830,1)

#endif   ///  RpcBis78_TRIGRAWDATA_H




