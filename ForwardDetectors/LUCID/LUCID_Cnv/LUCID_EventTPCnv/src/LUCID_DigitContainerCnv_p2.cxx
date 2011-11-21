/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_EventTPCnv/LUCID_DigitContainerCnv_p2.h"
#include "GaudiKernel/MsgStream.h"

static LUCID_DigitCnv_p2 digitConv_p2;

void LUCID_DigitContainerCnv_p2::transToPers(const LUCID_DigitContainer* transObj, LUCID_DigitContainer_p2* persObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p2::transToPers" << endreq;
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    LUCID_Digit_p2& digit = (*persObj)[i];
    
    digitConv_p2.transToPers((*transObj)[i], &digit, log);
  }    
}

void LUCID_DigitContainerCnv_p2::persToTrans(const LUCID_DigitContainer_p2* persObj, LUCID_DigitContainer* transObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p2::persToTrans" << endreq;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {

    const LUCID_Digit_p2* digit = &((*persObj)[i]);
    
    transObj->push_back(digitConv_p2.createTransient(digit, log));
  }    
}

