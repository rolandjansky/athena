/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p2.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p2.h"

void TrigTrtHitCountsCnv_p2::persToTrans( const TrigTrtHitCounts_p2 *persObj,
					  TrigTrtHitCounts    *transObj,
					  MsgStream& log ) {
  log << MSG::DEBUG << "TrigTrtHitCountsCnv_p2::persToTrans" << endreq;
  
  m_trigHistoCnv.persToTrans(&persObj->m_endcapC, &transObj->m_endcapC, log);
  m_trigHistoCnv.persToTrans(&persObj->m_barrel, &transObj->m_barrel, log);
  m_trigHistoCnv.persToTrans(&persObj->m_endcapA, &transObj->m_endcapA, log);
}

void TrigTrtHitCountsCnv_p2 :: transToPers( const TrigTrtHitCounts *transObj,
					    TrigTrtHitCounts_p2 *persObj,
					    MsgStream& log ) {
  log << MSG::DEBUG << "TrigTrtHitCountsCnv_p2::transToPers" << endreq;
  
  m_trigHistoCnv.transToPers(&transObj->m_endcapC, &persObj->m_endcapC, log);
  m_trigHistoCnv.transToPers(&transObj->m_barrel, &persObj->m_barrel, log);
  m_trigHistoCnv.transToPers(&transObj->m_endcapA, &persObj->m_endcapA, log);
}
