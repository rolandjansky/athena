/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/StgcRawDataCnv_p1.h"

void Muon::StgcRawDataCnv_p1::persToTrans( const Muon::StgcRawData_p1 *, Muon::sTGC_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::StgcRawDataCnv_p1::transToPers( const Muon::sTGC_RawData *transObj, Muon::StgcRawData_p1 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
}

Muon::sTGC_RawData* Muon::StgcRawDataCnv_p1::createTransient(const Muon::StgcRawData_p1* persObj, MsgStream& /**log*/)
{
    Muon::sTGC_RawData*  trans = new sTGC_RawData( Identifier (persObj->m_id) );
    // persToTrans(persObj, trans.get(), log);
    return trans;
}

