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

struct GCCXML_DUMMY_INSTANTIATION_EVENTTPCNV {
    std::set<std::pair<unsigned int, std::string> > m_set;
    std::pair<unsigned int, std::string> m_pair;
};

#endif
