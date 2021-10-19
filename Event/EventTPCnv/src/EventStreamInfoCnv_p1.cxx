/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfoCnv_p1.cxx
 *  @brief This file contains the implementation for the EventStreamInfoCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p1.cxx,v 1.4 2009-03-18 17:40:57 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"
#include "EventInfo/EventType.h"

#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventTypeCnv_p1.h"

static const EventTypeCnv_p1 typeConv;

void EventStreamInfoCnv_p1::persToTrans(const EventStreamInfo_p1* pers, EventStreamInfo* trans, MsgStream &log)  const {
   *trans = EventStreamInfo();
   trans->addEvent (pers->m_numberOfEvents);
   for (unsigned int rn : pers->m_runNumbers)
     trans->insertRunNumber (rn);
   for (unsigned int lbn : pers->m_lumiBlockNumbers)
     trans->insertLumiBlockNumber (lbn);
   for (const std::string& tag : pers->m_processingTags)
     trans->insertProcessingTag (tag);
   for (const std::pair<CLID, std::string>& p : pers->m_itemList)
     trans->insertItemList (p.first, p.second);
   for (const EventType_p1& ptype : pers->m_eventTypes)
   {
      std::unique_ptr<EventType> p (typeConv.createTransientConst(&ptype, log));
      trans->insertEventType(*p);
   }
}

void EventStreamInfoCnv_p1::transToPers(const EventStreamInfo* trans, EventStreamInfo_p1* pers, MsgStream &log) const {
   pers->m_numberOfEvents = trans->getNumberOfEvents();
   pers->m_runNumbers = trans->getRunNumbers();
   pers->m_lumiBlockNumbers = trans->getLumiBlockNumbers();
   pers->m_processingTags = trans->getProcessingTags();
   pers->m_itemList = trans->getItemList();
   pers->m_eventTypes.clear();
   for (const EventType& ttype : trans->getEventTypes())
   {
     EventType_p1 ptype;
     typeConv.transToPers(&ttype, &ptype, log);
     pers->m_eventTypes.insert(ptype);
   }
}
