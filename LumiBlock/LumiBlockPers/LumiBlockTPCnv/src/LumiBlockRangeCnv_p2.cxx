/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Header file for class LumiBlockRangeCnv_p2
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
///////////////////////////////////////////////////////////////////

#define private public
#define protected public
#include "LumiBlockData/LB_IOVRange.h"
#undef private
#undef protected

#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"

void LumiBlockRangeCnv_p2::transToPers(const LB_IOVRange* trans, LumiBlockRange_p2* pers, MsgStream &) {
  pers->m_start = (trans->start()).re_time();
  pers->m_stop = (trans->stop()).re_time();
  pers->m_NumExpected = trans->m_NumExpected;
  pers->m_NumSeen = trans->m_NumSeen;
}

void LumiBlockRangeCnv_p2::persToTrans(const LumiBlockRange_p2* pers, LB_IOVRange* trans, MsgStream &) {
  
  (trans->m_start).m_status = IOVTime::RUN_EVT;
  (trans->m_start).setRETime(pers->m_start);
  (trans->m_start).m_status = IOVTime::RUN_EVT;
  (trans->m_stop).setRETime(pers->m_stop);
  trans->m_NumExpected = pers->m_NumExpected;
  trans->m_NumSeen = pers->m_NumSeen;
}
