/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTruthEvent/TrigIDHitStats.h"
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"



void TrigIDHitStatsCnv_p1::persToTrans(const TrigIDHitStats_p1* persObj, 
				          TrigIDHitStats* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigIDHitStatsCnv_p1::persToTrans called " << endreq;

   for(unsigned i=0; i<3; i++) {
     (*transObj)[static_cast<TrigIDHitStats::IDSubDetType>(i)] = persObj->numHits[i];
   }
   
}


void TrigIDHitStatsCnv_p1::transToPers(const TrigIDHitStats* transObj, 
				          TrigIDHitStats_p1* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigIDHitStatsCnv_p1::transToPers called " << endreq;

   for(unsigned i=0; i<3; i++) {
     persObj->numHits[i] = (*transObj)[static_cast<TrigIDHitStats::IDSubDetType>(i)];
   }

}
