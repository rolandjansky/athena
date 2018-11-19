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
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::finalize
// =============================================================================
StatusCode MTCalibPebHypoAlg::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute_r
// =============================================================================
StatusCode MTCalibPebHypoAlg::execute_r(const EventContext& eventContext) const {
  ATH_MSG_DEBUG("Executing " << name());

  // New output
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());

  // Prepare input for hypo tools
  std::vector<MTCalibPebHypoTool::Input> toolInput;

  // Create new decision
  Decision* newd = newDecisionIn(decisions.get()); // DecisionContainer decisions owns the pointer
  toolInput.emplace_back(newd);

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
