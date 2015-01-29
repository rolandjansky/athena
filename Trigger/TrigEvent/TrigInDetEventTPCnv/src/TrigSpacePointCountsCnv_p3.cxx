/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigSpacePointCounts.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigSpacePointCounts_p3.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCnv_p3.h"


void TrigSpacePointCountsCnv_p3::persToTrans(const TrigSpacePointCounts_p3* persObj, 
					     TrigSpacePointCounts* transObj, 
					     MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p3::persToTrans called " << endreq;

   std::vector<unsigned int> droppedModuleIds;
   std::vector<unsigned int>::iterator itr;
   std::vector<unsigned int>::iterator itr_end;

   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapC, &transObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusBarrel, &transObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.persToTrans(&persObj->m_pixelClusEndcapA, &transObj->m_pixelClusEndcapA, log);

   transObj->m_droppedPixelModules.clear();
   droppedModuleIds.clear();
   droppedModuleIds = persObj->m_droppedPixelModules;
   itr = droppedModuleIds.begin();
   itr_end = droppedModuleIds.end();
   for(;itr != itr_end; ++itr) {
     transObj->m_droppedPixelModules.push_back(Identifier(*itr));
   }

   transObj->m_sctSpEndcapC     = persObj->m_sctSpEndcapC;
   transObj->m_sctSpBarrel      = persObj->m_sctSpBarrel;
   transObj->m_sctSpEndcapA     = persObj->m_sctSpEndcapA;

   transObj->m_droppedSctModules.clear();
   droppedModuleIds.clear();
   droppedModuleIds = persObj->m_droppedSctModules;
   itr = droppedModuleIds.begin();
   itr_end = droppedModuleIds.end();
   for(;itr != itr_end; ++itr) {
     transObj->m_droppedSctModules.push_back(Identifier(*itr));
   }
}


void TrigSpacePointCountsCnv_p3::transToPers(const TrigSpacePointCounts* transObj, 
					     TrigSpacePointCounts_p3* persObj, 
					     MsgStream &log)
{
   log << MSG::DEBUG << "TrigSpacePointCountsCnv_p3::transToPers called " << endreq;

   std::vector<Identifier> droppedIdentifiers;
   std::vector<Identifier>::iterator itr;
   std::vector<Identifier>::iterator itr_end;

   m_trigHistoCnv.transToPers(&transObj->m_pixelClusEndcapC, &persObj->m_pixelClusEndcapC, log);
   m_trigHistoCnv.transToPers(&transObj->m_pixelClusBarrel, &persObj->m_pixelClusBarrel, log);
   m_trigHistoCnv.transToPers(&transObj->m_pixelClusEndcapA, &persObj->m_pixelClusEndcapA, log);

   persObj->m_droppedPixelModules.clear();
   droppedIdentifiers.clear();
   droppedIdentifiers = transObj->m_droppedPixelModules;
   itr = droppedIdentifiers.begin();
   itr_end = droppedIdentifiers.end();
   for(;itr != itr_end; ++itr) {
     persObj->m_droppedPixelModules.push_back((*itr).get_compact());
   }

   persObj->m_sctSpEndcapC     = transObj->m_sctSpEndcapC;
   persObj->m_sctSpBarrel      = transObj->m_sctSpBarrel;
   persObj->m_sctSpEndcapA     = transObj->m_sctSpEndcapA;

   persObj->m_droppedSctModules.clear();
   droppedIdentifiers.clear();
   droppedIdentifiers = transObj->m_droppedSctModules;
   itr = droppedIdentifiers.begin();
   itr_end = droppedIdentifiers.end();
   for(;itr != itr_end; ++itr) {
     persObj->m_droppedSctModules.push_back((*itr).get_compact());
   }
}
