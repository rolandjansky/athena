/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMonitoringEvent/TrigConfSeq.h"
#undef private
#undef protected

// This class data and converter
#include "TrigMonitoringEventTPCnv/TrigConfSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSeqCnv_p1.h"

void TrigConfSeqCnv_p1::persToTrans(const TrigConfSeq_p1* persObj, 
				    TrigConfSeq* transObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSeqCnv_p1::persToTrans called " << endreq;
  }

  transObj->m_output_te_name  = persObj->m_output_te_name;
  transObj->m_output_te_index = persObj->m_output_te_index;
  transObj->m_output_te_id    = persObj->m_output_te_id;
  transObj->m_topo_te         = persObj->m_topo_te;
  transObj->m_input_te        = persObj->m_input_te;

  m_algCnv.persToTrans(&(persObj->m_alg), &(transObj->m_alg), log);
}


void TrigConfSeqCnv_p1::transToPers(const TrigConfSeq* transObj, 
				    TrigConfSeq_p1* persObj, 
				    MsgStream &log)
{
  if(log.level() <= MSG::DEBUG) {
    log << MSG::DEBUG << "TrigConfSeqCnv_p1::transToPers called " << endreq;
  }

  persObj->m_output_te_name  = transObj->m_output_te_name;
  persObj->m_output_te_index = transObj->m_output_te_index;
  persObj->m_output_te_id    = transObj->m_output_te_id;
  persObj->m_topo_te         = transObj->m_topo_te;
  persObj->m_input_te        = transObj->m_input_te;

  m_algCnv.transToPers(&(transObj->m_alg), &(persObj->m_alg), log);
}
