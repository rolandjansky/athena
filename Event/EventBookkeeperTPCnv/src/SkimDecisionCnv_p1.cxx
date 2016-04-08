/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class SkimDecisionCnv_p1
// Author: David Cote, September 2008. <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "EventBookkeeperMetaData/SkimDecision.h"

#include "EventBookkeeperTPCnv/SkimDecisionCnv_p1.h"

void SkimDecisionCnv_p1::transToPers(const SkimDecision* trans, SkimDecision_p1* pers, MsgStream &) {
  pers->m_name = trans->getName();
  pers->m_isAccepted = trans->isAccepted();
}

void SkimDecisionCnv_p1::persToTrans(const SkimDecision_p1* pers, SkimDecision* trans, MsgStream &) {
  trans->setName( pers->m_name );
  trans->setIsAccepted( pers->m_isAccepted );
}
