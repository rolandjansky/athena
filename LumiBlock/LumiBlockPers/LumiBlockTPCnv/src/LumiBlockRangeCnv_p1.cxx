/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Header file for class LumiBlockRangeCnv_p1
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
///////////////////////////////////////////////////////////////////

#define private public
#define protected public
#include "LumiBlockData/LB_IOVRange.h"
#undef private
#undef protected

#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"

void LumiBlockRangeCnv_p1::transToPers(const LB_IOVRange* trans, LumiBlockRange_p1* pers, MsgStream &) {
  pers->m_start = (trans->start()).re_time();
  pers->m_stop = (trans->stop()).re_time();
}

void LumiBlockRangeCnv_p1::persToTrans(const LumiBlockRange_p1* pers, LB_IOVRange* trans, MsgStream &) {
  
  (trans->m_start).m_status = IOVTime::RUN_EVT;
  (trans->m_start).setRETime(pers->m_start);
  (trans->m_start).m_status = IOVTime::RUN_EVT;
  (trans->m_stop).setRETime(pers->m_stop);
  trans->m_NumExpected = 0;
  trans->m_NumSeen = 0;

}
