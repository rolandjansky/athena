/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p1.h"

#undef private
#undef protected

#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p1.h"

using namespace LVL1CTP;

void Lvl1ResultCnv_p1::transToPers(const LVL1CTP::Lvl1Result* trans,
                                     Lvl1Result_p1* pers, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p1::transToPers called " << endreq;

  pers->m_configured         = trans->m_configured        ;
  //  pers->m_l1_items           = trans->m_l1_items          ;
  //  pers->m_l1_itemsPrescaled  = trans->m_l1_itemsPrescaled ;
  // this method will not be used anymore, since there is a Lvl1ResultCnv_p2 which is always used for saving the trans to pers !
}

void Lvl1ResultCnv_p1::persToTrans(const LVL1CTP::Lvl1Result_p1* pers,
                                     Lvl1Result* trans, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p1::persToTrans called " << endreq;
  // this is for reading in old, ie Lvl1Result_p1 class, into the new transient class

  trans->m_configured         = pers->m_configured;
  trans->m_l1_itemsTAV        = pers->m_l1_items  ;
  trans->m_l1_itemsTBP        = pers->m_l1_items  ;
  trans->m_l1_itemsTAP        = pers->m_l1_items  ;
}
