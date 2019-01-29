/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <TriggerAnalysisAlgorithms/TrigEventSelectionAlg.h>
#include <xAODEventInfo/EventInfo.h>

CP::TrigEventSelectionAlg::TrigEventSelectionAlg(const std::string &name,
                                             ISvcLocator *svcLoc)
  : EL::AnaAlgorithm(name, svcLoc),
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("tool", m_trigDecisionTool, "trigger decision tool");
  declareProperty("triggers", m_trigList, "trigger selection list");
  declareProperty("selectionDecoration", m_selectionDecoration, "the decoration the trigger pass status");
}

StatusCode CP::TrigEventSelectionAlg::initialize()
{
  if (m_trigList.empty()) {
    ATH_MSG_ERROR("A list of triggers needs to be provided");
    return StatusCode::FAILURE;
  }

  ANA_CHECK(m_trigDecisionTool.retrieve());

  if (!m_selectionDecoration.empty()) {
    for (const std::string &chain : m_trigList) {
      m_selectionAccessors.emplace_back(m_selectionDecoration + "_" + chain);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CP::TrigEventSelectionAlg::execute()
{
  m_total++;

  if (m_trigList.empty()) {
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }

  setFilterPassed(false);

  const xAOD::EventInfo *evtInfo = 0;
  ANA_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));

  bool passed = false;
  for (size_t i = 0; i < m_trigList.size(); i++) {
    bool trigPassed = m_trigDecisionTool->isPassed(m_trigList[i]);
    if (!m_selectionDecoration.empty()) {
      m_selectionAccessors[i](*evtInfo) = trigPassed;
    }
    passed = passed || trigPassed;
  }

  if (passed) {
    m_passed++;
    setFilterPassed(true);
  }

  return StatusCode::SUCCESS;
}

StatusCode CP::TrigEventSelectionAlg::finalize()
{
  ATH_MSG_INFO("Events passing triggers: " << m_passed << " / " << m_total);

  return StatusCode::SUCCESS;
}
