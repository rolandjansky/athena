/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSpacePointCounts.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p3.h"


void TrigSpacePointCountsCnv_p3::persToTrans(const TrigSpacePointCounts_p3* persObj, 
					     TrigSpacePointCounts* transObj, 
					     MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p3::persToTrans called " << endreq;

   TrigHisto2D pixelClusEndcapC;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapC, &pixelClusEndcapC, log);

   TrigHisto2D pixelClusBarrel;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusBarrel, &pixelClusBarrel, log);

   TrigHisto2D pixelClusEndcapA;
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapA, &pixelClusEndcapA, log);

   std::vector<Identifier> droppedPixelModules;
   droppedPixelModules.reserve (persObj->m_droppedPixelModules.size());
   for (unsigned int mod : persObj->m_droppedPixelModules)
     droppedPixelModules.emplace_back (mod);

   std::vector<Identifier> droppedSctModules;
   droppedSctModules.reserve (persObj->m_droppedSctModules.size());
   for (unsigned int mod : persObj->m_droppedSctModules)
     droppedSctModules.emplace_back (mod);

   *transObj = TrigSpacePointCounts (std::move(pixelClusEndcapC),
                                     std::move(pixelClusBarrel),
                                     std::move(pixelClusEndcapA),
                                     std::move(droppedPixelModules),
                                     persObj->m_sctSpEndcapC,
                                     persObj->m_sctSpBarrel,
                                     persObj->m_sctSpEndcapA,
                                     std::move(droppedSctModules));
}


void TrigSpacePointCountsCnv_p3::transToPers(const TrigSpacePointCounts* transObj, 
					     TrigSpacePointCounts_p3* persObj, 
					     MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p3::transToPers called " << endreq;

   m_trigHistoCnv.transToPers(&transObj->pixelClusEndcapC(), &persObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.transToPers(&transObj->pixelClusBarrel(), &persObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.transToPers(&transObj->pixelClusEndcapA(), &persObj->m_pixelClusEndcapA, log);

   persObj->m_droppedPixelModules.clear();
   for (const Identifier& id : transObj->droppedPixelModules())
     persObj->m_droppedPixelModules.push_back(id.get_identifier32().get_compact());

   persObj->m_sctSpEndcapC     = transObj->sctSpEndcapC();
   persObj->m_sctSpBarrel      = transObj->sctSpBarrel();
   persObj->m_sctSpEndcapA     = transObj->sctSpEndcapA();

   persObj->m_droppedSctModules.clear();
   for (const Identifier& id : transObj->droppedSctModules())
     persObj->m_droppedSctModules.push_back(id.get_identifier32().get_compact());
}
