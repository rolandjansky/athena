// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_EVENTINFODICT_H
# define EVENTINFO_EVENTINFODICT_H 1
/**
 * @file EventInfoDict.h
 *
 * @brief Header file for SEAL dictionary generation
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoDict.h,v 1.8 2009-05-13 23:53:34 binet Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/MergedEventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/TagInfo.h"
#include "AthLinks/DataLink.h"

class PyEventStreamInfo
{
public:
  static
  std::vector<unsigned int> 
  runNumbers(const ::EventStreamInfo& esi)
  { 
    return std::vector<unsigned int>(esi.getRunNumbers().begin(),
				     esi.getRunNumbers().end());
  }

  static 
  std::vector<unsigned int> 
  lumiBlockNumbers(const ::EventStreamInfo& esi)
  {
    return std::vector<unsigned int>(esi.getLumiBlockNumbers().begin(),
				     esi.getLumiBlockNumbers().end());
  }

  static
  std::vector<std::string> 
  processingTags(const ::EventStreamInfo& esi)
  {
    return std::vector<std::string>(esi.getProcessingTags().begin(),
				    esi.getProcessingTags().end());
  }

  static
  std::vector<std::pair<CLID,std::string> > 
  itemList(const ::EventStreamInfo& esi)
  {
    return std::vector<std::pair<CLID,std::string> >
      (esi.getItemList().begin(),
       esi.getItemList().end());
  }

  static
  std::vector<EventType> 
  eventTypes(const ::EventStreamInfo& esi)
  {
    return std::vector<EventType>(esi.getEventTypes().begin(),
				  esi.getEventTypes().end());
  }
};

class PyEventType
{
public:
  static 
  std::vector<unsigned int>
  bit_mask(const ::EventType& et)
  {
    return std::vector<unsigned int>(et.bit_mask_begin(),
				     et.bit_mask_end());
  }
};


struct EventInfoInstan
{
  std::list<PileUpEventInfo::SubEvent>::iterator i1;
  std::list<PileUpEventInfo::SubEvent>::const_iterator i2;
  std::pair<CLID, std::string> p;
};

template struct std::pair<CLID, std::string>;

#endif // EVENTINFO_EVENTINFODICT_H
