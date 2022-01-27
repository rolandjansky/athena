/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataCnv_p2.h"

void Muon::STGC_RawDataCnv_p2::persToTrans( const Muon::STGC_RawData_p2 *, Muon::STGC_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::STGC_RawDataCnv_p2::transToPers( const Muon::STGC_RawData *transObj, Muon::STGC_RawData_p2 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
  persObj->m_bcTag = transObj->bcTag();
  persObj->m_time = transObj->tdo();
  persObj->m_charge = transObj->charge();
  persObj->m_isDead = transObj->isDead();
}

Muon::STGC_RawData* Muon::STGC_RawDataCnv_p2::createTransient(const Muon::STGC_RawData_p2* persObj, MsgStream& /**log*/)
{
  Muon::STGC_RawData*  trans = new STGC_RawData( Identifier (persObj->m_id),
						 persObj->m_bcTag,
             static_cast<float>(persObj->m_time),//  place holder for tdo->time from calibration
						 persObj->m_time,
						 persObj->m_charge, 
						 persObj->m_isDead);
 
  return trans;
}

