/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEvent/TrigMonConfig.h"

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonConfig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"

void TrigMonConfigCnv_p1::persToTrans(const TrigMonConfig_p1* persObj, 
				      TrigMonConfig* transObj, 
				      MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonConfigCnv_p1::persToTrans called " << endmsg;
  }

  transObj->setEventID (persObj->m_event,
                        persObj->m_lumi,
                        persObj->m_run,
                        persObj->m_sec,
                        persObj->m_nsec);
  transObj->setTriggerKeys (persObj->m_master_key,
                            persObj->m_lv1_ps_key,
                            persObj->m_hlt_ps_key);

  transObj->getVarName()  = persObj->m_var_name;
  transObj->getVarId()    = persObj->m_var_id;
  transObj->getPairKey()  = persObj->m_pair_key;
  transObj->getPairVal()  = persObj->m_pair_val; 
  
  m_chainCnv.persToTrans(&(persObj->m_chain),
                         &transObj->getVec<TrigConfChain>(),
                         log);
  m_seqCnv  .persToTrans(&(persObj->m_seq),
                         &transObj->getVec<TrigConfSeq>(),
                         log);
}


void TrigMonConfigCnv_p1::transToPers(const TrigMonConfig* transObj, 
				      TrigMonConfig_p1* persObj, 
				      MsgStream &log) const
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonConfigCnv_p1::transToPers called " << endmsg;
  }

  persObj->m_event       = transObj->getEvent();
  persObj->m_lumi        = transObj->getLumi();
  persObj->m_run         = transObj->getRun();
  persObj->m_sec         = transObj->getSec();
  persObj->m_nsec        = transObj->getNanoSec();
  persObj->m_master_key  = transObj->getMasterKey();
  persObj->m_hlt_ps_key  = transObj->getHLTPrescaleKey();
  persObj->m_lv1_ps_key  = transObj->getLV1PrescaleKey();
  persObj->m_var_name    = transObj->getVarName();
  persObj->m_var_id      = transObj->getVarId();
  persObj->m_pair_key    = transObj->getPairKey();
  persObj->m_pair_val    = transObj->getPairVal();
  
  m_chainCnv.transToPers(&transObj->getVec<TrigConfChain>(),
                         &(persObj->m_chain), log);
  m_seqCnv  .transToPers(&transObj->getVec<TrigConfSeq>(),
                         &(persObj->m_seq),   log);
}
