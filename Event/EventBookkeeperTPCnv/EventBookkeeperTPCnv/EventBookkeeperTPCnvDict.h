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

#include "EventBookkeeperTPCnv/EventBookkeeper_p2.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p2.h"

#include "EventBookkeeperTPCnv/SkimDecision_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollection_p1.h"

namespace EventBookkeeperTPCnvDict {

  struct tmp {
    std::vector<EventBookkeeper_p1>                       m_eventbookkeepercollection_p1;

    std::vector<EventBookkeeper_p2>                       m_eventbookkeepercollection_p2;
    std::vector<EventBookkeeper_p2*>                      m_eventbookkeepercollection_p2_children;

    std::vector<SkimDecision_p1>                          m_skimdecisioncollection_p1;
  };
}

#endif 
