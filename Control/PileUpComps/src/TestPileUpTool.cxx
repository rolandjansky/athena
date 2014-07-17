/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TestPileUpTool.h"
TestPileUpTool::TestPileUpTool(const std::string& type, 
			       const std::string& name,
			       const IInterface* parent) : 
  PileUpToolBase(type, name, parent)
{
  declareInterface<IPileUpTool>(this);
}

StatusCode TestPileUpTool::prepareEvent(unsigned int nInputEvents) {

  ATH_MSG_DEBUG( "prepareEvent: expect to process " << nInputEvents << " events this time." );
  return StatusCode::SUCCESS;
}

StatusCode TestPileUpTool::mergeEvent() {
  std::vector<std::pair<unsigned int, int> >::iterator i(m_seen.begin()); 
  std::vector<std::pair<unsigned int, int> >::iterator e(m_seen.end()); 
  while (i != e) {
    ATH_MSG_DEBUG( "mergeEvent: there are " << i->first << " events in bunch xing " << i->second ); 
    ++i;
  }
  return StatusCode::SUCCESS;
}

StatusCode
TestPileUpTool::processBunchXing(int bunchXing,
				 PileUpEventInfo::SubEvent::const_iterator bSubEvents,
				 PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
    const EventInfo* pEI(0);
    if (seStore.retrieve(pEI).isSuccess()) {
      ATH_MSG_VERBOSE 
	("SubEvt EventInfo from StoreGate " << seStore.name() << "  : "
	 << " time offset: " << iEvt->time()
	 << " event: " << pEI->event_ID()->event_number() 
	 << " run: " << pEI->event_ID()->run_number());
      //FIXME << " contents: \n" << seStore.dump());
    }       
    ++iEvt;
  }
  return StatusCode::SUCCESS;
}
