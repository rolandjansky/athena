/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
//I should add TrigIDHitStatsCnv as a friend in TrigIDHitStats.h
#include "TrigInDetTruthEvent/TrigIDHitStats.h"
#undef private
#undef protected

#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"



void TrigIDHitStatsCnv_p1::persToTrans(const TrigIDHitStats_p1* persObj, 
				          TrigIDHitStats* transObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigIDHitStatsCnv_p1::persToTrans called " << endreq;

   for(unsigned i=0; i<3; i++) {
     transObj->numHits[i] = persObj->numHits[i];
   }
   
}


void TrigIDHitStatsCnv_p1::transToPers(const TrigIDHitStats* transObj, 
				          TrigIDHitStats_p1* persObj, 
				          MsgStream &log)
{
   log << MSG::DEBUG << "TrigIDHitStatsCnv_p1::transToPers called " << endreq;

   for(unsigned i=0; i<3; i++) {
     persObj->numHits[i] = transObj->numHits[i];
   }

}
