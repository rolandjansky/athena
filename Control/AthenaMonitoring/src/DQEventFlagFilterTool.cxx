/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQEventFlagFilterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "xAODEventInfo/EventInfo.h"

DQEventFlagFilterTool::DQEventFlagFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_alwaysReturnTrue(false)
, m_invert(false)
, m_doLAr(true)
, m_doTile(true)
, m_doSCT(true)
, m_doCore(true)
{
 declareInterface<IDQFilterTool>(this);
 declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
 declareProperty("invert", m_invert);
 declareProperty("doLAr", m_doLAr);
 declareProperty("doTile", m_doTile);
 declareProperty("doSCT", m_doSCT);
 declareProperty("doCore", m_doCore);
}
        
DQEventFlagFilterTool::~DQEventFlagFilterTool () {}

StatusCode DQEventFlagFilterTool::initialize()
{
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register callback if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;
}

bool DQEventFlagFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    const xAOD::EventInfo* eventInfo(0);
    CHECK( evtStore()->retrieve( eventInfo ) );
    bool passed(true); //event passes
    auto errorcode(xAOD::EventInfo::Error);
    if (m_doLAr && (eventInfo->errorState(xAOD::EventInfo::LAr) == errorcode)) {
      ATH_MSG_DEBUG("Event fails LAr event veto");
      passed = false;
    } else if (m_doTile && (eventInfo->errorState(xAOD::EventInfo::Tile) == errorcode)) {
      ATH_MSG_DEBUG("Event fails Tile event veto");
      passed = false;
    } else if (m_doSCT && (eventInfo->errorState(xAOD::EventInfo::SCT) == errorcode)) {
      ATH_MSG_DEBUG("Event fails SCT event veto");
      passed = false;
    } else if (m_doCore && (eventInfo->eventFlags(xAOD::EventInfo::Core) & 0x40000)) {
      ATH_MSG_DEBUG("Event fais data corruption veto");
      passed = false;
    }
    passed ^= m_invert;
    ATH_MSG_VERBOSE("Event flag DQ tool accept called, passed " << passed);
    return passed;
  }
}
