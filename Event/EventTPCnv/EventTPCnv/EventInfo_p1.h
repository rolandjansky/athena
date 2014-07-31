/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTINFO_P1_H
#define EVENTTPCNV_EVENTINFO_P1_H
/**
 * @file EventInfo_p1.h
 *
 * @brief This class is the persistent representation of EventInfo
 * @author Marcin.Nowak@cern.ch
 */

#include "EventTPCnv/EventID_p1.h"
#include "EventTPCnv/EventType_p1.h"
#include "EventTPCnv/TriggerInfo_p1.h"

class EventInfo_p1 {
public:
  EventInfo_p1();
  virtual ~EventInfo_p1();

  EventID_p1 			m_event_ID;
  EventType_p1 			m_event_type;
  TriggerInfo_p1 *		m_trigger_info;

};

#include "EventTPCnv/EventInfo_p1.icc"

#endif 


