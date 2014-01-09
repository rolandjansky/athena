/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Common code for generators that need reconstructed event data inputs.
// 
// Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008

#include "EmbeddedTrackGenerator/EmbeddedGeneratorCommon.h"

#include "GaudiKernel/IEvtSelector.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//--------------------------------------------------------------------------


EmbeddedGeneratorCommon::EmbeddedGeneratorCommon(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name, pSvcLocator), 
    m_pileUpSelector("InputDataEventSelector", name)
{
  //
}


//--------------------------------------------------------------------------


StatusCode EmbeddedGeneratorCommon::genInitialize() {

  evtStore()->setProxyProviderSvc(0);

  if (m_pileUpSelector.retrieve() != StatusCode::SUCCESS) {
    ATH_MSG_FATAL("genInitialize(): could not get eventSelector!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("genInitialize(): got InputDataEventSelector!");
  
  m_pileUpStream = PileUpStream("InputData_Event", serviceLocator(), &*m_pileUpSelector);

  bool READNEXTEVENT(false);
  if (!m_pileUpStream.setupStore(READNEXTEVENT)) {
    ATH_MSG_ERROR("Cannot setup m_pileUpStream store ");
    return StatusCode::FAILURE;
  }
  
  return embeddedGenInitialize();
}


//---------------------------------------------------------------------------


StatusCode EmbeddedGeneratorCommon::fillEvt(HepMC::GenEvent* genEvt) {
  if (! m_pileUpStream.nextEventPre() ) {
    ATH_MSG_FATAL("Could not load next data event. Not enough events on input?");
    return StatusCode::FAILURE;
  }
  
  const EventInfo* pInputEvent(0);
  m_pileUpStream.store().retrieve(pInputEvent);
  if (pInputEvent) {
    ATH_MSG_DEBUG("Event from file: " << *pInputEvent->event_ID());
  } else {
    ATH_MSG_DEBUG("Could not get EventInfo of the input event.");
  }
  
  const EventInfo* pOutputEvent(0);
  evtStore()->retrieve(pOutputEvent);
  if (pOutputEvent) {
    ATH_MSG_DEBUG("New event: "<< *pOutputEvent->event_ID());
  } else {
    ATH_MSG_DEBUG("Could not get EventInfo of the event being generated.");
    return StatusCode::FAILURE;
  }

  genEvt->set_event_number(pOutputEvent->event_ID()->event_number());
  return embeddedFillEvt(genEvt);
}
