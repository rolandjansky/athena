/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_EVENTINFO_P2_H
#define EVENTTPCNV_EVENTINFO_P2_H
/**
 * @file EventInfo_p2.h
 *
 * @brief This class is the persistent representation of EventInfo
 * @author R.D.Schaffer@cern.ch
 */

#include "EventTPCnv/EventID_p1.h"
#include "EventTPCnv/EventType_p1.h"
#include "EventTPCnv/TriggerInfo_p2.h"

class EventInfo_p2 {
public:
    EventInfo_p2() {}

    EventID_p1                  m_event_ID;
    EventType_p1                m_event_type;
    TriggerInfo_p2              m_trigger_info;
    std::vector<unsigned int>   m_event_flags;
};


#endif 


