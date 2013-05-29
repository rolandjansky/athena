/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static ALFA_LocRecODEventCnv_p1 recConv;
 
void ALFA_LocRecODEvCollectionCnv_p1::transToPers(const ALFA_LocRecODEvCollection* transObj, ALFA_LocRecODEvCollection_p1* persObj, MsgStream& log) {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_LocRecODEvent_p1& locRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &locRec, log);
  }    
}
 
void ALFA_LocRecODEvCollectionCnv_p1::persToTrans(const ALFA_LocRecODEvCollection_p1* persObj, ALFA_LocRecODEvCollection* transObj, MsgStream& log) {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_LocRecODEvent_p1* locRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransient(locRec, log));
  }    
}
 