/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBPhaseCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBPhase.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBPhase_p1.h"



void
TBPhaseCnv_p1::persToTrans(const TBPhase_p1* pers, 
                                     TBPhase* trans, MsgStream &/*log*/)
{

  trans->m_phase    = pers->m_phase;
  trans->m_phaseind = pers->m_phaseind;
  trans->m_dTtoWAC  = pers->m_dTtoWAC;

}


void
TBPhaseCnv_p1::transToPers(const TBPhase* trans, 
                                     TBPhase_p1* pers, MsgStream &/*log*/)
{

  pers->m_phase    = trans->m_phase;
  pers->m_phaseind = trans->m_phaseind;
  pers->m_dTtoWAC  = trans->m_dTtoWAC;

}

