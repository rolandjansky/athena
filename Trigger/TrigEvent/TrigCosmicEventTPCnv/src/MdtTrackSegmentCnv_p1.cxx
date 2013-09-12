/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigCosmicEvent/MdtTrackSegment.h"
#undef private
#undef protected

#include "TrigCosmicEventTPCnv/MdtTrackSegment_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p1.h"


void MdtTrackSegmentCnv_p1::persToTrans(const MdtTrackSegment_p1* persObj, 
				    MdtTrackSegment* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "MdtTrackSegmentCnv_p1::persToTrans called " << endreq;

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


void MdtTrackSegmentCnv_p1::transToPers(const MdtTrackSegment* transObj, 
				       MdtTrackSegment_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "MdtTrackSegmentCnv_p1::transToPers called " << endreq;

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
