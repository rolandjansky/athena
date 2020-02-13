/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonSeq.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonSeqCnv_p1.h"

void TrigMonSeqCnv_p1::persToTrans(const TrigMonSeq_p1* persObj, 
				   TrigMonSeq* transObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonSeqCnv_p1::persToTrans called " << endmsg;
  }

  transObj->m_encoded     = persObj->m_encoded;
  transObj->m_var_key     = persObj->m_var_key;
  transObj->m_var_val     = persObj->m_var_val;

  m_algCnv.persToTrans(&(persObj->m_alg), &(transObj->m_alg), log);
}


void TrigMonSeqCnv_p1::transToPers(const TrigMonSeq* transObj, 
				   TrigMonSeq_p1* persObj, 
				   MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonSeqCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_encoded    = transObj->getEncoded();
  persObj->m_var_key    = transObj->getVarKey();
  persObj->m_var_val    = transObj->getVarVal();

  m_algCnv.transToPers(&transObj->getAlg(), &(persObj->m_alg), log);
}
