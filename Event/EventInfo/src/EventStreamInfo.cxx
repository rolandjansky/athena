/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
   log << MSG::DEBUG << "EventStreamInfo number of events: " << m_numberOfEvents << endmsg;
   log << MSG::DEBUG << "EventStreamInfo Run Numbers: ";
   for (unsigned int rn : m_runNumbers) {
      log << MSG::DEBUG << rn << ", ";
   }
   log << MSG::DEBUG << endmsg;
   log << MSG::DEBUG << "EventStreamInfo LumiBlock Numbers: ";
   for (unsigned int lbn : m_lumiBlockNumbers) {
      log << MSG::DEBUG << lbn << ", ";
   }
   log << MSG::DEBUG << endmsg;
   log << MSG::DEBUG << "EventStreamInfo Processing Tags: ";
   for (const std::string tag : m_processingTags) {
      log << MSG::DEBUG << tag << ", ";
   }
   log << MSG::DEBUG << endmsg;
   log << MSG::DEBUG << "EventStreamInfo Event Types: ";
   for (const EventType& typ : m_eventTypes) {
      log << MSG::DEBUG << typ.typeToString() << ", ";
   }
   log << MSG::DEBUG << endmsg;
   return;
}
//______________________________________________________________________________
