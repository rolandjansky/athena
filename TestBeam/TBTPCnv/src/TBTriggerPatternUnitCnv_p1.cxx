/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBTriggerPatternUnitCnv_p1.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBTriggerPatternUnit_p1.h"



void
TBTriggerPatternUnitCnv_p1::persToTrans(const TBTriggerPatternUnit_p1* pers, 
                                     TBTriggerPatternUnit* trans, MsgStream &/*log*/) const
{
  trans->m_triggerWord    = pers->m_triggerWord;
  trans->m_triggers = pers->m_triggers;
}


void
TBTriggerPatternUnitCnv_p1::transToPers(const TBTriggerPatternUnit* trans, 
                                     TBTriggerPatternUnit_p1* pers, MsgStream &/*log*/) const
{
  pers->m_triggerWord    = trans->getTriggerWord();
  pers->m_triggers = trans->getTriggers();
}

