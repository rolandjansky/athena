/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonSeq.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonSeqCnv_p1.h"

void TrigMonSeqCnv_p1::persToTrans(const TrigMonSeq_p1* persObj, 
				   TrigMonSeq* transObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonSeqCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_encoded     = persObj->m_encoded;
  transObj->m_var_key     = persObj->m_var_key;
  transObj->m_var_val     = persObj->m_var_val;

  m_algCnv.persToTrans(&(persObj->m_alg), &(transObj->m_alg), log);
}


void TrigMonSeqCnv_p1::transToPers(const TrigMonSeq* transObj, 
				   TrigMonSeq_p1* persObj, 
				   MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonSeqCnv_p1::transToPers called " << endreq;
  }

  persObj->m_encoded    = transObj->m_encoded;
  persObj->m_var_key    = transObj->m_var_key;
  persObj->m_var_val    = transObj->m_var_val;

  m_algCnv.transToPers(&(transObj->m_alg), &(persObj->m_alg), log);
}
