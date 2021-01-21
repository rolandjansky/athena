/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/DuplicateEventFilter.h"

DuplicateEventFilter::DuplicateEventFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  GenFilter(name, pSvcLocator)
{
  declareProperty("MaxCheck", m_maxCheck = m_previousEvents.max_size());
}


StatusCode DuplicateEventFilter::filterEvent() {
  ATH_MSG_DEBUG("DuplicateEventFilter FILTERING");

  
  for(McEventCollection::const_iterator evt = events_const()->begin();
      evt != events_const()->end() ; ++evt){
    int eventNumber = (*evt)->event_number();
    if(m_previousEvents.count(eventNumber) != 0){
      setFilterPassed(false);
      ATH_MSG_INFO("Found multiple instances of event number" << eventNumber);
      return StatusCode::SUCCESS;
    }
    
    if(m_previousEvents.size() == m_maxCheck) m_previousEvents.erase(m_previousEvents.begin());
    
    m_previousEvents.insert(eventNumber);
  }
  
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}
