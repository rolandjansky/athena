/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfoCnv_p1.cxx
 *  @brief This file contains the implementation for the EventStreamInfoCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p1.cxx,v 1.4 2009-03-18 17:40:57 gemmeren Exp $
 **/

#define private public
#define protected public
#include "EventInfo/EventStreamInfo.h"
#undef private
#undef protected

#include "EventInfo/EventType.h"

#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventTypeCnv_p1.h"

static EventTypeCnv_p1 typeConv;

void EventStreamInfoCnv_p1::persToTrans(const EventStreamInfo_p1* pers, EventStreamInfo* trans, MsgStream &log)  {
   trans->m_numberOfEvents = pers->m_numberOfEvents;
   trans->m_runNumbers = pers->m_runNumbers;
   trans->m_lumiBlockNumbers = pers->m_lumiBlockNumbers;
   trans->m_processingTags = pers->m_processingTags;
   trans->m_itemList = pers->m_itemList;
   trans->m_eventTypes.clear();
   for (const EventType_p1& ptype : pers->m_eventTypes)
   {
      std::unique_ptr<EventType> p (typeConv.createTransient(&ptype, log));
      trans->m_eventTypes.insert(*p);
   }
}

void EventStreamInfoCnv_p1::transToPers(const EventStreamInfo* trans, EventStreamInfo_p1* pers, MsgStream &log) {
   pers->m_numberOfEvents = trans->m_numberOfEvents;
   pers->m_runNumbers = trans->m_runNumbers;
   pers->m_lumiBlockNumbers = trans->m_lumiBlockNumbers;
   pers->m_processingTags = trans->m_processingTags;
   pers->m_itemList = trans->m_itemList;
   pers->m_eventTypes.clear();
   for (const EventType& ttype : trans->m_eventTypes)
   {
     EventType_p1 ptype;
     typeConv.transToPers(&ttype, &ptype, log);
     pers->m_eventTypes.insert(ptype);
   }
}
