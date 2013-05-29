/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_GloRecEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static ALFA_GloRecEventCnv_p1 recConv;
 
void ALFA_GloRecEvCollectionCnv_p1::transToPers(const ALFA_GloRecEvCollection* transObj, ALFA_GloRecEvCollection_p1* persObj, MsgStream& log) {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_GloRecEvent_p1& gloRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &gloRec, log);
  }    
}
 
void ALFA_GloRecEvCollectionCnv_p1::persToTrans(const ALFA_GloRecEvCollection_p1* persObj, ALFA_GloRecEvCollection* transObj, MsgStream& log) {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_GloRecEvent_p1* gloRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransient(gloRec, log));
  }    
}
 