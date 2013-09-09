/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigConfSig.h"
#undef private
#undef protected

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSigCnv_p1.h"

void TrigConfSigCnv_p1::persToTrans(const TrigConfSig_p1* persObj, 
				    TrigConfSig* transObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSigCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_counter     = persObj->m_counter;
  transObj->m_logic       = persObj->m_logic;
  transObj->m_label       = persObj->m_label;
  transObj->m_output_te   = persObj->m_output_te; 
}


void TrigConfSigCnv_p1::transToPers(const TrigConfSig* transObj, 
				    TrigConfSig_p1* persObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSigCnv_p1::transToPers called " << endreq;
  }

  persObj->m_counter    = transObj->m_counter;
  persObj->m_logic      = transObj->m_logic;
  persObj->m_label      = transObj->m_label;
  persObj->m_output_te  = transObj->m_output_te; 
}
