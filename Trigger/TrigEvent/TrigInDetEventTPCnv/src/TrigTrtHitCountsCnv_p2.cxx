/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTrtHitCounts.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p2.h"

void TrigTrtHitCountsCnv_p2::persToTrans( const TrigTrtHitCounts_p2 *persObj,
					  TrigTrtHitCounts    *transObj,
					  MsgStream& log ) {
  log << MSG::DEBUG << "TrigTrtHitCountsCnv_p2::persToTrans" << endreq;
  
  TrigHisto1D endcapC;
  m_trigHistoCnv.persToTrans(&persObj->m_endcapC, &endcapC, log);
  TrigHisto1D barrel;
  m_trigHistoCnv.persToTrans(&persObj->m_barrel, &barrel, log);
  TrigHisto1D endcapA;
  m_trigHistoCnv.persToTrans(&persObj->m_endcapA, &endcapA, log);

  *transObj = TrigTrtHitCounts (std::move(endcapC),
                                std::move(barrel),
                                std::move(endcapA));
}

void TrigTrtHitCountsCnv_p2 :: transToPers( const TrigTrtHitCounts *transObj,
					    TrigTrtHitCounts_p2 *persObj,
					    MsgStream& log ) {
  log << MSG::DEBUG << "TrigTrtHitCountsCnv_p2::transToPers" << endreq;
  
  m_trigHistoCnv.transToPers(&transObj->endcapC(), &persObj->m_endcapC, log);
  m_trigHistoCnv.transToPers(&transObj->barrel(),  &persObj->m_barrel,  log);
  m_trigHistoCnv.transToPers(&transObj->endcapA(), &persObj->m_endcapA, log);
}
