/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTBOOKKEEPERTPCNVDICT_H
#define EVENTBOOKKEEPERTPCNVDICT_H

///////////////////////////////////////////////////////////////
// EventBookkeeperTPCnvDict.h
// Author: David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////

#include <vector>
#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollectionCnv_p1.h"

#include "EventBookkeeperTPCnv/EventBookkeeper_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollectionCnv_p2.h"

#include "EventBookkeeperTPCnv/SkimDecision_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollection_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCnv_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollectionCnv_p1.h"

namespace EventBookkeeperTPCnvDict {

  struct tmp {
    std::vector<EventBookkeeper_p1>                       m_eventbookkeepercollection_p1;
    T_TPCnv<EventBookkeeperCollection, EventBookkeeperCollection_p1> m_eventbookkeepercollectioncnv_p1;
    T_TPCnv<EventBookkeeper, EventBookkeeper_p1>                 m_eventbookkeepercnv_p1;

    std::vector<EventBookkeeper_p2>                       m_eventbookkeepercollection_p2;
    std::vector<EventBookkeeper_p2*>                      m_eventbookkeepercollection_p2_children;
    T_TPCnv<EventBookkeeperCollection, EventBookkeeperCollection_p2> m_eventbookkeepercollectioncnv_p2;
    T_TPCnv<EventBookkeeper, EventBookkeeper_p2>                 m_eventbookkeepercnv_p2;

    std::vector<SkimDecision_p1>                       m_skimdecisioncollection_p1;
    T_TPCnv<SkimDecisionCollection, SkimDecisionCollection_p1> m_skimdecisioncollectioncnv_p1;
    T_TPCnv<SkimDecision, SkimDecision_p1>                 m_skimdecisioncnv_p1;
  };
}

#endif 
