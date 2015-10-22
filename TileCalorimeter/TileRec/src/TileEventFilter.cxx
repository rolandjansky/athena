/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileEventFilter.cxx
 * @brief Implementation of class TileEventFilter
 */
  
#include <string>
#include "TileRec/TileEventFilter.h"

#include "xAODEventInfo/EventInfo.h"
    
TileEventFilter::TileEventFilter(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthFilterAlgorithm(name, pSvcLocator)
  , m_nProcessedEvents(0)
  , m_nAcceptedEvents(0)
{
  declareProperty("TriggerTypes", m_triggerTypes);
}

TileEventFilter::~TileEventFilter() {}

StatusCode TileEventFilter::initialize() { 

  if (m_triggerTypes.empty()) {
    ATH_MSG_INFO("No trigger type specified,  all received trigger types will be processed");
  } else {
    msg(MSG::INFO) << "Only events of the following trigger type(s) will be processed: ";
    for (auto trigger: m_triggerTypes) {
      msg(MSG::INFO) << trigger << " (0x" << std::hex << trigger << ") " << std::dec;
      
    }
    msg(MSG::INFO) << endmsg;
  }
  
  return StatusCode::SUCCESS; 
}

StatusCode TileEventFilter::execute() {

  ++m_nProcessedEvents;

  uint32_t lvl1info(0);
  //  uint32_t eventNumber(0);
  //  uint32_t lumiBlock(0);
  //  uint32_t eventBCID(0);
  
  const DataHandle<xAOD::EventInfo> eventInfo;
  if (evtStore()->retrieve(eventInfo).isSuccess()) {
    lvl1info = eventInfo->level1TriggerType();
    //    eventNumber = eventInfo->eventNumber();
    //    lumiBlock = eventInfo->lumiBlock();
    //    eventBCID = eventInfo->bcid();
  }

  if (m_triggerTypes.empty()
      || std::find( m_triggerTypes.begin(), m_triggerTypes.end(), lvl1info) != m_triggerTypes.end()) {

    setFilterPassed(true);
    ++m_nAcceptedEvents;
  } else {
    setFilterPassed(false);
  }

  return StatusCode::SUCCESS; 
}

StatusCode TileEventFilter::finalize() { 
  
  ATH_MSG_INFO ("Processed " << m_nProcessedEvents << " events, accepted " << m_nAcceptedEvents << " events.");

  return StatusCode::SUCCESS; 
}
