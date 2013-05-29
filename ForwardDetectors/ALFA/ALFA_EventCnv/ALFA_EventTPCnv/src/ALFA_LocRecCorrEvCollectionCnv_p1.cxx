/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static ALFA_LocRecCorrEventCnv_p1 recConv;
 
void ALFA_LocRecCorrEvCollectionCnv_p1::transToPers(const ALFA_LocRecCorrEvCollection* transObj, ALFA_LocRecCorrEvCollection_p1* persObj, MsgStream& log) {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_LocRecCorrEvent_p1& locRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &locRec, log);
  }    
}
 
void ALFA_LocRecCorrEvCollectionCnv_p1::persToTrans(const ALFA_LocRecCorrEvCollection_p1* persObj, ALFA_LocRecCorrEvCollection* transObj, MsgStream& log) {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_LocRecCorrEvent_p1* locRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransient(locRec, log));
  }    
}
 