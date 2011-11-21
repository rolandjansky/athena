/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_EventTPCnv/LUCID_RawDataContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static LUCID_RawDataCnv_p1 rawDataConv;

void LUCID_RawDataContainerCnv_p1::transToPers(const LUCID_RawDataContainer* transObj, LUCID_RawDataContainer_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_RawDataContainerCnv_p1::transToPers" << endreq;
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    LUCID_RawData_p1& rawData = (*persObj)[i];
    
    rawDataConv.transToPers((*transObj)[i], &rawData, log);
  }    
}

void LUCID_RawDataContainerCnv_p1::persToTrans(const LUCID_RawDataContainer_p1* persObj, LUCID_RawDataContainer* transObj, MsgStream& log) {

  log << MSG::DEBUG << "In LUCID_RawDataContainerCnv_p1::persToTrans" << endreq;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {

    const LUCID_RawData_p1* rawData = &((*persObj)[i]);
    
    transObj->push_back(rawDataConv.createTransient(rawData, log));
  }    
}

