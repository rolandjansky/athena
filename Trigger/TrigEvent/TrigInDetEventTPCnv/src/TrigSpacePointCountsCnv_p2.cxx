/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p2.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p2.h"


void TrigSpacePointCountsCnv_p2::persToTrans(const TrigSpacePointCounts_p2* persObj, 
				    TrigSpacePointCounts* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p2::persToTrans called " << endreq;

   TrigHisto2D pixelClusEndcapC;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapC, &pixelClusEndcapC, log);

   TrigHisto2D pixelClusBarrel;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusBarrel, &pixelClusBarrel, log);

   TrigHisto2D pixelClusEndcapA;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapA, &pixelClusEndcapA, log);

   *transObj = TrigSpacePointCounts (std::move(pixelClusEndcapC),
                                     std::move(pixelClusBarrel),
                                     std::move(pixelClusEndcapA),
                                     std::vector<Identifier>(),
                                     persObj->m_sctSpEndcapC,
                                     persObj->m_sctSpBarrel,
                                     persObj->m_sctSpEndcapA,
                                     std::vector<Identifier>());
}


void TrigSpacePointCountsCnv_p2::transToPers(const TrigSpacePointCounts* transObj, 
				       TrigSpacePointCounts_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p2::transToPers called " << endreq;

   m_trigHistoCnv.transToPers(&transObj->pixelClusEndcapC(), &persObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.transToPers(&transObj->pixelClusBarrel(), &persObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.transToPers(&transObj->pixelClusEndcapA(), &persObj->m_pixelClusEndcapA, log);
   persObj->m_sctSpEndcapC     = transObj->sctSpEndcapC();
   persObj->m_sctSpBarrel      = transObj->sctSpBarrel();
   persObj->m_sctSpEndcapA     = transObj->sctSpEndcapA();
}
