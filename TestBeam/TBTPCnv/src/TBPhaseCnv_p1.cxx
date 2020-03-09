/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBPhaseCnv_p1.h"
#include "TBEvent/TBPhase.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBPhase_p1.h"



void
TBPhaseCnv_p1::persToTrans(const TBPhase_p1* pers, 
                                     TBPhase* trans, MsgStream &/*log*/) const
{
  *trans = TBPhase (pers->m_phase,
                    pers->m_phaseind,
                    pers->m_dTtoWAC);
}


void
TBPhaseCnv_p1::transToPers(const TBPhase* trans, 
                                     TBPhase_p1* pers, MsgStream &/*log*/) const
{
  pers->m_phase    = trans->getPhase();
  pers->m_phaseind = trans->getPhaseind();
  pers->m_dTtoWAC  = trans->getdTtoWAC();
}

