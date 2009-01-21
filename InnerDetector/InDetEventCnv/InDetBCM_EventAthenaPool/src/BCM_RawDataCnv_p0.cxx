/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetBCM_RawData/BCM_RawData.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "BCM_RawDataCnv_p0.h"

void BCM_RawDataCnv_p0::persToTrans(const BCM_RawData_p0* persObj, BCM_RawData* transObj, MsgStream &log) {
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RawDataCnv_p0:persToTrans" << endreq;
  transObj->m_word1 = persObj->m_word1;
  transObj->m_word2 = persObj->m_word2;
}

void BCM_RawDataCnv_p0::transToPers(const BCM_RawData* transObj, BCM_RawData_p0* persObj, MsgStream &log) {
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RawDataCnv_p0:transToPers" << endreq;
  persObj->m_word1 = transObj->m_word1;
  persObj->m_word2 = transObj->m_word2;
}
