/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#include <AnaAlgorithm/FilterReporter.h>
#include <RootCoreUtils/StringUtil.h>
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
      m_selectionAccessors.emplace_back(m_selectionDecoration + "_" + RCU::substitute (chain, "-", "_"));
    }
  }

  ANA_CHECK (m_filterParams.initialize());

  return StatusCode::SUCCESS;
}

StatusCode CP::TrigEventSelectionAlg::execute()
{
  EL::FilterReporter filter (m_filterParams, false);

  if (m_trigList.empty()) {
    filter.setPassed(true);
    return StatusCode::SUCCESS;
  }

  const xAOD::EventInfo *evtInfo = 0;
  ANA_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));

  for (size_t i = 0; i < m_trigList.size(); i++) {
    bool trigPassed = m_trigDecisionTool->isPassed(m_trigList[i]);
    if (!m_selectionDecoration.empty()) {
      m_selectionAccessors[i](*evtInfo) = trigPassed;
    }
    if (trigPassed)
      filter.setPassed (true);
  }

  return StatusCode::SUCCESS;
}

StatusCode CP::TrigEventSelectionAlg::finalize()
{
  ANA_CHECK (m_filterParams.finalize());

  return StatusCode::SUCCESS;
}
