/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCTMotherTrigMonTool.cxx
 * @author Jose E. Garcia <Jose.Enrique.Garcia@cern.ch> @n
 * modified s.roe 2/9/2009
 *********************************************************************************/

// Local
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"
// Framework
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "StoreGate/ReadHandle.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

const std::string SCTMotherTrigMonTool::m_triggerNames[] = {
  "RNDM", "BPTX", "L1CAL", "TGC", "RPC", "MBTS", "COSM", "Calib"
};

SCTMotherTrigMonTool::SCTMotherTrigMonTool(const std::string &type, const std::string &name, const IInterface *parent)
  : ManagedMonitorToolBase(type, name, parent),
  m_doTrigger(true),
  m_isStream(false),
  m_firedTriggers(0),
  m_eventInfoKey(std::string("ByteStreamEventInfo")) {
  declareProperty("doTrigger", m_doTrigger);
}

StatusCode
SCTMotherTrigMonTool::initialize() {
  if (ManagedMonitorToolBase::initialize().isFailure()) {
    msg(MSG::ERROR) << "Could not initialize Monitor tool base!" << endmsg;
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_eventInfoKey.initialize() );

  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------
StatusCode
SCTMotherTrigMonTool::checkTriggers() {
  SG::ReadHandle<EventInfo> evtInfo(m_eventInfoKey);
  if (evtInfo.isValid()) {
    m_firedTriggers = evtInfo->trigger_info()->level1TriggerType();

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
SCTMotherTrigMonTool::printTriggers() const {
  for (int i(0); i != N_TRIGGER_TYPES; ++i) {
    if (msgLvl(MSG::INFO)) {
      msg(MSG::INFO) << std::setw(7) << m_triggerNames[i] << " : " << m_firedTriggers.test(i) << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

bool
SCTMotherTrigMonTool::hasTriggerFired(const unsigned int trigger) const {
  return((trigger < N_TRIGGER_TYPES) ? m_firedTriggers.test(trigger) : false);
}

bool
SCTMotherTrigMonTool::isCalibrationNoise(const std::string &L1_Item) {
  ATH_MSG_DEBUG("Trigger " << L1_Item << " = " << m_trigDecTool->isPassed(L1_Item));

  return m_trigDecTool->isPassed(L1_Item);
}

bool
SCTMotherTrigMonTool::isStream(const std::string &StreamName) {
  if (evtStore()->contains<EventInfo>(m_eventInfoKey.key())) {
    SG::ReadHandle<EventInfo> evtInfo(m_eventInfoKey);

    m_isStream = false;

    for (unsigned int i = 0; i < evtInfo->trigger_info()->streamTags().size(); ++i) {
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << " i " << i << " Stream-Name " << evtInfo->trigger_info()->streamTags()[i].name()
                        << " type " << evtInfo->trigger_info()->streamTags()[i].type() << endmsg;
      }

      if (evtInfo->trigger_info()->streamTags()[i].name().find(StreamName) != std::string::npos) {
        m_isStream = true;
      }
    }
  }

  if (m_isStream == true) {
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Found " << StreamName << " Calibration Stream Event" << endmsg;
    }
  }

  return m_isStream;
}
