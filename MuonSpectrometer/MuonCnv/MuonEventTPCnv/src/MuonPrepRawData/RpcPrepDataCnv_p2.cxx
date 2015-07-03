/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonPrepRawData/RpcPrepData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p2.h"

void RpcPrepDataCnv_p2::
persToTrans( const Muon::RpcPrepData_p2 *persObj, Muon::RpcPrepData *transObj,MsgStream &log ) 
{
   //log << MSG::DEBUG << "RpcPrepDataCnv_p2::persToTrans" << endreq;
   fillTransFromPStore( &m_prdCnv, persObj->m_prd, transObj, log );
   transObj->m_time           = persObj->m_time;
   transObj->m_triggerInfo    = persObj->m_triggerInfo;
   transObj->m_ambiguityFlag  = persObj->m_ambiguityFlag;
}

void RpcPrepDataCnv_p2::
transToPers( const Muon::RpcPrepData *transObj, Muon::RpcPrepData_p2 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "RpcPrepDataCnv_p2::transToPers" << endreq;
   persObj->m_prd = baseToPersistent( &m_prdCnv, transObj, log );
   persObj->m_time           = transObj->m_time;
   persObj->m_triggerInfo    = transObj->m_triggerInfo;
   persObj->m_ambiguityFlag  = transObj->m_ambiguityFlag;
}


