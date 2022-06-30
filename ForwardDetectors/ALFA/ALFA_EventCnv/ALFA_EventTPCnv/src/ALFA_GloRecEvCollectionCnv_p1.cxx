/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_GloRecEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static const ALFA_GloRecEventCnv_p1 recConv;
 
void ALFA_GloRecEvCollectionCnv_p1::transToPers(const ALFA_GloRecEvCollection* transObj, ALFA_GloRecEvCollection_p1* persObj, MsgStream& log) const {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_GloRecEvent_p1& gloRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &gloRec, log);
  }    
}
 
void ALFA_GloRecEvCollectionCnv_p1::persToTrans(const ALFA_GloRecEvCollection_p1* persObj, ALFA_GloRecEvCollection* transObj, MsgStream& log) const {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_GloRecEvent_p1* gloRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransientConst(gloRec, log));
  }    
}
