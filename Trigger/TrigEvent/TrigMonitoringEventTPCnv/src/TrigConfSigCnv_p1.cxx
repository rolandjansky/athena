/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigConfSig.h"

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSigCnv_p1.h"

void TrigConfSigCnv_p1::persToTrans(const TrigConfSig_p1* persObj, 
				    TrigConfSig* transObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSigCnv_p1::persToTrans called " << endmsg;
  }

  *transObj = TrigConfSig (persObj->m_counter,
                           persObj->m_logic,
                           persObj->m_label);
  for (uint32_t te : persObj->m_output_te) {
    transObj->addOutputTE (te);
  }
}


void TrigConfSigCnv_p1::transToPers(const TrigConfSig* transObj, 
				    TrigConfSig_p1* persObj, 
				    MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSigCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_counter    = transObj->getCounter();
  persObj->m_logic      = transObj->getLogic();
  persObj->m_label      = transObj->getLabel();
  persObj->m_output_te  = transObj->getOutputTEs(); 
}
