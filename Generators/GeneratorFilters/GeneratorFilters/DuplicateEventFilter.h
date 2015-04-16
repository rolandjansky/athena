/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_DUPLICATE_EVENT_FILTER_H
#define GENERATORFILTERS_DUPLICATE_EVENT_FILTER_H

#include "GeneratorModules/GenFilter.h"
#include <set>

/**
 *  Filter events to remove any with duplicated event numbers
 *  This is intended to be used with the EvtGen afterburner mode, which 
 *  (for some reason) duplicates the first event.  This filter removes that event
 */
class DuplicateEventFilter : public GenFilter {
public:
  DuplicateEventFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

  size_t m_maxCheck;
  std::set<int> m_previousEvents;
  
};

#endif
