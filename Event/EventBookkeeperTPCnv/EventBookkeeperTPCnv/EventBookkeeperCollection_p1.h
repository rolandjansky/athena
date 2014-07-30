/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: David Cote <david.cote@cern.ch>
#ifndef EVENTBOOKKEEPERCOLLECTION_p1_H
#define EVENTBOOKKEEPERCOLLECTION_p1_H

#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"

class EventBookkeeperCollection_p1 : public std::vector<EventBookkeeper_p1>
{};

#endif // EVENTBOOKKEEPERCOLLECTION_p1_H
