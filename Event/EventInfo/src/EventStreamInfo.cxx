/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventStreamInfo.cxx
 *  @brief This file contains the implementation for the EventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfo.cxx,v 1.4 2009-02-09 22:11:39 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"

#include "GaudiKernel/MsgStream.h"

//______________________________________________________________________________
unsigned int EventStreamInfo::getNumberOfEvents() const {
   return(m_numberOfEvents);
}
//______________________________________________________________________________
const std::set<unsigned int>& EventStreamInfo::getRunNumbers() const {
   return(m_runNumbers);
}
//______________________________________________________________________________
const std::set<unsigned int>& EventStreamInfo::getLumiBlockNumbers() const {
   return(m_lumiBlockNumbers);
}
//______________________________________________________________________________
const std::set<std::string>& EventStreamInfo::getProcessingTags() const {
   return(m_processingTags);
}
//______________________________________________________________________________
const std::set<std::pair<CLID, std::string> >& EventStreamInfo::getItemList() const {
   return(m_itemList);
}
//______________________________________________________________________________
const std::set<EventType>& EventStreamInfo::getEventTypes() const {
   return(m_eventTypes);
}
//______________________________________________________________________________
void EventStreamInfo::addEvent(unsigned int number) {
   m_numberOfEvents += number;
}
//______________________________________________________________________________
void EventStreamInfo::insertRunNumber(unsigned int run) {
   m_runNumbers.insert(run);
}
//______________________________________________________________________________
void EventStreamInfo::insertLumiBlockNumber(unsigned int lumiBlock) {
   m_lumiBlockNumbers.insert(lumiBlock);
}
//______________________________________________________________________________
void EventStreamInfo::insertProcessingTag(const std::string& process) {
   m_processingTags.insert(process);
}
//______________________________________________________________________________
void EventStreamInfo::insertItemList(CLID type, const std::string& key) {
   m_itemList.insert(std::pair<CLID, std::string>(type, key));
}
//______________________________________________________________________________
void EventStreamInfo::insertEventType(const EventType& event) {
   m_eventTypes.insert(event);
}
//______________________________________________________________________________
void EventStreamInfo::print(MsgStream& log) const {
   log << MSG::DEBUG << "EventStreamInfo number of events: " << m_numberOfEvents << endreq;
   log << MSG::DEBUG << "EventStreamInfo Run Numbers: ";
   for (std::set<unsigned int>::const_iterator iter = m_runNumbers.begin(),
		   last = m_runNumbers.end(); iter != last; iter++) {
      log << MSG::DEBUG << *iter << ", ";
   }
   log << MSG::DEBUG << endreq;
   log << MSG::DEBUG << "EventStreamInfo LumiBlock Numbers: ";
   for (std::set<unsigned int>::const_iterator iter = m_lumiBlockNumbers.begin(),
		   last = m_lumiBlockNumbers.end(); iter != last; iter++) {
      log << MSG::DEBUG << *iter << ", ";
   }
   log << MSG::DEBUG << endreq;
   log << MSG::DEBUG << "EventStreamInfo Processing Tags: ";
   for (std::set<std::string>::const_iterator iter = m_processingTags.begin(),
		   last = m_processingTags.end(); iter != last; iter++) {
      log << MSG::DEBUG << *iter << ", ";
   }
   log << MSG::DEBUG << endreq;
   log << MSG::DEBUG << "EventStreamInfo Event Types: ";
   for (std::set<EventType>::const_iterator iter = m_eventTypes.begin(),
		   last = m_eventTypes.end(); iter != last; iter++) {
      log << MSG::DEBUG << iter->typeToString() << ", ";
   }
   log << MSG::DEBUG << endreq;
   return;
}
//______________________________________________________________________________
