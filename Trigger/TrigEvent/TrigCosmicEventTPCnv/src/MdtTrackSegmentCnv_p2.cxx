/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigCosmicEvent/MdtTrackSegment.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p2.h"
#undef private
#undef protected

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p2.h"


void MdtTrackSegmentCnv_p2::persToTrans(const MdtTrackSegment_p2* persObj, 
				    MdtTrackSegment* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "MdtTrackSegmentCnv_p2::persToTrans called " << endreq;

   transObj->mStationId = persObj->mStationId;
   transObj->mTrackId   = persObj->mTrackId  ;
   transObj->mAlpha     = persObj->mAlpha    ;
   transObj->mB         = persObj->mB        ;
   transObj->mSwap      = persObj->mSwap     ;
   transObj->mT0        = persObj->mT0       ;
   transObj->mChi2      = persObj->mChi2     ;
   transObj->mNHits     = persObj->mNHits    ;
   transObj->mR         = persObj->mR        ;
   transObj->mZ         = persObj->mZ        ;
   
}


void MdtTrackSegmentCnv_p2::transToPers(const MdtTrackSegment* transObj, 
				       MdtTrackSegment_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "MdtTrackSegmentCnv_p2::transToPers called " << endreq;

   persObj->mStationId = transObj->mStationId;
   persObj->mTrackId   = transObj->mTrackId  ;
   persObj->mAlpha     = transObj->mAlpha    ;
   persObj->mB         = transObj->mB        ;
   persObj->mSwap      = transObj->mSwap     ;
   persObj->mT0        = transObj->mT0       ;
   persObj->mChi2      = transObj->mChi2     ;
   persObj->mNHits     = transObj->mNHits    ;
   persObj->mR         = transObj->mR        ;
   persObj->mZ         = transObj->mZ        ;
  
}
