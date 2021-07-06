/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/TgcRdo.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "AthenaKernel/errorcheck.h"

// Default constructor
TgcRdo::TgcRdo() : DataVector<TgcRawData>()
{
    m_id = 9999;
    clear();
}

// Creates an empty container ready for writing
TgcRdo::TgcRdo(uint16_t id, IdentifierHash idHash) : DataVector<TgcRawData>(), m_id(id), m_idHash(idHash)
{
    clear();
}

// Full constructor
TgcRdo::TgcRdo(uint16_t subDetectorId,
               uint16_t rodId,
               uint16_t bcId,
               uint16_t l1Id) : DataVector<TgcRawData>()
{
    clear();
    m_bcId = bcId;
    m_l1Id = l1Id;

    setOnlineId(subDetectorId, rodId);
}

// set Online ID
void TgcRdo::setOnlineId(uint16_t subDetectorId, uint16_t rodId)
{
    m_subDetectorId = subDetectorId;
    m_rodId = rodId;

    // set my ID
    m_id = calculateOnlineId(m_subDetectorId, m_rodId);
}

// class method for RawData identification
uint16_t TgcRdo::identifyRawData(const TgcRawData &rawData)
{
    return calculateOnlineId(rawData.subDetectorId(), rawData.rodId());
}

// Returns offset, MAX_N_ROD
std::pair<int, int> TgcRdo::initOnlineId()
{
  const char* name = "TgcRdo::calculateOnlineId";
  ServiceHandle<ITGCcablingServerSvc> tgcCabGet ("TGCcablingServerSvc", name);
  if (tgcCabGet.retrieve().isFailure()) {
    REPORT_ERROR_WITH_CONTEXT (StatusCode::FAILURE, name)
      << "Could not get TGCcablingServerSvc! " ;
    return std::make_pair (-1, -1);
  }

  bool isAtlas = tgcCabGet->isAtlas();
  int offset = isAtlas ? -1 : 0;
  int MAX_N_ROD = isAtlas ? 12 : 8;
  return std::make_pair (offset, MAX_N_ROD);
}

// online ID calculator
uint16_t TgcRdo::calculateOnlineId (uint16_t subDetectorId, uint16_t rodId)
{
  static const std::pair<int, int> offset_max = initOnlineId();
  int offset = offset_max.first;
  int MAX_N_ROD = offset_max.second;
  if (MAX_N_ROD < 0) {
    return 9999;
  }

  // A-side or C-side ?
  uint16_t is = (subDetectorId == 0x67) ? 0 : 1;
  
  return is * MAX_N_ROD + rodId + offset;
}

template <class stream>
stream& dump(stream& sl, const TgcRdo& coll)
{
    sl << "TgcRdo: "
    << " version=" << coll.version()
    << ", id=" << std::hex << coll.identify() << std::dec
    << ", rodId=" << coll.rodId()
    << ", subDetectorId=" << std::hex << coll.subDetectorId() << std::dec
    << ", triggerType=" << (int16_t)coll.triggerType()
    << ", bcId=" << coll.bcId()
    << ", l1Id=" << coll.l1Id()
    << ", errors=" << std::hex << *((uint16_t*)&coll.errors())
    << ", rodStatus=" << *((uint32_t*)&coll.rodStatus())
    << ", localStatus=" << *((uint32_t*)&coll.localStatus()) << std::dec
    << ", orbit=" << coll.orbit()
    << std::endl << "TgcRawData: [" << std::endl;
    int iRaw = 0;
    for (const TgcRawData* rd : coll)
    {
        sl << ++iRaw
        << ": " << *rd << std::endl;
    }
    sl << "]";
    return sl;
}

std::ostream& operator<<(std::ostream& sl, const TgcRdo& coll)
{
    dump(sl, coll) << std::endl;
    return sl;
}

void TgcRdo::clear()
{
    m_version = 300;
    m_subDetectorId = m_rodId = m_triggerType = m_bcId = m_l1Id = 0;
    memset(&m_errors, 0, sizeof(Errors));
    memset(&m_rodStatus, 0, sizeof(RodStatus));
    memset(&m_localStatus, 0, sizeof(LocalStatus));
    m_orbit = 0;
}

