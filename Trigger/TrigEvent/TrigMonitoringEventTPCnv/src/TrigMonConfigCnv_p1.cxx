/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonConfig.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonConfig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"

void TrigMonConfigCnv_p1::persToTrans(const TrigMonConfig_p1* persObj, 
				      TrigMonConfig* transObj, 
				      MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonConfigCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_event       = persObj->m_event;
  transObj->m_lumi        = persObj->m_lumi;
  transObj->m_run         = persObj->m_run;
  transObj->m_sec         = persObj->m_sec;
  transObj->m_nsec        = persObj->m_nsec;
  transObj->m_master_key  = persObj->m_master_key;
  transObj->m_hlt_ps_key  = persObj->m_hlt_ps_key;
  transObj->m_lv1_ps_key  = persObj->m_lv1_ps_key;
  transObj->m_var_name    = persObj->m_var_name;
  transObj->m_var_id      = persObj->m_var_id;
  transObj->m_pair_key    = persObj->m_pair_key;
  transObj->m_pair_val    = persObj->m_pair_val; 
  
  m_chainCnv.persToTrans(&(persObj->m_chain), &(transObj->m_chain), log);
  m_seqCnv  .persToTrans(&(persObj->m_seq),   &(transObj->m_seq),   log);
}


void TrigMonConfigCnv_p1::transToPers(const TrigMonConfig* transObj, 
				      TrigMonConfig_p1* persObj, 
				      MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigMonConfigCnv_p1::transToPers called " << endreq;
  }

  persObj->m_event       = transObj->m_event;
  persObj->m_lumi        = transObj->m_lumi;
  persObj->m_run         = transObj->m_run;
  persObj->m_sec         = transObj->m_sec;
  persObj->m_nsec        = transObj->m_nsec;
  persObj->m_master_key  = transObj->m_master_key;
  persObj->m_hlt_ps_key  = transObj->m_hlt_ps_key;
  persObj->m_lv1_ps_key  = transObj->m_lv1_ps_key;
  persObj->m_var_name    = transObj->m_var_name;
  persObj->m_var_id      = transObj->m_var_id;
  persObj->m_pair_key    = transObj->m_pair_key;
  persObj->m_pair_val    = transObj->m_pair_val;
  
  m_chainCnv.transToPers(&(transObj->m_chain), &(persObj->m_chain), log);
  m_seqCnv  .transToPers(&(transObj->m_seq),   &(persObj->m_seq),   log);
}
