/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/STGC_RawData.h"
#include "GaudiKernel/MsgStream.h"



// P1 ------------------------
// Constructor for Hit
Muon::STGC_RawData::STGC_RawData(const Identifier id)
  :m_id(id), m_time(0.0), m_charge(0.0), m_bcTag(0), m_isDead(false)
{
}

Muon::STGC_RawData::STGC_RawData(const Identifier id, const uint16_t bcTag, const float time, 
				 const float charge, const bool isDead)
  :m_id(id), m_bcTag(bcTag), m_time(time), m_charge(charge), m_isDead(isDead)
{
}

MsgStream& operator<<(MsgStream& sl, const Muon::STGC_RawData& data)
{
  sl << "STGC_RawData ("<< &data <<") "
  << ", Strip ID=" << data.identify();
  return sl;
}

std::ostream& operator<<(std::ostream& sl, const Muon::STGC_RawData& data)
{
  sl << "STGC_RawData ("<< &data <<") "
  << ", Strip ID=" << data.identify();
  return sl;
}

