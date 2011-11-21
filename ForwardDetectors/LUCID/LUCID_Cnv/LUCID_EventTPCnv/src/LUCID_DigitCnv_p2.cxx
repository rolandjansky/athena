/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LUCID_RawEvent/LUCID_Digit.h"
#undef private
#undef protected

#include "LUCID_EventTPCnv/LUCID_Digit_p2.h"
#include "LUCID_EventTPCnv/LUCID_DigitCnv_p2.h"

void LUCID_DigitCnv_p2::persToTrans(const LUCID_Digit_p2* persObj, LUCID_Digit* transObj, MsgStream &log) {

  log << MSG::DEBUG << " In LUCID_DigitCnv_p2::persToTrans " << endreq;
  
  transObj->m_tubeID = persObj->m_tubeID;
  transObj->m_npe    = persObj->m_npe;
  transObj->m_npeGas = persObj->m_npeGas;
  transObj->m_npePmt = persObj->m_npePmt;
  transObj->m_qdc    = persObj->m_qdc;
  transObj->m_tdc    = persObj->m_tdc;
  transObj->m_isHit  = persObj->m_isHit;
}

void LUCID_DigitCnv_p2::transToPers(const LUCID_Digit* transObj, LUCID_Digit_p2* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In LUCID_DigitCnv_p2::transToPers " << endreq;
  
  persObj->m_tubeID = transObj->m_tubeID;
  persObj->m_npe    = transObj->m_npe;
  persObj->m_npeGas = transObj->m_npeGas;
  persObj->m_npePmt = transObj->m_npePmt;
  persObj->m_qdc    = transObj->m_qdc;
  persObj->m_tdc    = transObj->m_tdc;
  persObj->m_isHit  = transObj->m_isHit;
}
