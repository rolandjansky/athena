/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfoCnv_p2.cxx
 *  @brief This file contains the implementation for the EventStreamInfoCnv_p2 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p2.cxx,v 1.1 2009-03-18 17:42:45 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"
#include "EventInfo/EventType.h"

#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "EventTPCnv/EventTypeCnv_p1.h"

static const EventTypeCnv_p1 typeConv;

void EventStreamInfoCnv_p2::persToTrans(const EventStreamInfo_p2* pers, EventStreamInfo* trans, MsgStream &log) const {
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

void EventStreamInfoCnv_p2::transToPers(const EventStreamInfo* trans, EventStreamInfo_p2* pers, MsgStream &log) const {
   pers->m_numberOfEvents = trans->getNumberOfEvents();

   pers->m_runNumbers.assign (trans->getRunNumbers().begin(),
                              trans->getRunNumbers().end());

   pers->m_lumiBlockNumbers.assign (trans->getLumiBlockNumbers().begin(),
                                    trans->getLumiBlockNumbers().end());

   pers->m_processingTags.assign (trans->getProcessingTags().begin(),
                                  trans->getProcessingTags().end());

   pers->m_itemList.assign (trans->getItemList().begin(),
                            trans->getItemList().end());

   pers->m_eventTypes.reserve(trans->getEventTypes().size());
   pers->m_eventTypes.clear();
   for (const EventType& ttype : trans->getEventTypes()) {
      EventType_p1 ptype;
      typeConv.transToPers(&ttype, &ptype, log);
      pers->m_eventTypes.push_back(ptype);
   }
}
