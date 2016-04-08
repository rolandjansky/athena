/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RawEvent/LUCID_RawData.h"
#include "GaudiKernel/MsgStream.h"
#include "LUCID_EventTPCnv/LUCID_RawData_p1.h"
#include "LUCID_EventTPCnv/LUCID_RawDataCnv_p1.h"

void LUCID_RawDataCnv_p1::persToTrans(const LUCID_RawData_p1* persObj, LUCID_RawData* transObj, MsgStream &log) {

  log << MSG::DEBUG << "In LUCID_RawDataCnv_p1:persToTrans" << endreq;
  
  transObj->setWord0  (persObj->m_word0);
  transObj->setWord1  (persObj->m_word1);
  transObj->setWord2  (persObj->m_word2);
  transObj->setWord3  (persObj->m_word3);

  transObj->setWord0p (persObj->m_word0p);
  transObj->setWord1p (persObj->m_word1p);
  transObj->setWord2p (persObj->m_word2p);
  transObj->setWord3p (persObj->m_word3p);

  transObj->setWord0n (persObj->m_word0n);
  transObj->setWord1n (persObj->m_word1n);
  transObj->setWord2n (persObj->m_word2n);
  transObj->setWord3n (persObj->m_word3n);

  transObj->setStatus (persObj->m_status);
}

void LUCID_RawDataCnv_p1::transToPers(const LUCID_RawData* transObj, LUCID_RawData_p1* persObj, MsgStream &log) {
  
  log << MSG::DEBUG << "In LUCID_RawDataCnv_p1:transToPers" << endreq;
  
  persObj->m_word0  = transObj->getWord0();
  persObj->m_word1  = transObj->getWord1();
  persObj->m_word2  = transObj->getWord2();
  persObj->m_word3  = transObj->getWord3();

  persObj->m_word0p = transObj->getWord0p();
  persObj->m_word1p = transObj->getWord1p();
  persObj->m_word2p = transObj->getWord2p();
  persObj->m_word3p = transObj->getWord3p();

  persObj->m_word0n = transObj->getWord0n();
  persObj->m_word1n = transObj->getWord1n();
  persObj->m_word2n = transObj->getWord2n();
  persObj->m_word3n = transObj->getWord3n();

  persObj->m_status = transObj->getStatus();
}
