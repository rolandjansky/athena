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
   for (std::set<EventType_p1>::const_iterator iter = pers->m_eventTypes.begin(),
		   last = pers->m_eventTypes.end(); iter != last; iter++) {
      trans->m_eventTypes.insert(*typeConv.createTransient(&(*iter), log));
   }
}

void EventStreamInfoCnv_p1::transToPers(const EventStreamInfo* trans, EventStreamInfo_p1* pers, MsgStream &log) {
   pers->m_numberOfEvents = trans->m_numberOfEvents;
   pers->m_runNumbers = trans->m_runNumbers;
   pers->m_lumiBlockNumbers = trans->m_lumiBlockNumbers;
   pers->m_processingTags = trans->m_processingTags;
   pers->m_itemList = trans->m_itemList;
   pers->m_eventTypes.clear();
   for (std::set<EventType>::const_iterator iter = trans->m_eventTypes.begin(),
		   last = trans->m_eventTypes.end(); iter != last; iter++) {
      EventType_p1 type;
      typeConv.transToPers(&(*iter), &type, log);
      pers->m_eventTypes.insert(type);
   }
}
