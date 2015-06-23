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
   trans->m_runNumbers.insert (pers->m_runNumbers.begin(),
                               pers->m_runNumbers.end());

   trans->m_lumiBlockNumbers.clear();
   trans->m_lumiBlockNumbers.insert (pers->m_lumiBlockNumbers.begin(),
                                     pers->m_lumiBlockNumbers.end());

   trans->m_processingTags.clear();
   trans->m_processingTags.insert (pers->m_processingTags.begin(),
                                   pers->m_processingTags.end());

   trans->m_itemList.clear();
   trans->m_itemList.insert (pers->m_itemList.begin(),
                             pers->m_itemList.end());

   trans->m_eventTypes.clear();
   std::set<EventType>::iterator lastType = trans->m_eventTypes.begin();
   for (const EventType_p3& ptype : pers->m_eventTypes) {
      std::unique_ptr<EventType> p (typeConv.createTransient(&ptype, log));
      lastType = trans->m_eventTypes.insert(lastType, *p);
   }
}

void EventStreamInfoCnv_p3::transToPers(const EventStreamInfo* trans, EventStreamInfo_p3* pers, MsgStream &log) {
   pers->m_numberOfEvents = trans->m_numberOfEvents;

   pers->m_runNumbers.reserve(trans->m_runNumbers.size());
   pers->m_runNumbers.assign (trans->m_runNumbers.begin(),
                              trans->m_runNumbers.end());

   pers->m_lumiBlockNumbers.reserve(trans->m_lumiBlockNumbers.size());
   pers->m_lumiBlockNumbers.assign (trans->m_lumiBlockNumbers.begin(),
                                    trans->m_lumiBlockNumbers.end());

   pers->m_processingTags.reserve(trans->m_processingTags.size());
   pers->m_processingTags.assign (trans->m_processingTags.begin(),
                                  trans->m_processingTags.end());

   pers->m_itemList.reserve(trans->m_itemList.size());
   pers->m_itemList.assign (trans->m_itemList.begin(),
                            trans->m_itemList.end());

   pers->m_eventTypes.reserve(trans->m_eventTypes.size());
   pers->m_eventTypes.clear();
   for (const EventType& ttype : trans->m_eventTypes) {
      EventType_p3 ptype;
      typeConv.transToPers(&ttype, &ptype, log);
      pers->m_eventTypes.push_back(ptype);
   }
}
