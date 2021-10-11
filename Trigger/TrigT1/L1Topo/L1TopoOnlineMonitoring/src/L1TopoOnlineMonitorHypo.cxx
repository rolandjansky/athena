/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoOnlineMonitorHypo.h"
#include "L1TopoOnlineMonitorHypoTool.h"

// TrigCompositeUtils types used here
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

// TrigCompositeUtils methods used here
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::newDecisionIn;
using TrigCompositeUtils::hypoAlgNodeName;


// =============================================================================
// Standard constructor
// =============================================================================
L1TopoOnlineMonitorHypo::L1TopoOnlineMonitorHypo(const std::string& name, ISvcLocator* svcLoc)
: HypoBase(name, svcLoc) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TopoOnlineMonitorHypo::initialize() {
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode L1TopoOnlineMonitorHypo::execute(const EventContext& eventContext) const {
  // ---------------------------------------------------------------------------
  // Retrieve previous decisions
  // ---------------------------------------------------------------------------
  SG::ReadHandle<DecisionContainer> previousDecisionsHandle = SG::makeHandle(decisionInput(), eventContext);
  ATH_CHECK(previousDecisionsHandle.isValid());
  ATH_MSG_DEBUG("Running with " << previousDecisionsHandle->size() << " previous decisions");

  // ---------------------------------------------------------------------------
  // Prepare new decisions linked to the previous and the tool input vector
  // ---------------------------------------------------------------------------
  SG::WriteHandle<DecisionContainer> decisions = createAndStore(decisionOutput(), eventContext);
  L1TopoOnlineMonitorHypoTool::InputVector pairedDecisions;
  for (const Decision* previousDecision: *previousDecisionsHandle) {
    pairedDecisions.emplace_back(
      newDecisionIn(decisions.ptr(), previousDecision, hypoAlgNodeName(), eventContext),
      previousDecision);
  }

  // ---------------------------------------------------------------------------
  // Call the hypo tools
  // ---------------------------------------------------------------------------
  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Calling " << tool.typeAndName());
    ATH_CHECK(tool->decide(pairedDecisions, eventContext));
  }

  // ---------------------------------------------------------------------------
  // Print the passing decisions
  // ---------------------------------------------------------------------------
  ATH_CHECK(decisions.isValid());
  ATH_MSG_DEBUG("Exit with " << decisions->size() << " decisions");
  ATH_CHECK(hypoBaseOutputProcessing(decisions));

  return StatusCode::SUCCESS;
}
