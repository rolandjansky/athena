/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTPTCNV_EVENTSTREAMINFOPTCNV_P3_H
#define EVENTPTCNV_EVENTSTREAMINFOPTCNV_P3_H
/**
 * @file EventStreamInfoPTCnv_p3.h
 *
 * @brief Header file for in-file metadata getters (file peeking without Athena)
 *
 * @author  <vaniachine@anl.gov>
 */

#define private public
#include "EventTPCnv/EventStreamInfo_p3.h"
#undef private

class EventStreamInfoPTCnv_p3 : EventStreamInfo_p3 {
public:
    static unsigned int getNumberOfEvents (const ::EventStreamInfo_p3& esip) { return esip.m_numberOfEvents; };
    static std::vector<unsigned int> runNumbers (const ::EventStreamInfo_p3& esip) { return esip.m_runNumbers; };
    static std::vector<unsigned int> lumiBlockNumbers (const ::EventStreamInfo_p3& esip) { return esip.m_lumiBlockNumbers; };
    static std::vector<std::string> processingTags (const ::EventStreamInfo_p3& esip) { return esip.m_processingTags; };
    static std::vector<std::pair<unsigned int, std::string> > itemList (const ::EventStreamInfo_p3& esip) { return esip.m_itemList; };
    static std::vector<EventType_p3> eventTypes (const ::EventStreamInfo_p3& esip) { return esip.m_eventTypes; };
};

#endif // EVENTPTCNV_EVENTSTREAMINFOPTCNV_P3_H
