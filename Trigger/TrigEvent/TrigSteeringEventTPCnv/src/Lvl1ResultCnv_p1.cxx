/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p1.h"

using namespace LVL1CTP;

void Lvl1ResultCnv_p1::transToPers(const LVL1CTP::Lvl1Result* trans,
                                     Lvl1Result_p1* pers, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p1::transToPers called " << endreq;

  pers->m_configured         = trans->isConfigured()        ;
  //  pers->m_l1_items           = trans->m_l1_items          ;
  //  pers->m_l1_itemsPrescaled  = trans->m_l1_itemsPrescaled ;
  // this method will not be used anymore, since there is a Lvl1ResultCnv_p2 which is always used for saving the trans to pers !
}

void Lvl1ResultCnv_p1::persToTrans(const LVL1CTP::Lvl1Result_p1* pers,
                                     Lvl1Result* trans, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p1::persToTrans called " << endreq;
  // this is for reading in old, ie Lvl1Result_p1 class, into the new transient class

  *trans = Lvl1Result (pers->m_configured);
  trans->itemsBeforePrescale() = pers->m_l1_items;
  trans->itemsAfterPrescale() = pers->m_l1_items;
  trans->itemsAfterVeto() = pers->m_l1_items;
}
