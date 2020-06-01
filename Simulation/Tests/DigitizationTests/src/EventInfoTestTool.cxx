/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoTestTool.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"

EventInfoTestTool::EventInfoTestTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : DigiTestToolBase(type, name, parent) {
  declareProperty("EventInfoName", m_rhkEventInfo);
}

StatusCode EventInfoTestTool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << " - package version "
                               << PACKAGE_VERSION);
  ATH_CHECK(m_rhkEventInfo.initialize());
  return StatusCode::SUCCESS;
}

StatusCode EventInfoTestTool::processEvent() {
  SG::ReadHandle<xAOD::EventInfo> pevt(m_rhkEventInfo);
  if (!pevt.isValid()) {
    ATH_MSG_ERROR("Could not get xAOD::EventInfo with key "
                  << m_rhkEventInfo.key());
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Successfully retrieved event info as xAOD::EventInfo");

  //+++ Get sub-event info object
  ATH_MSG_DEBUG("Main Event Info: ");
  ATH_MSG_INFO("RunNumber = " << pevt->runNumber());
  ATH_MSG_INFO("Event Number = " << pevt->eventNumber());
  ATH_MSG_INFO("LumiBlock = " << pevt->lumiBlock());
  ATH_MSG_INFO("TimeStamp = " << pevt->timeStamp());
  ATH_MSG_INFO("BCID = " << pevt->bcid());
  ATH_MSG_INFO("mu = " << pevt->actualInteractionsPerCrossing());
  ATH_MSG_INFO("<mu> = " << pevt->averageInteractionsPerCrossing());

  ATH_MSG_INFO("xAOD::EventInfo::SubEvent info: ");
  std::vector<xAOD::EventInfo::SubEvent>::const_iterator it =
      pevt->subEvents().begin();
  std::vector<xAOD::EventInfo::SubEvent>::const_iterator end =
      pevt->subEvents().end();
  if (it == end) ATH_MSG_INFO("No xAOD::EventInfo::SubEvent found");
  for (auto sevt : pevt->subEvents()) {
    if (sevt.ptr() != NULL) {
      ATH_MSG_INFO("SubEvent info:");
      ATH_MSG_INFO("  Time         : "
                   << (*it).time() << endmsg
                   << "  Index        : " << (*it).index() << endmsg
                   << "  Run Number   : " << sevt.ptr()->runNumber() << endmsg
                   << "  Event Number : " << sevt.ptr()->eventNumber() << endmsg
                   << "  ns Offset    : " << sevt.ptr()->timeStampNSOffset()
                   << endmsg << "  Lumi Block   : " << sevt.ptr()->lumiBlock()
                   << endmsg << "  BCID         : " << sevt.ptr()->bcid()
                   << endmsg << "  PileUpType   : " << (*it).type());
      ++it;
    } else
      ATH_MSG_INFO("xAOD::EventInfo::SubEvent is null");
  }

  return StatusCode::SUCCESS;
}

StatusCode EventInfoTestTool::finalize() { return StatusCode::SUCCESS; }
