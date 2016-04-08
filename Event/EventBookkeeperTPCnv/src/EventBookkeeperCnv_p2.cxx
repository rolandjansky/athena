/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class EventBookkeeperCnv_p2
// Author: David Cote, July 2009 <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p2.h"

void EventBookkeeperCnv_p2::transToPers(const EventBookkeeper* trans, EventBookkeeper_p2* pers, MsgStream &log) {
  pers->m_nWeightedAcceptedEvents = trans->getNWeightedAcceptedEvents();
  pers->m_nAcceptedEvents = trans->getNAcceptedEvents();
  pers->m_cycle = trans->getCycle();
  pers->m_name = trans->getName();
  pers->m_description = trans->getDescription();
  pers->m_inputstream = trans->getInputStream();
  pers->m_outputstream = trans->getOutputStream();
  pers->m_logic = trans->getLogic();
  //To fill children, create new EventBookkeeper_p2 and call transToPers iteratively
  for(unsigned int i=0; i<trans->getChildrenEventBookkeepers()->size(); i++){ 
    const EventBookkeeper* tmpTrans=trans->getChildrenEventBookkeepers()->at(i);
    EventBookkeeper_p2* tmpPers=new EventBookkeeper_p2();
    transToPers(tmpTrans,tmpPers,log);
    if (!pers->m_childrenEB){ pers->m_childrenEB = new std::vector<EventBookkeeper_p2*>; }
    pers->m_childrenEB->push_back(tmpPers);
  }
  return;
}

void EventBookkeeperCnv_p2::persToTrans(const EventBookkeeper_p2* pers, EventBookkeeper* trans, MsgStream &log) {
  trans->setNWeightedAcceptedEvents( pers->m_nWeightedAcceptedEvents );
  trans->setNAcceptedEvents( pers->m_nAcceptedEvents );
  trans->setCycle( pers->m_cycle );
  trans->setName( pers->m_name );
  trans->setDescription( pers->m_description );
  trans->setInputStream( pers->m_inputstream );
  trans->setOutputStream(pers->m_outputstream);
  trans->setLogic(pers->m_logic);
  //To fill children, create new EventBookkeeper and call persToTrans iteratively
  if (pers->m_childrenEB) {
    for(unsigned int i=0; i<pers->m_childrenEB->size(); i++){ 
      const EventBookkeeper_p2* tmpPers=pers->m_childrenEB->at(i);
      EventBookkeeper* tmpTrans=new EventBookkeeper();
      persToTrans(tmpPers,tmpTrans,log);
      trans->AddChild(tmpTrans);
    }
  }
  return;
}
