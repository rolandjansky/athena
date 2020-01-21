/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonRoi.h"

#include <iostream>
#include "TrigMonitoringEventTPCnv/TrigMonRoi_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoiCnv_p1.h"

void TrigMonRoiCnv_p1::persToTrans(const TrigMonRoi_p1* persObj, 
				   TrigMonRoi* transObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonRoiCnv_p1::persToTrans called " << endmsg;
  }

  transObj->getWord()   = persObj->m_word;
  transObj->getVarKey() = persObj->m_var_key;
  transObj->getVarVal() = persObj->m_var_val;
}


void TrigMonRoiCnv_p1::transToPers(const TrigMonRoi* transObj, 
				   TrigMonRoi_p1* persObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonRoiCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_word    = transObj->getWord();
  persObj->m_var_key = transObj->getVarKey();
  persObj->m_var_val = transObj->getVarVal();
}
