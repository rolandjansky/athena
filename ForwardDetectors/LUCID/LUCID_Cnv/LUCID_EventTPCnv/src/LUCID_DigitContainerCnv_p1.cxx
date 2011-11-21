/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_EventTPCnv/LUCID_DigitContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static LUCID_DigitCnv_p1 digitConv;

void LUCID_DigitContainerCnv_p1::transToPers(const LUCID_DigitContainer* transObj, LUCID_DigitContainer_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p1::transToPers" << endreq;
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    LUCID_Digit_p1& digit = (*persObj)[i];
    
    digitConv.transToPers((*transObj)[i], &digit, log);
  }    
}

void LUCID_DigitContainerCnv_p1::persToTrans(const LUCID_DigitContainer_p1* persObj, LUCID_DigitContainer* transObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p1::persToTrans" << endreq;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {

    const LUCID_Digit_p1* digit = &((*persObj)[i]);
    
    transObj->push_back(digitConv.createTransient(digit, log));
  }    
}

