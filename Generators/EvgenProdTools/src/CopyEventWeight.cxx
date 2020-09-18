/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/CopyEventWeight.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include <algorithm>
#include <cassert>



CopyEventWeight::CopyEventWeight(const std::string& name, ISvcLocator* svcLoc)
  : GenBase(name, svcLoc)
{
  declareProperty("McWeightEventInfoKey", m_mcWeightEventInfoKey="McWeightEventInfo");
}


StatusCode CopyEventWeight::execute() {
  // Check that the collection isn't empty
  /// @odo What is the best thing to do if it is?!
  const size_t nEvents = events_const()->size();
  if (nEvents == 0) {
    ATH_MSG_WARNING("McEventCollection is empty");
    return StatusCode::SUCCESS;
  }

  // Get the event info/type object to be filled
  const EventInfo* pInputEvt(0);
  CHECK(evtStore()->retrieve(pInputEvt));
  assert(pInputEvt);
  EventType* eventType = const_cast<EventType*>(pInputEvt->event_type());

  // Copy weights into EventInfo
  const size_t nw = event_const()->weights().size();
  if (nw == 0) ATH_MSG_WARNING("EVENT WEIGHT ARRAY EMPTY");
  for (size_t iw = 0; iw < nw; ++iw) {
    ATH_MSG_DEBUG("COPYING EVENT WEIGHT " << iw << "/" << nw << ": " << event_const()->weights()[iw]);
    eventType->set_mc_event_weight(event_const()->weights()[iw], iw, nw);
  }

  // Post-hoc debug printouts
  ATH_MSG_DEBUG("Copied HepMC signal event weight(s) to EventInfo");
  if (nEvents > 1) ATH_MSG_DEBUG("Ignored event weights of " << nEvents-1 << "bkg events");

  return StatusCode::SUCCESS;
}

#endif
