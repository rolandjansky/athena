/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"

#undef private
#undef protected

#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"

using namespace LVL1CTP;

void Lvl1ResultCnv_p2::transToPers(const LVL1CTP::Lvl1Result* trans,
                                     Lvl1Result_p2* pers, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p2::transToPers called " << endreq;

  pers->m_configured  = trans->m_configured;
  pers->m_l1_itemsTBP = trans->m_l1_itemsTBP;
  pers->m_l1_itemsTAP = trans->m_l1_itemsTAP;
  pers->m_l1_itemsTAV = trans->m_l1_itemsTAV;

}

void Lvl1ResultCnv_p2::persToTrans(const LVL1CTP::Lvl1Result_p2* pers,
                                     Lvl1Result* trans, MsgStream &log )
{

  log << MSG::DEBUG << "Lvl1ResultCnv_p2::persToTrans called " << endreq;

  trans->m_configured  = pers->m_configured        ;
  trans->m_l1_itemsTBP = pers->m_l1_itemsTBP;
  trans->m_l1_itemsTAP = pers->m_l1_itemsTAP;
  trans->m_l1_itemsTAV = pers->m_l1_itemsTAV;
}
