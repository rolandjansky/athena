/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_EventTPCnv/ALFA_DigitCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static const ALFA_DigitCnv_p1 digitConv;
 
void ALFA_DigitCollectionCnv_p1::transToPers(const ALFA_DigitCollection* transObj, ALFA_DigitCollection_p1* persObj, MsgStream& log) const {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    ALFA_Digit_p1& digit = (*persObj)[i];
     
    digitConv.transToPers((*transObj)[i], &digit, log);
  }    
}
 
void ALFA_DigitCollectionCnv_p1::persToTrans(const ALFA_DigitCollection_p1* persObj, ALFA_DigitCollection* transObj, MsgStream& log) const {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const ALFA_Digit_p1* digit = &((*persObj)[i]);
     
    transObj->push_back(digitConv.createTransientConst(digit, log));
  }    
}
