/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static const ALFA_LocRecEventCnv_p1 recConv;
 
void ALFA_LocRecEvCollectionCnv_p1::transToPers(const ALFA_LocRecEvCollection* transObj, ALFA_LocRecEvCollection_p1* persObj, MsgStream& log) const {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_LocRecEvent_p1& locRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &locRec, log);
  }    
}
 
void ALFA_LocRecEvCollectionCnv_p1::persToTrans(const ALFA_LocRecEvCollection_p1* persObj, ALFA_LocRecEvCollection* transObj, MsgStream& log) const {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_LocRecEvent_p1* locRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransientConst(locRec, log));
  }    
}
