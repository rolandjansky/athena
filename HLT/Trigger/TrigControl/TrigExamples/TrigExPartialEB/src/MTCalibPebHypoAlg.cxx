/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MTCalibPebHypoAlg.h"
#include "DecisionHandling/HLTIdentifier.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionAuxContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::newDecisionIn;

// =============================================================================
// Standard constructor
// =============================================================================
MTCalibPebHypoAlg::MTCalibPebHypoAlg(const std::string& name, ISvcLocator* svcLoc)
: HypoBase(name, svcLoc) {}

// =============================================================================
// Standard destructor
// =============================================================================
MTCalibPebHypoAlg::~MTCalibPebHypoAlg() {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode MTCalibPebHypoAlg::initialize() {
  ATH_MSG_INFO("Initialising " << name());
  ATH_CHECK(m_hypoTools.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode MTCalibPebHypoAlg::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  ATH_CHECK(m_hypoTools.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode MTCalibPebHypoAlg::execute(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // New output
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());

  // Create new decision
  Decision* newd = newDecisionIn(decisions.get()); // DecisionContainer decisions owns the pointer

  // Prepare input for hypo tools
  MTCalibPebHypoTool::Input toolInput(newd, eventContext);

  // Call the hypo tools
  for (const auto& tool: m_hypoTools) {
    ATH_MSG_DEBUG("Calling " << tool);
    ATH_CHECK(tool->decide(toolInput));
  }

  auto outputHandle = SG::makeHandle(decisionOutput(), eventContext);
  CHECK( outputHandle.record(std::move(decisions), std::move(aux) ) );

  ATH_MSG_DEBUG( "Exiting with "<< outputHandle->size() <<" decisions");

  for (auto outh: *outputHandle) {
    TrigCompositeUtils::DecisionIDContainer objDecisions;
    TrigCompositeUtils::decisionIDs(outh, objDecisions);
    ATH_MSG_DEBUG("Number of positive decisions for this input: " << objDecisions.size());
    for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG(" --- found new decision " << HLT::Identifier(id));
    }
  }

  return StatusCode::SUCCESS;
}
