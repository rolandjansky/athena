/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EVENTINFO_EVENTSTREAMINFO_H
#define EVENTINFO_EVENTSTREAMINFO_H

/** @file EventStreamInfo.h
 *  @brief This file contains the class definition for the EventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfo.h,v 1.5 2009-02-09 22:11:39 gemmeren Exp $
 **/

#include "GaudiKernel/ClassID.h"

#include "EventInfo/EventType.h"

#include <string>
#include <set>
#include <iostream>

// Forward declarations
class MsgStream;

/** @class EventStreamInfo
 *  @brief This class provides the summary information stored for data written as a Event Stream.
 **/
class EventStreamInfo {
public:
  // Constructor and Destructor
  /// Default Constructor
  EventStreamInfo();
  /// Destructor
  virtual ~EventStreamInfo();
public:
  // Non-static members
  /// @return the Number of Events
  unsigned int getNumberOfEvents() const;
  /// @return a set of Run Numbers
  const std::set<unsigned int>& getRunNumbers() const;
  /// @return a set of Luminosity Block Numbers
  const std::set<unsigned int>& getLumiBlockNumbers() const;
  /// @return a set of Processing Tags
  const std::set<std::string>& getProcessingTags() const;
  /// @return a set of ItemList Entries
  const std::set<std::pair<CLID, std::string> >& getItemList() const;
  /// @return a set of Event Types
  const std::set<EventType>& getEventTypes() const;

  /// Increase Event Counter
  void addEvent(unsigned int number = 1);
  /// Insert new Run Number into a set.
  /// @param run [IN] the Run Number to be inserted.
  void insertRunNumber(unsigned int run);
  /// Insert new Luminosity Block Number into a set.
  /// @param lumiBlock [IN] the Luminosity Block Number to be inserted.
  void insertLumiBlockNumber(unsigned int lumiBlock);
  /// Insert new Processing Tag into a set.
  /// @param process [IN] string with the Processing Tag to be inserted.
  void insertProcessingTag(const std::string& process);
  /// Insert new ItemList Entry into a set.
  /// @param type [IN] CLID for type to be inserted.
  /// @param key [IN] string with key to be inserted.
  void insertItemList(CLID type, const std::string& key);
  /// Insert new Event Type into a set.
  /// @param event [IN] EventType object of the Event to be inserted.
  void insertEventType(const EventType& event);

  /// Print a EventStreamInfo to the log file.
  void print(MsgStream& log) const;
private:
  unsigned int m_numberOfEvents;   // Number of Events
  std::set<unsigned int> m_runNumbers; // Run#
  std::set<unsigned int> m_lumiBlockNumbers;   // LumiBlock#
  std::set<std::string> m_processingTags;  // ProcessingTags
  std::set<std::pair<CLID, std::string> > m_itemList;  // ItemList
  std::set<EventType> m_eventTypes;    // EventTypes
};

std::ostream& operator<<(std::ostream& os, const EventStreamInfo& esi);

inline EventStreamInfo::EventStreamInfo() : m_numberOfEvents(0),
  m_runNumbers(),
  m_lumiBlockNumbers(),
  m_processingTags(),
  m_itemList(),
  m_eventTypes() {}
inline EventStreamInfo::~EventStreamInfo() {}


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(EventStreamInfo, 167728019, 1)
#include "AthenaKernel/MetaCont.h"
CLASS_DEF(MetaCont<EventStreamInfo>, 167729019, 1)
#include "AthenaKernel/BaseInfo.h"
SG_BASE(MetaCont<EventStreamInfo>, MetaContBase);
#endif
