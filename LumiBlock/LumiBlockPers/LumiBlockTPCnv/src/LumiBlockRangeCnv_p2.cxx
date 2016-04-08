/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Header file for class LumiBlockRangeCnv_p2
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
///////////////////////////////////////////////////////////////////

#include "LumiBlockData/LB_IOVRange.h"

#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"

void LumiBlockRangeCnv_p2::transToPers(const LB_IOVRange* trans, LumiBlockRange_p2* pers, MsgStream &) {
  pers->m_start = (trans->start()).re_time();
  pers->m_stop = (trans->stop()).re_time();
  pers->m_NumExpected = trans->getNumExpected();
  pers->m_NumSeen = trans->getNumSeen();
}

void LumiBlockRangeCnv_p2::persToTrans(const LumiBlockRange_p2* pers, LB_IOVRange* trans, MsgStream &)
{
  IOVTime tstart, tstop;
  tstart.setRETime (pers->m_start);
  tstop.setRETime (pers->m_stop);
  *trans = LB_IOVRange (IOVRange (tstart, tstop),
                        pers->m_NumExpected,
                        pers->m_NumSeen);
}
