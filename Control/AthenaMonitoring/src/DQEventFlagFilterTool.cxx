/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DQEventFlagFilterTool.h"
#include "AthenaKernel/errorcheck.h"

DQEventFlagFilterTool::DQEventFlagFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
{
 declareInterface<IDQFilterTool>(this);
}
        
DQEventFlagFilterTool::~DQEventFlagFilterTool () {}

StatusCode DQEventFlagFilterTool::initialize()
{
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register callback if we always return true anyway
  ATH_CHECK( m_EventInfoKey.initialize(!m_alwaysReturnTrue) );
  return StatusCode::SUCCESS;
}

bool DQEventFlagFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    SG::ReadHandle<xAOD::EventInfo> eventInfo(m_EventInfoKey);
    if (! eventInfo.isValid()) {
      ATH_MSG_ERROR("Cannot retrieve EventInfo object, returning false (or true if invert is set)");
      return m_invert;
    }
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
