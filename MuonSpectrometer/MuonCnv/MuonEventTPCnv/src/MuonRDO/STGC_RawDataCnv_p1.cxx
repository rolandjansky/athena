/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataCnv_p1.h"

void Muon::STGC_RawDataCnv_p1::persToTrans( const Muon::STGC_RawData_p1 *, Muon::STGC_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::STGC_RawDataCnv_p1::transToPers( const Muon::STGC_RawData *transObj, Muon::STGC_RawData_p1 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
}

Muon::STGC_RawData* Muon::STGC_RawDataCnv_p1::createTransient(const Muon::STGC_RawData_p1* persObj, MsgStream& /**log*/)
{
    Muon::STGC_RawData*  trans = new STGC_RawData( Identifier (persObj->m_id) );
    // persToTrans(persObj, trans.get(), log);
    return trans;
}

