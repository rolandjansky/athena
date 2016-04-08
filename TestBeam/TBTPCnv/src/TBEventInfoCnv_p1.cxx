/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBEventInfoCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBEventInfo.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBEventInfo_p1.h"



void
TBEventInfoCnv_p1::persToTrans(const TBEventInfo_p1* pers, 
                                     TBEventInfo* trans, MsgStream &/*log*/)
{
  trans->m_ev_number    = pers->m_ev_number;
  trans->m_ev_clock     = pers->m_ev_clock;
  trans->m_ev_type      = pers->m_ev_type;
  trans->m_run_num      = pers->m_run_num;
  trans->m_beam_moment  = pers->m_beam_moment;
  trans->m_beam_part    = pers->m_beam_part;
  trans->m_cryoX        = pers->m_cryoX;
  trans->m_cryoAngle    = pers->m_cryoAngle;
  trans->m_tableY       = pers->m_tableY;
}


void
TBEventInfoCnv_p1::transToPers(const TBEventInfo* trans, 
                                     TBEventInfo_p1* pers, MsgStream &/*log*/)
{
  pers->m_ev_number    = trans->m_ev_number;
  pers->m_ev_clock     = trans->m_ev_clock;
  pers->m_ev_type      = trans->m_ev_type;
  pers->m_run_num      = trans->m_run_num;
  pers->m_beam_moment  = trans->m_beam_moment;
  pers->m_beam_part    = trans->m_beam_part;
  pers->m_cryoX        = trans->m_cryoX;
  pers->m_cryoAngle    = trans->m_cryoAngle;
  pers->m_tableY       = trans->m_tableY;
  
}

