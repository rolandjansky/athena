/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBEventInfoCnv_p1.h"
#include "TBEvent/TBEventInfo.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBEventInfo_p1.h"



void
TBEventInfoCnv_p1::persToTrans(const TBEventInfo_p1* pers, 
                                     TBEventInfo* trans, MsgStream &/*log*/) const
{
  *trans = TBEventInfo (pers->m_ev_number,
                        pers->m_ev_clock,
                        pers->m_ev_type,
                        pers->m_run_num,
                        pers->m_beam_moment,
                        pers->m_beam_part,
                        pers->m_cryoX,
                        pers->m_cryoAngle,
                        pers->m_tableY);
}


void
TBEventInfoCnv_p1::transToPers(const TBEventInfo* trans, 
                                     TBEventInfo_p1* pers, MsgStream &/*log*/) const
{
  pers->m_ev_number    = trans->getEventNum();
  pers->m_ev_clock     = trans->getEventClock();
  pers->m_ev_type      = trans->getEventType();
  pers->m_run_num      = trans->getRunNum();
  pers->m_beam_moment  = trans->getBeamMomentum();
  pers->m_beam_part    = trans->getBeamParticle();
  pers->m_cryoX        = trans->getCryoX();
  pers->m_cryoAngle    = trans->getCryoAngle();
  pers->m_tableY       = trans->getTableY();
  
}

