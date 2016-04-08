/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Header file for class LumiBlockRangeCnv_p1
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
///////////////////////////////////////////////////////////////////

#include "LumiBlockData/LB_IOVRange.h"
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p1.h"

void LumiBlockRangeCnv_p1::transToPers(const LB_IOVRange* trans, LumiBlockRange_p1* pers, MsgStream &) {
  pers->m_start = (trans->start()).re_time();
  pers->m_stop = (trans->stop()).re_time();
}

void LumiBlockRangeCnv_p1::persToTrans(const LumiBlockRange_p1* pers, LB_IOVRange* trans, MsgStream &)
{
  IOVTime tstart, tstop;
  tstart.setRETime (pers->m_start);
  tstop.setRETime (pers->m_stop);
  *trans = LB_IOVRange (tstart, tstop);
}
