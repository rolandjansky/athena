/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcL1Rdo.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "AthenaKernel/errorcheck.h"

// Default constructor
TgcL1Rdo::TgcL1Rdo() : DataVector<TgcL1RawData>()
{
    m_id = 9999;
    clear();
}

// Creates an empty container ready for writing
TgcL1Rdo::TgcL1Rdo(uint16_t id, IdentifierHash idHash) : DataVector<TgcL1RawData>(), m_id(id), m_idHash(idHash)
{
    clear();
}

// Full constructor
TgcL1Rdo::TgcL1Rdo(uint16_t subDetectorId,
               uint16_t srodId,
               uint16_t bcId,
               uint16_t l1Id) : DataVector<TgcL1RawData>()
{
    clear();
    m_bcId = bcId;
    m_l1Id = l1Id;

    setOnlineId(subDetectorId, srodId);
}

// set Online ID
void TgcL1Rdo::setOnlineId(uint16_t subDetectorId, uint16_t srodId)
{
    m_subDetectorId = subDetectorId;
    m_srodId = srodId;

    // set my ID
    m_id = calculateOnlineId(m_subDetectorId, m_srodId);
}

// class method for RawData identification
uint16_t TgcL1Rdo::identifyRawData(const TgcL1RawData &rawData)
{
    return calculateOnlineId(rawData.subDetectorId(), rawData.srodId());
}

std::pair<int, int> TgcL1Rdo::initOnlineId()
{
  const char* name = "TgcRdo::calculateOnlineId";
  ServiceHandle<ITGCcablingServerSvc> tgcCabGet ("TGCcablingServerSvc", name);
  if (tgcCabGet.retrieve().isFailure()) {
    REPORT_ERROR_WITH_CONTEXT (StatusCode::FAILURE, name)
      << "Could not get TGCcablingServerSvc! " ;
    return std::make_pair (-1, -1);
  }

  int offset = -1; // 0 start 
  int MAX_N_SROD = 3;
  return std::make_pair (offset, MAX_N_SROD);
}

// online ID calculator
uint16_t TgcL1Rdo::calculateOnlineId (uint16_t subDetectorId, uint16_t srodId)
{  
  static const std::pair<int, int> offset_max = initOnlineId();
  int offset = offset_max.first;
  int MAX_N_SROD = offset_max.second;
  if (MAX_N_SROD < 0) {
    return 9999;
  }

  // A-side or C-side ?
  uint16_t is = (subDetectorId == 0x67) ? 0 : 1;
  
  return is * MAX_N_SROD + srodId + offset;
}

template <class stream>
stream& dump(stream& sl, const TgcL1Rdo& coll)
{
    sl << "TgcL1Rdo: "
    << " version=" << coll.version()
    << ", id=" << std::hex << coll.identify() << std::dec
    << ", srodId=" << coll.srodId()
    << ", subDetectorId=" << std::hex << coll.subDetectorId() << std::dec
    << ", triggerType=" << (int16_t)coll.triggerType()
    << ", bcId=" << coll.bcId()
    << ", l1Id=" << coll.l1Id()
    << ", errors=" << std::hex << *((uint16_t*)&coll.errors())
    << ", srodStatus=" << *((uint32_t*)&coll.srodStatus())
    << ", localStatus=" << *((uint32_t*)&coll.localStatus()) << std::dec
    << ", orbit=" << coll.orbit()
    << std::endl << "TgcL1RawData: [" << std::endl;
    int iRaw = 0;
    for (const TgcL1RawData* tgc : coll)
    {
        sl << ++iRaw
        << ": " << *tgc << std::endl;
    }
    sl << "]";
    return sl;
}

std::ostream& operator<<(std::ostream& sl, const TgcL1Rdo& coll)
{
    dump(sl, coll) << std::endl;
    return sl;
}

void TgcL1Rdo::clear()
{
    m_version = 300;
    m_subDetectorId = m_srodId = m_triggerType = m_bcId = m_l1Id = 0;
    memset(&m_errors, 0, sizeof(Errors));
    memset(&m_srodStatus, 0, sizeof(SRodStatus));
    memset(&m_localStatus, 0, sizeof(LocalStatus));
    m_orbit = 0;
}

