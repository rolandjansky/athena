/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/STGC_RawData.h"
#include "GaudiKernel/MsgStream.h"



// P1 ------------------------
// Constructor for Hit
Muon::STGC_RawData::STGC_RawData(const Identifier id)
  :m_id(id)
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

