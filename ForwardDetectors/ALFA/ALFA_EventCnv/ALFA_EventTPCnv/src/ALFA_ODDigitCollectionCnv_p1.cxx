/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_ODDigitCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static ALFA_ODDigitCnv_p1 digitConv;
 
void ALFA_ODDigitCollectionCnv_p1::transToPers(const ALFA_ODDigitCollection* transObj, ALFA_ODDigitCollection_p1* persObj, MsgStream& log) {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_ODDigit_p1& digit = (*persObj)[i];
     
    digitConv.transToPers((*transObj)[i], &digit, log);
  }    
}
 
void ALFA_ODDigitCollectionCnv_p1::persToTrans(const ALFA_ODDigitCollection_p1* persObj, ALFA_ODDigitCollection* transObj, MsgStream& log) {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_ODDigit_p1* digit = &((*persObj)[i]);
     
    transObj->push_back(digitConv.createTransient(digit, log));
  }    
}
 