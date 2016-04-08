/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class EventBookkeeperCnv_p1
// Author: David Cote, September 2008 <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p1.h"

void EventBookkeeperCnv_p1::transToPers(const EventBookkeeper* trans, EventBookkeeper_p1* pers, MsgStream &log) {
  log << MSG::ERROR << "EventBookkeeperCnv_p1::transToPers should never be called. There is a mistake." << endreq;
  pers->m_nWeightedAcceptedEvents = trans->getNWeightedAcceptedEvents();
  pers->m_nAcceptedEvents = trans->getNAcceptedEvents();
  pers->m_cycle = trans->getCycle();
  pers->m_name = trans->getName();
  pers->m_description = trans->getDescription();
  //pers->m_parent = trans->getParent();
  pers->m_outputstream = trans->getOutputStream();
  pers->m_logic = trans->getLogic();
  //pers->m_childrenEB = trans->getChildrenEventBookkeepers();
}

void EventBookkeeperCnv_p1::persToTrans(const EventBookkeeper_p1* pers, EventBookkeeper* trans, MsgStream &) {
  trans->setNWeightedAcceptedEvents( pers->m_nWeightedAcceptedEvents );
  trans->setNAcceptedEvents( pers->m_nAcceptedEvents );
  trans->setCycle( int(pers->m_cycle) );
  trans->setName( pers->m_name );
  trans->setDescription( pers->m_description );
  trans->setInputStream( pers->m_parent );
  trans->setOutputStream(pers->m_outputstream);
  trans->setLogic(pers->m_logic);
  //trans->setChildrenEventBookkeepers(pers->m_childrenEB);
}
