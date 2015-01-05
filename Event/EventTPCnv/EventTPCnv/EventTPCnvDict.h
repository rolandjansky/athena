/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTPCNV_DICT_H
#define EVENTTPCNV_DICT_H
/**
 * @file EventTPCnvDict.h
 *
 * @brief Header file for dictionary generation
 *
 * @author  <Marcin.Nowak@cern.ch>
 */

#include "EventTPCnv/EventID_p1.h"
#include "EventTPCnv/EventType_p1.h"
#include "EventTPCnv/EventType_p3.h"
#include "EventTPCnv/TriggerInfo_p1.h"
#include "EventTPCnv/TriggerInfo_p2.h"
#include "EventTPCnv/EventInfo_p1.h"
#include "EventTPCnv/EventInfo_p2.h"
#include "EventTPCnv/EventInfo_p3.h"
#include "EventTPCnv/EventInfo_p4.h"
#include "EventTPCnv/EventStreamInfo_p1.h"
#include "EventTPCnv/EventStreamInfo_p2.h"
#include "EventTPCnv/EventStreamInfo_p3.h"
#include "EventTPCnv/MergedEventInfo_p1.h"
#include "EventTPCnv/MergedEventInfo_p2.h"
#include "EventTPCnv/PileUpEventInfo_p1.h"
#include "EventTPCnv/PileUpEventInfo_p2.h"
#include "EventTPCnv/PileUpEventInfo_p3.h"
#include "EventTPCnv/PileUpEventInfo_p4.h"
#include "EventTPCnv/PileUpEventInfo_p5.h"
#include "EventTPCnv/ByteStreamMetadata_p1.h"
#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventInfoCnv_p4.h"
#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "EventTPCnv/EventStreamInfoCnv_p3.h"
#include "EventTPCnv/MergedEventInfoCnv_p1.h"
#include "EventTPCnv/MergedEventInfoCnv_p2.h"
#include "EventTPCnv/PileUpEventInfoCnv_p1.h"
#include "EventTPCnv/PileUpEventInfoCnv_p2.h"
#include "EventTPCnv/PileUpEventInfoCnv_p3.h"
#include "EventTPCnv/PileUpEventInfoCnv_p4.h"
#include "EventTPCnv/PileUpEventInfoCnv_p5.h"
#include "EventTPCnv/ByteStreamMetadataCnv_p1.h"

struct GCCXML_DUMMY_INSTANTIATION_EVENTTPCNV {
    T_TPCnv<EventInfo, EventInfo_p1>             m_eventinfo_p1;
    T_TPCnv<EventInfo, EventInfo_p2>             m_eventinfo_p2;
    T_TPCnv<EventInfo, EventInfo_p3>             m_eventinfo_p3;
    T_TPCnv<EventInfo, EventInfo_p4>             m_eventinfo_p4;
    T_TPCnv<PileUpEventInfo, PileUpEventInfo_p1> m_pileupeventinfo_p1;
    T_TPCnv<PileUpEventInfo, PileUpEventInfo_p2> m_pileupeventinfo_p2;
    T_TPCnv<PileUpEventInfo, PileUpEventInfo_p3> m_pileupeventinfo_p3;
    T_TPCnv<PileUpEventInfo, PileUpEventInfo_p4> m_pileupeventinfo_p4;
    T_TPCnv<PileUpEventInfo, PileUpEventInfo_p5> m_pileupeventinfo_p5;
    T_TPCnv<MergedEventInfo, MergedEventInfo_p1> m_mergedeventinfo_p1;
    T_TPCnv<MergedEventInfo, MergedEventInfo_p2> m_mergedeventinfo_p2;
    T_TPCnv<EventStreamInfo, EventStreamInfo_p1> m_eventstreaminfo_p1;
    T_TPCnv<EventStreamInfo, EventStreamInfo_p2> m_eventstreaminfo_p2;
    T_TPCnv<EventStreamInfo, EventStreamInfo_p3> m_eventstreaminfo_p3;

    std::set<std::pair<unsigned int, std::string> > m_set;
    std::pair<unsigned int, std::string> m_pair;
};

#endif
