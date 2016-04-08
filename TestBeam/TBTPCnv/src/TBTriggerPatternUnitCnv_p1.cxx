/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTriggerPatternUnitCnv_p1.h"
#define private public
#define protected public
#include "TBEvent/TBTriggerPatternUnit.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTriggerPatternUnit_p1.h"



void
TBTriggerPatternUnitCnv_p1::persToTrans(const TBTriggerPatternUnit_p1* pers, 
                                     TBTriggerPatternUnit* trans, MsgStream &/*log*/)
{

  trans->m_triggerWord    = pers->m_triggerWord;

  unsigned int nTriggers=pers->m_triggers.size();
  trans->m_triggers.reserve(nTriggers);
  trans->m_triggers = pers->m_triggers;

}


void
TBTriggerPatternUnitCnv_p1::transToPers(const TBTriggerPatternUnit* trans, 
                                     TBTriggerPatternUnit_p1* pers, MsgStream &/*log*/)
{

  pers->m_triggerWord    = trans->m_triggerWord;
 
  unsigned int nTriggers=trans->m_triggers.size();
  pers->m_triggers.reserve(nTriggers);
  pers->m_triggers = trans->m_triggers;

}

