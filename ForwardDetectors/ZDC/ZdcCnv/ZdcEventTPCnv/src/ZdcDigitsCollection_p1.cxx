/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcEventTPCnv/ZdcDigitsCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static ZdcDigitsCnv_p1 digitConv;

void ZdcDigitsCollectionCnv_p1::transToPers(const ZdcDigitsCollection* transObj, ZdcDigitsCollection_p1* persObj, MsgStream& log) {
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    ZdcDigits_p1& digit = (*persObj)[i];
    
    digitConv.transToPers((*transObj)[i], &digit, log);
  }    
}

void ZdcDigitsCollectionCnv_p1::persToTrans(const ZdcDigitsCollection_p1* persObj, ZdcDigitsCollection* transObj, MsgStream& log) {

  //log << MSG::INFO << " size = " << persObj->size() << endmsg;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {
    
    //log << MSG::INFO << " i = " << i << endmsg;
    const ZdcDigits_p1* digit = &((*persObj)[i]);
    
    transObj->push_back(digitConv.createTransient(digit, log));
  }    
}
