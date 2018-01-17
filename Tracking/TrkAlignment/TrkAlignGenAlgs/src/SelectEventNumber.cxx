/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignGenAlgs/SelectEventNumber.h"

#include "xAODEventInfo/EventInfo.h"

#include <fstream>

namespace Trk {

//___________________________________________________________________________
SelectEventNumber::SelectEventNumber(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm (name, pSvcLocator)
  , m_lastGoodEvt(0)
{

  declareProperty("EventList", m_eventListName = "goodEvents.txt");

}

//___________________________________________________________________________
SelectEventNumber::~SelectEventNumber() 
{
    
}

//___________________________________________________________________________
StatusCode SelectEventNumber::initialize() 
{
  ATH_MSG_DEBUG("in SelectEventNumber::initialize()");
  std::ifstream input(m_eventListName.c_str());
  int run,event;
  if (input.is_open()) {
    while (input>>run>>event) {
      m_goodEventList.push_back(std::make_pair(run,event));
      ATH_MSG_DEBUG("adding run "<<run<<", event "<<event);
    }
  } 
  else {
    ATH_MSG_WARNING("could not open "<<m_eventListName);
  }   
  
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode SelectEventNumber::execute() 
{
  ATH_MSG_DEBUG("in SelectEventNumber::execute()");

  const xAOD::EventInfo* eventInfo = nullptr;
  StatusCode sc=evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) { 
    ATH_MSG_ERROR("Couldn't retrieve event info"); 
  }

  int run=eventInfo->runNumber();
  int evt=eventInfo->eventNumber();

  // check if this run/evt is in the good file
  int currentevt=m_lastGoodEvt;
  bool goodevt=false;
  std::vector<std::pair<int,int> >::iterator it=
    m_goodEventList.begin()+m_lastGoodEvt;
  for (; it!=m_goodEventList.end(); ++it,currentevt++) {	

    if (evt==(*it).second &&
	run==(*it).first) {
      m_lastGoodEvt=currentevt;
      goodevt=true;
      break;
    }
  }

  ATH_MSG_DEBUG("setting filter passed to "<<goodevt);

  setFilterPassed(goodevt);
  
  ATH_MSG_DEBUG("goodevt "<<goodevt);
  return StatusCode::SUCCESS;
}
  
  
//___________________________________________________________________________
StatusCode SelectEventNumber::finalize() 
{
  return StatusCode::SUCCESS;
}

}
