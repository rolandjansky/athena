/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/sTGC_RawData.h"
#include "GaudiKernel/MsgStream.h"



// P1 ------------------------
// Constructor for Hit
Muon::sTGC_RawData::sTGC_RawData(const Identifier id)
  :m_id(id)
{
}

MsgStream& operator<<(MsgStream& sl, const Muon::sTGC_RawData& data)
{
  sl << "sTGC_RawData ("<< &data <<") "
  << ", Strip ID=" << data.identify();
  return sl;
}

std::ostream& operator<<(std::ostream& sl, const Muon::sTGC_RawData& data)
{
  sl << "sTGC_RawData ("<< &data <<") "
  << ", Strip ID=" << data.identify();
  return sl;
}

