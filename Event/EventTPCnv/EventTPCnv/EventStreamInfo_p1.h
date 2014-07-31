/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTSTREAMINFO_P1_H
#define EVENTTPCNV_EVENTSTREAMINFO_P1_H

/** @file EventStreamInfo_p1.h
 *  @brief This file contains the class definition for the EventStreamInfo_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfo_p1.h,v 1.3 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include "EventTPCnv/EventType_p1.h"

#include <string>
#include <set>

/** @class EventStreamInfo_p1
 *  @brief This class is the persistent representation of EventStreamInfo
 **/
class  EventStreamInfo_p1 {
public: // Constructor and Destructor
   /// Default Constructor
   EventStreamInfo_p1();
   /// Copy Constructor
   EventStreamInfo_p1(const EventStreamInfo_p1& rhs);
   /// Destructor
   virtual ~EventStreamInfo_p1();

public: // Non-static members
   /// Overloaded Assignment Operator
   EventStreamInfo_p1& operator=(const EventStreamInfo_p1& rhs);

friend class EventStreamInfoCnv_p1;

private:
   unsigned int	m_numberOfEvents;	// Number of Events
   std::set<unsigned int> m_runNumbers;	// Run#
   std::set<unsigned int> m_lumiBlockNumbers;	// LumiBlock#
   std::set<std::string> m_processingTags;	// ProcessingTags
   std::set<std::pair<unsigned int, std::string> > m_itemList;   // ItemList
   std::set<EventType_p1> m_eventTypes;	// EventTypes
};

inline EventStreamInfo_p1::EventStreamInfo_p1() : m_numberOfEvents(0),
	m_runNumbers(),
	m_lumiBlockNumbers(),
	m_processingTags(),
	m_itemList(),
	m_eventTypes() {}
inline EventStreamInfo_p1::EventStreamInfo_p1(const EventStreamInfo_p1& rhs) : m_numberOfEvents(rhs.m_numberOfEvents),
	m_runNumbers(rhs.m_runNumbers),
	m_lumiBlockNumbers(rhs.m_lumiBlockNumbers),
	m_processingTags(rhs.m_processingTags),
	m_itemList(rhs.m_itemList),
	m_eventTypes(rhs.m_eventTypes) {}
inline EventStreamInfo_p1::~EventStreamInfo_p1() {}

inline EventStreamInfo_p1& EventStreamInfo_p1::operator=(const EventStreamInfo_p1& rhs) {
   if (this != &rhs) {
      m_numberOfEvents = rhs.m_numberOfEvents;
      m_runNumbers = rhs.m_runNumbers;
      m_lumiBlockNumbers = rhs.m_lumiBlockNumbers;
      m_processingTags = rhs.m_processingTags;
      m_itemList = rhs.m_itemList;
      m_eventTypes = rhs.m_eventTypes;
   }
   return *this;
}

#endif
