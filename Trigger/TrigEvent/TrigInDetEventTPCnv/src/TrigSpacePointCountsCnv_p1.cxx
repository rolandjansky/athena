/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p1.h"


void TrigSpacePointCountsCnv_p1::persToTrans(const TrigSpacePointCounts_p1* persObj, 
				             TrigSpacePointCounts* transObj, 
				             MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p1::persToTrans called " << endreq;

   transObj->m_sctSpEndcapC = persObj->m_SPsctECCcnt;
   transObj->m_sctSpBarrel  = persObj->m_SPsctBarr_cnt;
   transObj->m_sctSpEndcapA = persObj->m_SPsctECAcnt;
   
}


void TrigSpacePointCountsCnv_p1::transToPers(const TrigSpacePointCounts* transObj, 
				       TrigSpacePointCounts_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p1::transToPers called " << endreq;

   persObj->m_pixSPcnt     = 0; 
   persObj->m_sctSPcnt     = transObj->m_sctSpBarrel + transObj->m_sctSpEndcapA + transObj->m_sctSpEndcapC;

   persObj->m_pixCL1cnt    = 0;
   persObj->m_pixCL2cnt    = 0;
   persObj->m_pixCLmin3cnt = 0;
   persObj->m_SPpixBarr_cnt = transObj->m_sctSpBarrel;
   persObj->m_SPpixECAcnt  = transObj->m_sctSpEndcapA;
   persObj->m_SPpixECCcnt  = transObj->m_sctSpEndcapC;
}
