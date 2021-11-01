/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/MM_RawDataCnv_p1.h"

void Muon::MM_RawDataCnv_p1::persToTrans( const Muon::MM_RawData_p1 *, Muon::MM_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::MM_RawDataCnv_p1::transToPers( const Muon::MM_RawData *transObj, Muon::MM_RawData_p1 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
  persObj->m_channel = transObj->channel();
  persObj->m_time = transObj->time();
  persObj->m_charge = transObj->charge();
}

Muon::MM_RawData* Muon::MM_RawDataCnv_p1::createTransient(const Muon::MM_RawData_p1* persObj, MsgStream& /**log*/)
{
  Muon::MM_RawData*  trans = new MM_RawData( Identifier (persObj->m_id),
					     persObj->m_channel,
					     persObj->m_time, 
					     persObj->m_charge,
               0 );
  
  return trans;
}

