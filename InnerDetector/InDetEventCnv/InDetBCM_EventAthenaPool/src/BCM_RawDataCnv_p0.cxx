/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBCM_RawData/BCM_RawData.h"
#include "GaudiKernel/MsgStream.h"
#include "BCM_RawDataCnv_p0.h"

void BCM_RawDataCnv_p0::persToTrans(const BCM_RawData_p0* persObj, BCM_RawData* transObj, MsgStream &log) {
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RawDataCnv_p0:persToTrans" << endreq;
  *transObj = BCM_RawData (persObj->m_word1, persObj->m_word2);
}

void BCM_RawDataCnv_p0::transToPers(const BCM_RawData* transObj, BCM_RawData_p0* persObj, MsgStream &log) {
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RawDataCnv_p0:transToPers" << endreq;
  persObj->m_word1 = transObj->getWord1();
  persObj->m_word2 = transObj->getWord2();
}
