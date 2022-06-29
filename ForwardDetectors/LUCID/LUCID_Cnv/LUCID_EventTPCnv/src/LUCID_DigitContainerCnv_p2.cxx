/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_EventTPCnv/LUCID_DigitContainerCnv_p2.h"
#include "GaudiKernel/MsgStream.h"

static const LUCID_DigitCnv_p2 digitConv_p2;

void LUCID_DigitContainerCnv_p2::transToPers(const LUCID_DigitContainer* transObj, LUCID_DigitContainer_p2* persObj, MsgStream& log) const {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p2::transToPers" << endmsg;
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    LUCID_Digit_p2& digit = (*persObj)[i];
    
    digitConv_p2.transToPers((*transObj)[i], &digit, log);
  }    
}

void LUCID_DigitContainerCnv_p2::persToTrans(const LUCID_DigitContainer_p2* persObj, LUCID_DigitContainer* transObj, MsgStream& log) const {

  log << MSG::DEBUG << "In LUCID_DigitContainerCnv_p2::persToTrans" << endmsg;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {

    const LUCID_Digit_p2* digit = &((*persObj)[i]);
    
    transObj->push_back(digitConv_p2.createTransientConst(digit, log));
  }    
}

