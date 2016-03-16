/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"


void TrigSpacePointCountsCnv_p1::persToTrans(const TrigSpacePointCounts_p1* persObj, 
				             TrigSpacePointCounts* transObj, 
				             MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p1::persToTrans called " << endreq;

   *transObj = TrigSpacePointCounts (TrigHisto2D(),
                                     TrigHisto2D(),
                                     TrigHisto2D(),
                                     std::vector<Identifier>(),
                                     persObj->m_SPsctECCcnt,
                                     persObj->m_SPsctBarr_cnt,
                                     persObj->m_SPsctECAcnt,
                                     std::vector<Identifier>());
}


void TrigSpacePointCountsCnv_p1::transToPers(const TrigSpacePointCounts* transObj, 
				       TrigSpacePointCounts_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p1::transToPers called " << endreq;

   persObj->m_pixSPcnt     = 0; 
   persObj->m_sctSPcnt     = transObj->sctSpBarrel() + transObj->sctSpEndcapA() + transObj->sctSpEndcapC();

   persObj->m_pixCL1cnt    = 0;
   persObj->m_pixCL2cnt    = 0;
   persObj->m_pixCLmin3cnt = 0;
   persObj->m_SPsctBarr_cnt = transObj->sctSpBarrel();
   persObj->m_SPsctECAcnt  = transObj->sctSpEndcapA();
   persObj->m_SPsctECCcnt  = transObj->sctSpEndcapC();
}
