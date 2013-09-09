/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigMonEvent.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"

void TrigMonEventCnv_p1::persToTrans(const TrigMonEvent_p1* persObj, 
				     TrigMonEvent* transObj, 
				     MsgStream &log)
{
  transObj->m_event       = persObj->m_event;
  transObj->m_lumi        = persObj->m_lumi;
  transObj->m_bxid        = persObj->m_bxid;
  transObj->m_run         = persObj->m_run;
  transObj->m_sec         = persObj->m_sec;
  transObj->m_nsec        = persObj->m_nsec;
  transObj->m_word        = persObj->m_word;
  transObj->m_l1          = persObj->m_l1;
  transObj->m_hlt         = persObj->m_hlt;
  transObj->m_var_key     = persObj->m_var_key;
  transObj->m_var_val     = persObj->m_var_val;

  m_robCnv.persToTrans(&(persObj->m_rob), &(transObj->m_rob), log);
  m_roiCnv.persToTrans(&(persObj->m_roi), &(transObj->m_roi), log);
  m_seqCnv.persToTrans(&(persObj->m_seq), &(transObj->m_seq), log);
  m_teCnv. persToTrans(&(persObj->m_te),  &(transObj->m_te),  log);

  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG 
	<< "TrigMonEventCnv_p1::persToTrans called - persistent to transient :" << endreq
	<< " event: " << transObj->m_event << " to " << persObj->m_event << endreq
	<< " lumi:  " << transObj->m_lumi  << " to " << persObj->m_lumi  << endreq
	<< " run:   " << transObj->m_run   << " to " << persObj->m_run   << endreq;
  }
}


void TrigMonEventCnv_p1::transToPers(const TrigMonEvent* transObj, 
				     TrigMonEvent_p1* persObj, 
				     MsgStream &log)
{
  persObj->m_event      = transObj->m_event;
  persObj->m_lumi       = transObj->m_lumi;
  persObj->m_bxid       = transObj->m_bxid;
  persObj->m_run        = transObj->m_run;
  persObj->m_sec        = transObj->m_sec;
  persObj->m_nsec       = transObj->m_nsec;
  persObj->m_word       = transObj->m_word;
  persObj->m_l1         = transObj->m_l1;
  persObj->m_hlt        = transObj->m_hlt;
  persObj->m_var_key    = transObj->m_var_key;
  persObj->m_var_val    = transObj->m_var_val;

  m_robCnv.transToPers(&(transObj->m_rob), &(persObj->m_rob), log);
  m_roiCnv.transToPers(&(transObj->m_roi), &(persObj->m_roi), log);
  m_seqCnv.transToPers(&(transObj->m_seq), &(persObj->m_seq), log);
  m_teCnv. transToPers(&(transObj->m_te),  &(persObj->m_te),  log);

  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG 
	<< "TrigMonEventCnv_p1::transToPers called - persistent to transient:" << endreq
	<< " event: " << persObj->m_event << " to " << transObj->m_event << endreq
	<< " lumi:  " << persObj->m_lumi  << " to " << transObj->m_lumi  << endreq
	<< " run:   " << persObj->m_run   << " to " << transObj->m_run   << endreq;
  }
}
