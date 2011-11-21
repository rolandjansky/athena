/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LUCID_RawEvent/LUCID_RawData.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "LUCID_RawData_p1.h"
#include "LUCID_RawDataCnv_p1.h"

void LUCID_RawDataCnv_p1::persToTrans(const LUCID_RawData_p1* persObj, LUCID_RawData* transObj, MsgStream &log) {

  log << MSG::DEBUG << "In LUCID_RawDataCnv_p1:persToTrans" << endreq;
  
  transObj->m_word0  = persObj->m_word0;
  transObj->m_word1  = persObj->m_word1;
  transObj->m_word2  = persObj->m_word2;
  transObj->m_word3  = persObj->m_word3;

  transObj->m_word0p = persObj->m_word0p;
  transObj->m_word1p = persObj->m_word1p;
  transObj->m_word2p = persObj->m_word2p;
  transObj->m_word3p = persObj->m_word3p;

  transObj->m_word0n = persObj->m_word0n;
  transObj->m_word1n = persObj->m_word1n;
  transObj->m_word2n = persObj->m_word2n;
  transObj->m_word3n = persObj->m_word3n;

  transObj->m_status = persObj->m_status;
}

void LUCID_RawDataCnv_p1::transToPers(const LUCID_RawData* transObj, LUCID_RawData_p1* persObj, MsgStream &log) {
  
  log << MSG::DEBUG << "In LUCID_RawDataCnv_p1:transToPers" << endreq;
  
  persObj->m_word0  = transObj->m_word0;
  persObj->m_word1  = transObj->m_word1;
  persObj->m_word2  = transObj->m_word2;
  persObj->m_word3  = transObj->m_word3;

  persObj->m_word0p = transObj->m_word0p;
  persObj->m_word1p = transObj->m_word1p;
  persObj->m_word2p = transObj->m_word2p;
  persObj->m_word3p = transObj->m_word3p;

  persObj->m_word0n = transObj->m_word0n;
  persObj->m_word1n = transObj->m_word1n;
  persObj->m_word2n = transObj->m_word2n;
  persObj->m_word3n = transObj->m_word3n;

  persObj->m_status = transObj->m_status;
}
