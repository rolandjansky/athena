/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"


void TrigSpacePointCountsCnv_p2::persToTrans(const TrigSpacePointCounts_p2* persObj, 
				    TrigSpacePointCounts* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p2::persToTrans called " << endreq;

   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapC, &transObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusBarrel, &transObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapA, &transObj->m_pixelClusEndcapA, log);
   transObj->m_sctSpEndcapC     = persObj->m_sctSpEndcapC;
   transObj->m_sctSpBarrel      = persObj->m_sctSpBarrel;
   transObj->m_sctSpEndcapA     = persObj->m_sctSpEndcapA;
}


void TrigSpacePointCountsCnv_p2::transToPers(const TrigSpacePointCounts* transObj, 
				       TrigSpacePointCounts_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p2::transToPers called " << endreq;

   m_trigHistoCnv.transToPers(&transObj->m_pixelClusEndcapC, &persObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.transToPers(&transObj->m_pixelClusBarrel, &persObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.transToPers(&transObj->m_pixelClusEndcapA, &persObj->m_pixelClusEndcapA, log);
   persObj->m_sctSpEndcapC     = transObj->m_sctSpEndcapC;
   persObj->m_sctSpBarrel      = transObj->m_sctSpBarrel;
   persObj->m_sctSpEndcapA     = transObj->m_sctSpEndcapA;
}
