/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/CscPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p1.h"

void CscPrepDataCnv_p1::
persToTrans( const Muon::CscPrepData_p1 *persObj, Muon::CscPrepData *transObj,MsgStream &log ) 
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p1::persToTrans" << endreq;
   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
   transObj->m_charge         = persObj->m_charge;
   transObj->m_time           = persObj->m_time;
   transObj->m_status         = static_cast<const Muon::CscClusterStatus>((persObj->m_status)&0xFF); // First 8 bits reserved for ClusterStatus.
   transObj->m_timeStatus     = static_cast<const Muon::CscTimeStatus>((persObj->m_status)>>8);
}

void CscPrepDataCnv_p1::
transToPers( const Muon::CscPrepData *transObj, Muon::CscPrepData_p1 *persObj, MsgStream &log )
{
   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "CscPrepDataCnv_p1::transToPers" << endreq;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_charge         = transObj->m_charge;
   persObj->m_time           = transObj->m_time;
   persObj->m_status         = (transObj->m_timeStatus<<8); // First 8 bits reserved for ClusterStatus.
   persObj->m_status         += transObj->m_status;
}


