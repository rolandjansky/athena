/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#undef private
#undef protected


#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_p1.h"



void TrigInDetTrackCollectionCnv_p1::persToTrans(const TrigInDetTrackCollection_p1* persObj, 
				       TrigInDetTrackCollection* transObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigInDetTrackCollectionCnv_p1::persToTrans called " << endreq;

   transObj->m_RoI_ID = persObj->m_RoI_ID; 

   m_trigInDetTrackVectorCnv.persToTrans( &(persObj->m_trigInDetTrackVector), transObj, log );
}


void TrigInDetTrackCollectionCnv_p1::transToPers(const TrigInDetTrackCollection* transObj, 
				       TrigInDetTrackCollection_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigInDetTrackCollectionCnv_p1::transToPers called " << endreq;

   persObj->m_RoI_ID = transObj->m_RoI_ID;

   m_trigInDetTrackVectorCnv.transToPers( transObj, &(persObj->m_trigInDetTrackVector), log );

}
