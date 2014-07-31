/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTSTREAMINFO_P2_H
#define EVENTTPCNV_EVENTSTREAMINFO_P2_H

/** @file EventStreamInfo_p2.h
 *  @brief This file contains the class definition for the EventStreamInfo_p2 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfo_p2.h,v 1.1 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include "EventTPCnv/EventType_p1.h"

#include <string>
#include <vector>

/** @class EventStreamInfo_p2
 *  @brief This class is the persistent representation of EventStreamInfo
 **/
class  EventStreamInfo_p2 {
public: // Constructor and Destructor
   /// Default Constructor
   EventStreamInfo_p2();
   /// Copy Constructor
   EventStreamInfo_p2(const EventStreamInfo_p2& rhs);
   /// Destructor
   virtual ~EventStreamInfo_p2();

public: // Non-static members
   /// Overloaded Assignment Operator
   EventStreamInfo_p2& operator=(const EventStreamInfo_p2& rhs);

friend class EventStreamInfoCnv_p2;

private:
   unsigned int	m_numberOfEvents;	// Number of Events
   std::vector<unsigned int> m_runNumbers;	// Run#
   std::vector<unsigned int> m_lumiBlockNumbers;	// LumiBlock#
   std::vector<std::string> m_processingTags;	// ProcessingTags
   std::vector<std::pair<unsigned int, std::string> > m_itemList;   // ItemList
   std::vector<EventType_p1> m_eventTypes;	// EventTypes
};

inline EventStreamInfo_p2::EventStreamInfo_p2() : m_numberOfEvents(0),
	m_runNumbers(),
	m_lumiBlockNumbers(),
	m_processingTags(),
	m_itemList(),
	m_eventTypes() {}
inline EventStreamInfo_p2::EventStreamInfo_p2(const EventStreamInfo_p2& rhs) : m_numberOfEvents(rhs.m_numberOfEvents),
	m_runNumbers(rhs.m_runNumbers),
	m_lumiBlockNumbers(rhs.m_lumiBlockNumbers),
	m_processingTags(rhs.m_processingTags),
	m_itemList(rhs.m_itemList),
	m_eventTypes(rhs.m_eventTypes) {}
inline EventStreamInfo_p2::~EventStreamInfo_p2() {}

inline EventStreamInfo_p2& EventStreamInfo_p2::operator=(const EventStreamInfo_p2& rhs) {
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
