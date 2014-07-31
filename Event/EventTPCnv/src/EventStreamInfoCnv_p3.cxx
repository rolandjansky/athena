/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfoCnv_p3.cxx
 *  @brief This file contains the implementation for the EventStreamInfoCnv_p3 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p3.cxx,v 1.1 2009-03-18 17:42:45 gemmeren Exp $
 **/

#define private public
#define protected public
#include "EventInfo/EventStreamInfo.h"
#undef private
#undef protected

#include "EventInfo/EventType.h"

#include "EventTPCnv/EventStreamInfoCnv_p3.h"
#include "EventTPCnv/EventTypeCnv_p3.h"

static EventTypeCnv_p3 typeConv;

void EventStreamInfoCnv_p3::persToTrans(const EventStreamInfo_p3* pers, EventStreamInfo* trans, MsgStream &log)  {
   trans->m_numberOfEvents = pers->m_numberOfEvents;
   trans->m_runNumbers.clear();
   std::set<unsigned int>::iterator lastRun = trans->m_runNumbers.begin();
   for (std::vector<unsigned int>::const_iterator iter = pers->m_runNumbers.begin(),
		   last = pers->m_runNumbers.end(); iter != last; iter++) {
      lastRun = trans->m_runNumbers.insert(lastRun, *iter);
   }
   trans->m_lumiBlockNumbers.clear();
   std::set<unsigned int>::iterator lastLB = trans->m_lumiBlockNumbers.begin();
   for (std::vector<unsigned int>::const_iterator iter = pers->m_lumiBlockNumbers.begin(),
		   last = pers->m_lumiBlockNumbers.end(); iter != last; iter++) {
      lastLB = trans->m_lumiBlockNumbers.insert(lastLB, *iter);
   }
   trans->m_processingTags.clear();
   std::set<std::string>::iterator lastTag = trans->m_processingTags.begin();
   for (std::vector<std::string>::const_iterator iter = pers->m_processingTags.begin(),
		   last = pers->m_processingTags.end(); iter != last; iter++) {
      lastTag = trans->m_processingTags.insert(lastTag, *iter);
   }
   trans->m_itemList.clear();
   std::set<std::pair<unsigned int, std::string> >::iterator lastItem = trans->m_itemList.begin();
   for (std::vector<std::pair<unsigned int, std::string> >::const_iterator iter = pers->m_itemList.begin(),
		   last = pers->m_itemList.end(); iter != last; iter++) {
      lastItem = trans->m_itemList.insert(lastItem, *iter);
   }
   trans->m_eventTypes.clear();
   std::set<EventType>::iterator lastType = trans->m_eventTypes.begin();
   for (std::vector<EventType_p3>::const_iterator iter = pers->m_eventTypes.begin(),
		   last = pers->m_eventTypes.end(); iter != last; iter++) {
      lastType = trans->m_eventTypes.insert(lastType, *typeConv.createTransient(&(*iter), log));
   }
}

void EventStreamInfoCnv_p3::transToPers(const EventStreamInfo* trans, EventStreamInfo_p3* pers, MsgStream &log) {
   pers->m_numberOfEvents = trans->m_numberOfEvents;

   pers->m_runNumbers.clear();
   pers->m_runNumbers.reserve(trans->m_runNumbers.size());
   for (std::set<unsigned int>::const_iterator iter = trans->m_runNumbers.begin(),
		   last = trans->m_runNumbers.end(); iter != last; iter++) {
      pers->m_runNumbers.push_back(*iter);
   }
   pers->m_lumiBlockNumbers.clear();
   pers->m_lumiBlockNumbers.reserve(trans->m_lumiBlockNumbers.size());
   for (std::set<unsigned int>::const_iterator iter = trans->m_lumiBlockNumbers.begin(),
		   last = trans->m_lumiBlockNumbers.end(); iter != last; iter++) {
      pers->m_lumiBlockNumbers.push_back(*iter);
   }
   pers->m_processingTags.clear();
   pers->m_processingTags.reserve(trans->m_processingTags.size());
   for (std::set<std::string>::const_iterator iter = trans->m_processingTags.begin(),
		   last = trans->m_processingTags.end(); iter != last; iter++) {
      pers->m_processingTags.push_back(*iter);
   }
   pers->m_itemList.clear();
   pers->m_itemList.reserve(trans->m_itemList.size());
   for (std::set<std::pair<unsigned int, std::string> >::const_iterator iter = trans->m_itemList.begin(),
		   last = trans->m_itemList.end(); iter != last; iter++) {
      pers->m_itemList.push_back(*iter);
   }
   pers->m_eventTypes.clear();
   pers->m_eventTypes.reserve(trans->m_eventTypes.size());
   for (std::set<EventType>::const_iterator iter = trans->m_eventTypes.begin(),
		   last = trans->m_eventTypes.end(); iter != last; iter++) {
      EventType_p3 type;
      typeConv.transToPers(&(*iter), &type, log);
      pers->m_eventTypes.push_back(type);
   }
}
