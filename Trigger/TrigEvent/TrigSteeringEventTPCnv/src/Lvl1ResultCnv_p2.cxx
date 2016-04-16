/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"

using namespace LVL1CTP;

void Lvl1ResultCnv_p2::transToPers(const LVL1CTP::Lvl1Result* trans,
                                     Lvl1Result_p2* pers, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p2::transToPers called " << endreq;

  pers->m_configured  = trans->isConfigured();
  pers->m_l1_itemsTBP = trans->itemsBeforePrescale();
  pers->m_l1_itemsTAP = trans->itemsAfterPrescale();
  pers->m_l1_itemsTAV = trans->itemsAfterVeto();

}

void Lvl1ResultCnv_p2::persToTrans(const LVL1CTP::Lvl1Result_p2* pers,
                                     Lvl1Result* trans, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p2::persToTrans called " << endreq;

  *trans = Lvl1Result (pers->m_configured);
  trans->itemsBeforePrescale() = pers->m_l1_itemsTBP;
  trans->itemsAfterPrescale() = pers->m_l1_itemsTAP;
  trans->itemsAfterVeto() = pers->m_l1_itemsTAV;
}
