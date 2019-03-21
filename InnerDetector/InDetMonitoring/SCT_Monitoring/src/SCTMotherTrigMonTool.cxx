/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCTMotherTrigMonTool.cxx
 * @author Jose E. Garcia <Jose.Enrique.Garcia@cern.ch> @n
 * modified s.roe 2/9/2009
 *********************************************************************************/

// Local
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"
// Framework
#include "StoreGate/ReadHandle.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

const std::string SCTMotherTrigMonTool::m_triggerNames[] = {
  "RNDM", "BPTX", "L1CAL", "TGC", "RPC", "MBTS", "COSM", "Calib"
};

SCTMotherTrigMonTool::SCTMotherTrigMonTool(const std::string& type, const std::string& name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
  m_isStream(false),
  m_firedTriggers(0) {
  declareProperty("doTrigger", m_doTrigger=true);
}

StatusCode
SCTMotherTrigMonTool::initialize() {
  if (ManagedMonitorToolBase::initialize().isFailure()) {
    ATH_MSG_ERROR("Could not initialize Monitor tool base!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_eventInfoKey.initialize() );

  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------
StatusCode
SCTMotherTrigMonTool::checkTriggers() {
  SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfoKey);
  if (evtInfo.isValid()) {
    m_firedTriggers = evtInfo->level1TriggerType();

    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
SCTMotherTrigMonTool::printTriggers() const {
  for (int i(0); i != N_TRIGGER_TYPES; ++i) {
    ATH_MSG_INFO(std::setw(7) << m_triggerNames[i] << " : " << m_firedTriggers.test(i));
  }
  return StatusCode::SUCCESS;
}

bool
SCTMotherTrigMonTool::hasTriggerFired(const unsigned int trigger) const {
  return((trigger < N_TRIGGER_TYPES) ? m_firedTriggers.test(trigger) : false);
}

bool
SCTMotherTrigMonTool::isCalibrationNoise(const std::string& L1_Item) {
  ATH_MSG_DEBUG("Trigger " << L1_Item << " = " << m_trigDecTool->isPassed(L1_Item));

  return m_trigDecTool->isPassed(L1_Item);
}

bool
SCTMotherTrigMonTool::isStream(const std::string& StreamName) {
  SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfoKey);
  if (evtInfo.isValid()) {
    m_isStream = false;

    for (unsigned int i = 0; i < evtInfo->streamTags().size(); ++i) {
      ATH_MSG_DEBUG(" i " << i << " Stream-Name " << evtInfo->streamTags()[i].name()
                    << " type " << evtInfo->streamTags()[i].type());

      if (evtInfo->streamTags()[i].name().find(StreamName) != std::string::npos) {
        m_isStream = true;
      }
    }
  }

  if (m_isStream == true) {
    ATH_MSG_DEBUG("Found " << StreamName << " Calibration Stream Event");
  }

  return m_isStream;
}
